// DOSCheat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "HackProcess.h"
#include <vector>
#include <iostream>

#include <thread>
#include <chrono>
#include <ctime>

CHackProcess fProcess;
using namespace std;

QWORD activeAddressGamedata = 0x012E3230;
QWORD activeAddressGamedata2 = 0x012E3230;


std::vector<QWORD> offsets;
std::vector<QWORD> offsets2;

struct  GameData
{
	int Health = 0;

};

struct u8Byte {
	BYTE inst1; // hex
	BYTE inst2;
	BYTE inst3;
	BYTE inst4;
	BYTE inst5;
	BYTE inst6;
	BYTE inst7;
	BYTE inst8;
};


union GM
{
	u8Byte u8Bytes;
	BYTE Bytes[8];
	QWORD qWORD;
} uGM;


GameData data1;
GameData data2;


void findActiveAddress()
{
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + offsets[0]), //(player * EntityLoopDistance)
		&activeAddressGamedata, sizeof(QWORD), 0);

	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + offsets2[0]), //(player * EntityLoopDistance)
		&activeAddressGamedata2, sizeof(QWORD), 0);

	for (QWORD it = 1; it < offsets.size() - 1; it++)
	{
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + offsets[it]), //(player * EntityLoopDistance)
			&activeAddressGamedata, sizeof(QWORD), 0);
	}	

	for (QWORD it = 1; it < offsets.size() - 1; it++)
	{
		ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata2 + offsets2[it]), //(player * EntityLoopDistance)
			&activeAddressGamedata2, sizeof(QWORD), 0);
	}
}

void readInformation()
{
	 ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + offsets[offsets.size() - 1]), //(player * EntityLoopDistance)
		&data1, sizeof(GameData), 0);

	 ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata2 + offsets2[offsets.size() - 1]), //(player * EntityLoopDistance)
		 &data2, sizeof(GameData), 0);


}

void writeInformation()
{
	WriteProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + offsets[offsets.size() - 1]), //(player * EntityLoopDistance)
			&data1, sizeof(GameData), 0);

	WriteProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata2 + offsets2[offsets.size() - 1]), //(player * EntityLoopDistance)
			&data2, sizeof(GameData), 0);

}

void enableGodMod()
{

	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + 0xBAD156), //(B1CE75)
		&uGM.qWORD, sizeof(DWORD) + 2 * sizeof(BYTE), 0);
	std::cout << "INSTRUCTION SET: ";
	for (int i = 0; i < sizeof(DWORD) + 2 * sizeof(BYTE); i++)
	{
		std::cout << hex << (int)uGM.Bytes[i] << ":";
	}
	std::cout << std::endl;

	
	for (int i = 0; i < sizeof(DWORD) + 2 * sizeof(BYTE); i++)
	{
		uGM.Bytes[i] = 0x90;
	}

	WriteProcessMemory(fProcess.__HandleProcess, (PBYTE*)(fProcess.__dwordEngine + 0xBAD156), //(B1CE75)
		&uGM.qWORD, sizeof(DWORD) + 2 * sizeof(BYTE), 0);

}


int main()
{
	/*
	offsets.emplace_back(0x012E3230);
	offsets.emplace_back(0x8);
	offsets.emplace_back(0x0);
	offsets.emplace_back(0xC0);
	offsets.emplace_back(0x12C);

	offsets2.emplace_back(0x012E3230);
	offsets2.emplace_back(0x8);
	offsets2.emplace_back(0x10);
	offsets2.emplace_back(0xC0);
	offsets2.emplace_back(0x12C);
	*/

	

	offsets.emplace_back(0x0134EAF0);
	offsets.emplace_back(0x180);
	offsets.emplace_back(0x0);
	offsets.emplace_back(0xC0);
	offsets.emplace_back(0x12C);

	offsets2.emplace_back(0x0134EAF0);
	offsets2.emplace_back(0x180);
	offsets2.emplace_back(0x8);
	offsets2.emplace_back(0xC0);
	offsets2.emplace_back(0x12C);


	fProcess.RunProcess();
	std::cout << "Game: found... Running." << std::endl;

	
	


	while (!((GetAsyncKeyState(VK_F10) & 1)))
	{
		bool bChanged = false;
		findActiveAddress();
		readInformation();

		if ((GetAsyncKeyState(VK_F1) & 1))
		{
			std::cout << std::hex
				<< "Active gameAdress1: " << activeAddressGamedata << " " << &data1
				<< std::endl;
			std::cout
				<< "Active gameAdress1: " << activeAddressGamedata2 << " " << &data2
				<< std::endl;

			std::cout << std::dec
				<< "Health 1st: " << data1.Health << " "		
				<< std::endl;
			std::cout
				<< "Health 2nd: " << data2.Health << " "
				<< std::endl;
		}

		if ((GetAsyncKeyState(VK_F2) & 1))
		{
			enableGodMod();
		}



		if ((GetAsyncKeyState(VK_OEM_PLUS) & 1))
		{
			data1.Health += 100;
			bChanged = true;
		}

		if ((GetAsyncKeyState(VK_OEM_MINUS) & 1))
		{
			data2.Health += 100;
			bChanged = true;
		}

		if (bChanged) writeInformation();	

		Sleep(20);

	}
	Sleep(20);



}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
