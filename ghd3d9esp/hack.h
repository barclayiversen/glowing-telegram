#pragma once
#include "csgo.hpp"
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}
#define ABS(x) ((x<0) ? (-x) : (x))
#define TORAD(x) ((x) * 0.01745329252)
#define W2S(x,y) hack->WorldToScreen(x, y)

struct Vec2 {
	float x, y;
};

struct Vec3 {
	float x, y, z;
	Vec3 operator+(Vec3 d)
	{
		return { x + d.x, y + d.y, z + d.z };
	}
	Vec3 operator*(float d)
	{
		return { x * d, y * d, z * d };
	}
};

struct Vec4 {
	float x, y, z, w;
};

class Vector3
{
public:
	float x;
	float y;
	float z;

	Vector3()
	{
		x = y = z = 0;
	}

	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3& operator+(Vector3 arg)
	{
		x += arg.x;
		y += arg.y;
		z += arg.z;
		return *this;
	}
};

class Ent {
public:
	union {
		//isDormant
		DEFINE_MEMBER_N(bool, bDormant, hazedumper::signatures::m_bDormant);
		//iHealth
		DEFINE_MEMBER_N(int, iHealth, hazedumper::netvars::m_iHealth);
		//vecOrigin
		DEFINE_MEMBER_N(Vec3, vecOrigin, hazedumper::netvars::m_vecOrigin);
		//iTeamnum
		DEFINE_MEMBER_N(int, iTeamNum, hazedumper::netvars::m_iTeamNum);
		//BoneMatrix
		DEFINE_MEMBER_N(int, boneMatrix, hazedumper::netvars::m_dwBoneMatrix);
		//armorValue
		DEFINE_MEMBER_N(int, ArmorValue, hazedumper::netvars::m_ArmorValue);
		//aimPunchangle
		DEFINE_MEMBER_N(Vec3, aimPunchAngle, hazedumper::netvars::m_aimPunchAngle);
		//angEyeAnglesX
		DEFINE_MEMBER_N(float, angEyeAnglesX, hazedumper::netvars::m_angEyeAnglesX);
		//angEyeAnglesY
		DEFINE_MEMBER_N(float, angEyeAnglesY, hazedumper::netvars::m_angEyeAnglesY);
		//vecVelocity
		DEFINE_MEMBER_N(Vec3, vecVelocity, hazedumper::netvars::m_vecVelocity);
		//bHasHelmet
		DEFINE_MEMBER_N(bool, bHasHelmet, hazedumper::netvars::m_bHasHelmet);
	};

};

class Player
{
private:
	Player();

public:
	static int* GetMaxPlayer();
	static Player* GetPlayer(int index);

	int* GetHealth();
	int* GetTeam();
	Vector3* GetOrigin();
	Vector3* GetViewOffset();
	Vector3* GetBonePos(int boneID);
};

class LocalPlayer
{
private:
	LocalPlayer();

public:
	static LocalPlayer* Get();

	Vector3* GetOrigin();
	Vector3* GetViewOffset();
	int* GetHealth();
	int* GetTeam();
	void AimAt(Vector3* target);
	float GetDistance(Vector3*);
};

class EntListObj {
public:
	struct Ent* ent;
	char padding[12];
};

class EntList {
public:
	EntListObj ents[32];
};

class Hack {
public:
	uintptr_t dwEntityList = hazedumper::signatures::dwEntityList;
	uintptr_t dwViewMatrix = hazedumper::signatures::dwViewMatrix;

	uintptr_t engine;
	uintptr_t client;

	Ent* localEnt;
	EntList* entList;
	float viewMatrix[16];

	ID3DXLine* LineL;
	ID3DXFont* FontF;

	Vec2 crosshair2D;
	int crosshairSize = 4;

	~Hack();

	void RunAimbot();
	void Init();
	void Update();
	void GetLocalEnt();
	void CheckButtons();
	bool CheckValidEnt(Ent* ent);
	bool WorldToScreen(Vec3 pos, Vec2& screen);
	Vec3 GetBonePos(Ent* ent, int bone);
	Vec3 TransformVec(Vec3 src, Vec3 ang, float d);
	Player* GetClosestEnemy();

	struct Settings {
		bool showMenu = false;
		bool showTeamates = false;
		bool snapLines = false;
		bool box2D = false;
		bool statusText = false;
		//bool status2D = false;
		bool aimbot = false;
		bool box3D = false;
		bool velEsp = false;
		bool headlineEsp = false;
		bool rcsCrosshair = true;
	}settings;

	struct buttons {
		DWORD showMenuBtn = VK_INSERT;
		DWORD showTeamatesBtn = VK_F1;
		DWORD snapLinesBtn =	 VK_F2;
		DWORD box2DBtn =		 VK_F3;
		DWORD statusTextBtn =   VK_F4;
		//DWORD status2DBtn =	 VK_F5;
		DWORD aimbotBtn = VK_F5;
		DWORD box3DBtn =		 VK_F6;
		DWORD velEspBtn =		 VK_F7;
		DWORD headlineEspBtn =  VK_F8;
		DWORD rcsCrosshairBtn = VK_F9;
	}button;

	struct Colors {
		D3DCOLOR health = D3DCOLOR_ARGB(255, 46, 145, 90);
		D3DCOLOR armor = D3DCOLOR_ARGB(255, 35, 145, 250);
		D3DCOLOR crosshair = D3DCOLOR_ARGB(255, 255, 255 ,255);
		struct Team {
			D3DCOLOR esp = D3DCOLOR_ARGB(255, 0, 255, 0);
			D3DCOLOR snapline = D3DCOLOR_ARGB(255, 0, 255, 0);
			D3DCOLOR velocity = D3DCOLOR_ARGB(255, 0, 50, 255);
			D3DCOLOR headline = D3DCOLOR_ARGB(255, 0, 150, 255);
		}team;
		struct Enemy {
			D3DCOLOR esp = D3DCOLOR_ARGB(255, 255, 0, 0);
			D3DCOLOR snapline = D3DCOLOR_ARGB(255, 255, 0, 0);
			D3DCOLOR velocity = D3DCOLOR_ARGB(255, 0, 255, 255);
			D3DCOLOR headline = D3DCOLOR_ARGB(255, 0, 255, 255);
		}enemy;
	}color;
};
