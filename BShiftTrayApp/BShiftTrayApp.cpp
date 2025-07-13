// bshift_tray_app.cpp
// System Tray App for BShift – allows profile switching and toggling the service

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <winsvc.h>

using json = nlohmann::json;

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001
#define ID_TRAY_TOGGLE_SERVICE 1002
#define ID_TRAY_PROFILE_BASE 2000

NOTIFYICONDATA nid = {};
HMENU hMenu = nullptr;
bool serviceEnabled = true;
std::string currentProfile = "Default";
json loadedConfig;

bool ToggleServiceByName(const std::wstring& serviceName, bool enable) {
    SC_HANDLE hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    if (!hSCManager) return false;

    SC_HANDLE hService = OpenServiceW(hSCManager, serviceName.c_str(), SERVICE_START | SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (!hService) {
        CloseServiceHandle(hSCManager);
        return false;
    }

    SERVICE_STATUS status;
    bool result = false;
    if (QueryServiceStatus(hService, &status)) {
        if (enable && status.dwCurrentState == SERVICE_STOPPED) {
            result = StartService(hService, 0, nullptr);
        }
        else if (!enable && status.dwCurrentState == SERVICE_RUNNING) {
            result = ControlService(hService, SERVICE_CONTROL_STOP, &status);
        }
        else {
            result = true; // Already in desired state
        }
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return result;
}

void LoadProfiles() {
    std::ifstream file("C:/bshift/bshift.json");
    if (!file.is_open()) return;
    file >> loadedConfig;
}

void ToggleServiceState() {
    serviceEnabled = !serviceEnabled;
    ToggleServiceByName(L"BShiftService", serviceEnabled);
}

void SwitchProfile(const std::string& profileName) {
    currentProfile = profileName;
    // TODO: Implement IPC to tell service to switch profile
}

void ShowTrayMenu(HWND hwnd) {
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, nullptr);
    PostMessage(hwnd, WM_NULL, 0, 0);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAYICON && lParam == WM_RBUTTONUP) {
        ShowTrayMenu(hwnd);
        return 0;
    }
    else if (msg == WM_COMMAND) {
        int cmd = LOWORD(wParam);
        if (cmd == ID_TRAY_EXIT) {
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
        }
        else if (cmd == ID_TRAY_TOGGLE_SERVICE) {
            ToggleServiceState();
        }
        else if (cmd >= ID_TRAY_PROFILE_BASE) {
            std::string profileName = loadedConfig["profiles"][cmd - ID_TRAY_PROFILE_BASE]["name"];
            SwitchProfile(profileName);
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    HWND hwnd = CreateWindowEx(0, L"STATIC", L"BShiftTrayWnd", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hInstance, nullptr);
    LoadProfiles();

    hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_TRAY_TOGGLE_SERVICE, serviceEnabled ? L"Disable BShift" : L"Enable BShift");

    int index = 0;
    for (auto& profile : loadedConfig["profiles"]) {
        std::wstring name(profile["name"].begin(), profile["name"].end());
        AppendMenu(hMenu, MF_STRING, ID_TRAY_PROFILE_BASE + index, name.c_str());
        index++;
    }
    AppendMenu(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");

    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"BShift Tray");
    Shell_NotifyIcon(NIM_ADD, &nid);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}