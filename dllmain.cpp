#include "CA.h"
#include "Menu.h"
#include "Scaleform.h"
#include "Logging.h"
#include "EntityInterface.h"

#include <detours.h>

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

// This will work for now, but I need to write a replacement that will restore the original call bytes, we just overwrite them.
void hookFunctionCall(int offset, void* replacementFunction)
{
    printf_s(("[DevTools] Attaching function call hook at location: " + std::to_string(offset) + "\n").c_str());

	const SIZE_T patchSize = 5;
	DWORD oldProtect;
	char* patchLocation = reinterpret_cast<char*>(offset);

	// Change the memory page protection.
	VirtualProtect(patchLocation, patchSize, PAGE_EXECUTE_READWRITE, &oldProtect);
	//to fill out the last 4 bytes of instruction, we need the offset between 
	//the payload function and the instruction immediately AFTER the call instruction
    
	//32 bit relative call opcode is E8, takes 1 32 bit operand for call offset
    uint8_t instruction[patchSize] = {0xE8, 0x0, 0x0, 0x0, 0x0};
	const uint32_t relativeAddress = reinterpret_cast<uint32_t>(replacementFunction) - (reinterpret_cast<uint32_t>(patchLocation) + sizeof instruction);

	// Copy the remaining bytes of the instruction (after the opcode).
	memcpy_s(instruction + 1, patchSize - 1, &relativeAddress, patchSize - 1);
	
	// Install the hook.
	memcpy_s(patchLocation, patchSize, instruction, sizeof(instruction));

    // Restore original memory page protections.
	VirtualProtect(patchLocation, patchSize, oldProtect, &oldProtect);
}


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

        // Overwrite the game data integrity checks (the game verifies all .PKG files and MODELS_LEVEL.BIN files by comparing them against SHA1 hashes).
		// Here we overwrite the assembly code of the checks in memory with the "NOP" (No Operation) instruction, preventing the game from comparing the hashes.

        DWORD oldProtect;
        int offset = 0x009E8BAF;
        for (int i = 0; i < 25; i++) {
            printf_s(("Patching location: " + std::to_string(offset + i) + "\n").c_str());
            auto* patchLocation = reinterpret_cast<char*>(offset + i);
            VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
            memcpy_s(patchLocation, 1, "\x90", 1);
            VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
        }
        offset = 0x007DBFB0;
        for (int i = 0; i < 9; i++) {
            printf_s(("Patching location: " + std::to_string(offset + i) + "\n").c_str());
            auto* patchLocation = reinterpret_cast<char*>(offset + i);
            VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
            memcpy_s(patchLocation, 1, "\x90", 1);
            VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
        }

    	// Overwrite some engine logging calls of interest.
    	// These will have to be manually done here, unfortunately, as MSVC has optimised out every logger call so that they now all call one empty function.
    	// This results in thousands of calls to said empty function. Hooking it crashes the game, due to the widely varying number (and type) of parameters being passed.

    	// Method taken from http://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html, thanks!
    	// Havok error logs.
    	hookFunctionCall(0x007F8EB9, CATHODE::Logging::HavokLogger::hijackedPrint);
    	// MusicController - MusicEvent
       	hookFunctionCall(0x006B95AA, CATHODE::Logging::MusicControllerLogger::hijackedPrint_MusicEvent);
    	// AnimationCommandLogger - Command changes
        hookFunctionCall(0x00678E57, CATHODE::Logging::AnimationCommandLogger::hijackedPrint);
    	// AnimationDataLogger logs.
    	// "Attempt to load bad animation data - set: %s - label: %s\n"
    	int hookAddresses[7] = {
			0x004E4661,
    		0x004E4798,
    		0x00579568,
    		0x0057969C,
    		0x00579F21,
    		0x0057A031,
    		// "The animation set \"%s\" does not contain animation \"%s\" - Check the script! - Carlo\n"
    		0x004E4E9B
    	};
    	for (int i = 0; i <= 6; i++)
    	{
    		hookFunctionCall(hookAddresses[i], CATHODE::Logging::AnimationDataLogger::hijackedPrint);
    	}
    	
    	// Door logs.
    	// "only 1 animated composite can be attached to a door"
    	hookFunctionCall(0x004D90C3, CATHODE::Logging::DoorLogger::hijackedPrint);
    	// "Not enough streaming budget to load other side of door, opening anyway!"
        hookFunctionCall(0x004E6B96, CATHODE::Logging::DoorLogger::hijackedPrint);
    	// "This door has no script so cannot animate! The door must be posed manually, makesure \"detach_anim=true\" by setting \"powered_on_reset=false\""
    	hookFunctionCall(0x004E8237, CATHODE::Logging::DoorLogger::hijackedPrint);
    	// Animation logs.
        // "Unable to find a valid animated model attached to the entity!"
    	hookFunctionCall(0x004D8D11, CATHODE::Logging::AnimationLogger::hijackedPrint);
    	// Character node logs.
        // "Node %s not found on player character skeleton."
    	hookFunctionCall(0x005D9C6D, CATHODE::Logging::CharacterNodeLogger::hijackedPrint);
    	// "node [%s] not found for character [%s]"
    	hookFunctionCall(0x004CDB5A, CATHODE::Logging::CharacterNodeLogger::hijackedPrint);
    	// 
    	hookFunctionCall(0x004D6C9C, CATHODE::Logging::CharacterNodeLogger::hijackedPrint);
    	// Node logs.
    	//
    	// Need to make sure I am hooking this right.
    	//hookFunctionCall(0x004D8751, CATHODE::Logging::CharacterNodeLogger::hijackedPrint);
    	// Zone logs.
        // "cannot assign a zone in this way!!!"
    	hookFunctionCall(0x004E508C, CATHODE::Logging::ZoneLogger::hijackedPrint);
        // "cannot assign a zone link in this way!!!"
    	hookFunctionCall(0x004E756C, CATHODE::Logging::ZoneLogger::hijackedPrint);
    	// "suspend_on_unload must be the same for both zones!!!"
        hookFunctionCall(0x004E78E1, CATHODE::Logging::ZoneLogger::hijackedPrint);
    	// Speech logs.
    	// "%s: priority = %s, timeout = %.2f"
    	hookFunctionCall(0x006B943E, CATHODE::Logging::SpeechLogger::hijackedPrint_RequestPriority);
    	// "Queued speech [size = %u]:"
    	hookFunctionCall(0x006B9460, CATHODE::Logging::SpeechLogger::hijackedPrint_SpeechSize);
        // "Playing speech:"
        hookFunctionCall(0x006B946F, CATHODE::Logging::SpeechLogger::hijackedPrint);
    	// "%s [%s]"
    	hookFunctionCall(0x006B94BF, CATHODE::Logging::SpeechLogger::hijackedPrint_SpacialAudioType);
        // "SPEECH: No valid entity connected to speechscript entity attempting to play %s. I need a valid character or sound object - see Jack if you don\'t understand.\n"
    	hookFunctionCall(0x004AD774, CATHODE::Logging::SpeechLogger::hijackedPrint_SpeechEntity);
        // "SPEECH: No valid entity connected to speech entity attempting to play %s.  I needa valid character or sound object - see Jack if you don\'t understand.\n"
    	hookFunctionCall(0x004E026F, CATHODE::Logging::SpeechLogger::hijackedPrint_SpeechEntity);
    	
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

        // Attach the EntityInterface hooks.
        DetourAttach(&reinterpret_cast<PVOID&>(CATHODE::EntityInterface::FindParameterBool), CATHODE::EntityInterface::hFindParameterBool);
    	
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

        // Detach the EntityInterface hooks.
    	DetourDetach(&reinterpret_cast<PVOID&>(CATHODE::EntityInterface::FindParameterBool), CATHODE::EntityInterface::hFindParameterBool);
    	
        const long ret = DetourTransactionCommit();

        printf_s("[DevTools] Removed hooks. (result=%ld)\n", ret);
        fflush(stdout);
    }
	
    return TRUE;
}

