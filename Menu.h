#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <cstdio>
#include <string>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#define DETOURS_VERSION 0x4c0c1
#include "detours.h"

class Menu
{
public:
	Menu();
	~Menu();
	static LRESULT CALLBACK WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void InitMenu(IDXGISwapChain* pSwapChain);
	static bool IsInitialised();
	static IDXGISwapChain* GetSwapChain();
	static void DrawMenu();
	static void ShutdownMenu();
	static HRESULT GetDeviceAndContextFromSwapChain(IDXGISwapChain* pSwapChain, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
};

