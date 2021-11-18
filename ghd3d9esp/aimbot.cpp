#include "includes.h"

double PI = 3.14159265358979323846;

LocalPlayer* LocalPlayer::Get()
{
	static uint32_t clientModule = (uint32_t)(GetModuleHandle("client.dll"));
	static LocalPlayer* localPlayer = (LocalPlayer*)(clientModule + hazedumper::signatures::dwLocalPlayer);

	return localPlayer;
}

Vector3* LocalPlayer::GetOrigin()
{
	return (Vector3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecOrigin);
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

float LocalPlayer::GetDistance(Vector3* other)
{
	Vector3* myPos = GetOrigin();
	Vector3 delta = Vector3(other->x - myPos->x, other->y - myPos->y, other->z - myPos->z);

	return sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
}

void LocalPlayer::AimAt(Vector3* target)
{
	static uint32_t engineModule = (uint32_t)GetModuleHandle("engine.dll");
	static Vector3* viewAngles = (Vector3*)(*(uint32_t*)(engineModule + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);

	Vector3 origin = *GetOrigin();
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

Vector3* Player::GetOrigin()
{
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