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
#include "include/backends/imgui_impl_dx9.h"
#include "Functions.h"
#include "Overlay.h"


#include <d3d9.h>
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

LPCSTR TargetProcess = "Soulstorm.exe"; //"D3D9Test.exe";

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
	WNDCLASSEXA WindowClass;
	HWND Hwnd;
	LPCSTR Name = "W40kSS ImGui";
}

namespace DirectX9Interface {
	IDirect3D9Ex* Direct3D9 = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;
	D3DPRESENT_PARAMETERS pParams = { NULL };
	MARGINS Margin = { 0, 0, Process::WindowLeft, Process::WindowBottom };
	MSG Message = { NULL };
}


void Render();
bool DirectXInit();


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static bool ShowMenu = false;
static bool bWriteData = false;
static bool bHaveCredit = false;
static bool CreateConsole = true;
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



// Helper functions

bool DirectXInit() {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9Interface::Direct3D9))) {
		DebugBreak();
		return false;
	}

	D3DPRESENT_PARAMETERS Params = { 0 };
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = OverlayWindow::Hwnd;
	Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = Process::WindowWidth;
	Params.BackBufferHeight = Process::WindowHeight;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if (FAILED(DirectX9Interface::Direct3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, OverlayWindow::Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9Interface::pDevice))) {
		DirectX9Interface::Direct3D9->Release();
		DebugBreak();
		return false;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::GetIO().WantCaptureMouse; //|| ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplWin32_Init(OverlayWindow::Hwnd);
	ImGui_ImplDX9_Init(DirectX9Interface::pDevice);
	DirectX9Interface::Direct3D9->Release();
	return true;
}

void SetupWindow() {
	OverlayWindow::WindowClass = {
		sizeof(WNDCLASSEXA), 0, WndProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, OverlayWindow::Name, LoadIcon(nullptr, IDI_APPLICATION)
	};

	RegisterClassExA(&OverlayWindow::WindowClass);
	if (Process::Hwnd) {
		static RECT TempRect = { NULL };
		static POINT TempPoint;
		GetClientRect(Process::Hwnd, &TempRect);
		ClientToScreen(Process::Hwnd, &TempPoint);
		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		Process::WindowWidth = TempRect.right;
		Process::WindowHeight = TempRect.bottom;
	}

	OverlayWindow::Hwnd = CreateWindowExA(NULL, OverlayWindow::Name, OverlayWindow::Name, WS_POPUP | WS_VISIBLE, Process::WindowLeft, Process::WindowTop, Process::WindowWidth, Process::WindowHeight, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(OverlayWindow::Hwnd, &DirectX9Interface::Margin);
	SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	ShowWindow(OverlayWindow::Hwnd, SW_SHOW);
	UpdateWindow(OverlayWindow::Hwnd);
}

DWORD WINAPI ProcessCheck(LPVOID lpParameter) {
	while (true) {
		if (Process::Hwnd != nullptr) {
			if (GetProcessId(TargetProcess) == 0) {
				exit(0);
			}
		}
	}
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Draw() {
	char FpsInfo[64];
	sprintf(FpsInfo, "Overlay FPS: %0.f", ImGui::GetIO().Framerate);
	RGBA White = { 255,255,255,255 };
	DrawStrokeText(30, 44, &White, FpsInfo);

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

		ImGui::SliderFloat("Influence", &fInfluence, 0.0f, 25000.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("Power", &fPower, 0.0f, 25000.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("Relic", &fRelic, 0.0f, 25000.0f, nullptr, 1);            // Edit 1 float using a slider from 0.0f to 1.0f



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
			std::thread creditThread(CreateCredit, std::move(data), std::move(fPersent), std::move(iTime));
			creditThread.detach();
			bHaveCredit = true;
		}

		//ImGui::Text("counter = %d", counter);
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}



	ImGui::EndFrame();

}

void Render() {
	if (GetAsyncKeyState(VK_INSERT) & 1) ShowMenu = !ShowMenu;

	ImGui::GetIO().MouseDrawCursor = ShowMenu;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	Draw();
	

	ImGui::EndFrame();

	DirectX9Interface::pDevice->SetRenderState(D3DRS_ZENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	DirectX9Interface::pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

	DirectX9Interface::pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (DirectX9Interface::pDevice->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		DirectX9Interface::pDevice->EndScene();
	}

	HRESULT result = DirectX9Interface::pDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && DirectX9Interface::pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

void MainLoop()
{
	bool done = false;

	while (!done)
	{
		static RECT OldRect;
		// Poll and handle messages (inputs, window resize, etc.)
	   // See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		HWND ForegroundWindow = GetForegroundWindow();
		if (ForegroundWindow == Process::Hwnd) {
			HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
			SetWindowPos(OverlayWindow::Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}


		RECT TempRect;
		POINT TempPoint;
		ZeroMemory(&TempRect, sizeof(RECT));
		ZeroMemory(&TempPoint, sizeof(POINT));

		GetClientRect(Process::Hwnd, &TempRect);
		ClientToScreen(Process::Hwnd, &TempPoint);

		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = Process::Hwnd;

		POINT TempPoint2;
		GetCursorPos(&TempPoint2);
		io.MousePos.x = TempPoint2.x - TempPoint.x;
		io.MousePos.y = TempPoint2.y - TempPoint.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else {
			io.MouseDown[0] = false;
		}

		if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
			OldRect = TempRect;
			Process::WindowWidth = TempRect.right;
			Process::WindowHeight = TempRect.bottom;
			DirectX9Interface::pParams.BackBufferWidth = Process::WindowWidth;
			DirectX9Interface::pParams.BackBufferHeight = Process::WindowHeight;
			SetWindowPos(OverlayWindow::Hwnd, (HWND)0, TempPoint.x, TempPoint.y, Process::WindowWidth, Process::WindowHeight, SWP_NOREDRAW);
		}


		// Start the Dear ImGui frame

		Render();
	}
}


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


	case  WM_QUIT:

		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		if (DirectX9Interface::pDevice != nullptr) {
			DirectX9Interface::pDevice->EndScene();
			DirectX9Interface::pDevice->Release();
		}
		if (DirectX9Interface::Direct3D9 != nullptr) {
			DirectX9Interface::Direct3D9->Release();
		}
		DestroyWindow(OverlayWindow::Hwnd);
		UnregisterClassA(OverlayWindow::WindowClass.lpszClassName, OverlayWindow::WindowClass.hInstance);

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

int main() {
	offsets.emplace_back(0x0083CD8C); //address 
	offsets.emplace_back(0x10); //offset1
	offsets.emplace_back(0x60); //offset2
	offsets.emplace_back(0x8); //offset3
	offsets.emplace_back(0x60); //offset4
	offsets.emplace_back(0x188); //offset5

	fProcess.RunProcess(TargetProcess);
	cout << "Game Found! Running ..." << endl;

	findActiveAddress();

	ImGui_ImplWin32_EnableDpiAwareness();

	OverlayWindow::Hwnd = fProcess.__HWND;
	OverlayWindow::Name = "W40kSS Trainer";

	std::thread cheatThread(cheatCycle);
	cheatThread.detach();

	if (CreateConsole == false)
		ShowWindow(GetConsoleWindow(), SW_HIDE);

	bool WindowFocus = false;
	bool bRepeat = true;
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

	OverlayWindow::Name = "KEK V";
	SetupWindow();
	DirectXInit();
	CreateThread(0, 0, ProcessCheck, 0, 0, 0);
	while (TRUE) {
		MainLoop();
	}
}