#include <Windows.h>
#include <sstream>
#include "HackProcess.h"
#include <math.h>

//Create our 'hooking' and process managing object
CHackProcess fProcess;
using namespace std;
#define F6_Key 0x75
#define RIGHT_MOUSE 0x02

//Here we store the num of players and update it regularly to know how many enemies we are dealing with
int NumOfPlayers = 32;
const DWORD dw_PlayerCountOffs = 0x5EF6BC;//Engine.dll

const DWORD Player_Base = 0x4C6708; //LocalPlayer
const DWORD dwClientId = 0x4c;
const DWORD dw_Health = 0x94;//client
const DWORD dw_mTeamOffset = 0x9C;//client
const DWORD dw_Pos = 0x260;//client
const DWORD dwAngle = 0x26c;
const DWORD EntityPlayer_Base = 0x4D3904; //entitylist
const DWORD EntityLoopDistance = 0x10;

//Set of initial variables you'll need
HDC HDC_Desktop;
HBRUSH EnemyBrush; //Brush to paint ESP etc
HFONT Font;
RECT m_Rect;

//We will use this struct throughout all other tutorials adding more variables every time
struct MyPlayer_t
{
	DWORD CLocalPlayer;
	int Team;
	int Health;
	//WorldToScreenMatrix_t WorldToScreenMatrix;
	float Position[3];
	void ReadInformation()
	{
		// Reading CLocalPlayer Pointer to our "CLocalPlayer" DWORD.
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + Player_Base), &CLocalPlayer, sizeof(DWORD), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_mTeamOffset), &Team, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_Health), &Health, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_Pos), &Position, sizeof(float[3]), 0);
		//get number of players
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_PlayerCountOffs), &NumOfPlayers, sizeof(int), 0);
	}
}MyPlayer;

//ENemy struct
struct PlayerList_t
{
	DWORD CBaseEntity;
	int Team;
	int Health;
	float Position[3];
	float AimbotAngle[3];
	char Name[39];

	void ReadInformation(int Player)
	{
		// Reading CBaseEntity Pointer to our "CBaseEntity" DWORD + Current Player in the loop. 0x10 is the CBaseEntity List Size
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + EntityPlayer_Base + (Player * EntityLoopDistance)), &CBaseEntity, sizeof(DWORD), 0);
		// Reading out our Team to our "Team" Varible.
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_mTeamOffset), &Team, sizeof(int), 0);
		// Reading out our Health to our "Health" Varible.
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_Health), &Health, sizeof(int), 0);
		// Reading out our Position to our "Position" Varible.
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_Pos), &Position, sizeof(float[3]), 0);
	}
}PlayerList[32];

void DrawFilledRect(int x, int y, int w, int h)
{
	RECT rect = { x, y, x + w, y + h };
	FillRect(HDC_Desktop, &rect, EnemyBrush);
}

void DrawBorderBox(int x, int y, int w, int h, int thickness)
{
	DrawFilledRect(x, y, w, thickness); //Top horiz line
	DrawFilledRect(x, y, thickness, h); //Left vertical line
	DrawFilledRect((x + w), y, thickness, h); //right vertical line
	DrawFilledRect(x, y + h, w + thickness, thickness); //bottom horiz line
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
	SetTextAlign(HDC_Desktop, TA_CENTER | TA_NOUPDATECP);
	SetBkColor(HDC_Desktop, RGB(0, 0, 0));
	SetBkMode(HDC_Desktop, TRANSPARENT);
	SetTextColor(HDC_Desktop, color);
	SelectObject(HDC_Desktop, Font);
	TextOutA(HDC_Desktop, x, y, text, strlen(text));
	DeleteObject(Font);
}

int main()
{
	//Get The Process
	fProcess.RunProcess();

	//Our infinite loop will go here

	return 0;
}