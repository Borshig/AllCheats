// DOSCheat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS

#include "HackProcess.h"


#include <vector>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>

//#include "include/d3dhook/DllMain.h"

#include "include/imgui.h"
#include "include/backends/imgui_impl_win32.h"
#include "include/backends/imgui_impl_dx11.h"
#include "Functions.h"
#include "Overlay.h"

#include <d3d11.h>
#include <tchar.h>

bool bExit = false;
bool bUseVsync = true;
bool bGodModToEnable = false;
bool bGodModEnabled = false;
int windowstate = 1;

DWORD instructionPositionGodMode = 0;
DWORD instructionPositionResetKills = 0;
DWORD instructionPositionUnlimitedAmmo = 0;

void ChangeClickability(bool canclick, HWND ownd)
{
	long style = GetWindowLong(
		ownd, GWL_EXSTYLE);
	if (canclick) {
		style &= ~WS_EX_LAYERED;
		SetWindowLong(ownd,
			GWL_EXSTYLE, style);
		SetForegroundWindow(ownd);
		windowstate = 1;
	}
	else {
		style |= WS_EX_LAYERED;
		SetWindowLong(ownd,
			GWL_EXSTYLE, style);
		windowstate = 0;
	}
}

LPCSTR TargetProcess = "EoCApp.exe"; //"D3D9Test.exe";

CHackProcess32 fProcess32;
using namespace std;

DWORD activeAddressGamedata = 0x012E3230;

std::vector<DWORD> healthOffsets;

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
struct u4Byte {
	BYTE inst1; // hex
	BYTE inst2;
	BYTE inst3;
	BYTE inst4;
};
union GM8
{
	u8Byte u8Bytes;
	BYTE Bytes[8];
	QWORD qWORD;
};

union GM4
{
	u4Byte u4Bytes;
	BYTE Bytes[4];
	DWORD dWORD;
};

void* allocatedMemGodMode = nullptr;
bool bGodModeEnabled = false;
bool bAllreadyAllocated = false;
static bool ShowMenu = true;
static bool ShowDemoMenu = false;
static bool ShowMenuSwitched = false;

int	screenX = GetSystemMetrics(SM_CXSCREEN);
int	screenY = GetSystemMetrics(SM_CYSCREEN);

namespace Process {
	DWORD ID;
	HANDLE Handle;
	HWND Hwnd;
	WNDPROC WndProc;
	int WindowWidth;
	int WindowHeight;
	int WindowLeft;
	int WindowRight;
	int WindowTop;
	int WindowBottom;
	LPCSTR Title;
	LPCSTR ClassName;
	LPCSTR Path;
}

namespace OverlayWindow {
	WNDCLASSEX WindowClass;
	HWND Hwnd;
	LPCWSTR Name = L"ImGui";
}



namespace DirectX11Interface {
	static ID3D11Device* pd3dDevice = nullptr;
	static ID3D11DeviceContext* pd3dDeviceContext = nullptr;
	static IDXGISwapChain* g_pSwapChain = nullptr;
	static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
	static D3DPRESENT_PARAMETERS pParams = { NULL };
	static MARGINS Margin = { -1 };
	static MSG Message = { nullptr };
}


GameData data1;
//-------RENDER-------
void Render();
bool DirectXInit();
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

void findActiveAddress();
void readInformation();
void writeInformation();
void switchGodMod(bool bEnable);
void searchAllEnemies();
void takeInputLoop();
bool CreateOverlay();
void checkAppAlive(); // TODO: release
void cheatCycle();    // TODO: release
void mainLoop();
DWORD searchInMemory(std::vector<BYTE>& arr);


//-----MY--FUNCS-------
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


void findActiveAddress()
{
	OPTICK_EVENT();

	/*
	ReadProcessMemory(fProcess32.__HandleProcess, (PBYTE*)(healthOffsets[0] - 0x00000718), //(player * EntityLoopDistance)
		&activeAddressGamedata, sizeof(DWORD), 0);
	
	for (DWORD it = 1; it < healthOffsets.size() - 1; it++)
	{
		ReadProcessMemory(fProcess32.__HandleProcess, (PBYTE*)(activeAddressGamedata + healthOffsets[it]), //(player * EntityLoopDistance)
			&activeAddressGamedata, sizeof(DWORD), 0);
	}	
	*/

}

void readInformation()
{
	OPTICK_EVENT();

	/*
	 ReadProcessMemory(fProcess32.__HandleProcess, (PBYTE*)(activeAddressGamedata + healthOffsets[healthOffsets.size() - 1]), //(player * EntityLoopDistance)
		&data1, sizeof(GameData), 0);
	*/

}

void writeInformation()
{
	OPTICK_EVENT();
	/*
	WriteProcessMemory(fProcess32.__HandleProcess, (PBYTE*)(activeAddressGamedata + healthOffsets[healthOffsets.size() - 1]), //(player * EntityLoopDistance)
		&data1, sizeof(GameData), 0);
	*/	
}

void switchGodMod(bool bEnable)
{
	OPTICK_EVENT();


	/*
	DWORD instructionPositionGodMode 
	DWORD instructionPositionResetKills  
	*/
	//89 51 54
	BYTE originalBytesOHR[3] = { 0x89, 0x51, 0x54 }; // for onHitReset
	BYTE newBytesOHR[3] = { 0x90,0x90,0x90 };

	
	if (bEnable)
	{
		WriteProcessMemory(fProcess32.__HandleProcess,
			(void*)(instructionPositionResetKills +1),//	allocatedMemGodMode,// where
			newBytesOHR,							// for what
			sizeof(newBytesOHR)						// sizeof new code segment
			, 0);
		
	}
	else
	{
		WriteProcessMemory(fProcess32.__HandleProcess,
			(void*)(instructionPositionResetKills +1),//	allocatedMemGodMode,// where
			originalBytesOHR,							// for what
			sizeof(originalBytesOHR)						// sizeof new code segment
			, 0);
	}
	

	// https://guidedhacking.com/threads/c-memory-allocation-and-jumps.11571/#post-66075 //

	
	/*GM newdata[4]; //new instructions
	newdata[0].qWORD = 0x0F0100000190BB83;
	newdata[1].qWORD = 0x2C83C70000000A84;
	newdata[2].qWORD = 0xE900000000000001; 
	newdata[3].qWORD = 0x00000000FF68D140; //E9 40 D1 68 FF 00 00 00 00 00 00 jump back jmp "EoCApp.exe"+BAD15C
	*/

	DWORD sourceAddressJmpoffset = 0x0;
	DWORD destAddressJmpoffset = 0x0;

	//0:  83 7a 30 01             cmp    DWORD PTR[edx + 0x30], 0x1
	//4 : c7 82 04 01 00 00 00    mov    DWORD PTR[edx + 0x104], 0x0

	BYTE bytes[] = { 0x83,0x7A,0x30,0x01, 0x0F,0x85,0x90,0x90,0x90,0x90   ,0xC7,0x82,0x04,0x01,0x00,0x00,0x00, 0x00,0x00,0x00,   0xE9,0x90,0x90,0x90,0x90 };
	
	if (bEnable)
	{
		if (!bAllreadyAllocated)
		{		
			uintptr_t pLoc = fProcess32.__dwordClient;
			while (allocatedMemGodMode == nullptr)
			{
				
				allocatedMemGodMode = VirtualAllocEx(fProcess32.__HandleProcess, (void*)(pLoc), 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				
				if (!allocatedMemGodMode)
				{
					pLoc += 0x2000; //0x500;
					continue;
				}
				DWORD readedValue = 0;
				ReadProcessMemory(fProcess32.__HandleProcess, (PBYTE*)(allocatedMemGodMode), //(B1CE75)
					&readedValue, sizeof(DWORD), 0); //change me
			}

			// another address
			
			sourceAddressJmpoffset = ((DWORD)((DWORD)(instructionPositionGodMode)-(DWORD)(allocatedMemGodMode)));
			GM4 backJumpBytes;
			backJumpBytes.dWORD = sourceAddressJmpoffset - 0x4;
			
			for (int i = 0; i < 4; i++)
			{
				bytes[i + 6] = backJumpBytes.Bytes[i];
			}

			backJumpBytes.dWORD = sourceAddressJmpoffset - 0x13;
			for (int i = 0; i < 4; i++)
			{
				bytes[i + 21] = backJumpBytes.Bytes[i];
			}

			WriteProcessMemory(fProcess32.__HandleProcess,
				(void*)(allocatedMemGodMode),//	allocatedMemGodMode,// where
				bytes,							// for what
				sizeof(bytes)						// sizeof new code segment
				, 0);
			bAllreadyAllocated = true;

		}
		GM4 newJumpBytes;
		newJumpBytes.dWORD = destAddressJmpoffset = (DWORD)(allocatedMemGodMode)-(DWORD)(instructionPositionGodMode)-5;
		


		BYTE bBytes[6] = { 0xE9,newJumpBytes.Bytes[0],newJumpBytes.Bytes[1],newJumpBytes.Bytes[2],newJumpBytes.Bytes[3],0x90 };


		//Write JMP Instruction Address
		WriteProcessMemory(fProcess32.__HandleProcess, (PBYTE*)(instructionPositionGodMode),
			&bBytes, 6, 0); //change me
	}
	else if (bAllreadyAllocated)
	{
		//89 82 04 01 00 00
		BYTE bBytes[] = { 0x89, 0x82,0x04,0x01,0x00,0x00 };
		
		/*
		uGM[0].qWORD = 0x83 48 00 00 01 2C BB 89 BE B0 01  00 00 00 90 90;
		uGM[1].qWORD = 0x909000000001B0BE;
		*/

		WriteProcessMemory(fProcess32.__HandleProcess, (PBYTE*)(instructionPositionGodMode),
			&bBytes, sizeof(bBytes), 0); //change me


		VirtualFree(allocatedMemGodMode, 0x100, MEM_DECOMMIT | MEM_RELEASE);
	}
	
	std::cout << std::hex << allocatedMemGodMode << std::endl;
	

	bGodModEnabled = bEnable;
	bGodModToEnable = bGodModEnabled;

}

void searchAllEnemies()
{
	OPTICK_EVENT();	
}

void takeInputLoop()
{
	OPTICK_EVENT();
	bool bChanged = false;
	if (GetAsyncKeyState(VK_HOME) & 1)
	{
		ShowMenuSwitched = true;
		ShowMenu = !ShowMenu;

		std::cout << "HOME key Pressed" << std::endl;
	}

	if ((GetAsyncKeyState(VK_F3) & 1))
	{
		std::cout << std::hex
			<< "Active gameAdress1: " << activeAddressGamedata << " " << &data1
			<< std::endl;
		
		std::cout << std::dec
			<< "Health 1st: " << data1.Health << " "
			<< std::endl;
		

		searchAllEnemies();

	}

	if ((GetAsyncKeyState(VK_F1) & 1))
	{
		std::cout << "God mode switched on" << std::endl;
		switchGodMod(true);
	}
	if ((GetAsyncKeyState(VK_F2) & 1))
	{
		std::cout << "God mode switched off" << std::endl;
		switchGodMod(false); // mov [rbx+0000012C],edi  // __dwordEngine + 0xBAD156
	}

	if (bGodModEnabled != bGodModToEnable)
	{
		std::cout << "God mode switched" << std::endl;
		switchGodMod(bGodModToEnable); // mov [rbx+0000012C],edi  // __dwordEngine + 0xBAD156
	}


	if ((GetAsyncKeyState(VK_OEM_PLUS) & 1))
	{
		data1.Health += 100;
		bChanged = true;
	}

	if ((GetAsyncKeyState(VK_INSERT) & 1))
	{
		//ShowMenu = !ShowMenu;
		std::cout << "Insert key Pressed" << std::endl;
		std::cout << Process::Hwnd << std::endl;
		std::cout << Process::WindowRight << std::endl;
		std::cout << Process::WindowWidth << std::endl;	
		std::cout << Process::WindowBottom << std::endl;
		std::cout << Process::WindowHeight << std::endl;

	}
	if ((GetAsyncKeyState(VK_END) & 1))
	{
		ShowDemoMenu = !ShowDemoMenu;
		std::cout << "End key Pressed" << std::endl;
	}

	if (bChanged) writeInformation();
}

void checkAppAlive() // TODO: release
{
	OPTICK_EVENT();
} 

void cheatCycle()   // TODO: release
{
	OPTICK_EVENT();
}   

bool CreateDeviceD3D(HWND hWnd)
{
	OPTICK_EVENT();
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Process::WindowWidth;
	sd.BufferDesc.Height = Process::WindowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 144;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &DirectX11Interface::g_pSwapChain, &DirectX11Interface::pd3dDevice, &featureLevel, &DirectX11Interface::pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	OPTICK_EVENT();
	CleanupRenderTarget();
	if (DirectX11Interface::g_pSwapChain) { DirectX11Interface::g_pSwapChain->Release(); DirectX11Interface::g_pSwapChain = nullptr; }
	if (DirectX11Interface::pd3dDeviceContext) { DirectX11Interface::pd3dDeviceContext->Release(); DirectX11Interface::pd3dDeviceContext = nullptr; }
	if (DirectX11Interface::pd3dDevice) { DirectX11Interface::pd3dDevice->Release(); DirectX11Interface::pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	OPTICK_EVENT();
	ID3D11Texture2D* pBackBuffer;
	DirectX11Interface::g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	DirectX11Interface::pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &DirectX11Interface::g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	OPTICK_EVENT();
	if (DirectX11Interface::g_mainRenderTargetView) { DirectX11Interface::g_mainRenderTargetView->Release(); DirectX11Interface::g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


bool DirectXInit()
{
	OPTICK_EVENT();

	D3DPRESENT_PARAMETERS Params = { 0 };
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = OverlayWindow::Hwnd;
	//Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8; //D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = Process::WindowWidth;
	Params.BackBufferHeight = Process::WindowHeight;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	Params.BackBufferCount = 1;
	Params.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

	if (!(CreateDeviceD3D(OverlayWindow::Hwnd)))
	{
		CleanupDeviceD3D();
		DebugBreak();
		::UnregisterClass(OverlayWindow::WindowClass.lpszClassName, OverlayWindow::WindowClass.hInstance);	
		return true;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::GetIO().WantCaptureMouse; //|| ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	
	ImGui_ImplDX11_Init(DirectX11Interface::pd3dDevice, DirectX11Interface::pd3dDeviceContext);
	ImGui_ImplWin32_Init(OverlayWindow::Hwnd);
	DirectX11Interface::pd3dDevice->Release();
	return false;
}

/*
void SetupWindow()
{
	OPTICK_EVENT();
	if (fProcess.__HWND) {
		static RECT TempRect = { NULL };
		static POINT TempPoint;
		GetClientRect(fProcess.__HWND, &TempRect);
		ClientToScreen(fProcess.__HWND, &TempPoint);
		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		Process::WindowWidth = TempRect.right;
		Process::WindowHeight = TempRect.bottom;
	}
	Process::WindowWidth = 2560;
	Process::WindowHeight = 1440;

	OverlayWindow::Hwnd = CreateWindowExA(NULL, OverlayWindow::Name, OverlayWindow::Name, WS_POPUP | WS_VISIBLE, Process::WindowLeft, Process::WindowTop, Process::WindowWidth, Process::WindowHeight, nullptr, nullptr, 0, nullptr);
	DwmExtendFrameIntoClientArea(OverlayWindow::Hwnd, &DirectX11Interface::Margin);
	SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	ShowWindow(OverlayWindow::Hwnd, SW_SHOW | SW_MAXIMIZE);
	UpdateWindow(OverlayWindow::Hwnd);
}
*/

bool CreateOverlay()
{	
	
	OPTICK_EVENT();

	
	// Create application window
	const wchar_t* windowtitle = OverlayWindow::Name;
	{
		OPTICK_EVENT("CREATE WINDOW CLASS");
		OverlayWindow::WindowClass = { sizeof(WNDCLASSEX), ACS_TRANSPARENT, WndProc,0L,0L, GetModuleHandle(NULL),NULL, LoadCursor(NULL, IDC_ARROW),	NULL, 	NULL,	OverlayWindow::Name,	NULL };
	}

	{
		OPTICK_EVENT("REGISTER WINDOW CLASS");
		::RegisterClassEx(&OverlayWindow::WindowClass);
	}

	{
		OPTICK_EVENT("CREATE WINDOW");
		OverlayWindow::Hwnd = ::CreateWindowExW( WS_EX_TOPMOST | WS_EX_WINDOWEDGE | WS_EX_TRANSPARENT,
			windowtitle,
			windowtitle,
			WS_THICKFRAME  | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
			0, 0, Process::WindowWidth+20, Process::WindowHeight+30, 0, 0, 0, 0);
	}
	
	{
		OPTICK_EVENT("SetWindowLong");
		::SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
	}
	
	/*
	{
		OPTICK_EVENT("SetLayeredWindowAttributes");
		::SetLayeredWindowAttributes(OverlayWindow::Hwnd, 0, 0, LWA_ALPHA);
	}
	*/
	//::SetLayeredWindowAttributes(OverlayWindow::Hwnd, 0, 255, LWA_COLORKEY);
	

	//and you should call DwmExtendFrameIntoClientArea after your SetLayeredWindowAttributes calls, iirc.

	//OverlayWindow::Hwnd = fProcess.__HWND;
	{
		OPTICK_EVENT("DwmExtendFrameIntoClientArea");
		MARGINS margins = { -1,0,0,0 };
		//DwmExtendFrameIntoClientArea(OverlayWindow::Hwnd, &margins);
	}
	bool bDirectInited = false;
	{
		OPTICK_EVENT("DirectXInit");
		bool bDirectInited = DirectXInit();
		//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(15.f, 25.f, 65.0f, 1.0f));
	}

	// Show the window
	{
		OPTICK_EVENT("Show and update WINDOW");
		::ShowWindow(OverlayWindow::Hwnd, SW_SHOWDEFAULT);
		::UpdateWindow(OverlayWindow::Hwnd);
	}
	// Setup ImGui binding
	
	{
	//Set To clickable not clickable
		OPTICK_EVENT("ChangeClickability");
		ChangeClickability(true, OverlayWindow::Hwnd);
	}

	
	return false;
	//return bDirectInited;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Draw()
{
	OPTICK_EVENT();
	
	{

		char FpsInfo[64];
		sprintf(FpsInfo, "Overlay FPS: %0.f", ImGui::GetIO().Framerate);
		//DrawStrokeText(30, 44, &White, FpsInfo);	

		if (ShowDemoMenu)ImGui::ShowDemoWindow(&ShowDemoMenu);

		
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoBackground;
		//window_flags |= ImGuiWindowFlags_NoTitleBar;
		

		// etc.
		bool* open_ptr = nullptr;
		int* pHealth = &data1.Health;
		// etc.
		ImGui::SetNextWindowBgAlpha(0.2f);
		ImGui::Begin("Cheat menu"); 
		//ImGui::Begin("Cheat menu", open_ptr, window_flags);                          // Create a window called "Hello, world!" and append into it.

		std::string str = "HEALTH: " + std::to_string(*pHealth);
		ImGui::Text(FpsInfo);               // Display some text (you can use a format strings too)	
		ImGui::Text(str.c_str());               // Display some text (you can use a format strings too)	

		ImGui::SameLine(300);
		
		str = "VSync";
		bool* pUseVsync = &bUseVsync;
		ImGui::Checkbox(str.c_str(), pUseVsync);


		str = "GodMode";
		bool* pGodModToEnable = &bGodModToEnable;
		ImGui::Checkbox(str.c_str(), pGodModToEnable);


		/*
		std::string str = std::to_string(data.influence);
		//ImGui::TextColoredV(ImVec4(0.10f, 0.20f, 0.80f, 1.00f), "%s", (char*)str.c_str());            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(90, 120, 220, 255));
		ImGui::InputText("##text1", (char*)str.c_str(), sizeof((char*)str.c_str()));
		ImGui::PopStyleColor();


		str = std::to_string(data.power);
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(15, 220, 15, 255));
		ImGui::InputText("##text2", (char*)str.c_str(), sizeof((char*)str.c_str()));
		ImGui::PopStyleColor();

		//ImGui::TextColoredV(ImVec4(0.10f, 0.55f, 0.10f, 1.00f), "%s", (char*)str.c_str());            // Edit 1 float using a slider from 0.0f to 1.0f

		str = std::to_string(data.relic);
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 100, 100, 255));
		ImGui::InputText("##text3", (char*)str.c_str(), sizeof((char*)str.c_str()));
		ImGui::PopStyleColor();
		//ImGui::TextColoredV(ImVec4(0.60f, 0.20f, 0.10f, 1.00f), "%s", (char*)str.c_str());            // Edit 1 float using a slider from 0.0f to 1.0f





		ImGui::Text("Classic adding resources");               // Display some text (you can use a format strings too)	

		//ImGui::SliderFloat("Influence", &fInfluence, 0.0f, 25000.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f



		//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
		GameData addData;




		if (ImGui::Button("ButtonAdd")) // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			
		}

		//ImGui::SameLine();

		ImGui::Text("Credit adding resources");               // Display some text (you can use a format strings too)

		ImGui::SliderFloat("Influence2", &fInfluenceCred, 0.0f, 25000.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("Power2", &fPowerCred, 0.0f, 25000.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("Relic2", &fRelicCred, 0.0f, 25000.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f

		ImGui::SliderFloat("Percents", &fPersent, 5.0f, 25.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderInt("TimeInMinutes", &iTime, 2.0f, 15.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f


		//ImGui::Text("counter = %d", counter);
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		*/

		ImGui::End();
	}
	//ImGui::EndFrame();

}

void Render() 
{
	OPTICK_EVENT();
	/*
	if (ShowMenuSwitched)
	{
		if (ShowMenu)
		{
			::ShowWindow(OverlayWindow::Hwnd, SW_HIDE);
			::UpdateWindow(OverlayWindow::Hwnd);
		}
		else
		{
			::ShowWindow(OverlayWindow::Hwnd, SW_SHOWMAXIMIZED);
			::UpdateWindow(OverlayWindow::Hwnd);
		}
		ShowMenuSwitched = false;
	}
	*/
	

	ImVec4 clear_color = ImVec4(0.15f, 0.25f, 0.65f, 0.25f);
	//float clear_color[4] = {15.f, 25.f, 65.0f, 1.0f};


	ImGui::GetIO().MouseDrawCursor = ShowMenu;
	{
		OPTICK_EVENT("ImGui_ImplDX11_NewFrame");
		ImGui_ImplDX11_NewFrame();
	}
	{
		OPTICK_EVENT("ImGui_ImplWin32_NewFrame");
		ImGui_ImplWin32_NewFrame();
	}

	
	{
		OPTICK_EVENT("ImGui::NewFrame()");
		ImGui::NewFrame();
	}

	Draw();

	{
		OPTICK_EVENT("ImGui::Render()");
		ImGui::Render();
	}
	const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	{
		OPTICK_EVENT("pd3dDeviceContext->OMSetRenderTargets");
		DirectX11Interface::pd3dDeviceContext->OMSetRenderTargets(1, &DirectX11Interface::g_mainRenderTargetView, NULL);
	}
	{
		OPTICK_EVENT("pd3dDeviceContext->ClearRenderTargetView");
		DirectX11Interface::pd3dDeviceContext->ClearRenderTargetView(DirectX11Interface::g_mainRenderTargetView, clear_color_with_alpha);
	}
	{
		OPTICK_EVENT("ImGui_ImplDX11_RenderDrawData");
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	
	{
		OPTICK_EVENT("g_pSwapChain->Present");
		if (bUseVsync)
		{
			DirectX11Interface::g_pSwapChain->Present(1, 0); // Present with vsync
		}
		else
		{
			DirectX11Interface::g_pSwapChain->Present(0, 0);			
		}
	}
	
	


}

size_t findInString(std::vector<BYTE>& whereFound,std::vector<BYTE>& whatFound) 
{
	OPTICK_EVENT();

	for (UINT j = 0; j < whereFound.size(); j++)
	{
		UINT enterPosition = 0;
		bool firstEntrense = false;
		bool lastElementsEqual = false;
		int totalElementEquals = 0;

		UINT nextElement = 0;
		for (UINT i = 0; i < whatFound.size(); i++)
		{
			if ((j+ nextElement) >= whereFound.size()) break;


			if (whereFound[j+ nextElement] == whatFound[i])
			{
				if (!firstEntrense)
				{
					enterPosition = j;
					firstEntrense = true;
				}

				lastElementsEqual = true;
				totalElementEquals++;

				if (totalElementEquals == whatFound.size())
				{
					return enterPosition;
				}

				nextElement++;
			}
			else
			{
				lastElementsEqual = false;
				totalElementEquals = 0;
				enterPosition = 0;
				bool firstEntrense = false;
				break;
			}
			//std::cout << " i: " << i << " j: " << j;

		}
	}


	return 0xffffffff; // not found
}

DWORD searchInMemory(std::vector<BYTE>& arr, DWORD baseAddr = 0x11FF0000)
{
	std::string str;
	for (int i = 0; i < arr.size(); i++)
	{
		str += arr[i];
	}
		
	//DWORD baseAddr = 0x11FF0000; //fProcess32.__dwordClient;
	bool isAddressNotFound = true;
	

	//testAddress += "abcd1234qwer";

	const UINT memoryScanSize = 16000;

	char* testString = new char[memoryScanSize+1];
	DWORD iFounded = 0x000000;
	while (isAddressNotFound && baseAddr < 0x7FFFFFFF)
	{
		{
			OPTICK_EVENT("ReadProccessMemory")
			ReadProcessMemory(fProcess32.__HandleProcess, (PBYTE*)(baseAddr), //(player * EntityLoopDistance)
				testString, memoryScanSize, 0);
		}
		std::vector<BYTE> sourceBytes;

	
		{
			OPTICK_EVENT("FillArray")
			for (int i = 0; i < memoryScanSize; i++)
			{
				sourceBytes.emplace_back(testString[i]);
			}
		}
		


		isAddressNotFound = true;

		
		{
			OPTICK_EVENT("findInString")
			iFounded = findInString(sourceBytes, arr);
		}
		if (iFounded != 0xffffffff)
		{
			isAddressNotFound = false;

		}
		else
		{
			baseAddr += memoryScanSize;
		}
		
		
		
		
		

	}
	delete[] testString;

	return (baseAddr + iFounded);
}

UINT scanValues()
{
	OPTICK_FRAME("ScanValues");
		// 8B 42 2C 89 45 F8 03 - gold
		// 8B 88 4C 03 00 00 89 4D F8 03 - cells
		// 
		// 89 51 1C 8B 45 08 8B 48 08 89 4D E0 - unlimited ammo
		// FC 89 51 54 8B 45 08 - kills does not resets on hit -- 89 51 54 (AOB + 1) change on 3 nops 90 90 90

		// F2 0F 11 50 78 8B 45 F0 - no skill cooldown
		// 8B 91 98 02 00 00 89 55 * 8B 41 - unlimited jumps

		// 89 82 04 01 00 00 8B 4D 0C GodMod
		// 89 82 04 01 00 00 - disable GodMod


	std::vector<BYTE> arrayOfBytes;
	
	{
		// 89 82 04 01 00 00 8B 4D 0C // // godMode no hit + one hit kill
		OPTICK_EVENT("ScanValuesAOB#GodMode");
		arrayOfBytes.emplace_back(0x89);
		arrayOfBytes.emplace_back(0x82);
		arrayOfBytes.emplace_back(0x04);
		arrayOfBytes.emplace_back(0x01);
		arrayOfBytes.emplace_back(0x00);
		arrayOfBytes.emplace_back(0x00);
		arrayOfBytes.emplace_back(0x8B);
		arrayOfBytes.emplace_back(0x4D);
		arrayOfBytes.emplace_back(0x0C);

		instructionPositionGodMode = searchInMemory(arrayOfBytes,0x11BFFFFF); //AOBScan for
#ifdef DEBUG
		std::cout << "instructionPositionGodMode: 0x" << std::hex << instructionPositionGodMode << std::dec << std::endl;
#endif
		arrayOfBytes.clear();
	}
	{
		// FC 89 51 54 8B 45 08
		OPTICK_EVENT("ScanValuesAOB#ResetKills");
		arrayOfBytes.emplace_back(0xFC);
		arrayOfBytes.emplace_back(0x89);
		arrayOfBytes.emplace_back(0x51);
		arrayOfBytes.emplace_back(0x54);
		arrayOfBytes.emplace_back(0x8B);
		arrayOfBytes.emplace_back(0x45);
		arrayOfBytes.emplace_back(0x08);

		instructionPositionResetKills = searchInMemory(arrayOfBytes, 0x122FFFFF); //AOBScan for
#ifdef DEBUG
		std::cout << "instructionPositionResetKills: 0x" << std::hex << instructionPositionResetKills << std::dec << std::endl;
#endif
		arrayOfBytes.clear();
	}

	{

		//89 51 1C 8B 45 08 8B 48 08 89 4D E0
		OPTICK_EVENT("ScanValuesAOB#UnlimitedAmmo");
		arrayOfBytes.emplace_back(0x89);
		arrayOfBytes.emplace_back(0x51);
		arrayOfBytes.emplace_back(0x1C);
		arrayOfBytes.emplace_back(0x8B);
		arrayOfBytes.emplace_back(0x45);
		arrayOfBytes.emplace_back(0x08);
		arrayOfBytes.emplace_back(0x8B);
		arrayOfBytes.emplace_back(0x48);
		arrayOfBytes.emplace_back(0x08);
		arrayOfBytes.emplace_back(0x89);
		arrayOfBytes.emplace_back(0x4D);
		arrayOfBytes.emplace_back(0xE0);

		instructionPositionUnlimitedAmmo = searchInMemory(arrayOfBytes, 0x13CFFFFF); //AOBScan for
#ifdef DEBUG
		std::cout << "instructionPositionUnlimitedAmmo: 0x" << std::hex << instructionPositionUnlimitedAmmo << std::dec << std::endl;
#endif
		arrayOfBytes.clear();
	}
	OPTICK_EVENT("ScanValues#End");
	OPTICK_FRAME_FLIP();

	return 0;
}


void mainLoop()
{

	OPTICK_FRAME("Main Loop#2");	
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		switch (msg.message)
		{
		case  WM_QUIT:
			//switchGodMod(false);
				bExit = true;
				return;
			break;
		}
	}

	checkAppAlive();
	findActiveAddress();
	readInformation();
	takeInputLoop();
	
	//HookD3D();
	Render();
	SleepTime(0);
	
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{


	case  WM_QUIT:


		

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		if (DirectX11Interface::pd3dDevice != nullptr)
		{
			CleanupDeviceD3D();	
		}
		
		DestroyWindow(OverlayWindow::Hwnd);
		::UnregisterClassW(OverlayWindow::WindowClass.lpszClassName, OverlayWindow::WindowClass.hInstance);

		break;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;

	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

int main()
{
	SleepTime(1000);
	OPTICK_FRAME("Main Thread#1");
	
#ifdef OPTIC_PAUSE
	int tick = 0;
	while (tick < 4*5)
	{
		SleepTime(250);
		tick++;
		std::cout << tick << std::endl;
	}
#endif
	
	std::cout << "Started" << std::endl;
	
	Process::WindowLeft = Process::WindowTop = 0;
	Process::WindowRight = Process::WindowWidth = 400;	
	Process::WindowHeight = Process::WindowBottom = 300;
	
	//savedGM.qWORD = 0x00000000012cbb89;
	
	healthOffsets.emplace_back(0x00AFF8A8); // health
	healthOffsets.emplace_back(0x18);
	healthOffsets.emplace_back(0x8C);
	healthOffsets.emplace_back(0x64);
	healthOffsets.emplace_back(0x104);
	



	// 
	/*
	healthOffsets.emplace_back(0x00AFF8A8); // health
	healthOffsets.emplace_back(0x18);
	healthOffsets.emplace_back(0x5C);
	healthOffsets.emplace_back(0x38);
	healthOffsets.emplace_back(0x64);
	healthOffsets.emplace_back(0x104);
	*/

	fProcess32.RunProcess("deadcells.exe", "Dead Cells");
	
	std::cout << "Game: found... Running." << std::endl;
	ImGui_ImplWin32_EnableDpiAwareness();
	
	Process::Hwnd = fProcess32.__HWND;
	OverlayWindow::Name = L"DeadCells Trainer";
	
	//std::thread cheatThread(cheatCycle);
	//cheatThread.detach();

#ifndef  _DEBUG	
	//if (CreateConsole == false)
	//ShowWindow(GetConsoleWindow(), SW_HIDE);	
#endif // ! _DEBUG


	

	bool WindowFocus = false;
	bool bRepeat = true;
	
	/*
	while (WindowFocus == false || bRepeat) {
		bRepeat = false;
		DWORD ForegroundWindowProcessID;
		GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
		if (GetProcessId(TargetProcess) == ForegroundWindowProcessID) {
			Process::ID = GetCurrentProcessId();
			Process::Handle = GetCurrentProcess();
			Process::Hwnd = fProcess.__HWND;

			RECT TempRect;
			GetWindowRect(Process::Hwnd, &TempRect);
			Process::WindowWidth = TempRect.right - TempRect.left;
			Process::WindowHeight = TempRect.bottom - TempRect.top;
			Process::WindowLeft = TempRect.left;
			Process::WindowRight = TempRect.right;
			Process::WindowTop = TempRect.top;
			Process::WindowBottom = TempRect.bottom;

			if (Process::WindowLeft < 0 || Process::WindowWidth <= 0 || Process::WindowHeight <= 0) bRepeat = true;


			char TempTitle[MAX_PATH];
			GetWindowTextA(Process::Hwnd, TempTitle, sizeof(TempTitle));
			Process::Title = TempTitle;

			char TempClassName[MAX_PATH];
			GetClassNameA(Process::Hwnd, TempClassName, sizeof(TempClassName));
			Process::ClassName = TempClassName;

			char TempPath[MAX_PATH];
			GetModuleFileNameEx(Process::Handle, nullptr, TempPath, sizeof(TempPath));
			Process::Path = TempPath;

			WindowFocus = true;
		}
	}
	*/

	//SetupWindow();
	CreateOverlay();
	//DirectXInit();

	//HookD3D();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::GetIO().WantCaptureMouse; //|| ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();

	scanValues();


	while (!bExit && !(GetAsyncKeyState(VK_F8) & 1))
	{
		mainLoop();
	}
	
	SleepTime(10);

	if (bGodModEnabled)
	{
		switchGodMod(false);		
	}

	CleanupDeviceD3D();
	//WriteMem(ogPresent, ogBytes, PRESENT_STUB_SIZE);
	//VirtualFree((void*)ogPresentTramp, 0x1000, MEM_RELEASE);
	//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, pHandle, 0, 0);
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
