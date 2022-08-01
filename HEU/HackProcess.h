#pragma once
#include <Windows.h>
#include <TlHelp32.h>
//THIS FILE SIMPLY DOES MOST OF THE BACKEND WORK FOR US, FROM FINDING THE PROCESS TO SETTING UP CORRECT MEMORY ACCESS

//using QWORD = unsigned long long;

class CHackProcess
{
public:

	PROCESSENTRY32 __gameProcess;
	HANDLE __HandleProcess;
	HWND __HWNDCss;
	DWORD __dwordEngine;

	BOOL ListProcessModules(DWORD dwPID)
	{
		HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
		MODULEENTRY32 me32;

		//  Take a snapshot of all modules in the specified process. 
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
		if (hModuleSnap == INVALID_HANDLE_VALUE)
		{
			//printError(TEXT("CreateToolhelp32Snapshot (of modules)"));
			return(FALSE);
		}

		//  Set the size of the structure before using it. 
		me32.dwSize = sizeof(MODULEENTRY32);

		//  Retrieve information about the first module, 
		//  and exit if unsuccessful 
		if (!Module32First(hModuleSnap, &me32))
		{
			//printError(TEXT("Module32First"));  // Show cause of failure 
			CloseHandle(hModuleSnap);     // Must clean up the snapshot object! 
			return(FALSE);
		}

		//  Now walk the module list of the process, 
		//  and display information about each module 
		do
		{
			printf_s("\n\n   MODULE NAME:     %s", me32.szModule);
			printf_s("\n     executable     = %s", me32.szExePath);
			printf_s("\n     process ID     = 0x%08X", me32.th32ProcessID);
			printf_s("\n     ref count (g)  =     0x%04X", me32.GlblcntUsage);
			printf_s("\n     ref count (p)  =     0x%04X", me32.ProccntUsage);
			printf_s("\n     base address   = 0x%08X", (DWORD)me32.modBaseAddr);
			printf_s("\n     base size      = %d", me32.modBaseSize);

		} while (Module32Next(hModuleSnap, &me32));

		printf_s("\n");

		//  Do not forget to clean up the snapshot object. 
		CloseHandle(hModuleSnap);
		return(TRUE);
	}

	DWORD FindProcessName(const char* __ProcessName, PROCESSENTRY32* pEntry) //return th32ProcessID(PID)
	{


		PROCESSENTRY32 __ProcessEntry;
		__ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		HANDLE hSnapshot = INVALID_HANDLE_VALUE;
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnapshot == INVALID_HANDLE_VALUE) return 0;
		if (!Process32First(hSnapshot, &__ProcessEntry))
		{
			CloseHandle(hSnapshot);
			return 0;
		}
		do {
			if (!_strcmpi(__ProcessEntry.szExeFile, __ProcessName))
			{
				memcpy((void*)pEntry, (void*)&__ProcessEntry, sizeof(PROCESSENTRY32));
				CloseHandle(hSnapshot);
				return __ProcessEntry.th32ProcessID;
			}
		} while (Process32Next(hSnapshot, &__ProcessEntry));
		CloseHandle(hSnapshot);
		return 0;
	}

	DWORD getThreadByProcess(DWORD __DwordProcess)
	{
		THREADENTRY32 __ThreadEntry;
		__ThreadEntry.dwSize = sizeof(THREADENTRY32);
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

		if (!Thread32First(hSnapshot, &__ThreadEntry)) { CloseHandle(hSnapshot); return 0; }

		do
		{
			if (__ThreadEntry.th32OwnerProcessID == __DwordProcess)
			{
				CloseHandle(hSnapshot);
				return __ThreadEntry.th32ThreadID;
			}
		} while (Thread32Next(hSnapshot, &__ThreadEntry));
		CloseHandle(hSnapshot);
		return 0;
	}

	DWORD GetModuleNamePointer(const char* LPSTRModuleName, DWORD __DwordProcessId)
	{
		MODULEENTRY32 lpModuleEntry = { 0 };
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, __DwordProcessId);



		if (!hSnapShot)
			return NULL;
		lpModuleEntry.dwSize = sizeof(lpModuleEntry);
		BOOL __RunModule = Module32First(hSnapShot, &lpModuleEntry);
		while (__RunModule)
		{
			if (!strcmp(lpModuleEntry.szModule, LPSTRModuleName))
			{
				CloseHandle(hSnapShot);
				return (DWORD)lpModuleEntry.modBaseAddr;
			}
			__RunModule = Module32Next(hSnapShot, &lpModuleEntry);
		}
		CloseHandle(hSnapShot);
		return NULL;
	}

	void runSetDebugPrivs()
	{
		HANDLE __HandleProcess = GetCurrentProcess(), __HandleToken;
		TOKEN_PRIVILEGES priv;
		LUID __LUID;
		OpenProcessToken(__HandleProcess, TOKEN_ADJUST_PRIVILEGES, &__HandleToken);
		LookupPrivilegeValueA(0, "seDebugPrivilege", &__LUID);
		priv.PrivilegeCount = 1;
		priv.Privileges[0].Luid = __LUID;
		priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(__HandleToken, false, &priv, 0, 0, 0);
		CloseHandle(__HandleToken);
		CloseHandle(__HandleProcess);
	}

	void RunProcess()
	{



		//commented lines are for non steam versions of the game
		runSetDebugPrivs();
		while (!FindProcessName("Hacker Evolution Untold.exe", &__gameProcess)) Sleep(12); //"hl2.exe"
		while (!(getThreadByProcess(__gameProcess.th32ProcessID))) Sleep(12);
		__HandleProcess = OpenProcess(PROCESS_ALL_ACCESS, false, __gameProcess.th32ProcessID);

		switch (GetLastError())
		{
		case 5: std::cout << "ERROR_ACCESS_DENIED" << " CODE: " << ERROR_ACCESS_DENIED << std::endl; break;
		case 6: std::cout << "ERROR_INVALID_HANDLE" << " CODE: " << ERROR_INVALID_HANDLE << std::endl; break;

		}


		while (__dwordEngine == 0x0) __dwordEngine = GetModuleNamePointer("Hacker Evolution Untold.exe", __gameProcess.th32ProcessID);
		//while (__dwordGameAssembly == 0x0) __dwordGameAssembly = GetModuleNamePointer("GameAssembly.dll", __gameProcess.th32ProcessID);
		//while(__dwordOverlay == 0x0) __dwordOverlay = GetModuleNamePointer("gameoverlayrenderer.dll", __gameProcess.th32ProcessID);
		//while (__dwordVGui == 0x0) __dwordVGui = GetModuleNamePointer((LPSTR)"vguimatsurface.dll", __gameProcess.th32ProcessID);
		//while(__dwordLibCef == 0x0) __dwordLibCef = GetModuleNamePointer("libcef.dll", __gameProcess.th32ProcessID);
	//	while(__dwordSteam == 0x0) __dwordSteam = GetModuleNamePointer("steam.dll", __gameProcess.th32ProcessID);
		__HWNDCss = FindWindowA(NULL, "Counter-Strike: Global Offensive");

		return;
	}
};

extern CHackProcess fProcess;