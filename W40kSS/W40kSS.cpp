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

#include "include/imgui.h"
#include "include/backends/imgui_impl_win32.h"
#include "include/backends/imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>



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


#ifdef _DEBUG
//#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


static bool bWriteData = false;
static bool bHaveCredit = false;
static float fInfluence, fPower, fRelic = 0.0f;

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

void readInformation(GameData &data)
{


	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + offsets[offsets.size()-1]), //(player * EntityLoopDistance)
		&data, sizeof(GameData), 0);

	/*
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + dataOffset4), //(player * EntityLoopDistance)
		&gameData, sizeof(GameData), 0);
	*/
	/*
	ReadProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressRequisition + requisitionOffset4), //(player * EntityLoopDistance)
		&requisition, sizeof(DWORD), 0);
	*/
}

void writeInformation(GameData &data, int mode)
{

	switch (mode)
	{
	case GameModes::Game:

		
		WriteProcessMemory(fProcess.__HandleProcess, (PBYTE*)(activeAddressGamedata + offsets[offsets.size()-1]), //(player * EntityLoopDistance)
			&data, sizeof(GameData), 0);

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


void addResources(GameData &data)
{
	GameData currentData;
	readInformation(currentData);

	currentData.influence += data.influence;
	currentData.power += data.power;
	currentData.relic += data.relic;

	writeInformation(currentData, GameModes::Game);
}

void cheatCycle()
{
	GameData data;
	while (!((GetAsyncKeyState(VK_F10) & 1)))
	{
		
		readInformation(data);


		if ((GetAsyncKeyState(VK_F1) & 1))
		{
			std::cout
				<< "Influence: " << data.influence << " "
				<< "Power: " << data.power << " "
				<< "Relic: " << data.relic << " "
				//<< "unknownData1: " << gameData.unknownData1 << " "
				//<< "unknownData2: " << gameData.unknownData2 << " "
				//<< "requisition: " << requisition << " "
				<< std::endl;
		}

		

		int currentGameMode = GameModes::Unknowns;

		/*

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
		*/

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
			data.influence += fInfluence;
			data.power += fPower;
			data.relic += fRelic;

			writeInformation(data,currentGameMode);
		}
		Sleep(20);

	}
	Sleep(100);
}

int CreateCredit(GameData gCredit, float fPersent , int iTimeInMinutes)
{
	

	GameData initialCredit = gCredit;
	GameData currentData;
	double dTimeInSeconds = iTimeInMinutes * 60;
	double dTimeInSecondsLeft = dTimeInSeconds;

	float fPers = (fPersent / 100.f + 1.f);

	gCredit.influence	*= fPers;
	gCredit.power		*= fPers;
	gCredit.relic		*= fPers;

	initialCredit = gCredit;
	readInformation(currentData);

	currentData.influence += gCredit.influence;
	currentData.power += gCredit.power;
	currentData.relic += gCredit.relic;


	writeInformation(currentData, GameModes::Game);
	
	auto clockLast = std::chrono::steady_clock::now();

	

	while(gCredit.influence > 0 || gCredit.power > 0 || gCredit.relic > 0)
	{
		readInformation(currentData);
		auto clockNow = std::chrono::steady_clock::now();

		
		std::chrono::duration<double> elapsed_seconds_this_cicle = clockNow - clockLast;
		double dSecondsAtThisCicle = elapsed_seconds_this_cicle.count();
		dTimeInSecondsLeft -= dSecondsAtThisCicle;

		float deltaTime = (dSecondsAtThisCicle / dTimeInSeconds);
		float deltaInfluence = deltaTime * initialCredit.influence;
		float deltaPower	 = deltaTime * initialCredit.power;
		float deltaRelic     = deltaTime * initialCredit.relic;

		currentData.influence -= deltaInfluence;
		gCredit.influence     -= deltaInfluence;

		currentData.power -= deltaPower;
		gCredit.power	  -= deltaPower;

		currentData.relic -= deltaRelic;
		gCredit.relic     -= deltaRelic;




		currentData.power	  -= (dSecondsAtThisCicle / dTimeInSeconds) * initialCredit.power;


		currentData.relic     -= (dSecondsAtThisCicle / dTimeInSeconds) * initialCredit.relic;

		clockLast = std::chrono::steady_clock::now();
		writeInformation(currentData, GameModes::Game);

		Sleep(50);
	}
	

	bHaveCredit = false;

	return 0;
}

int main()
{
	offsets.emplace_back(0x0083CD8C); //address 
	offsets.emplace_back(0x10); //offset1
	offsets.emplace_back(0x60); //offset2
	offsets.emplace_back(0x8); //offset3
	offsets.emplace_back(0x60); //offset4
	offsets.emplace_back(0x188); //offset5

	fProcess.RunProcess();
	cout << "Game Found! Running ..." << endl;

	findActiveAddress();
	
	// Create application window
    ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("W40kSS Trainer"), NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("W40kSS Trainer HWND"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	std::thread cheatThread(cheatCycle);
	cheatThread.detach();

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	bool done = false;

	while (!done)
	{
		// Poll and handle messages (inputs, window resize, etc.)
	   // See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			
			static float fInfluenceCred, fPowerCred, fRelicCred = 0.0f;
			static int counter = 0;
			static int iTime = 0;
			static float fPersent = 0;
			
			GameData data;
			readInformation(data);

			ImGui::Begin("Cheat menu");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("Current resources");               // Display some text (you can use a format strings too)	

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

			ImGui::SliderFloat("Influence", &fInfluence, 0.0f, 25000.0f,nullptr,1);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Power", &fPower, 0.0f, 25000.0f, nullptr,1);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Relic", &fRelic, 0.0f, 25000.0f, nullptr,1);            // Edit 1 float using a slider from 0.0f to 1.0f



			//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
			GameData addData;

			addData.influence = fInfluence;
			addData.power = fPower;
			addData.relic = fRelic;


			if (ImGui::Button("ButtonAdd")) // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				addResources(addData);
			}
				
			//ImGui::SameLine();

			ImGui::Text("Credit adding resources");               // Display some text (you can use a format strings too)

			ImGui::SliderFloat("Influence2", &fInfluenceCred, 0.0f, 25000.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Power2", &fPowerCred, 0.0f, 25000.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Relic2", &fRelicCred, 0.0f, 25000.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f

			ImGui::SliderFloat("Percents", &fPersent, 5.0f, 25.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderInt("TimeInMinutes", &iTime, 2.0f, 15.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f


			if (ImGui::Button("ButtonAddCred") && !bHaveCredit) // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				GameData data = { fInfluenceCred , fPowerCred ,fRelicCred };
				std::thread creditThread(CreateCredit, std::move(data), std::move(fPersent) , std::move(iTime));
				creditThread.detach();
				bHaveCredit = true;
			}

			//ImGui::Text("counter = %d", counter);
			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		

		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync



	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
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
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
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
