#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#ifdef UNICODE
#undef UNICODE
#endif
//#include "CssOffsets.h"
#include "HackProcess.h"
#include <vector>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <chrono>
#include <time.h>
#include <random>
#include <D3DX10math.h>
#include <thread>


CHackProcess fProcess;
using namespace std;
typedef std::chrono::system_clock Clock;


#define F1_KEY 0x70
#define F6_KEY 0x75
#define LEFT_MOUSE 0x1
#define RIGHT_MOUSE 0x2
#define MIDDLE_MOUSE 0x4
#define VK_LCONTROL 0xA2
#define VK_KEY_C 0x43
#define F9_KEY 0x78



//const int cAddNumOfPlayers = 0x50F500;

int windowsWidth = 2560;
int windowsHeight = 1440;

std::chrono::milliseconds ticks;


//const DWORD dw_initModuleUnityOffset = 0x03DF2308;

DWORD EngineRotation = 0;


//Player
const DWORD dwLocalPlayer = 0xDA746C;  //  0x109B74     0x10F4F4   // 0x4C6708 client 0x53FB04
const DWORD dwEntityList = 0x4DC179C;
const DWORD dwRotationWritebleEngine = 0x00589FCC; // "engine.dll"+00589FCC


//const DWORD dw_teamOffset = 0x32C; // 0x98 dw_health+8
const DWORD dw_health = 0x100;     //0x90
//const DWORD dw_armor = 0xFC;     //0x90
//const DWORD dw_name = 0x225;     //0x225
const DWORD dw_isReloading = 0x32B5;
const DWORD dw_isDefusing = 0x997C;
const DWORD dw_isScoped = 0x9974;
const DWORD dw_isBombOwner = 0x997C;

const DWORD dw_team = 0xF4;
const DWORD dw_LifeState = 0x25F; // >0 - dead // 0 - alive

// Readeble
const DWORD dw_pos = 0xA0;       //0xA0 & 0xAC offsets for CSGO
//Writeble engine.dll+00589FCC + 0x4D90
const DWORD dw_EngineRotation = 0x4D90;
const DWORD dw_bDormant = 0xED;

const DWORD dw_angRotation = 0x12C;  // 12C, 130
const DWORD dw_HeadOffset = 0x110;
const DWORD dw_vecSpeed = 0x108; //0x108 & 0x110
const DWORD dw_zoomLevel = 0x33E0;

const DWORD dw_winWidth = 0x110C94; //TODO: find it!
const DWORD dw_winHeight = 0x110C98; //TODO: find it!




//ESP Vars 
/**/
/*const*/ DWORD dw_vMatrix = 0x4DB30A4;




DWORD viewMatrix = 0x501AE8;

const DWORD NumOfPlayers = 20;

HDC HDC_Desktop;
HBRUSH EnemyBrush; //Brush to paint ESP etc
HFONT Font;
RECT m_Rect;




HWND TargetWnd;
HWND Handle;
DWORD DwProcId;

COLORREF SnapLineCOLOR;
COLORREF TextCOLOR;


typedef struct
{
	float flMatrix[16];
	//float flMatrix[4][4];
}WorldToScreenMatrix_t;

struct PlayerList_t
{
	DWORD CBaseEntity = 0;
	DWORD Player = 0;
	DWORD Health = 0;
	DWORD Team = 0;
	DWORD isAlive = 0;
	DWORD isReloading = 0;
	DWORD isDefusing = 0;
	DWORD isScoped = 0;
	DWORD isBombOwner = 0;

	bool isValid = false;
	bool bDormant = false;

	char Name[16];

	float Position[3];
	float AimbotAngle[2];
	float Rotation[2];
	float HeadOffset = 0;

	bool ReadInformation(int player)
	{
		

		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + dwEntityList + (player * 0x10)), //(player * EntityLoopDistance)
			&CBaseEntity, sizeof(DWORD), 0);		
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_health), 
			&Health, sizeof(DWORD), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_pos), 
			&Position, sizeof(float[3]), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_angRotation), 
			&Rotation, sizeof(float[2]), 0);	
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_HeadOffset), 
			&HeadOffset, sizeof(float), 0);
		

		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_LifeState), 
			&isAlive, sizeof(DWORD), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_isBombOwner), 
			&isBombOwner, sizeof(DWORD), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_isDefusing), 
			&isDefusing, sizeof(DWORD), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_isReloading), 
			&isReloading, sizeof(DWORD), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_isScoped), 
			&isScoped, sizeof(DWORD), 0);

		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_bDormant), 
			&bDormant, sizeof(bool), 0);



		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_team),
			&Team, sizeof(DWORD), 0);
		

		if ( (CBaseEntity)
			&& (Team > 1 && Team < 4)
			&& (Health > 0 && Health <= 100)
			&& (HeadOffset > 0.f && HeadOffset < 100.f)			
			&& (!isAlive)
			)isValid = true;


#ifdef _DEBUG

		if (ticks.count() % 144 <= 1)
		{
			std::cout << "CBaseEntity: " << std::hex << "0x" << CBaseEntity << std::endl;
			std::cout << "Player Name: " << Name << std::endl;
			std::cout << "Team: " << std::dec << Team << std::endl;
			std::cout << "isBombOwner: " << std::dec << isBombOwner << std::endl;
			std::cout << "isScoped: " << std::dec << isScoped << std::endl;
			std::cout << "isDefusing: " << std::dec << isDefusing << std::endl;
			std::cout << "isReloading: " << std::dec << isReloading << std::endl;
			std::cout << "isScoped: " << std::dec << isScoped << std::endl;
			std::cout << "Health: " << std::dec << Health << std::endl;
			std::cout << "HeadOffset: " << std::dec << HeadOffset << std::endl;
			std::cout << "isValid: " << std::dec << isValid << std::endl;
			std::cout << "Position: ";
			for (int i = 0; i < 3; i++) std::cout << " " << Position[i];
			std::cout << std::endl << std::endl;
		}		
#endif
	
		return isValid;
	}

}PlayerList[NumOfPlayers];

struct MyPlayer_t
{
	DWORD CLocalPlayer = 0;
	
	DWORD Team = 0;
	DWORD Health = 0;
	DWORD Armor = 0;

	DWORD MainGunAmmo = 0;
	DWORD SecondGunAmmo = 0;

	DWORD zoomLevel = 0;


	float Position[3];
	float Rotation[2];

	WorldToScreenMatrix_t ReversedWorldToScreenMatrix;
	WorldToScreenMatrix_t WorldToScreenMatrix;

	
	WorldToScreenMatrix_t reverseMatrix(WorldToScreenMatrix_t curMatrix)
	{
		WorldToScreenMatrix_t revMatrix;


		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				revMatrix.flMatrix[x * y + x] = revMatrix.flMatrix[x * y + x];
			}
		}



		return revMatrix;

	}
	


	void ReadInformation()
	{

		
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + dwLocalPlayer), //(player * EntityLoopDistance)
			&CLocalPlayer, sizeof(DWORD), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_health),
			&Health, sizeof(DWORD), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_pos),
			&Position, sizeof(float[3]), 0);

		
		 
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dwRotationWritebleEngine),
			&EngineRotation, sizeof(DWORD), 0);

		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(EngineRotation + dw_EngineRotation),
			&Rotation, sizeof(float[2]), 0);

		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_team),
			&Team, sizeof(DWORD), 0);

		/*
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_zoomLevel),
			&zoomLevel, sizeof(DWORD), 0);
		*/

		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + dw_vMatrix),
			&WorldToScreenMatrix.flMatrix, sizeof(float[16]), 0);


		
		auto now = Clock::now();
		auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
		auto fraction = now - seconds;
		time_t cnow = Clock::to_time_t(now);
		ticks = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);


#ifdef _DEBUG
		if (ticks.count() % 144  <= 1)
		{
			std::cout << "CLocalPlayer:\t" << std::hex << CLocalPlayer << "-> &0x" << std::hex << &CLocalPlayer << std::endl;
			std::cout << "Health: \t" << std::dec << Health << "-> &0x" << std::hex << &Health << std::endl;

			for (int i = 0; i < 3; i++)
			{
				std::cout << "Position[" << i << "]:\t" << Position[i] << "-> &0x" << std::hex << &Position << std::endl;
			}

			for (int i = 0; i < 2; i++)
			{
				std::cout << "Rotation[" << i << "]:\t" << Rotation[i] << "-> &0x" << std::hex << &Rotation << std::endl;
			}
			
		}
#endif
				

		float aimbotAngle[3] = { 0,0,0 };


		//Sleep(0);
	}

}MyPlayer;


struct TargetList_t
{
	float Distance = 0;
	float AimbotAngle[2] = { 0,0 };

	TargetList_t()
	{
		Distance = -1.0f;
	}

	TargetList_t(float _AimbotAngle[2], const float  MyCoords[3], const float EnemyCoords[3])
	{
		Distance = -1.0f;
		Distance = Get3dDistance(MyCoords[0], MyCoords[1], MyCoords[2],
			EnemyCoords[0], EnemyCoords[1], EnemyCoords[2]);

		AimbotAngle[0] = _AimbotAngle[0];
		AimbotAngle[1] = _AimbotAngle[1];
		//AimbotAngle[2] = _AimbotAngle[2];

	}

	float Get3dDistance(float myCoordsX, float myCoordsY, float myCoordsZ,
		float enX, float enY, float enZ)
	{
		return sqrt(pow(double(enX - myCoordsX), 2.0f) +
			pow(double(enY - myCoordsY), 2.0f) +
			pow(double(enZ - myCoordsZ), 2.0f));
	}


}; // for aimbot

struct CompareTargetEnArray
{
	bool operator()(TargetList_t& lhs, TargetList_t& rhs)
	{
		return lhs.Distance < rhs.Distance;
	}
};

//Calculate distance between two coordinates in 3d space
float Get3dDistance(float* myCoords, float* enemyCoords)
{
	return sqrt(
		pow(double((double)enemyCoords[0] - (double)myCoords[0]), 2.0) +
		pow(double((double)enemyCoords[1] - (double)myCoords[1]), 2.0) +
		pow(double((double)enemyCoords[2] - (double)myCoords[2]), 2.0)
	);

}
void SetupDrawing(HDC hDesktop, HWND handle)
{
	HDC_Desktop = hDesktop;
	Handle = handle;
	EnemyBrush = CreateSolidBrush(RGB(255, 0, 0));
	SnapLineCOLOR = RGB(0, 0, 255);
	TextCOLOR = RGB(0, 255, 0);
}
//We will use this struct throughout all other tutorials adding more variables every time

void DrawFilledRect(int x, int y, int w, int h)
{
	RECT rect = { x, y, x + w, y + h };
	FillRect(HDC_Desktop, &rect, EnemyBrush);
}

void DrawFilledRect(int x, int y, int w, int h, HBRUSH brush)
{
	RECT rect = { x, y, x + w, y + h };
	FillRect(HDC_Desktop, &rect, brush);
}

void DrawBorderBox(int x, int y, int w, int h, int thickness)
{
	DrawFilledRect(x, y, w, thickness); //Top horiz line
	DrawFilledRect(x, y, thickness, h); //Left vertical line
	DrawFilledRect((x + w), y, thickness, h); //right vertical line
	DrawFilledRect(x, y + h, w + thickness, thickness); //bottom horiz line
}

void DrawBorderBox(int x, int y, int w, int h, int thickness, HBRUSH brush)
{
	DrawFilledRect(x, y, w, thickness, brush ); //Top horiz line
	DrawFilledRect(x, y, thickness, h, brush); //Left vertical line
	DrawFilledRect((x + w), y, thickness, h, brush); //right vertical line
	DrawFilledRect(x, y + h, w + thickness, thickness, brush); //bottom horiz line
}

void DrawLine(float StartX, float StartY, float EndX, float EndY, COLORREF Pen)
{
	int a, b = 0;
	HPEN hOPen;
	HPEN hNPen = CreatePen(PS_SOLID, 2, Pen);// penstyle, width, color
	hOPen = (HPEN)SelectObject(HDC_Desktop, hNPen);
	MoveToEx(HDC_Desktop, StartX, StartY, NULL); //start
	a = LineTo(HDC_Desktop, EndX, EndY); //end
	DeleteObject(SelectObject(HDC_Desktop, hOPen));
}
void DrawString(int x, int y, COLORREF color, const char* text)
{
	SetTextAlign(HDC_Desktop, TA_CENTER | TA_NOUPDATECP); //TA_CENTER | TA_NOUPDATECP);
	SetBkColor(HDC_Desktop, RGB(0, 0, 0));
	SetBkMode(HDC_Desktop, 2);
	SetTextColor(HDC_Desktop, color);

	//Font = CreateFontA(20, 8, 0, 0, 600, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, 0);

	SelectObject(HDC_Desktop, Font);


	TextOutA(HDC_Desktop, x, y, text, strlen(text));
	DeleteObject(Font);
}

bool WorldToScreen(D3DXVECTOR3 pos, D3DXVECTOR2& screen, int windowsWidth, int windowsHeight)
{


	
	//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
	D3DXVECTOR4 clipCoords;

	clipCoords.x = pos.x * MyPlayer.WorldToScreenMatrix.flMatrix[0] + pos.y * MyPlayer.WorldToScreenMatrix.flMatrix[1] + pos.z * MyPlayer.WorldToScreenMatrix.flMatrix[2] + MyPlayer.WorldToScreenMatrix.flMatrix[3];
	clipCoords.y = pos.x * MyPlayer.WorldToScreenMatrix.flMatrix[4] + pos.y * MyPlayer.WorldToScreenMatrix.flMatrix[5] + pos.z * MyPlayer.WorldToScreenMatrix.flMatrix[6] + MyPlayer.WorldToScreenMatrix.flMatrix[7];
	clipCoords.z = pos.x * MyPlayer.WorldToScreenMatrix.flMatrix[8] + pos.y * MyPlayer.WorldToScreenMatrix.flMatrix[9] + pos.z * MyPlayer.WorldToScreenMatrix.flMatrix[10] + MyPlayer.WorldToScreenMatrix.flMatrix[11];
	clipCoords.w = pos.x * MyPlayer.WorldToScreenMatrix.flMatrix[12] + pos.y * MyPlayer.WorldToScreenMatrix.flMatrix[13] + pos.z * MyPlayer.WorldToScreenMatrix.flMatrix[14] + MyPlayer.WorldToScreenMatrix.flMatrix[15];



	
	if (clipCoords.w < 0.1f)
		return false;
	

	//perspective division, dividing by clip.W = Normalized Device Coordinates
	D3DXVECTOR3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;


	screen.x = ((float)windowsWidth / 2 * NDC.x) + (NDC.x + (float)windowsWidth / 2);
	screen.y = -((float)windowsHeight / 2 * NDC.y) + (NDC.y + (float)windowsHeight / 2);

	
	
	/*
	if (screen.x <= -(windowsWidth* 2) || screen.x >= windowsWidth*2 || screen.y <= -(windowsHeight * 2) || screen.y >= windowsHeight * 2)
		return false;

	*/

	return true;
}

void DrawESP(int x, int y, float distance, int enemyIndex)
{
	//ESP RECTANGLE
	char* name = PlayerList[enemyIndex].Name;
	float* fpos = PlayerList[enemyIndex].Position;
	bool isAlive = PlayerList[enemyIndex].isAlive;
	bool isScoped = PlayerList[enemyIndex].isScoped;
	bool isDefusing = PlayerList[enemyIndex].isDefusing;
	bool isReloading = PlayerList[enemyIndex].isReloading;
	bool isBombOwner = PlayerList[enemyIndex].isBombOwner;
	float headOffset = PlayerList[enemyIndex].HeadOffset;

	int healh = PlayerList[enemyIndex].Health;

	distance /= 14;

	int width = 0;
	int height = 0;

	//if (GetAsyncKeyState(0x10) || GetAsyncKeyState(0x11) || GetAsyncKeyState(0x12)) distance /= 8;

	width = (float)2260 / (float)1920 * (float)windowsWidth / distance;  // 1280
	height = (float)3600 / (float)1080 * (float)windowsHeight / distance; // 8005


	
	if (headOffset > 60)
	{
		DrawBorderBox(x - (width / 2), y - height, width, height, 1);
	}
	else
	{
		DrawBorderBox(x - (width / 2), y - height, width, height/(64/40), 1);
	}

		


	
	/*
	DrawBorderBox(x - (width / 2), y - height - 30, width, 6, 8);
	DrawBorderBox(x - (width / 2), y - height - 30, width, 6, 6, (HBRUSH)CreateSolidBrush(RGB(0, 255, 0)));
	*/


	 
	
	//DrawBorderBox(x - (width / 2), y - height, width, height, 1); //ESP CODE

	//Sandwich ++
	//DrawLine((m_Rect.right - m_Rect.left) / 2, m_Rect.bottom - m_Rect.top, x, y, SnapLineCOLOR);

	{
		std::stringstream ss;
		ss << healh;
		char* printInfo = new char[ss.str().size() + 1];
		strcpy(printInfo, ss.str().c_str());
		DrawString(x, y - 30, TextCOLOR, printInfo);
		delete[] printInfo;

	}

	for (int i = 0; i < 5; i++)
	{

		std::stringstream ss;

		int offsetX = 0;
		int offsetY = i*12;
		bool bNotBreak = true;
		switch (i)
		{
		case 1:
			ss << name;
			break;		
		case 2:
			if (isScoped) ss << "Scoped";
			break;
		case 3: 
			if (isDefusing) ss << "Defusing";
			break;
		case 4:
			if (isBombOwner) ss << "Bomb";
			break;
		case 5:
			if (isReloading) ss << "Reload";
			break;
		}

		if (bNotBreak)
		{
			char* printInfo = new char[ss.str().size() + 1];
			strcpy(printInfo, ss.str().c_str());
			DrawString(x, y + offsetY, TextCOLOR, printInfo);
			delete[] printInfo;
		}
		
		
	}


	/*
	for (int i = 0; i < 3; i++)
	{
		std::stringstream sa;
		sa << (float)fpos[i];
		char* pos = new char[sa.str().size() + 1];
		strcpy(pos, sa.str().c_str());

		DrawString(x, y + 80 + i * 20, TextCOLOR, pos);
		delete[] pos;
	}


	DrawString(x, y - 30, TextCOLOR, name);
	*/


	
	
}

void ESP()
{
	GetWindowRect(FindWindow(NULL, "Counter-Strike: Global Offensive"), &m_Rect);

	for (int i = 0; i < NumOfPlayers; i++)
	{
		PlayerList[i].ReadInformation(i);

		if (PlayerList[i].isAlive > 0)	continue;
		if (PlayerList[i].Team == MyPlayer.Team ||( PlayerList[i].Team != 2 && PlayerList[i].Team !=3)) continue;
		if (PlayerList[i].bDormant) continue;


		D3DXVECTOR2 screen = { 0,0 };

		if (WorldToScreen(PlayerList[i].Position, screen, windowsWidth, windowsHeight))
		{
			DrawESP(screen.x, screen.y, Get3dDistance(MyPlayer.Position, PlayerList[i].Position), i);
		}

	}
}





void CalcAngles(const float* src, const float* dist, float* angles);
int targetLoop = 0;



void DrawPlayerInfo()
{
	

	std::stringstream ss;
	ss << "Health: " << MyPlayer.Health;
	char* Health = new char[ss.str().size() + 1];
	strcpy(Health, ss.str().c_str());
	DrawString(1800, 1000, TextCOLOR, Health);
	delete[] Health;

	for (int i = 0; i < 3; i++)
	{
		std::stringstream ss1;
		ss1 << "Pos[" << i << "]: " << MyPlayer.Position[i];
		char* Pos = new char[ss1.str().size() + 1];
		strcpy(Pos, ss1.str().c_str());
		DrawString(1800, 1040+i*30, TextCOLOR, Pos);
		delete[] Pos;
	}

	for (int i = 0; i < 2; i++)
	{
		std::stringstream ss1;
		ss1 << "Rot[" << i << "]: " << MyPlayer.Rotation[i];
		char* Rot = new char[ss1.str().size() + 1];
		strcpy(Rot, ss1.str().c_str());
		DrawString(1800, 1130 + i * 30, TextCOLOR, Rot);
		delete[] Rot;
	}


	Sleep(0);

}


void CalcAngles(const float* src, const float* dist, float* angles, int targetIndex)
{

	if (src[0] == dist[0] && src[1] == dist[1] && src[2] == dist[2])
	{
		angles[0] = 0;
		angles[1] = 0;
		return;

	}
	
	double delta[3] = {(dist[0] - src[0]), (src[1] - dist[1]), (src[2] - dist[2]) };	
	//delta[2] -= 1.5;



	//
	double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);	


	angles[0] = (float)((asin(delta[2] / hyp)) * 57.295779513085f);     // pitch
	hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);


	
	//angles[1] = (float)(90-(acosf(delta[0] / hyp) * 57.295779513085f));  // yaw
	
	float rotX = PlayerList[targetIndex].Rotation[1];
	rotX -= 180.f;

	float sinus = (5.f * (sinf(90.f - rotX)));
	
	/*
	if (delta[1] >= 0.0f)
	{
		delta[1] += (float)(5.f * (sinf(90.f - rotX)));
	}
	else
	{
		delta[1] += (float)(5.f * (sinf(90.f + rotX)));
	}
	*/

	if (delta[0] >= 0.0f )  // yaw
	{	
		//delta[0] += (float)(5.f * (cosf(90.f + rotX)));
		
		
		angles[1] = (float)(-90.f + (acosf(delta[1] / hyp) * 57.295779513085f));

	}	
	else 
	{		
		//delta[0] += (float)(5.f * (cosf(90.f + rotX)));
		//delta[1] += (float)(5.f * (sinf(90.f + rotX)));
		angles[1] = (float)(-90.f - (acosf(delta[1] / hyp) * 57.295779513085f));

	}
	

	if (angles[1] > 180.f || angles[1] < -180.f) angles[1] == 50.0f;
	if (angles[0] > 89.f || angles[0] < -89.f) angles[0] == 50.0f;


	
	
	


	///////


	//angles[1] += (float)(acosf(delta[1]))


	///////
	

	/*
	double delta[3] = { (dist[0] - src[0]), (src[1] - dist[1]), (src[2] - dist[2]) };
	double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
	angles[1] = (float)(asinf(delta[2] / hyp) * 57.295779513085f);     // pitch

	hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);


	if (delta[1] >= 0.0f)
	{
		angles[0] = (float)(90 - (acosf(delta[0] / hyp) * 57.295779513085f));
	}
	else
	{
		angles[0] = (float)(90 + (acosf(delta[0] / hyp) * 57.295779513085f));
	}
	*/

	//angles[2] = 0;


}

void Aimbot()
{
	
	auto now = Clock::now();
	auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	auto fraction = now - seconds;
	time_t cnow = Clock::to_time_t(now);

	auto ticks2 = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);


	

	TargetList_t* TargetList = new TargetList_t[NumOfPlayers];



	for (int i = 0; i < NumOfPlayers; i++)
	{
		//if(!PlayerList[i].ReadInformation(i)) continue;
		//if (PlayerList[i].bDormant || PlayerList[i].Health < 1 || PlayerList[i].Health > 100)
		
			
		

		CalcAngles(MyPlayer.Position, PlayerList[i].Position, PlayerList[i].AimbotAngle,i);

		TargetList[i] = TargetList_t(PlayerList[i].AimbotAngle, MyPlayer.Position, PlayerList[i].Position);

	}

	if (PlayerList[targetLoop].Team == MyPlayer.Team || PlayerList[targetLoop].bDormant || PlayerList[targetLoop].Health < 1 || PlayerList[targetLoop].Health > 100)
	{
		targetLoop++;
		if (targetLoop > NumOfPlayers )
		{
			targetLoop = 0;
		}
	}

	

	int targetLoop2 = 0;
	bool bAssepted = true;
	
	while (GetAsyncKeyState(VK_KEY_C) & 1)
	{
		
		
		targetLoop2++;

		
		if (targetLoop2 >= NumOfPlayers)
		{
			bAssepted = false;
			break;
		}

		if (PlayerList[targetLoop].isValid && TargetList[targetLoop].Distance > 0.05f)
		{
			bAssepted = true;
			break; 
		}
		
		
		
	}
	
	
	{
		

#ifdef _DEBUG

		if (ticks.count() % 60 == 0)
		{
			std::cout << "Target: " << "PlayerList[" << targetLoop << "]=" << PlayerList[targetLoop].Health << std::endl;

			for (int i = 0; i < 3; i++)
			{
				std::cout << "PlayerList[" << targetLoop << "][" << i << "]:\t" << PlayerList[targetLoop].Position[i] << std::endl;
			}

			for (int i = 0; i < 2; i++)
			{
				std::cout << "PlayerList[" << targetLoop << "][" << i << "]:\t" << PlayerList[targetLoop].Rotation[i] << std::endl;
			}
			std::cout << std::endl << std::endl;
		}
#endif




	}
	if (GetAsyncKeyState(MIDDLE_MOUSE))
	{
		bool bAssepted2 = true;

		
		
		if (TargetList[targetLoop].Distance <= 0.1f)
			bAssepted2 = false;
		if (TargetList[targetLoop].AimbotAngle[0] <= -90.0f && TargetList[targetLoop].AimbotAngle[0] >= 90.0f)
			bAssepted2 = false;
		if (TargetList[targetLoop].AimbotAngle[1] <= -180.0f && TargetList[targetLoop].AimbotAngle[1] >= 180.0f)
			bAssepted2 = false;

		if (bAssepted && bAssepted2)
		{
			


			WriteProcessMemory(fProcess.__HandleProcess,
				(PBYTE)(EngineRotation + 0x4D90),
				TargetList[targetLoop].AimbotAngle, sizeof(float[2]), 0);
		}
		
	}


	delete[] TargetList;
	
}

void SearchMatrix()
{

	static time_t nextTimeCheck;
	auto now = Clock::now();
	auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	auto fraction = now - seconds;
	time_t cnow = Clock::to_time_t(now);

	auto ticks2 = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);
	auto dticks = ticks2.count() - ticks.count();

	if (dticks < 0)
		dticks *= -1;

	bool bPlusPressed = GetAsyncKeyState(VK_OEM_PLUS);
	bool bMisusPressed = GetAsyncKeyState(VK_OEM_MINUS);


	if (nextTimeCheck > cnow || (bPlusPressed && bMisusPressed))
		return;

	if (bPlusPressed)
	{
		dw_vMatrix += 4;
		std::cout << "0x" << std::hex << dw_vMatrix << std::endl;
	}
	if (bMisusPressed)
	{
		dw_vMatrix -= 4;
		std::cout << "0x" << std::hex <<dw_vMatrix << std::endl;
	}


	nextTimeCheck = cnow + 1;



}


void parallel_task1()
{
	while (!(GetAsyncKeyState(F1_KEY) && GetAsyncKeyState(VK_LCONTROL)))
	{
		DrawPlayerInfo();		
	}
}

void parallel_task2()
{
	while (!(GetAsyncKeyState(F1_KEY) && GetAsyncKeyState(VK_LCONTROL)))
	{
		ESP();
	}
}

int main()
{
	while (!GetAsyncKeyState(F6_KEY))
	{
		fProcess.RunProcess();
		cout << "Game Found! Running ..." << endl;

		ShowWindow(FindWindowA("Counter-Strike: Global Offensive", NULL), false);
		TargetWnd = FindWindow(0, "Counter-Strike: Global Offensive");
		HDC HDC_Desktop = GetDC(TargetWnd);
		SetupDrawing(HDC_Desktop, TargetWnd);


		auto now = Clock::now();
		auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
		auto fraction = now - seconds;
		time_t cnow = Clock::to_time_t(now);
		ticks = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);

#ifdef _DEBUG
		thread thread1(parallel_task1);
		thread1.detach();
#endif // DEBUG



		thread thread2(parallel_task2);
		thread2.detach();

		while (!GetAsyncKeyState(F9_KEY))
		{
			MyPlayer.ReadInformation();
			//ESP();
			Aimbot();

			Sleep(1);
		}

	
		
	}

}
