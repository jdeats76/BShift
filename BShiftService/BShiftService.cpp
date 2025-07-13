// bshift_service.cpp
// Windows service for remapping Redragon M908 mouse side buttons with G-Shift support

#include <windows.h>
#include <string>
#include <fstream>
#include <thread>
#include <unordered_map>
#include <cctype>
#include <nlohmann/json.hpp> // Requires nlohmann JSON library

using json = nlohmann::json;

SERVICE_STATUS g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = nullptr;
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;

std::unordered_map<int, std::string> g_ButtonMappings;
bool g_ServiceEnabled = true;
bool g_RapidFireHeld = false; // G-Shift state
std::string g_ShiftButton = "13"; // Default shift button, can be mouse button number or keyboard key
int g_ShiftButtonId = 13; // Parsed button ID for mouse buttons
int g_ShiftKeyCode = 0; // Virtual key code for keyboard keys

// Convert key string to virtual key code
int GetVirtualKeyCode(const std::string& key) {
    if (key.length() == 1) {
        char c = std::toupper(key[0]);
        if (c >= 'A' && c <= 'Z') {
            return c; // A-Z keys
        }
        if (c >= '0' && c <= '9') {
            return c; // 0-9 keys
        }
    }
    
    // Handle special keys
    if (key == "Space") return VK_SPACE;
    if (key == "Tab") return VK_TAB;
    if (key == "Enter") return VK_RETURN;
    if (key == "Escape") return VK_ESCAPE;
    if (key == "Shift") return VK_SHIFT;
    if (key == "Ctrl") return VK_CONTROL;
    if (key == "Alt") return VK_MENU;
    if (key == "Win") return VK_LWIN;
    
    // Function keys
    if (key.substr(0, 1) == "F" && key.length() <= 3) {
        int fNum = std::stoi(key.substr(1));
        if (fNum >= 1 && fNum <= 12) {
            return VK_F1 + (fNum - 1);
        }
    }
    
    return 0; // Invalid key
}

// Parse shift button configuration (supports both numeric button IDs and keyboard keys)
void ParseShiftButton(const std::string& shiftButtonConfig) {
    g_ShiftButton = shiftButtonConfig;
    g_ShiftButtonId = 0;
    g_ShiftKeyCode = 0;
    
    // Try to parse as numeric button ID first
    try {
        g_ShiftButtonId = std::stoi(shiftButtonConfig);
        return; // Successfully parsed as button ID
    }
    catch (const std::exception&) {
        // Not a number, try to parse as keyboard key
        g_ShiftKeyCode = GetVirtualKeyCode(shiftButtonConfig);
    }
}

void LoadConfig(const std::string& profileName) {
    std::ifstream configFile("C:/bshift/bshift.json");
    if (!configFile.is_open()) return;

    json config;
    configFile >> config;

    for (auto& profile : config["profiles"]) {
        if (profile["name"] == profileName) {
            g_ButtonMappings.clear();

            // Load the shift button configuration - can be numeric (mouse button) or alphanumeric (keyboard key)
            if (profile.contains("shiftButton")) {
                if (profile["shiftButton"].is_string()) {
                    ParseShiftButton(profile["shiftButton"].get<std::string>());
                }
                else if (profile["shiftButton"].is_number()) {
                    ParseShiftButton(std::to_string(profile["shiftButton"].get<int>()));
                }
            }
            else {
                ParseShiftButton("13"); // Default to button 13
            }

            for (auto& item : profile["buttonMappings"].items()) {
                int btn = std::stoi(item.key().substr(6));
                g_ButtonMappings[btn] = item.value();
            }
            break;
        }
    }
}

void SimulateKeypress(const std::string& keys) {
    // Placeholder: Parse and simulate the key combo using SendInput
    // This must support Ctrl, Alt, Shift, Win modifiers
}

bool IsSideButton(int buttonId) {
    return buttonId >= 1 && buttonId <= 12;
}

void InputMonitorThread() {
    while (WaitForSingleObject(g_ServiceStopEvent, 10) == WAIT_TIMEOUT) {
        if (!g_ServiceEnabled) continue;

        // Placeholder: Replace with Raw Input or Interception event capture
        int simulatedButtonId = 0; // Replace with real mouse button value
        bool isPressed = false;    // Replace with real press state
        
        // Check if shift button is pressed (mouse button or keyboard key)
        bool shiftPressed = false;
        if (g_ShiftButtonId > 0) {
            // Mouse button shift
            shiftPressed = (simulatedButtonId == g_ShiftButtonId && isPressed);
        }
        else if (g_ShiftKeyCode > 0) {
            // Keyboard key shift - check if key is currently pressed
            shiftPressed = (GetAsyncKeyState(g_ShiftKeyCode) & 0x8000) != 0;
        }
        
        if (g_ShiftButtonId > 0 && simulatedButtonId == g_ShiftButtonId) {
            g_RapidFireHeld = isPressed;
        }
        else if (g_ShiftKeyCode > 0) {
            g_RapidFireHeld = shiftPressed;
        }
        else if (IsSideButton(simulatedButtonId) && isPressed) {
            int mapIndex = g_RapidFireHeld ? simulatedButtonId + 12 : simulatedButtonId;
            if (g_ButtonMappings.find(mapIndex) != g_ButtonMappings.end()) {
                SimulateKeypress(g_ButtonMappings[mapIndex]);
            }
        }
    }
}

void WINAPI ServiceCtrlHandler(DWORD CtrlCode) {
    switch (CtrlCode) {
    case SERVICE_CONTROL_STOP:
        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING) break;
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
        SetEvent(g_ServiceStopEvent);
        break;
    default:
        break;
    }
}

void WINAPI ServiceMain(DWORD argc, LPWSTR* argv) {
    g_StatusHandle = RegisterServiceCtrlHandlerW(L"BShiftService", ServiceCtrlHandler);
    if (!g_StatusHandle) return;

    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwControlsAccepted = 0;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    g_ServiceStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (g_ServiceStopEvent == nullptr) {
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
        return;
    }

    LoadConfig("Default");

    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    std::thread monitor(InputMonitorThread);
    monitor.join();

    CloseHandle(g_ServiceStopEvent);
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
}

int wmain(int argc, wchar_t* argv[]) {
    SERVICE_TABLE_ENTRYW ServiceTable[] = {
        { (LPWSTR)L"BShiftService", (LPSERVICE_MAIN_FUNCTIONW)ServiceMain },
        { nullptr, nullptr }
    };

    if (!StartServiceCtrlDispatcherW(ServiceTable)) {
        return GetLastError();
    }

    return 0;
}
