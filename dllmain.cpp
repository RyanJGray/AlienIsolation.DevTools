// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <cstdio>
#include <fstream>
#include <detours.h>
#include <d3d11.h>

#include "Scaleform.h"
#include "CA.h"
#include "Menu.h"

#if defined _M_IX86
#pragma comment(lib, "detours.lib")
#endif

// Appears to get called for every ActionScript function call from the engine to Scaleform.
// Attempting to even initialise variables inside the hooked function crashes the game with access violations, I tried changing the
// calling convention but it didn't help, and only made it worse.
// I'll have to look at this later but for now it is being left alone.
typedef void(__stdcall* tScaleform_UI_CallASFunction)(int**, char*, char*);
tScaleform_UI_CallASFunction Scaleform_UI_CallASFunction = reinterpret_cast<tScaleform_UI_CallASFunction>(0x0081b060);

typedef HRESULT(WINAPI* tD3D11CreateDeviceAndSwapChain)(
    void* pAdapter,
    D3D_DRIVER_TYPE      DriverType,
    HMODULE              Software,
    UINT                 Flags,
    const void* pFeatureLevels,
    UINT                 FeatureLevels,
    UINT                 SDKVersion,
    const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    IDXGISwapChain** ppSwapChain,
    ID3D11Device** ppDevice,
    void* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext
);

// Menu - D3D11 CreateDeviceAndSwapChain hook.
tD3D11CreateDeviceAndSwapChain d3d11CreateDeviceAndSwapChain = nullptr;

typedef HRESULT(WINAPI* tD3D11Present)(
    IDXGISwapChain* swapChain,
    UINT            SyncInterval,
    UINT            Flags
);

// Menu - D3D11 Present hook.
tD3D11Present d3d11Present = nullptr;


__declspec(noinline)
void __stdcall hScaleform_UI_CallASFunction(int** thisPtr, char* actionScriptFunction, char* actionScriptParameter)
{
    printf_s("[Scaleform::UI::CallASFunction] thisPtr: 0x%p; actionScriptFunction: %s; actionScriptParameter: %s\n", thisPtr, actionScriptFunction, actionScriptParameter);

    Scaleform_UI_CallASFunction(thisPtr, actionScriptFunction, actionScriptParameter);
}

HRESULT WINAPI hD3D11Present(
    IDXGISwapChain* swapChain,
    UINT        SyncInterval,
    UINT        Flags
) {
    Menu::DrawMenu();

    return d3d11Present(swapChain, SyncInterval, Flags);
}

HRESULT WINAPI hD3D11CreateDeviceAndSwapChain(
    void* pAdapter,
    D3D_DRIVER_TYPE      DriverType,
    HMODULE              Software,
    UINT                 Flags,
    const void* pFeatureLevels,
    UINT                 FeatureLevels,
    UINT                 SDKVersion,
    const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    IDXGISwapChain** ppSwapChain,
    ID3D11Device** ppDevice,
    void* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext
) {
    HRESULT res = d3d11CreateDeviceAndSwapChain(
        pAdapter,
        DriverType,
        Software,
        Flags,
        pFeatureLevels,
        FeatureLevels,
        SDKVersion,
        pSwapChainDesc,
        ppSwapChain,
        ppDevice,
        pFeatureLevel,
        ppImmediateContext
    );

    printf_s("[DevTools] Executed D3D11CreateDeviceAndSwapChain hook, for the menu!\n");

    printf_s("[DevTools] Menu::IsInitialised returned %d\n", Menu::IsInitialised());
    printf_s("[DevTools] D3D11CreateDeviceAndSwapChain ppSwapChain = 0x%p\n", *ppSwapChain);
	
    // If the Menu class hasn't already been initialised, initialise it now.
    if (!Menu::IsInitialised())
    {
        printf_s("[DevTools] Initialised menu!\n");
        Menu::InitMenu(*ppSwapChain);

        if (*ppSwapChain)
        {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());

            void** pVMTPresent = *reinterpret_cast<void***>(*ppSwapChain);

            printf_s("[DevTools] pVMTPresent = 0x%p\n", pVMTPresent);
        	
        	// Store reference to the original D3D11Present function from the SwapChain VTable.
            d3d11Present = static_cast<tD3D11Present>(pVMTPresent[8]);
        	
            printf_s("[DevTools] d3d11Present = 0x%p\n", d3d11Present);
            DetourAttach(&reinterpret_cast<PVOID&>(d3d11Present), hD3D11Present);

            const long result = DetourTransactionCommit();
            printf_s("[DevTools] Installed rendering hooks. (result=%ld)\n", result);
		}
    }
	
    return res;
}


BOOL APIENTRY DllMain( HMODULE /*hModule*/,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/
                     )
{
    if (DetourIsHelperProcess())
    {
        return TRUE;
    }

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DetourRestoreAfterWith();
    	
        AllocConsole();
        FILE* consoleOut;
        freopen_s(&consoleOut, "CONOUT$", "w", stdout);

        printf_s("Detours Architecture: " DETOURS_STRINGIFY(DETOURS_BITS) "\n");
        fflush(stdout);
    	
        DetourTransactionBegin();
        printf_s("[Detours] DetourTransactionBegin()\n");
        
        DetourUpdateThread(GetCurrentThread());
        printf_s("[Detours] DetourUpdateThread()\n");

    	// Menu hooks / initialisation code, adapted from Alias Isolation.
        const HMODULE hModule = GetModuleHandleA("d3d11");
    	
        if (hModule)
        {
            printf_s("[DevTools] Got handle to d3d11.dll!\n");
        	
            d3d11CreateDeviceAndSwapChain = reinterpret_cast<tD3D11CreateDeviceAndSwapChain>(GetProcAddress(hModule, "D3D11CreateDeviceAndSwapChain"));
        	
            if (d3d11CreateDeviceAndSwapChain)
            {
                printf_s("[DevTools] Got D3D11CreateDeviceAndSwapChain proc address!\n");
                DetourAttach(&reinterpret_cast<PVOID&>(d3d11CreateDeviceAndSwapChain), hD3D11CreateDeviceAndSwapChain);
            }
            else
            {
                printf_s("[DevTools] GetProcAddress(\"D3D11CreateDeviceAndSwapChain\") failed!\n");
            }
        }
        else
        {
            printf_s("[DevTools] GetModuleHandleA(\"d3d11\") failed: MODULE_NOT_FOUND!\n");
        }

        MessageBox(nullptr, L"Injected!", L"AlienIsolation.DevTools", NULL);

        // Overwrite the game data integrity checks (the game verifies all .PKG files and MODELS_LEVEL.BIN files by comparing them against SHA1 hashes).
		// Here we overwrite the assembly code of the checks in memory with the "NOP" (No Operation) instruction, preventing the game from comparing the hashes.

    	// I need a better way to do this rather than overwriting each address, one at a time...
    	
        DWORD oldProtect;
        auto* patchLocation = reinterpret_cast<char*>(0x009E8BAF);
    	
    	// Patch 1
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
        // Patch 2
        patchLocation = reinterpret_cast<char*>(0x009E8BB0);
    	VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
        // Patch 3
        patchLocation = reinterpret_cast<char*>(0x009E8BB1);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
        // Patch 4
        patchLocation = reinterpret_cast<char*>(0x009E8BB2);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 5
        patchLocation = reinterpret_cast<char*>(0x009E8BB3);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 6
        patchLocation = reinterpret_cast<char*>(0x009E8BB4);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 7
        patchLocation = reinterpret_cast<char*>(0x009E8BB5);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 8
        patchLocation = reinterpret_cast<char*>(0x009E8BB6);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 9
        patchLocation = reinterpret_cast<char*>(0x009E8BB7);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 10
        patchLocation = reinterpret_cast<char*>(0x009E8BB8);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 11
        patchLocation = reinterpret_cast<char*>(0x009E8BB9);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 12
        patchLocation = reinterpret_cast<char*>(0x009E8BBA);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 13
        patchLocation = reinterpret_cast<char*>(0x009E8BBB);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 14
        patchLocation = reinterpret_cast<char*>(0x009E8BBC);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 15
        patchLocation = reinterpret_cast<char*>(0x009E8BBD);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 16
        patchLocation = reinterpret_cast<char*>(0x009E8BBE);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 17
        patchLocation = reinterpret_cast<char*>(0x009E8BBF);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 18
        patchLocation = reinterpret_cast<char*>(0x009E8BC0);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 19
        patchLocation = reinterpret_cast<char*>(0x009E8BC1);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 20
        patchLocation = reinterpret_cast<char*>(0x009E8BC2);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 21
        patchLocation = reinterpret_cast<char*>(0x009E8BC3);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 22
        patchLocation = reinterpret_cast<char*>(0x009E8BC4);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 23
        patchLocation = reinterpret_cast<char*>(0x009E8BC5);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 24
        patchLocation = reinterpret_cast<char*>(0x009E8BC6);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	// Patch 25
        patchLocation = reinterpret_cast<char*>(0x009E8BC7);
        VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy_s(patchLocation, 1, "\x90", 1);
        VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
    	
    	// Attach the Scaleform hooks.
        DetourAttach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::Callback::GameMenu::LoadLevel), CATHODE::Scaleform::Callback::GameMenu::hLoadLevel);
        DetourAttach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::UI::GetLevelPointer), CATHODE::Scaleform::UI::hGetLevelPointer);
        DetourAttach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::UI::LoadLevelUnknownFunc1), CATHODE::Scaleform::UI::hLoadLevelUnknownFunc1);
        DetourAttach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::UI::LoadLevelUnknownFunc2), CATHODE::Scaleform::UI::hLoadLevelUnknownFunc2);
        DetourAttach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::UI::SetNextLevel), CATHODE::Scaleform::UI::hSetNextLevel);
        DetourAttach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::UI::LoadLevel), CATHODE::Scaleform::UI::hLoadLevel);
        DetourAttach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::UI::DispatchRequestToNodeHandler), CATHODE::Scaleform::UI::hDispatchRequestToNodeHandler);
        //DetourAttach(&reinterpret_cast<PVOID&>(Scaleform_UI_CallASFunction), hScaleform_UI_CallASFunction);

        // Attach the CA hooks.
        DetourAttach(&reinterpret_cast<PVOID&>(CATHODE::CA::FILE_HASHES::verify_integrity), CATHODE::CA::FILE_HASHES::h_verify_integrity);
        //DetourAttach(&reinterpret_cast<PVOID&>(CATHODE::CA::FILE_HASHES::terminate_game), CATHODE::CA::FILE_HASHES::h_terminate_game);
        DetourAttach(&reinterpret_cast<PVOID&>(CATHODE::CA::FILE_HASHES::sha1_portable_hash), CATHODE::CA::FILE_HASHES::h_sha1_portable_hash);
    	
        const long result = DetourTransactionCommit();
        printf_s("[DevTools] Installed hooks. (result=%ld)\n", result);

        if (result != NO_ERROR)
        {
            switch (result)
            {
            case ERROR_INVALID_BLOCK:
                printf_s("[Detours] The function referenced is too small to be detoured (%ld)\n", result);
                break;
            case ERROR_INVALID_HANDLE:
                printf_s("[Detours] The ppPointer parameter is null or points to a null pointer (%ld)\n", result);
                break;
            case ERROR_INVALID_OPERATION:
                printf_s("[Detours] No pending transaction exists (%ld)\n", result);
                break;
            case ERROR_NOT_ENOUGH_MEMORY:
                printf_s("[Detours] Not enough memory exists to complete the operation (%ld)\n", result);
                break;
            case ERROR_INVALID_PARAMETER:
                printf_s("[Detours] An invalid parameter has been passed (%ld)\n", result);
                break;
            default:
                printf_s("[Detours] Unknown error (%ld)\n", result);
                break;
            }
        }

        if (result == NO_ERROR)
        {
            printf_s("[DevTools] Hooks active!\n\n");
        }
        else
        {
            printf_s("[DevTools] Hooks failed!\n");
        }
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

    	// Detach the rendering hooks.
        DetourDetach(&reinterpret_cast<PVOID&>(d3d11CreateDeviceAndSwapChain), hD3D11CreateDeviceAndSwapChain);
        DetourDetach(&reinterpret_cast<PVOID&>(d3d11Present), hD3D11Present);

    	// Detach the Scaleform hooks.
        DetourDetach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::Callback::GameMenu::LoadLevel), CATHODE::Scaleform::Callback::GameMenu::hLoadLevel);
        DetourDetach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::UI::GetLevelPointer), CATHODE::Scaleform::UI::hGetLevelPointer);
        DetourDetach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::UI::LoadLevelUnknownFunc1), CATHODE::Scaleform::UI::hLoadLevelUnknownFunc1);
        DetourDetach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::UI::LoadLevelUnknownFunc2), CATHODE::Scaleform::UI::hLoadLevelUnknownFunc2);
        DetourDetach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::UI::SetNextLevel), CATHODE::Scaleform::UI::hSetNextLevel);
        DetourDetach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::UI::LoadLevel), CATHODE::Scaleform::UI::hLoadLevel);
        DetourDetach(&reinterpret_cast<PVOID&>(CATHODE::Scaleform::UI::DispatchRequestToNodeHandler), CATHODE::Scaleform::UI::hDispatchRequestToNodeHandler);
        //DetourDetach(&reinterpret_cast<PVOID&>(Scaleform_UI_CallASFunction), hScaleform_UI_CallASFunction);

        // Detach the CA hooks.
        DetourDetach(&reinterpret_cast<PVOID&>(CATHODE::CA::FILE_HASHES::verify_integrity), CATHODE::CA::FILE_HASHES::h_verify_integrity);
        //DetourDetach(&reinterpret_cast<PVOID&>(CATHODE::CA::FILE_HASHES::terminate_game), CATHODE::CA::FILE_HASHES::h_terminate_game);
        DetourDetach(&reinterpret_cast<PVOID&>(CATHODE::CA::FILE_HASHES::sha1_portable_hash), CATHODE::CA::FILE_HASHES::h_sha1_portable_hash);
    	
        const long ret = DetourTransactionCommit();

        printf_s("[DevTools] Removed hooks. (result=%ld)\n", ret);
        fflush(stdout);
    }
	
    return TRUE;
}

