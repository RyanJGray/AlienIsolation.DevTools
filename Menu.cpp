#include "Menu.h"
#include "Menu_Log.hpp"
#include "CATHODE.h"

// Game-specific code classes.
#include "GAME_LEVEL_MANAGER.h"
#include "AI_BEHAVIORAL.h"

// Engine-specific code classes.


#include <detours.h>
#include <map>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

IDXGISwapChain* g_swapChain = nullptr;
ID3D11Device* g_device = nullptr;
ID3D11DeviceContext* g_context = nullptr;
ID3D11RenderTargetView* g_renderTargetView = nullptr;
HWND                    g_hWindow = nullptr;
WNDPROC                 g_originalWndProcHandler = nullptr;

bool g_menuInitialised = false;
bool g_showDemoWindow = false;
bool g_showMenu = false;

const std::string g_modName = "Alien: Isolation DevTools";
const std::string g_modVersion = "0.1.4 (Alpha)";
const std::string g_modFullName = g_modName + " " + g_modVersion;

//extern std::map<CATHODE::ShortGuid, bool> permittedEnumGuids;
//extern std::map<CATHODE::ShortGuid, std::pair<CATHODE::Enum, bool>> enumOverrides;
//extern std::map<CATHODE::ShortGuid, std::pair<CATHODE::Enum, std::string>> entityStates;
//extern std::map<CATHODE::ShortGuid, CATHODE::Enum*> enums;
extern std::map<int, int*> g_entityBehaviourAnimEnumValues;
extern std::map<CATHODE::DataTypes::ShortGuid, bool> boolOverrides;
extern bool g_shouldOverrideBehaviourAnim;
extern bool g_GameLevelManager_levelLoadInProgress;
extern CATHODE::DataTypes::Vector g_lightingColourOverride;
extern bool g_shouldOverrideLightingColour;

Menu::Menu()
{
	
}

// Destructor for the Menu class.
Menu::~Menu() {
    g_swapChain = nullptr;
    g_context = nullptr;
    g_renderTargetView = nullptr;
    g_hWindow = nullptr;
    g_originalWndProcHandler = nullptr;
}

// Our custom window handler to override A:I's own handler.
// This lets us detect keyboard inputs and pass mouse + keyboard input control to ImGui.
LRESULT CALLBACK Menu::WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    ImGuiIO& io = ImGui::GetIO();
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(g_hWindow, &cursorPos);
    io.MousePos.x = cursorPos.x;
    io.MousePos.y = cursorPos.y;

    if (uMsg == WM_KEYUP) {
        if (wParam == VK_DELETE) {
            g_showMenu = !g_showMenu;
        }
    }

    if (g_showMenu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
        return true;
    }

    return CallWindowProc(g_originalWndProcHandler, hWnd, uMsg, wParam, lParam);
}

void Menu::InitMenu(IDXGISwapChain* pSwapChain)
{
    g_swapChain = pSwapChain;
}

IDXGISwapChain* Menu::GetSwapChain()
{
    return g_swapChain;
}

bool Menu::IsInitialised()
{
    return g_swapChain != nullptr;
}

void Menu::DrawMenu() {
    if (!g_menuInitialised) {
        if (FAILED(GetDeviceAndContextFromSwapChain(g_swapChain, &g_device, &g_context))) {
            DebugBreak();
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
    	// Enable ImGUI support for keyboard and gamepad input.
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        // Get the swapchain description (we use this to get the handle to the game's window).
        DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
        //DX_CHECK(g_swapChain->GetDesc(&dxgiSwapChainDesc));
        if (FAILED(g_swapChain->GetDesc(&dxgiSwapChainDesc)))
        {
	        DebugBreak();
        	return;
        }

        ImGui::StyleColorsDark();
    	
        g_hWindow = dxgiSwapChainDesc.OutputWindow;
        // Replace the game's window handler with our own so we can intercept key press events in the game.
        g_originalWndProcHandler = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHandler)));

        ImGui_ImplWin32_Init(g_hWindow);
        ImGui_ImplDX11_Init(g_device, g_context);
        //io.ImeWindowHandle = g_hWindow;

        ID3D11Texture2D* pBackBuffer = nullptr;
        //DX_CHECK(g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer)));
        if (FAILED(g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer))))
        {
            DebugBreak();
            return;
        }

        // Ensure the back buffer is not 0 or null.
        if (pBackBuffer) {
            //DX_CHECK(g_device->CreateRenderTargetView(pBackBuffer, NULL, &g_renderTargetView));
            if (FAILED(g_device->CreateRenderTargetView(pBackBuffer, nullptr, &g_renderTargetView)))
            {
                DebugBreak();
                return;
            }
        }
        else {
            DebugBreak();
        	return;
        }

        pBackBuffer->Release();
    	
        g_menuInitialised = true;
    }

    // Feed inputs to dear imgui, start new frame.
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

	static int alienIsolation_devTools_menu_lastBehaviourAnimTargetValue = 0;
	static int alienIsolation_devTools_menu_behaviourAnimTargetValue = -1;
    static bool alienIsolation_devTools_menu_showEngineLog = true;
	static bool alienIsolation_devTools_enum_hasClearedEnumValueMap = false;
    static bool alienIsolation_devTools_enum_doClearEnumValueMap = false;
    static bool alienIsolation_devTools_enum_doReapplyEnumValueOverride = false;

    union
    {
        unsigned long rgbVector;
        float rgbValue;
    } lightingColour{};

    // Convert the IEE-754 floating point hex value to a regular float that we can interact with.
    lightingColour.rgbVector = g_lightingColourOverride.v1;
    const float rValue = lightingColour.rgbValue;
    lightingColour.rgbVector = g_lightingColourOverride.v2;
    const float gValue = lightingColour.rgbValue;
    lightingColour.rgbVector = g_lightingColourOverride.v3;
    const float bValue = lightingColour.rgbValue;

    // Clear the stored enum value pointers between level loads (avoids null pointer exceptions).
    if ((g_GameLevelManager_levelLoadInProgress || alienIsolation_devTools_enum_doClearEnumValueMap) && !alienIsolation_devTools_enum_hasClearedEnumValueMap)
    {
        logger.AddLog("[Menu] Cleared behaviour anim enum map!\n");
        g_entityBehaviourAnimEnumValues.clear();
        alienIsolation_devTools_enum_hasClearedEnumValueMap = true;
        alienIsolation_devTools_enum_doClearEnumValueMap = false;
    }
    else if (!g_GameLevelManager_levelLoadInProgress && !alienIsolation_devTools_enum_doClearEnumValueMap && alienIsolation_devTools_enum_hasClearedEnumValueMap)
    {
        logger.AddLog("[Menu] Reset enum cleared flag!\n");
        alienIsolation_devTools_enum_hasClearedEnumValueMap = false;
        //alienIsolation_devTools_enum_doReapplyEnumValueOverride = true;
    }

	// If the value has changed since the last frame, or if we are after loading a new level, re-override all entities.
    if (alienIsolation_devTools_menu_lastBehaviourAnimTargetValue != alienIsolation_devTools_menu_behaviourAnimTargetValue || alienIsolation_devTools_enum_doReapplyEnumValueOverride)
    {
        alienIsolation_devTools_menu_lastBehaviourAnimTargetValue = alienIsolation_devTools_menu_behaviourAnimTargetValue;

        for (const auto& [fst, snd] : g_entityBehaviourAnimEnumValues)
        {
            *snd = alienIsolation_devTools_menu_behaviourAnimTargetValue;
        }
    }

	if (alienIsolation_devTools_menu_showEngineLog)
    {
    	// Show our CATHODE log output window.
	    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    	
	    // Actually call in the regular Log helper.
	    logger.Draw((g_modName + " - Engine Log").c_str(), &alienIsolation_devTools_menu_showEngineLog);
    }

	{
    	ImGui::SetNextWindowSize(ImVec2(480, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin((g_modName + " - Monitor").c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);

        static const char* behaviourAnim_enumString = "";

        switch (alienIsolation_devTools_menu_behaviourAnimTargetValue)
        {
        case -1:
            behaviourAnim_enumString = "Not Available";
            break;
        case 0:
            behaviourAnim_enumString = "UNIFORM";
            break;
        case 1:
            behaviourAnim_enumString = "PULSATE";
            break;
        case 2:
            behaviourAnim_enumString = "OSCILLATE";
            break;
        case 3:
            behaviourAnim_enumString = "FLICKER";
            break;
        case 4:
            behaviourAnim_enumString = "FLUCTUATE";
            break;
        case 5:
            behaviourAnim_enumString = "FLICKER_OFF";
            break;
        case 6:
            behaviourAnim_enumString = "SPARKING";
        	break;
        case 7:
            behaviourAnim_enumString = "BLINK";
            break;
        default:
            behaviourAnim_enumString = "Unknown?!";
        }

        ImGui::Text("Lighting Behaviour Animation: %s (%d)", behaviourAnim_enumString, alienIsolation_devTools_menu_behaviourAnimTargetValue);
        ImGui::Text("Level load in progress: %s", g_GameLevelManager_levelLoadInProgress ? "Yes" : "No");
        ImGui::Text("Has cleared behaviour animation map: %s", alienIsolation_devTools_enum_hasClearedEnumValueMap ? "Yes" : "No");
        ImGui::Text("Lighting RGB Colour Override: R=%.2f, G=%.2f, B=%.2f", rValue, gValue, bValue);

        ImGui::End();
    }
	
    // If the user wants to display the menu, then render it.
    if (g_showMenu) {
        static bool alienIsolation_devTools_menu_showAboutWindow = false;

        if (alienIsolation_devTools_menu_showAboutWindow) {
            if (ImGui::Begin((g_modName + " - About").c_str(), &alienIsolation_devTools_menu_showAboutWindow, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
                ImGui::TextColored(ImVec4(255, 170, 0, 1), (g_modFullName + " - Built on %s at %s").c_str(), __DATE__, __TIME__);
                ImGui::Text("Dear ImGui %s", ImGui::GetVersion());
                ImGui::Text("Detours %s", DETOURS_STRINGIFY(DETOURS_VERSION));
                ImGui::Separator();
                ImGui::Text("Build information:");
#ifdef _DEBUG
                ImGui::Text("Target: Debug");
#else
                ImGui::Text("Target: Release");
#endif
#ifdef _WIN32
                ImGui::Text("Architecture: x86");
#elif _WIN64
                ImGui::Text("Architecture: x64");
#endif
#ifdef _MSC_VER
                ImGui::Text("MSVC Compiler Version: %d (%d)", _MSC_VER, _MSC_FULL_VER);
#endif
#ifdef _MSVC_LANG
                ImGui::Text("MSVC C++ Standard: %d", _MSVC_LANG);
#endif
                ImGui::Separator();
                ImGui::Text("Mod credits:");
                ImGui::Text("Developed by RyanJGray with help from the following people, thanks!");
                ImGui::Text("MattFiler - thanks for all your help and support!");
            }

            ImGui::End();
        }

        // Get the center of the screen.
        const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        {
            ImGui::Begin((g_modName + " - Settings").c_str(), nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Help")) {
                    if (ImGui::MenuItem("About")) {
                        alienIsolation_devTools_menu_showAboutWindow = true;
                    }

                    ImGui::EndMenu();
                }
            	if (ImGui::BeginMenu("Tools")) {
	                if (ImGui::MenuItem("Engine Log")) {
		                alienIsolation_devTools_menu_showEngineLog = true;
	                }

            		ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            ImGui::TextColored(ImVec4(255, 170, 0, 1), g_modFullName.c_str());

            ImGui::Separator();

            ImGui::Text("Alien Withdrawl Control");

            if (ImGui::Button("Force Alien withdrawl"))
            {
                AI_BEHAVIORAL::WithdrawalManager::request_withdraw(AI_BEHAVIORAL::WithdrawalManager::m_this, 15.0f, 3.0f, -1.0f, false);
            }

            ImGui::Separator();

            ImGui::Text("Level Loading Control");

        	static char levelName[128] = "";
            static std::string levelPrefix = "Production\\";
            std::string levelToLoad;
            static bool invalidLevelError = false;
            ImGui::InputTextWithHint("Level name", "e.g. TECH_RnD_HzdLab", levelName, IM_ARRAYSIZE(levelName));
        	
            if (ImGui::Button("Load to provided level"))
            {
                levelToLoad = levelPrefix + levelName;

                const int level = GAME_LEVEL_MANAGER::get_level_from_name(GAME_LEVEL_MANAGER::m_instance, const_cast<char*>(levelToLoad.c_str()));

            	// When GAME_LEVEL_MANAGER::get_level_from_name returns int 0, this indicates that the engine does not recognise the level provided.
            	// Callback::GameMenu::LoadLevel has a hard-coded check for GAME_LEVEL_MANAGER::get_level_from_name returning int 0, it will terminate execution early if it detects that this happens.
                if (level == 0)
                {
                    invalidLevelError = true;
                }
            	else
            	{
                    invalidLevelError = false;
                    
                    // Add the level we want to the level queue.
                    GAME_LEVEL_MANAGER::queue_level(GAME_LEVEL_MANAGER::m_instance, level);

                    // Request that the game loads the next level in the queue.
                    GAME_LEVEL_MANAGER::request_next_level(GAME_LEVEL_MANAGER::m_instance, false);
                    
                    // Inform the enum map reset code that the UI wants the map to be cleared.
                    alienIsolation_devTools_enum_doClearEnumValueMap = true;
                }
            }

        	if (invalidLevelError)
        	{
                ImGui::TextColored(ImVec4(255, 170, 0, 1), "WARNING: Request blocked - Level not recognised by the engine!");
        	}

            ImGui::Separator();

            ImGui::Text("Enum Global Overrides");

            /*ImGui::Checkbox("Allow access to PostprocessingSettings::blend_mode?", &permittedEnumGuids[0xD11717AB]);
            ImGui::Checkbox("Allow access to PostprocessingSettings::priority?", &permittedEnumGuids[0x19E7816E]);
            ImGui::Checkbox("Allow access to FollowCameraModifier::modifier_type?", &permittedEnumGuids[0x6DF6A556]);
            ImGui::Checkbox("Allow access to LightAdaptationSettings::adaptation_mechanism?", &permittedEnumGuids[0xCE00E13E]);
            ImGui::Checkbox("Allow access to CMD_HolsterWeapon::equipment_slot?", &permittedEnumGuids[0x5068733]);
            ImGui::Checkbox("Allow access to CameraShake::shake_type?", &permittedEnumGuids[0x251F6445]);
            ImGui::Checkbox("Allow access to TriggerSequence::trigger_mode?", &permittedEnumGuids[0x29B2E7D]);
            ImGui::Checkbox("Allow access to MusicTrigger::rtpc_set_return_mode?", &permittedEnumGuids[0x1AB9598A]);
            ImGui::Checkbox("Allow access to FilterIsEnemyOfAllianceGroup::alliance_group?", &permittedEnumGuids[0xA1D92FE0]);
            ImGui::Checkbox("Allow access to TriggerBindAllCharactersOfType::character_class?", &permittedEnumGuids[0xC878CAC6]);
            ImGui::Checkbox("Allow access to FilterIsCharacterClassCombo::character_classes?", &permittedEnumGuids[0xAF865204]);
            ImGui::Checkbox("Allow access to NPC_SetAlertness::AlertState?", &permittedEnumGuids[0x80451778]);*/

            /*static CATHODE::ShortGuid enumGuid_alertState = 0x80451778;
            static CATHODE::ShortGuid enumTableGuid_alertState = 0x4882B94C;
            static std::pair<CATHODE::Enum, bool> &enumOverride_alertState = enumOverrides.find(enumGuid_alertState)->second;

			ImGui::Checkbox("Override NPC_SetAlertness::AlertState?", &enumOverride_alertState.second);

            // Does the user want us to override this Enum?
            if (enumOverride_alertState.second)
        	{
                // If the ShortGuid doesn't match, then set it.
                if (enumOverride_alertState.first.enum_short_guid != enumTableGuid_alertState)
                {
                	enumOverride_alertState.first.enum_short_guid = enumTableGuid_alertState;
                }

                ImGui::RadioButton("IGNORE_PLAYER", &enumOverride_alertState.first.enum_value_index, 0); ImGui::SameLine();
                ImGui::RadioButton("ALERT", &enumOverride_alertState.first.enum_value_index, 1); ImGui::SameLine();
                ImGui::RadioButton("AGGRESSIVE", &enumOverride_alertState.first.enum_value_index, 2);
            }*/

            /*static CATHODE::ShortGuid enumGuid_behaviourAnim = 0x28F2A5AE;
            static CATHODE::ShortGuid enumTableGuid_behaviourAnim = 0xDE1AD255;
            static std::pair<CATHODE::Enum, bool> &enumOverride_behaviourAnim = enumOverrides.find(enumGuid_behaviourAnim)->second;*/

        	ImGui::Checkbox("Override FloatModulateRandom::behaviour_anim?", &g_shouldOverrideBehaviourAnim);

            // Does the user want us to override this Enum?
            if (g_shouldOverrideBehaviourAnim)
        	{
                /*// If the ShortGuid doesn't match, then set it.
                if (enumOverride_behaviourAnim.first.enum_short_guid != enumTableGuid_behaviourAnim)
                {
                	enumOverride_behaviourAnim.first.enum_short_guid = enumTableGuid_behaviourAnim;
                }*/

                /*ImGui::RadioButton("UNIFORM", &enumOverride_behaviourAnim.first.enum_value_index, 0); ImGui::SameLine();
                ImGui::RadioButton("PULSATE", &enumOverride_behaviourAnim.first.enum_value_index, 1); ImGui::SameLine();
                ImGui::RadioButton("OSCILLATE", &enumOverride_behaviourAnim.first.enum_value_index, 2); ImGui::SameLine();
                ImGui::RadioButton("FLICKER", &enumOverride_behaviourAnim.first.enum_value_index, 3); ImGui::SameLine();
                ImGui::RadioButton("FLUCTUATE", &enumOverride_behaviourAnim.first.enum_value_index, 4); ImGui::SameLine();
                ImGui::RadioButton("FLICKER_OFF", &enumOverride_behaviourAnim.first.enum_value_index, 5); ImGui::SameLine();
                ImGui::RadioButton("SPARKING", &enumOverride_behaviourAnim.first.enum_value_index, 6); ImGui::SameLine();
                ImGui::RadioButton("BLINK", &enumOverride_behaviourAnim.first.enum_value_index, 7);*/

                ImGui::Text("Live override FloatModulateRandom::behaviour_anim value on all entities");

        		ImGui::RadioButton("UNIFORM", &alienIsolation_devTools_menu_behaviourAnimTargetValue, 0); ImGui::SameLine();
	            ImGui::RadioButton("PULSATE", &alienIsolation_devTools_menu_behaviourAnimTargetValue, 1); ImGui::SameLine();
	            ImGui::RadioButton("OSCILLATE", &alienIsolation_devTools_menu_behaviourAnimTargetValue, 2); ImGui::SameLine();
	            ImGui::RadioButton("FLICKER", &alienIsolation_devTools_menu_behaviourAnimTargetValue, 3); ImGui::SameLine();
	            ImGui::RadioButton("FLUCTUATE", &alienIsolation_devTools_menu_behaviourAnimTargetValue, 4); ImGui::SameLine();
	            ImGui::RadioButton("FLICKER_OFF", &alienIsolation_devTools_menu_behaviourAnimTargetValue, 5); ImGui::SameLine();
	            ImGui::RadioButton("SPARKING", &alienIsolation_devTools_menu_behaviourAnimTargetValue, 6); ImGui::SameLine();
	            ImGui::RadioButton("BLINK", &alienIsolation_devTools_menu_behaviourAnimTargetValue, 7);
            }

            ImGui::Checkbox("Override LightReference::colour?", &g_shouldOverrideLightingColour);

            if (g_shouldOverrideLightingColour)
            {
                static ImVec4 colour(rValue, gValue, bValue, 1.0f);

	            ImGui::ColorPicker3("LightReference::colour", reinterpret_cast<float*>(&colour), ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_DisplayRGB);

                static constexpr float sc = 1.0f / 255.0f;

                lightingColour.rgbValue = colour.x / sc;
                g_lightingColourOverride.v1 = lightingColour.rgbVector;
                lightingColour.rgbValue = colour.y / sc;
                g_lightingColourOverride.v2 = lightingColour.rgbVector;
                lightingColour.rgbValue = colour.z / sc;
                g_lightingColourOverride.v3 = lightingColour.rgbVector;
            }

            //static bool& override_LightReference_IncludeInPlanarReflections = boolOverrides.find(0xB3EB5895)->second;

            //ImGui::Checkbox("Override LightReference::include_in_planar_reflections?", &override_LightReference_IncludeInPlanarReflections);

            //static bool& override_LightReference_Volume = boolOverrides.find(0xB41E5E17)->second;

            //ImGui::Checkbox("Override LightReference::volume?", &override_LightReference_Volume);

            ImGui::End();
        }
    }

    ImGui::EndFrame();

    // Render dear imgui onto the screen.
    ImGui::Render();
    g_context->OMSetRenderTargets(1, &g_renderTargetView, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // A call to DXGISwapChain->Present is not needed here, as we are hooked into DirectX 11's Present function, so anything we draw here will be presented by the game in its own time.
}

void Menu::ShutdownMenu() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

// Code taken (and changed a little) from Niemand's wonderful article on hooking DX11 and using ImGUI.
// https://niemand.com.ar/2019/01/01/how-to-hook-directx-11-imgui/
HRESULT Menu::GetDeviceAndContextFromSwapChain(IDXGISwapChain* pSwapChain, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext) {
    const HRESULT ret = pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<PVOID*>(ppDevice));

    if (SUCCEEDED(ret)) {
        (*ppDevice)->GetImmediateContext(ppContext);
    }

    return ret;
}