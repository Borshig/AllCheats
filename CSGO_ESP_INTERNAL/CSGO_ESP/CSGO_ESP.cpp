#include <iostream>
#include <windows.h>
//#include <TlHelp32.h>
#include "HZ_offsets.hpp"
#include <cstdlib>

using namespace hazedumper;
HMODULE hDll;

/*
#define dwLocalPlayer 0xD36894
#define m_iTeamNum 0xF4
#define m_iHealth 0x100
#define m_vecOrigin 0x138
*/


void clearConsole()
{
	COORD topleft = { 0,0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topleft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topleft, &written
	);
	SetConsoleCursorPosition(console, topleft);

}

void printptrinto(DWORD* ptr, const char* ptrName)
{
	std::cout << ptrName << " ptr: " << ptr << " *ptr: " << *ptr /* << " &ptr: " << &ptr << " *&ptr:" << *&ptr*/ << std::endl;
}

DWORD* printptrinfo2(DWORD* first, ptrdiff_t second)
{
	std::cout << "printptrinfo2: " << std::endl;
	std::cout << "\t first : " << first  << std::endl;
	std::cout << "\t second: " << second << std::endl;
	DWORD* casted = reinterpret_cast<DWORD*>(first + second);

	std::cout << "\t after cast: " << casted  << " and *" << *casted << std::endl;
	return casted;
}


void _stdcall init()
{
	//MessageBoxA(nullptr, "DLL injected", "OK", MB_OK);
	DWORD* bClientBase = nullptr;
	DWORD* BSteamnetworkingBase = nullptr;
	DWORD* bEngineBase = nullptr;

	SetConsoleTitleA("bClientBase not inited");
	
	do 
	{
			
		bClientBase = reinterpret_cast<DWORD*>(GetModuleHandleW(L"panorama.dll"/*"Client.dll"*/));
		BSteamnetworkingBase = reinterpret_cast<DWORD*>(GetModuleHandleW(L"Steamnetworkingsockets.dll"));
		bEngineBase = reinterpret_cast<DWORD*>(GetModuleHandleW(L"Engine.dll")); 
		Sleep(3);

	} while (bClientBase == nullptr || bClientBase == 0);




	AllocConsole();
	FILE* console = nullptr;
	freopen_s(&console, "CONOUT$", "w", stdout);
	SetConsoleTitleA("CS GO READER 1");
	
	
	


	while (hDll != INVALID_HANDLE_VALUE)
	{
		
		
		DWORD* pLocal = reinterpret_cast<DWORD*>(bClientBase + signatures::dwLocalPlayer);
	
			

		DWORD* localTeamNum = reinterpret_cast<DWORD*>(pLocal + netvars::m_iTeamNum);
		DWORD* localHealth = reinterpret_cast<DWORD*>(pLocal + netvars::m_iHealth);
		float* position = reinterpret_cast<float*>(pLocal + netvars::m_vecOrigin);				
		DWORD* dwEntityList = reinterpret_cast<DWORD*>(bClientBase+ signatures::dwEntityList);
		
		clearConsole();


		printf_s("signatures::dwLocalPlayer = 0x%X\r\n", signatures::dwLocalPlayer);
		printf_s("netvars::m_iTeamNum = 0x%X\r\n", netvars::m_iTeamNum);
		printf_s("netvars::m_iHealth= 0x%X\r\n", netvars::m_iHealth);
		printf_s("netvars::m_vecOrigin = 0x%X\r\n", netvars::m_vecOrigin);

		printf_s("localHealth: 0x%X\r\n", localHealth);
		printf_s("localTeamNum: 0x%X\r\n", localTeamNum);
		printf_s("dwEntityList: 0x%X\r\n", dwEntityList);
		

		printf_s("clientbase + dwLocalPlayer = 0x%X\r\n", bClientBase +  signatures::dwLocalPlayer);
		printf_s("clientbase + dwLocalPlayer + health offset + 28551F94 = 0x%X\r\n", bClientBase + signatures::dwLocalPlayer + netvars::m_iHealth + 0x28551F94);

		printf_s("clientbase + dwLocalPlayer = 0x%X\r\n", *bClientBase + signatures::dwLocalPlayer);
		printf_s("clientbase + dwLocalPlayer + health offset = 0x%X\r\n", bClientBase + signatures::dwLocalPlayer + netvars::m_ArmorValue + 0x28551F94);

	

		printptrinto(bClientBase, "(Client.dll) ClientBase:\t\t\t");
		printptrinto(bEngineBase, "(Engine.dll) EngineBase:\t\t\t");	
		printptrinto(BSteamnetworkingBase, "(Steamnetworking.dll) SteamnetworkingBase:");


		/*
		//std::cout << "client.dll base address: 0x" << bClientBase << std::endl;
		printptrinto(pLocal, "local player base");
		printptrinto(localHealth, "pLocal->health");

		std::cout << "pLocal->position: <" << position[0] << "," << position[1] << "," << position[2] << ">" << std::endl;
		//std::cout << "mLocal->position: <" << position2[0] << "," << position2[1] << "," << position2[2] << ">" << std::endl;


		printptrinto(dwEntityList, "dwEntityList");

		//std::cout << "position: X: " << position << " Y: " << position << " Z: " << position << std::endl;

		//printf_s("client_panorama.dll base address: 0x%X\r\n", bClientBase);
		//printf_s("pLocal base: 0x%X\r\n", pLocal);
		//printf_s("pLocal->health base: 0x%d\r\n", localHealth);
		//printf_s("pLocal->team base: 0x%d\r\n", localTeamNum);
		//printf_s("pLocal->position: <%f, %f, %f>\r\n", position[0], position[1], position[2]);

		**/

		Sleep(1000);
	}


	
}

//INTERNAL cheat

/*
BOOL WINAPI DllMain(
	HINSTANCE hInstDLL,
	DWORD fdwReason,
	LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		hDll = hInstDLL;
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(init), nullptr, 0, nullptr);
	}

	return TRUE;
}
*/

int main()
{
	init();
}