#include "DevTools.h"
#include "network/websocket/websocket_server.h"

#include "CA.h"
#include "Menu.h"
#include "Scaleform.h"
#include "Logging.h"

// Game-specific code classes.
#include "GAME_LEVEL_MANAGER.h"
#include "GameFlow.h"
#include "AI_BEHAVIORAL.h"

// CATHODE-specific code classes.
#include "EntityInterface.h"
#include "ShortGuid.h"
#include "StringTable.h"
#include "FLOAT_MODULATE_RANDOM.h"
#include "HackingGame.h"

// External includes.
#include <detours.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


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
	const SIZE_T patchSize = 5;
	DWORD oldProtect;
	char* patchLocation = reinterpret_cast<char*>(offset);

	// Change the memory page protection.
	VirtualProtect(patchLocation, patchSize, PAGE_EXECUTE_READWRITE, &oldProtect);
	//to fill out the last 4 bytes of instruction, we need the offset between 
	//the payload function and the instruction immediately AFTER the call instruction
    
	//32 bit relative call opcode is E8, takes 1 32 bit operand for call offset
    uint8_t instruction[patchSize] = {0xE8, 0x0, 0x0, 0x0, 0x0};
	const uint32_t relativeAddress = reinterpret_cast<uint32_t>(replacementFunction) - (reinterpret_cast<uint32_t>(patchLocation) + sizeof(instruction));

	// Copy the remaining bytes of the instruction (after the opcode).
	memcpy_s(instruction + 1, patchSize - 1, &relativeAddress, patchSize - 1);
	
	// Install the hook.
	memcpy_s(patchLocation, patchSize, instruction, sizeof(instruction));

    // Restore original memory page protections.
	VirtualProtect(patchLocation, patchSize, oldProtect, &oldProtect);
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
	
    // If the Menu class hasn't already been initialised, initialise it now.
    if (!Menu::IsInitialised())
    {
        Menu::InitMenu(*ppSwapChain);

        if (*ppSwapChain)
        {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());

            void** pVMTPresent = *reinterpret_cast<void***>(*ppSwapChain);
        	
        	// Store reference to the original D3D11Present function from the SwapChain VTable.
            d3d11Present = static_cast<tD3D11Present>(pVMTPresent[8]);

            DEVTOOLS_DETOURS_ATTACH(d3d11Present, hD3D11Present);

            const auto result = DetourTransactionCommit();
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
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

    	// Menu hooks / initialisation code, adapted from Alias Isolation.
        const HMODULE hModule = GetModuleHandle(L"d3d11");
    	
        if (hModule)
        {
            d3d11CreateDeviceAndSwapChain = reinterpret_cast<tD3D11CreateDeviceAndSwapChain>(GetProcAddress(hModule, "D3D11CreateDeviceAndSwapChain"));
        	
            if (d3d11CreateDeviceAndSwapChain)
            {
                DEVTOOLS_DETOURS_ATTACH(d3d11CreateDeviceAndSwapChain, hD3D11CreateDeviceAndSwapChain);
            }
            else
            {
                MessageBox(NULL, L"Fatal Error - GetProcAddress(\"D3D11CreateDeviceAndSwapChain\") failed!", L"AlienIsolation.DevTools", MB_ICONERROR);
            }
        }
        else
        {
            MessageBox(NULL, L"Fatal Error - GetModuleHandle(\"d3d11\") failed: MODULE_NOT_FOUND!", L"AlienIsolation.DevTools", MB_ICONERROR);
        }

        // Overwrite the game data integrity checks (the game verifies all .PKG files and MODELS_LEVEL.BIN files by comparing them against SHA1 hashes).
		// Here we overwrite the assembly code of the checks in memory with the "NOP" (No Operation) instruction, preventing the game from comparing the hashes.

        DWORD oldProtect;
        auto offset = DEVTOOLS_RELATIVE_ADDRESS(0x005e8baf);
        for (int i = 0; i < 25; i++) {
            auto* patchLocation = reinterpret_cast<char*>(offset + i);
            VirtualProtect(patchLocation, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
            memcpy_s(patchLocation, 1, "\x90", 1);
            VirtualProtect(patchLocation, 1, oldProtect, &oldProtect);
        }
        offset = DEVTOOLS_RELATIVE_ADDRESS(0x003dbfb0);
        for (int i = 0; i < 9; i++) {
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
    	/*
        hookFunctionCall(DEVTOOLS_RELATIVE_ADDRESS(0x003f8eb9), CATHODE::Logging::HavokLogger::hijackedPrint);
    	// MusicController - MusicEvent
       	hookFunctionCall(DEVTOOLS_RELATIVE_ADDRESS(0x002b95aa), CATHODE::Logging::MusicControllerLogger::hijackedPrint_MusicEvent);
    	// AnimationCommandLogger - Command changes
        hookFunctionCall(DEVTOOLS_RELATIVE_ADDRESS(0x00278e57), CATHODE::Logging::AnimationCommandLogger::hijackedPrint);
    	// AnimationDataLogger logs.
    	// "Attempt to load bad animation data - set: %s - label: %s\n"
    	const int hookAddresses[7] = {
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
        //hookFunctionCall(0x004E78E1, CATHODE::Logging::ZoneLogger::hijackedPrint);
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
        */

        // Attach the GAME_LEVEL_MANAGER hooks.
        DEVTOOLS_DETOURS_ATTACH(GAME_LEVEL_MANAGER::get_level_from_name, GAME_LEVEL_MANAGER::h_get_level_from_name);
        DEVTOOLS_DETOURS_ATTACH(GAME_LEVEL_MANAGER::queue_level, GAME_LEVEL_MANAGER::h_queue_level);
        DEVTOOLS_DETOURS_ATTACH(GAME_LEVEL_MANAGER::request_next_level, GAME_LEVEL_MANAGER::h_request_next_level);

        // Attach the GameFlow hooks.
        DEVTOOLS_DETOURS_ATTACH(GameFlow::start_gameplay, GameFlow::h_start_gameplay);

        // Attach the AI_BEHAVIORAL hooks.
        DEVTOOLS_DETOURS_ATTACH(AI_BEHAVIORAL::WithdrawalManager::request_withdraw, AI_BEHAVIORAL::WithdrawalManager::h_request_withdraw);
        DEVTOOLS_DETOURS_ATTACH(AI_BEHAVIORAL::WithdrawalManager::set_withdraw_state, AI_BEHAVIORAL::WithdrawalManager::h_set_withdraw_state);

        // Attach the HackingGame hooks (testing only).
        DEVTOOLS_DETOURS_ATTACH(CATHODE::HackingGame::on_custom_method, CATHODE::HackingGame::h_on_custom_method);

    	// Attach the Scaleform hooks.
        //DEVTOOLS_DETOURS_ATTACH(CATHODE::Scaleform::Callback::GameMenu::LoadLevel, CATHODE::Scaleform::Callback::GameMenu::hLoadLevel);
        //DEVTOOLS_DETOURS_ATTACH(CATHODE::Scaleform::UI::LoadLevelUnknownFunc1, CATHODE::Scaleform::UI::hLoadLevelUnknownFunc1);
        //DEVTOOLS_DETOURS_ATTACH(CATHODE::Scaleform::UI::LoadLevelUnknownFunc2, CATHODE::Scaleform::UI::hLoadLevelUnknownFunc2);
        //DEVTOOLS_DETOURS_ATTACH(CATHODE::Scaleform::UI::DispatchRequestToNodeHandler, CATHODE::Scaleform::UI::hDispatchRequestToNodeHandler);

        // Attach the CA hooks.
        DEVTOOLS_DETOURS_ATTACH(CATHODE::CA::FILE_HASHES::verify_integrity, CATHODE::CA::FILE_HASHES::h_verify_integrity);
        //DEVTOOLS_DETOURS_ATTACH(CATHODE::CA::FILE_HASHES::terminate_game, CATHODE::CA::FILE_HASHES::h_terminate_game);
        DEVTOOLS_DETOURS_ATTACH(CATHODE::CA::FILE_HASHES::sha1_portable_hash, CATHODE::CA::FILE_HASHES::h_sha1_portable_hash);

        // Attach the EntityInterface hooks.
        DEVTOOLS_DETOURS_ATTACH(CATHODE::EntityInterface::FindParameterString, CATHODE::EntityInterface::hFindParameterString);
        DEVTOOLS_DETOURS_ATTACH(CATHODE::EntityInterface::FindParameterBool, CATHODE::EntityInterface::hFindParameterBool);
        DEVTOOLS_DETOURS_ATTACH(CATHODE::EntityInterface::FindParameterVector, CATHODE::EntityInterface::hFindParameterVector);
    	DEVTOOLS_DETOURS_ATTACH(CATHODE::EntityInterface::FindParameterFloat, CATHODE::EntityInterface::hFindParameterFloat);
    	DEVTOOLS_DETOURS_ATTACH(CATHODE::EntityInterface::FindParameterEnum, CATHODE::EntityInterface::hFindParameterEnum);

        // Attach the FLOAT_MODULATE_RANDOM hooks.
        DEVTOOLS_DETOURS_ATTACH(CATHODE::FLOAT_MODULATE_RANDOM::SaveValues, CATHODE::FLOAT_MODULATE_RANDOM::hSaveValues);

        // Attach the ShortGuid hooks.
        DEVTOOLS_DETOURS_ATTACH(CATHODE::ShortGuid::ShortGuid, CATHODE::ShortGuid::hShortGuid);

        // Attach the StringTable hooks.
        DEVTOOLS_DETOURS_ATTACH(CATHODE::StringTable::string_from_offset, CATHODE::StringTable::h_string_from_offset);

        const long result = DetourTransactionCommit();

        if (result != NO_ERROR)
        {
            switch (result)
            {
            case ERROR_INVALID_BLOCK:
                MessageBox(NULL, L"Fatal Error - The function referenced is too small to be detoured", L"AlienIsolation.DevTools", MB_ICONERROR);
                break;
            case ERROR_INVALID_HANDLE:
                MessageBox(NULL, L"Fatal Error - The ppPointer parameter is null or points to a null pointer", L"AlienIsolation.DevTools", MB_ICONERROR);
                break;
            case ERROR_INVALID_OPERATION:
                MessageBox(NULL, L"Fatal Error - No pending transaction exists", L"AlienIsolation.DevTools", MB_ICONERROR);
                break;
            case ERROR_NOT_ENOUGH_MEMORY:
                MessageBox(NULL, L"Fatal Error - Not enough memory exists to complete the operation", L"AlienIsolation.DevTools", MB_ICONERROR);
                break;
            case ERROR_INVALID_PARAMETER:
                MessageBox(NULL, L"Fatal Error - An invalid parameter has been passed", L"AlienIsolation.DevTools", MB_ICONERROR);
                break;
            default:
                MessageBox(NULL, L"Fatal Error - Unknown Detours error", L"AlienIsolation.DevTools", MB_ICONERROR);
                break;
            }
        }

        auto* _g_WebSocketServer = new WebSocketServer;
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

    	// Detach the rendering hooks.
        DEVTOOLS_DETOURS_DETACH(d3d11CreateDeviceAndSwapChain, hD3D11CreateDeviceAndSwapChain);
        DEVTOOLS_DETOURS_DETACH(d3d11Present, hD3D11Present);

        // Detach the GAME_LEVEL_MANAGER hooks.
        DEVTOOLS_DETOURS_DETACH(GAME_LEVEL_MANAGER::get_level_from_name, GAME_LEVEL_MANAGER::h_get_level_from_name);
        DEVTOOLS_DETOURS_DETACH(GAME_LEVEL_MANAGER::queue_level, GAME_LEVEL_MANAGER::h_queue_level);
        DEVTOOLS_DETOURS_DETACH(GAME_LEVEL_MANAGER::request_next_level, GAME_LEVEL_MANAGER::h_request_next_level);

        // Detach the GameFlow hooks.
        DEVTOOLS_DETOURS_DETACH(GameFlow::start_gameplay, GameFlow::h_start_gameplay);
        
        // Detach the AI_BEHAVIORAL hooks.
        DEVTOOLS_DETOURS_DETACH(AI_BEHAVIORAL::WithdrawalManager::request_withdraw, AI_BEHAVIORAL::WithdrawalManager::h_request_withdraw);
        DEVTOOLS_DETOURS_DETACH(AI_BEHAVIORAL::WithdrawalManager::set_withdraw_state, AI_BEHAVIORAL::WithdrawalManager::h_set_withdraw_state);

        // Detach the HackingGame hooks (testing only).
        DEVTOOLS_DETOURS_DETACH(CATHODE::HackingGame::on_custom_method, CATHODE::HackingGame::h_on_custom_method);

    	// Detach the Scaleform hooks.
        //DEVTOOLS_DETOURS_DETACH(CATHODE::Scaleform::Callback::GameMenu::LoadLevel, CATHODE::Scaleform::Callback::GameMenu::hLoadLevel);
        //DEVTOOLS_DETOURS_DETACH(CATHODE::Scaleform::UI::LoadLevelUnknownFunc1, CATHODE::Scaleform::UI::hLoadLevelUnknownFunc1);
        //DEVTOOLS_DETOURS_DETACH(CATHODE::Scaleform::UI::LoadLevelUnknownFunc2, CATHODE::Scaleform::UI::hLoadLevelUnknownFunc2);
        //DEVTOOLS_DETOURS_DETACH(CATHODE::Scaleform::UI::DispatchRequestToNodeHandler, CATHODE::Scaleform::UI::hDispatchRequestToNodeHandler);

        // Detach the CA hooks.
        DEVTOOLS_DETOURS_DETACH(CATHODE::CA::FILE_HASHES::verify_integrity, CATHODE::CA::FILE_HASHES::h_verify_integrity);
        //DEVTOOLS_DETOURS_DETACH(CATHODE::CA::FILE_HASHES::terminate_game, CATHODE::CA::FILE_HASHES::h_terminate_game);
        DEVTOOLS_DETOURS_DETACH(CATHODE::CA::FILE_HASHES::sha1_portable_hash, CATHODE::CA::FILE_HASHES::h_sha1_portable_hash);

        // Detach the EntityInterface hooks.
        DEVTOOLS_DETOURS_DETACH(CATHODE::EntityInterface::FindParameterString, CATHODE::EntityInterface::hFindParameterString);
        DEVTOOLS_DETOURS_DETACH(CATHODE::EntityInterface::FindParameterBool, CATHODE::EntityInterface::hFindParameterBool);
        DEVTOOLS_DETOURS_DETACH(CATHODE::EntityInterface::FindParameterVector, CATHODE::EntityInterface::hFindParameterVector);
        DEVTOOLS_DETOURS_DETACH(CATHODE::EntityInterface::FindParameterFloat, CATHODE::EntityInterface::hFindParameterFloat);
        DEVTOOLS_DETOURS_DETACH(CATHODE::EntityInterface::FindParameterEnum, CATHODE::EntityInterface::hFindParameterEnum);

    	// Attach the FLOAT_MODULATE_RANDOM hooks.
        DEVTOOLS_DETOURS_DETACH(CATHODE::FLOAT_MODULATE_RANDOM::SaveValues, CATHODE::FLOAT_MODULATE_RANDOM::hSaveValues);
       
        // Detach the ShortGuid hooks.
        DEVTOOLS_DETOURS_DETACH(CATHODE::ShortGuid::ShortGuid, CATHODE::ShortGuid::hShortGuid);
        
        // Attach the StringTable hooks.
        DEVTOOLS_DETOURS_DETACH(CATHODE::StringTable::string_from_offset, CATHODE::StringTable::h_string_from_offset);

        DetourTransactionCommit();
    }
	
    return TRUE;
}
