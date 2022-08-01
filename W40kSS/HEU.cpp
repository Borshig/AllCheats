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
#include <cmath>
#include <chrono>
#include <ctime>
#include <random>
#include <thread>


CHackProcess fProcess;
using namespace std;

DWORD dataOffset = 0x12D4628;
DWORD scoreOffset = 0x12D4628;
DWORD traceOffset = 0x12D462C;
DWORD moneyOffset = 0x12D4630;

/*
SCORE
TRACE
MONEY
HACKEDCOUNT
*/
#define VK_SHIFT	0x10	
#define VK_CONTROL	0x11
#define VK_PAGE_UP	    0x21	
#define VK_PAGE_DOWN 	0x22

struct  GameData
{
	int score;
	int trace;
	int money;
	int hackedCount;
}gameData;


void readInformation()
{
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dataOffset), //(player * EntityLoopDistance)
		&gameData, sizeof(GameData), 0);
}

void writeInformation()
{
	WriteProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dataOffset), //(player * EntityLoopDistance)
		&gameData, sizeof(GameData), 0);
}

int main()
{
	fProcess.RunProcess();
	cout << "Game Found! Running ..." << endl;
	while (true)
	{
		readInformation();
		/*
		std::cout 
			<< "Score: " <<  gameData.score << " "
			<< "Trace: " << gameData.trace << " "
			<< "Money: " << gameData.money << " "
			<< "Hacked Count: " << gameData.hackedCount << " "
			<< std::endl;
		*/
		bool bWriteData = false;
		if ((GetAsyncKeyState(VK_SHIFT)) && (GetAsyncKeyState(VK_PAGE_UP) & 1))
		{
			gameData.money += 1000;
			bWriteData = true;
		}
		if ((GetAsyncKeyState(VK_SHIFT)) && (GetAsyncKeyState(VK_PAGE_DOWN) & 1))
		{
			gameData.trace = 0;
			bWriteData = true;
		}



		if (bWriteData) writeInformation();

		Sleep(20);
	}
	


}
