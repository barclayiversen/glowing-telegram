#pragma warning(disable : 4996)

#include "includes.h"
#include <sstream>



Hack::~Hack()
{
	this->FontF->Release();
	this->LineL->Release();
}

void Hack::Init() 
{
	client = (uintptr_t)GetModuleHandle("client.dll");
	engine = (uintptr_t)GetModuleHandle("engine.dll");
	entList = (EntList*)(client + dwEntityList);
	
	uintptr_t localPlayer = *(uintptr_t*)(client + hazedumper::signatures::dwLocalPlayer);

	for (int i = 0; i < 32; i++)
	{
		uintptr_t curEnt = (uintptr_t)entList->ents[i].ent;
		if (curEnt == localPlayer)
		{
			localEnt = entList->ents[i].ent;
			break;
		}
	}

}

void Hack::GetLocalEnt()
{
	client = (uintptr_t)GetModuleHandle("client.dll");
	entList = (EntList*)(client + dwEntityList);
	
	uintptr_t localPlayer = *(uintptr_t*)(client + hazedumper::signatures::dwLocalPlayer);

	for (int i = 0; i < 32; i++)
	{
		uintptr_t curEnt = (uintptr_t)entList->ents[i].ent;
		if (curEnt == localPlayer)
		{
			localEnt = entList->ents[i].ent;
			break;
		}
	}
}

void Hack::Update()
{
	memcpy(&viewMatrix, (PBYTE*)(client + dwViewMatrix), sizeof(viewMatrix));
	this->CheckButtons();

}

void Hack::CheckButtons() {
	if (GetAsyncKeyState(button.showMenuBtn) & 1)
		settings.showMenu = !settings.showMenu;
	if (GetAsyncKeyState(button.showTeamatesBtn) & 1)
		settings.showTeamates = !settings.showTeamates;
	if (GetAsyncKeyState(button.snapLinesBtn) & 1)
		settings.snapLines = !settings.snapLines;

	if (GetAsyncKeyState(button.box2DBtn) & 1)
		settings.box2D = !settings.box2D;

//	if (GetAsyncKeyState(button.status2DBtn) & 1)
//		settings.status2D = !settings.status2D;
	if (GetAsyncKeyState(button.aimbotBtn) & 1)
		settings.aimbot = !settings.aimbot;
	
	if (GetAsyncKeyState(button.statusTextBtn) & 1)
		settings.statusText = !settings.statusText;

	if (GetAsyncKeyState(button.box3DBtn) & 1)
		settings.box3D = !settings.box3D;

	if (GetAsyncKeyState(button.velEspBtn) & 1)
		settings.velEsp = !settings.velEsp;

	if (GetAsyncKeyState(button.headlineEspBtn) & 1)
		settings.headlineEsp = !settings.headlineEsp;

	if (GetAsyncKeyState(button.rcsCrosshairBtn) & 1)
		settings.rcsCrosshair = !settings.rcsCrosshair;
}

bool Hack::CheckValidEnt(Ent* ent) {
	if (ent == nullptr)
	{
		return false;
	}

	if (ent == localEnt)
	{
		return false;
	}
		
	if (ent->iHealth <= 0)
	{
		return false;
	}
		
	if (ent->bDormant)
	{
		return false;
	}
	return true;
}

bool Hack::WorldToScreen(Vec3 pos, Vec2& screen)
{	
	Vec4 clipCoords;
	clipCoords.x = pos.x * viewMatrix[0] + pos.y * viewMatrix[1] + pos.z * viewMatrix[2] + viewMatrix[3];
	clipCoords.y = pos.x * viewMatrix[4] + pos.y * viewMatrix[5] + pos.z * viewMatrix[6] + viewMatrix[7];
	clipCoords.z = pos.x * viewMatrix[8] + pos.y * viewMatrix[9] + pos.z * viewMatrix[10] + viewMatrix[11];
	clipCoords.w = pos.x * viewMatrix[12] + pos.y * viewMatrix[13] + pos.z * viewMatrix[14] + viewMatrix[15];

	if (clipCoords.w < 0.1f)
	{
		return false;

	}

	Vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

Vec3 Hack::GetBonePos(Ent* ent, int bone)
{
	uintptr_t bonePtr = ent->boneMatrix;
	Vec3 bonePos;
	bonePos.x = *(float*)(bonePtr + 0x30 * bone + 0x0C);
	bonePos.y = *(float*)(bonePtr + 0x30 * bone + 0x1C);
	bonePos.z = *(float*)(bonePtr + 0x30 * bone + 0x2C);
	return bonePos;
}

Vec3 Hack::TransformVec(Vec3 src, Vec3 ang, float d)
{
	Vec3 newPos;
	newPos.x = src.x + (cosf(TORAD(ang.y)) * d);
	newPos.y = src.y + (sinf(TORAD(ang.y)) * d);
	newPos.z = src.z + (tanf(TORAD(ang.z)) * d);

	return newPos;
}

Player* Hack::GetClosestEnemy()
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

void Hack::RunAimbot()
{
	Player* closestEnemy = GetClosestEnemy();

	if (closestEnemy)
	{
		LocalPlayer::Get()->AimAt(closestEnemy->GetBonePos(8));
	}
}