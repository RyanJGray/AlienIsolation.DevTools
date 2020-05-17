// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <cstdio>
#include <detours.h>

#if defined _M_IX86
#pragma comment(lib, "detours.lib")
#endif

// Memory protection flags to check the executable address.
#define PAGE_EXECUTE_FLAGS \
    (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)


typedef char(__stdcall* tNodeDebugMessage)(char);
tNodeDebugMessage NodeDebugMessage = (tNodeDebugMessage)0x00e1bcc0;

BOOL bNodeDebugMessages = FALSE;
//DWORD NodeDebugMessage = 0x6CBF23AC;
//DWORD NodeDebugMessage = 0x00e1bcc0;
DWORD oldProtect;


// Detour function which overrides NodeDebugMessage.
__declspec(noinline)
char* __stdcall hNodeDebugMessage(char* param_1)
{
    // Calls to this function happen probably thousands of times per frame, and hit the FPS a lot, the debug prints are set to off by default.
    if (GetKeyState(VK_MENU) & 0x8000)
    {
        if (!bNodeDebugMessages)
        {
            bNodeDebugMessages = TRUE;
        }
        else
        {
            bNodeDebugMessages = FALSE;
        }
    }

    // Only print the debug messages if the user has explicitly turned them on.
    if (bNodeDebugMessages)
    {
        if (strcmp(param_1, "on_update") || param_1 != "CATHODE:EntityManager::process_triggers" || param_1 != "CATHODE:EntityManager::process_messages")
        {
            printf("hNodeDebugMessage: ");
            printf(param_1);
            printf("\n");
        }
    }

    return param_1;
}

/*
    Creates a JMP in code in memory from the @origin to the @destination,
    return address and function return are @length+@origin
    @param origin
    @param destination
    @parmam length
    @return byte* retrun address
*/
byte* UnprotectMemory(byte* address)
{
    VirtualProtect(address, 10, PAGE_EXECUTE_READWRITE, &oldProtect);
    return address;
}

/*
    Creates a JMP in code in memory from the @origin to the @destination,
    return address and function return are @length+@origin
    @param origin
    @param destination
    @parmam length
    @return byte* retrun address
*/
byte* ReprotectMemory(byte* address)
{
    VirtualProtect(address, 10, oldProtect, &oldProtect);
    return address;
}

BOOL IsExecutableAddress(LPVOID pAddress)
{
    MEMORY_BASIC_INFORMATION mi;
    VirtualQuery(pAddress, &mi, sizeof(mi));

    return (mi.State == MEM_COMMIT && (mi.Protect & PAGE_EXECUTE_FLAGS));
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
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
        freopen("CONOUT$", "w", stdout);
        MessageBoxW(0, L"Injected!", L"", 0);

        printf("Detours " DETOURS_STRINGIFY(DETOURS_BITS) "\n");
        fflush(stdout);

        printf("NodeDebugFunction: Injected!\n");

        /*
        // Initialize MinHook.
        if (MH_Initialize() != MH_OK)
        {
            std::cout << "NodeDebugFunction: MH Initialisation Failed!" << std::endl;
            return 1;
        }
        */

        printf("NodeDebugFunction: Hooking into address 0x%x\n", NodeDebugMessage);

        /*SigScanner* sigScanner;
        sigScanner = new SigScanner();
        DWORD nodeDebugAddress;
        DWORD baseAddress;
        baseAddress = (DWORD)hModule;
        nodeDebugAddress = sigScanner->FindAddress(nodeDebugMask, 10, baseAddress, 10);

        std::cout << "Base address: " << (char)baseAddress << std::endl;
        std::cout << "nodeDebugAddress: " << (char)nodeDebugAddress << std::endl;
        */

        //UnprotectMemory((byte*)&NodeDebugMessage);

        //printf("NodeDebugFunction: IsExecutableAddress (%d)\n", IsExecutableAddress(&NodeDebugMessage));

        DetourTransactionBegin();
        printf("NodeDebugFunction: DetourTransactionBegin()\n");
        //std::cout << "NodeDebugFunction: DetourTransactionBegin()" << std::endl;
        
        DetourUpdateThread(GetCurrentThread());
        printf("NodeDebugFunction: DetourUpdateThread()\n");
        //std::cout << "NodeDebugFunction: DetourUpdateThread()" << std::endl;

        PDETOUR_TRAMPOLINE pDetourTrampoline = NULL;
        PVOID pRealTarget = NULL;
        PVOID pRealDetour = NULL;
        //const auto result = DetourAttachEx((PVOID*)NodeDebugMessage, (PVOID)hNodeDebugMessage, pDetourTrampoline, pRealTarget, pRealDetour);
        DetourAttachEx(&(PVOID&)NodeDebugMessage, hNodeDebugMessage, &pDetourTrampoline, &pRealTarget, &pRealDetour);
        printf("NodeDebugFunction: DetourAttachEx()\n");
        printf("NodeDebugFunction: pDetourTrampoline = %d; pRealTarget = %d; pRealDetour = %d\n", pDetourTrampoline, pRealTarget, pRealDetour);
        //pNodeDebugMessage = (char*(__stdcall*)(char*))pRealTarget;
        //printf("NodeDebugFunction: pNodeDebugMessage = %d\n", pNodeDebugMessage);
        //std::cout << "NodeDebugFunction: DetourAttach()" << std::endl;

        const LONG result = DetourTransactionCommit();
        printf("NodeDebugFunction: DetourTransactionCommit() = %d\n", result);
        //std::cout << "NodeDebugFunction: DetourTransactionCommit()" << std::endl;

        if (result != NO_ERROR)
        {
            switch (result)
            {
            case ERROR_INVALID_BLOCK:
                printf("Detour: The function referenced is too small to be detoured (%ld)\n", result);

            case ERROR_INVALID_HANDLE:
                printf("Detour: The ppPointer parameter is null or points to a null pointer (%ld)\n", result);

            case ERROR_INVALID_OPERATION:
                printf("Detour: No pending transaction exists (%ld)\n", result);

            case ERROR_NOT_ENOUGH_MEMORY:
                printf("Detour: Not enough memory exists to complete the operation (%ld)\n", result);

            case ERROR_INVALID_PARAMETER:
                printf("Detour: An invalid parameter has been passed (%ld)\n", result);

            default:
                printf("Detour: Unknown error (%ld)\n", result);
            }
        }

        /*
        MH_STATUS createHook;
        createHook = MH_CreateHook(&NodeDebugMessage, &hNodeDebugMessage, (LPVOID*)oNodeDebugMessage);
        */

        //ReprotectMemory((byte*)&NodeDebugMessage);

        /*
        // Create a hook in a disabled state.
        if (createHook != MH_OK)
        {
            std::cout << "NodeDebugFunction: Failed to create MH hook! (" << MH_StatusToString(createHook) << ")" << std::endl;
            return 1;
        }

        if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
        {
            std::cout << "NodeDebugFunction: Failed to enable MH hook!" << std::endl;
            return 1;
        }
        */

        if (result == NO_ERROR)
        {
            printf("NodeDebugFunction: Hook active!\n");
        }
        else
        {
            printf("NodeDebugFunction: Hook failed!\n");
        }
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        /*
        // Expected to tell "Hooked!".
        //MessageBoxW(NULL, L"Not hooked...", L"MinHook Sample", MB_OK);

        // Disable the hook.
        if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK)
        {
            return 1;
        }

        // Expected to tell "Not hooked...".
        //MessageBoxW(NULL, L"Not hooked...", L"MinHook Sample", MB_OK);

        // Uninitialize MinHook.
        if (MH_Uninitialize() != MH_OK)
        {
            return 1;
        }
        */

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)NodeDebugMessage, hNodeDebugMessage);
        const LONG ret = DetourTransactionCommit();

        printf("NodeDebugFunction: Removed hook. (result=%d)\n", ret);
        fflush(stdout);
    }
    return TRUE;
}

