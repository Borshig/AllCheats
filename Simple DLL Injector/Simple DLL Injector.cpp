#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcId(const char* procName) 
{
    DWORD procID = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);


        if (Process32First(hSnap, &procEntry))
        {
            do 
            {
                if (!_stricmp(procEntry.szExeFile, procName))
                {
                    procID = procEntry.th32ProcessID;
                    break;
                }


            } while (Process32Next(hSnap, &procEntry));
        }

        CloseHandle(hSnap);
        return procID;

    }
     
}


int main()
{
    //https://www.youtube.com/watch?v=PZLhlWUmMs0
    const char* dllPath = "C:\\Users\\Borshig\\source\\repos\\CSGO_RCS\\Release\\CSGO_RCS.dll";
    const char* procName = "csgo.exe";
    DWORD procId = 0;

    while (!procId)
    {
        procId = GetProcId(procName);
        Sleep(30);
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {
        void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (loc)
        {
            WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);
            
        } // else loc == 0
        
        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);


        if (hThread)
        {
            CloseHandle(hThread);
        }
        
    }

	if (hProc)
	{
		CloseHandle(hProc);
	}

    return 0;
}

