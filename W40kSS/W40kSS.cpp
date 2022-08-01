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

DWORD dataBaseAddress = 0x0083CD8C;
DWORD dataOffset1 = 0x74;
DWORD dataOffset2 = 0x8;

//DWORD dataOffset3 = 0x1F4;
DWORD dataOffset3 = 0x60;
DWORD dataOffset4 = 0x188;

DWORD requisitionBaseAddress = 0x004321B0;
DWORD requisitionOffset1 = 0x4;
DWORD requisitionOffset2 = 0x38;
DWORD requisitionOffset3 = 0x10;
DWORD requisitionOffset4 = 0x0;

std::vector<DWORD> offsets;

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

DWORD activeAddressGamedata = 0xDEAD;
DWORD activeAddressRequisition = 0xDEAD;


enum GameModes
{
	Unknowns = 0,
	Game,
	CompainGlobalMap
};


struct  GameData
{
	float influence;
	float power;
	float relic;
	//float unknownData1;
	//float unknownData2;

}gameData;

DWORD requisition = 0;

void findActiveAddress()
{
	/*
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + dataBaseAddress), //(player * EntityLoopDistance)
		&activeAddressGamedata, sizeof(DWORD), 0);
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + dataOffset1), //(player * EntityLoopDistance)
		&activeAddressGamedata, sizeof(DWORD), 0);
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + dataOffset2), //(player * EntityLoopDistance)
		&activeAddressGamedata, sizeof(DWORD), 0);
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + dataOffset3), //(player * EntityLoopDistance)
		&activeAddressGamedata, sizeof(DWORD), 0);
	*/

	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + offsets[0]), //(player * EntityLoopDistance)
		&activeAddressGamedata, sizeof(DWORD), 0);

	for (DWORD it = 1; it < offsets.size()-1; it++)
	{
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + offsets[it]), //(player * EntityLoopDistance)
			&activeAddressGamedata, sizeof(DWORD), 0);
	}



	/*
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordWXPModDLL + requisitionBaseAddress), //(player * EntityLoopDistance)
		&activeAddressRequisition, sizeof(DWORD), 0);
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressRequisition + requisitionOffset1), //(player * EntityLoopDistance)
		&activeAddressRequisition, sizeof(DWORD), 0);
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressRequisition + requisitionOffset2), //(player * EntityLoopDistance)
		&activeAddressRequisition, sizeof(DWORD), 0);
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressRequisition + requisitionOffset3), //(player * EntityLoopDistance)
		&activeAddressRequisition, sizeof(DWORD), 0);
	*/

}

void readInformation()
{


	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + offsets[offsets.size()-1]), //(player * EntityLoopDistance)
		&gameData, sizeof(GameData), 0);

	/*
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + dataOffset4), //(player * EntityLoopDistance)
		&gameData, sizeof(GameData), 0);
	*/
	/*
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressRequisition + requisitionOffset4), //(player * EntityLoopDistance)
		&requisition, sizeof(DWORD), 0);
	*/
}

void writeInformation(int mode)
{

	switch (mode)
	{
	case GameModes::Game:

		
		WriteProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + offsets[offsets.size()-1]), //(player * EntityLoopDistance)
			&gameData, sizeof(GameData), 0);

		/*
		WriteProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + dataOffset4), //(player * EntityLoopDistance)
			&gameData, sizeof(GameData), 0);

		*/
		break;
		/*
		case GameModes::CompainGlobalMap:
		WriteProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressRequisition + requisitionOffset4), //(player * EntityLoopDistance)
			&requisition, sizeof(GameData), 0);
		break;
		*/
	default:
		break;
	}


}

int main()
{
	offsets.emplace_back(0x0083CD8C); //address 
	offsets.emplace_back(0x10); //offset1
	offsets.emplace_back(0x60); //offset2
	offsets.emplace_back(0x8); //offset3
	offsets.emplace_back(0x60); //offset4
	offsets.emplace_back(0x188); //offset5
	

	while (true)
	{
		fProcess.RunProcess();
		cout << "Game Found! Running ..." << endl;

		findActiveAddress();

		while (!((GetAsyncKeyState(VK_F10)& 1)))
		{
			readInformation();


			if ((GetAsyncKeyState(VK_F1) & 1))
			{
				std::cout
					<< "Influence: " << gameData.influence << " "
					<< "Power: " << gameData.power << " "
					<< "Relic: " << gameData.relic << " "
					//<< "unknownData1: " << gameData.unknownData1 << " "
					//<< "unknownData2: " << gameData.unknownData2 << " "
					//<< "requisition: " << requisition << " "
					<< std::endl;
			}

			bool bWriteData = false;

			int currentGameMode = GameModes::Unknowns;

			if ((GetAsyncKeyState(VK_SHIFT)) && ((GetAsyncKeyState(VK_PAGE_UP)) & 1))
			{
				gameData.influence += 2500;
				bWriteData = true;
				currentGameMode = GameModes::Game;
			}

			if ((GetAsyncKeyState(VK_SHIFT)) && ((GetAsyncKeyState(VK_PAGE_DOWN)) & 1))
			{
				gameData.power += 2500;
				bWriteData = true;
				currentGameMode = GameModes::Game;
			}

			if ((GetAsyncKeyState(VK_CONTROL)) && ((GetAsyncKeyState(VK_PAGE_UP)) & 1))
			{
				gameData.relic += 1000;
				bWriteData = true;
				currentGameMode = GameModes::Game;
			}
			/*
			if ((GetAsyncKeyState(VK_CONTROL)) && ((GetAsyncKeyState(VK_PAGE_DOWN)) & 1))
			{
				requisition += 250;
				bWriteData = true;
				currentGameMode = GameModes::CompainGlobalMap;
			}
			*/

			if (bWriteData)
			{
				writeInformation(currentGameMode);
				Sleep(80);
			}
			Sleep(20);

		}
		Sleep(500);		
	}

}
