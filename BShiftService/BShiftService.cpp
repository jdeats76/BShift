// bshift_service.cpp
// Windows service for remapping Redragon M908 mouse side buttons with G-Shift support

#include <windows.h>
#include <string>
#include <fstream>
#include <thread>
#include <unordered_map>
#include <nlohmann/json.hpp> // Requires nlohmann JSON library

using json = nlohmann::json;

SERVICE_STATUS g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = nullptr;
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;

std::unordered_map<int, std::string> g_ButtonMappings;
bool g_ServiceEnabled = true;
bool g_RapidFireHeld = false; // G-Shift state
int g_ShiftButtonId = 13; // Default, can be overridden per profile

void LoadConfig(const std::string& profileName) {
    std::ifstream configFile("C:/bshift/bshift.json");
    if (!configFile.is_open()) return;

    json config;
    configFile >> config;

    for (auto& profile : config["profiles"]) {
        if (profile["name"] == profileName) {
            g_ButtonMappings.clear();

            // Load the shift button ID if defined, otherwise default to 13
            if (profile.contains("shiftButton")) {
                g_ShiftButtonId = profile["shiftButton"].get<int>();
            }
            else {
                g_ShiftButtonId = 13;
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
        int simulatedButtonId = 0; // Replace with real value
        bool isPressed = false;    // Replace with real value

        if (simulatedButtonId == g_ShiftButtonId) {
            g_RapidFireHeld = isPressed;
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
