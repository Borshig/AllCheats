#include <Windows.h>
#include <sstream>
#include "HackProcess.h"
#include <math.h>
//This is same as fleeps original code, just cleaned up

//Create our 'hooking' and process managing object
CHackProcess fProcess;
using namespace std;
#define F6_Key 0x75
#define RIGHT_MOUSE 0x02

//Here we store the num of players and update it regularly to know how many enemies we are dealing with
//Get these from hazedumper if you don't know
int NumOfPlayers = 32;
const DWORD dw_PlayerCountOffs = 0x5E96BC; //0x5EF6BC;//Engine.dll

const DWORD Player_Base = 0x4C6708;   //0x4C6708; //LocalPlayer
const DWORD dwClientId = 0x4c; //0x4c;
const DWORD dw_Health = 0x94; //0x94;//client
const DWORD dw_mTeamOffset = 0x9C; //0x9C;//client
const DWORD dw_Pos = 0x260;   //0x260;//client
const DWORD dwAngle = 0x26c; //0x26c;
const DWORD EntityPlayer_Base = 0x4D3904;// 0x4D3904; //entitylist
const DWORD EntityLoopDistance = 0x10;

const DWORD dw_vMatrix = (0x5AD648); //= 0x5B3BF0; // 0x5A7894                --58C45C ///  (0x5A7894+0x38)=0x5A78EC engine.dll+5AD648
// antiflick = matrix - 1A4; //antiflick doesn't work in 2018
//const DWORD dw_antiFlick = 0x5916B8;

//GDI Variables
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
	float flMatrix[4][4];
}WorldToScreenMatrix_t;
//Calculate distance between two coordinates in 3d space
float Get3dDistance(float * myCoords, float * enemyCoords)
{
	return sqrt(
		pow(double(enemyCoords[0] - myCoords[0]), 2.0) +
		pow(double(enemyCoords[1] - myCoords[1]), 2.0) +
		pow(double(enemyCoords[2] - myCoords[2]), 2.0));

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
struct MyPlayer_t
{
	DWORD CLocalPlayer;
	int Team;
	int Health;
	WorldToScreenMatrix_t WorldToScreenMatrix;
	float Position[3];
	//int flickerCheck; not working in 2018
	void ReadInformation()
	{
		// Reading CLocalPlayer Pointer to our "CLocalPlayer" DWORD.
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordClient + Player_Base), &CLocalPlayer, sizeof(DWORD), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_mTeamOffset), &Team, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_Health), &Health, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CLocalPlayer + dw_Pos), &Position, sizeof(float[3]), 0);
		//get number of players
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_PlayerCountOffs), &NumOfPlayers, sizeof(int), 0);

		/*anti flicker not working in 2018
		//ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_antiFlick), &flickerCheck, sizeof(int), 0);
		//VMATRIX
		//if (flickerCheck == 0)
		*/
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dw_vMatrix), &WorldToScreenMatrix, sizeof(WorldToScreenMatrix), 0);
		
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
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_mTeamOffset), &Team, sizeof(int), 0);
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(CBaseEntity + dw_Health), &Health, sizeof(int), 0);
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
bool WorldToScreen(float * from, float * to)
{
	float w = 0.0f;

	to[0] = MyPlayer.WorldToScreenMatrix.flMatrix[0][0] * from[0] + MyPlayer.WorldToScreenMatrix.flMatrix[0][1] * from[1] + MyPlayer.WorldToScreenMatrix.flMatrix[0][2] * from[2] + MyPlayer.WorldToScreenMatrix.flMatrix[0][3];
	to[1] = MyPlayer.WorldToScreenMatrix.flMatrix[1][0] * from[0] + MyPlayer.WorldToScreenMatrix.flMatrix[1][1] * from[1] + MyPlayer.WorldToScreenMatrix.flMatrix[1][2] * from[2] + MyPlayer.WorldToScreenMatrix.flMatrix[1][3];
	w = MyPlayer.WorldToScreenMatrix.flMatrix[3][0] * from[0] + MyPlayer.WorldToScreenMatrix.flMatrix[3][1] * from[1] + MyPlayer.WorldToScreenMatrix.flMatrix[3][2] * from[2] + MyPlayer.WorldToScreenMatrix.flMatrix[3][3];

	if (w < 0.01f)
		return false;

	float invw = 1.0f / w;
	to[0] *= invw;
	to[1] *= invw;

	int width = (int)(m_Rect.right - m_Rect.left);
	int height = (int)(m_Rect.bottom - m_Rect.top);

	float x = width / 2;
	float y = height / 2;

	x += 0.5 * to[0] * width + 0.5;
	y -= 0.5 * to[1] * height + 0.5;

	to[0] = x + m_Rect.left;
	to[1] = y + m_Rect.top;

	return true;
}
void DrawESP(int x, int y, float distance)
{
	//ESP RECTANGLE

	

	int width = 0;
	int height = 0;

	if (GetAsyncKeyState(0x10) || GetAsyncKeyState(0x11) || GetAsyncKeyState(0x12)) distance /= 8;
	
	width = 18100 / distance;
	height = 50000 / distance;
	


	DrawBorderBox(x - (width / 2), y - height - 5 , width, height - 5, 1);

	//Sandwich ++
	DrawLine((m_Rect.right - m_Rect.left) / 2, m_Rect.bottom - m_Rect.top, x, y, SnapLineCOLOR);

	std::stringstream ss;
	ss << (int)distance;

	char * distanceInfo = new char[ss.str().size() + 1];
	strcpy(distanceInfo, ss.str().c_str());

	DrawString(x, y, TextCOLOR, distanceInfo);
	delete[] distanceInfo;
}
void ESP()
{
	GetWindowRect(FindWindow(NULL, "Counter-Strike Source"), &m_Rect);

	for (int i = 0; i < NumOfPlayers; i++)
	{
		PlayerList[i].ReadInformation(i);

		if (PlayerList[i].Health < 2 || PlayerList[i].Health > 100)
			continue;

		if (PlayerList[i].Team == MyPlayer.Team)
			continue;

		

		

		float EnemyXY[3];
		if (WorldToScreen(PlayerList[i].Position, EnemyXY))
		{
			DrawESP(EnemyXY[0] - m_Rect.left, EnemyXY[1] - m_Rect.top, Get3dDistance(MyPlayer.Position, PlayerList[i].Position));
		}
	}
}
int main()
{
	//Get The Process
	fProcess.RunProcess();

	ShowWindow(FindWindowA("ConsoleWindowClass", NULL), false);
	TargetWnd = FindWindow(0, "Counter-Strike Source");
	HDC HDC_Desktop = GetDC(TargetWnd);
	SetupDrawing(HDC_Desktop, TargetWnd);

	while (!GetAsyncKeyState(VK_INSERT))
	{
		MyPlayer.ReadInformation();
		ESP();
	}

	return 0;
}