#include "Windows.h"

/*
DLL_PROCESS_ATTACH:
DLL_THREAD_ATTACH:
DLL_THREAD_DETACH:
DLL_PROCESS_DETACH:
*/

//vec3 class
struct Vec3
{
    float x, y, z;

    Vec3 operator+(Vec3 d)
    {
       return { x + d.x , y + d.y , z + d.z };
    }
	Vec3 operator-(Vec3 d)
	{
       return { x - d.x , y - d.y , z - d.z };
	}
    Vec3 operator*(float d)
    {
        return {x* d, y* d, z* d};
	}

    void Normalize()
    {
        while (y < -180) { y += 360; }
        while (y > 180) { y -= 360; }
        if (x > 89) { x = 89; }
        if (x < -89) { x = -89; }


    }

};
//offsets
uintptr_t dwLocalPlayer = 0xDA746C;
uintptr_t dwClientState = 0x589FCC;
uintptr_t dwClientState_ViewAngles = 0x4D90;
uintptr_t m_iShotsFired = 0x103E0;
uintptr_t m_aimPunchAngle = 0x303C;
uintptr_t m_aimPunchAngleVel = 0x3048;


//setting

uintptr_t exitKey = VK_F6;

int WINAPI HackThread(HMODULE hModule)
{
    // data

    //clientmodule
    uintptr_t clientModule = (uintptr_t)GetModuleHandle("client.dll");
    uintptr_t engineModule = (uintptr_t)GetModuleHandle("engine.dll");
    uintptr_t LocalPlayer = *(uintptr_t*)(clientModule + dwLocalPlayer);
    Vec3* viewAngle = (Vec3*)(*(uintptr_t*)(engineModule + dwClientState)+ dwClientState_ViewAngles);
    int* iShotsFired = (int*)(LocalPlayer + m_iShotsFired);
    Vec3* aimPunchAngle = (Vec3*)(LocalPlayer + m_aimPunchAngle);
    Vec3 oPunch{ 0,0,0 };

    Vec3* aimPunchAngleVel = (Vec3*)(LocalPlayer + m_aimPunchAngle);


	//hackloop
    while (!GetAsyncKeyState(exitKey))
    {
        *aimPunchAngleVel = { 0,0,0 };

        Vec3 punchAngle = (*aimPunchAngle) * 2;

        if (*iShotsFired > 1)
        {
            //calc rcs
            Vec3 newAngle = *viewAngle + oPunch - punchAngle;
            //normilize
            newAngle.Normalize();
            //set
            *viewAngle = newAngle;
                        
        }
        oPunch = punchAngle;
    }

    FreeLibraryAndExitThread(hModule, 0);
    CloseHandle(hModule);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call,LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0);
    return TRUE;
}

