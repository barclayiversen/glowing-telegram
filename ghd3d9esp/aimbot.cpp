#include "includes.h"

double PI = 3.14159265358979323846;

LocalPlayer* LocalPlayer::Get()
{
	static uint32_t clientModule = (uint32_t)(GetModuleHandle("client.dll"));
	static LocalPlayer* localPlayer = (LocalPlayer*)(clientModule + hazedumper::signatures::dwLocalPlayer);

	return localPlayer;
}

Vector3 LocalPlayer::GetOrigin()
{
	return *(Vector3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecOrigin);
}

Vector3* LocalPlayer::GetViewOffset()
{
	return (Vector3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecViewOffset);
}

int* LocalPlayer::GetHealth()
{
	return (int*)(*(uintptr_t*)this + hazedumper::netvars::m_iHealth);
}

int* LocalPlayer::GetTeam()
{
	return (int*)(*(uintptr_t*)this + hazedumper::netvars::m_iTeamNum);
}

float LocalPlayer::GetDistance(Vector3* otherguy)
{
	Vector3 other = *(otherguy);
	Vector3 myPos = GetOrigin();
	Vector3 delta = Vector3(other.x - myPos.x, other.y - myPos.y, other.z - myPos.z);

	return sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
}

void LocalPlayer::AimAt(Vector3* target)
{
	static uint32_t engineModule = (uint32_t)GetModuleHandle("engine.dll");
	static Vector3* viewAngles = (Vector3*)(*(uint32_t*)(engineModule + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);

	Vector3 origin = GetOrigin();
	Vector3 viewOffset = *GetViewOffset();
	Vector3* myPos = &(origin + viewOffset);

	Vector3 deltaVec = { target->x - myPos->x, target->y - myPos->y, target->z - myPos->z };
	float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);

	float pitch = -asin(deltaVec.z / deltaVecLength) * (180 / PI);
	float yaw = atan2(deltaVec.y, deltaVec.x) * (180 / PI);

	if (pitch >= -89 && pitch <= 89 && yaw >= -180 && yaw <= 180)
	{
		viewAngles->x = pitch;
		viewAngles->y = yaw;
	}


}

Player* Player::GetPlayer(int index)
{
	static uint32_t moduleBase = (uint32_t)GetModuleHandle("client.dll");
	static uint32_t entityList = moduleBase + hazedumper::signatures::dwEntityList;

	return (Player*)(entityList + index * 0x10);
}

int* Player::GetMaxPlayer()
{
	static uint32_t moduleBase = (uintptr_t)GetModuleHandle("engine.dll");
	return (int*)(*(uint32_t*)(moduleBase + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_MaxPlayer);
}
//Access violation reading location 0x00000138
Vector3* Player::GetOrigin()
{
	std::cout << this;
	return (Vector3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecOrigin);
}

Vector3* Player::GetViewOffset()
{
	return (Vector3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecViewOffset);
}

Vector3* Player::GetBonePos(int boneID)
{
	uint32_t boneMatrix = *(uint32_t*)(*(uint32_t*)this + hazedumper::netvars::m_dwBoneMatrix);
	static Vector3 bonePos;
	bonePos.x = *(float*)(boneMatrix + 0x30 * boneID + 0x0C);
	bonePos.y = *(float*)(boneMatrix + 0x30 * boneID + 0x1C);
	bonePos.z = *(float*)(boneMatrix + 0x30 * boneID + 0x2C);

	return &bonePos;
}

int* Player::GetHealth()
{
	return (int*)(*(uint32_t*)this + hazedumper::netvars::m_iHealth);
}

int* Player::GetTeam()
{
	return (int*)(*(uint32_t*)this + hazedumper::netvars::m_iTeamNum);
}

Vector3 Subtract(Vector3 src, Vector3 dst)
{
	Vector3 diff;
	diff.x = src.x - dst.x;
	diff.y = src.y - dst.y;
	diff.z = src.z - dst.z;
	return diff;
}

float Magnitude(Vector3 vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

float Distance(Vector3 src, Vector3 dst)
{
	Vector3 diff = Subtract(src, dst);
	return Magnitude(diff);
}

Vector3 Aimbot::CalcAngle(Vector3 src, Vector3* dest)
{
	Vector3 angles;
	Vector3 dst = *(dest);
	angles.x = ((float)atan2(dst.x - src.x, dst.y - src.y)) / PI * 180.0f;
	angles.y = asinf((dst.z - src.z) / Distance(src, dst)) * 180.0f / PI;
	angles.z = 0.0f;

	return angles;
}

Player* Aimbot::GetBestTarget()
{
	float oldDistance = FLT_MAX;
	float newDistance = 0;
	Player* target = nullptr;
	LocalPlayer* localPlayer = LocalPlayer::Get();
	int local = *Player::GetMaxPlayer();
	for (int i = 1; i < local; i++)
	{
		Player* currentPlayer = Player::GetPlayer(i);
		std::cout << currentPlayer;
		/*if (currentPlayer && currentPlayer != (Player*)LocalPlayer::Get() && *currentPlayer->GetHealth() > 0)
		{

			Vector3 angleTo = CalcAngle(localPlayer->GetOrigin(), currentPlayer->GetOrigin());
			newDistance = Distance(*localPlayer->GetViewOffset(), angleTo);
			if (newDistance < oldDistance)
			{
				oldDistance = newDistance;
				target = currentPlayer;
			}
		}*/

		if (!currentPlayer || !(*(uint32_t*)currentPlayer) || (uint32_t)currentPlayer == (uint32_t)localPlayer)
		{
			continue;
		}
		if (*currentPlayer->GetTeam() == *localPlayer->GetTeam())
		{
			continue;
		}
		if (*currentPlayer->GetHealth() < 1 || *localPlayer->GetHealth() < 1)
		{
			continue;
		}

		Vector3 angleTo = CalcAngle(localPlayer->GetOrigin(), currentPlayer->GetOrigin());
		newDistance = Distance(*localPlayer->GetViewOffset(), angleTo);
		if (newDistance < oldDistance)
		{
			oldDistance = newDistance;
			target = currentPlayer;
		}
	}
	return target;
}

Player* Aimbot::GetClosestEnemy()
{
	LocalPlayer* localPlayer = LocalPlayer::Get();

	float closestDistance = 1000000;
	int closestDistanceIndex = 1;
	for (int i = 1; i < *Player::GetMaxPlayer(); i++)
	{
		Player* currentPlayer = Player::GetPlayer(i);
		if (!currentPlayer || !(*(uint32_t*)currentPlayer) || (uint32_t)currentPlayer == (uint32_t)localPlayer)
		{
			continue;
		}
		if (*currentPlayer->GetTeam() == *localPlayer->GetTeam())
		{
			continue;
		}
		if (*currentPlayer->GetHealth() < 1 || *localPlayer->GetHealth() < 1)
		{
			continue;
		}
		float currentDistance = localPlayer->GetDistance(currentPlayer->GetOrigin());
		if (currentDistance < closestDistance)
		{
			closestDistance = currentDistance;
			closestDistanceIndex = i;
		}
	}

	if (closestDistanceIndex == -1)
	{
		return NULL;
	}
	return Player::GetPlayer(closestDistanceIndex);
}

void Aimbot::RunAimbot()
{
	Player* closestEnemy = GetClosestEnemy();
	if (closestEnemy)
	{
		LocalPlayer::Get()->AimAt(closestEnemy->GetBonePos(8));
	}
	/*Player* bestTarget = GetBestTarget();
	if (bestTarget)
	{
		LocalPlayer::Get()->AimAt(bestTarget->GetBonePos(8));
	}*/
}

