#include <iostream>
#include <windows.h>
#include <unordered_map>
#include <string>

bool isShiftPressed() {
    return (GetAsyncKeyState(VK_SHIFT) & 0x8000);
}

bool isCtrlPressed() {
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000);
}

bool isAltPressed() {
    return (GetAsyncKeyState(VK_MENU) & 0x8000);
}

// Map VK_* codes to printable names
std::unordered_map<int, std::string> keyNames = {
    { 'A', "A" }, { 'B', "B" }, { 'C', "C" }, { 'D', "D" }, { 'E', "E" },
    { 'F', "F" }, { 'G', "G" }, { 'H', "H" }, { 'I', "I" }, { 'J', "J" },
    { 'K', "K" }, { 'L', "L" }, { 'M', "M" }, { 'N', "N" }, { 'O', "O" },
    { 'P', "P" }, { 'Q', "Q" }, { 'R', "R" }, { 'S', "S" }, { 'T', "T" },
    { 'U', "U" }, { 'V', "V" }, { 'W', "W" }, { 'X', "X" }, { 'Y', "Y" },
    { 'Z', "Z" },
    { '0', "0" }, { '1', "1" }, { '2', "2" }, { '3', "3" }, { '4', "4" },
    { '5', "5" }, { '6', "6" }, { '7', "7" }, { '8', "8" }, { '9', "9" },
    { VK_SPACE, "Space" }, { VK_RETURN, "Enter" }, { VK_ESCAPE, "Esc" },
    { VK_TAB, "Tab" }, { VK_BACK, "Backspace" }, { VK_DELETE, "Delete" },
    { VK_LEFT, "Left" }, { VK_RIGHT, "Right" }, { VK_UP, "Up" }, { VK_DOWN, "Down" },
    { VK_HOME, "Home" }, { VK_END, "End" }, { VK_PRIOR, "PageUp" }, { VK_NEXT, "PageDown" },
    { VK_LCONTROL, "Ctrl" }, { VK_RCONTROL, "Ctrl" }, { VK_LSHIFT, "Shift" }, { VK_RSHIFT, "Shift" },
    { VK_MENU, "Alt" }, // VK_MENU is Alt
    { VK_MEDIA_PLAY_PAUSE, "MediaPlayPause" },
    { VK_MEDIA_NEXT_TRACK, "MediaNextTrack" },
    { VK_MEDIA_PREV_TRACK, "MediaPrevTrack" },
    { VK_VOLUME_MUTE, "MediaMute" },
    { VK_VOLUME_DOWN, "VolumeDown" },
    { VK_VOLUME_UP, "VolumeUp" },
    { VK_SNAPSHOT, "PrintScreen" }
};

std::string buildKeyCombo(int vkCode) {
    std::string result;
    if (isCtrlPressed()) result += "Ctrl+";
    if (isShiftPressed()) result += "Shift+";
    if (isAltPressed()) result += "Alt+";

    if (keyNames.count(vkCode)) {
        result += keyNames[vkCode];
    }
    else {
        char ch = MapVirtualKeyA(vkCode, MAPVK_VK_TO_CHAR);
        if (isprint(ch)) {
            result += ch;
        }
        else {
            result += "VK_" + std::to_string(vkCode);
        }
    }

    return result;
}

int main() {
    std::cout << "Press Shift+ESC to exit\n\n";

    SHORT prevState[256] = { 0 };

    while (true) {
        for (int vk = 0; vk < 256; ++vk) {
            SHORT keyState = GetAsyncKeyState(vk);
            if ((keyState & 0x8000) && !(prevState[vk] & 0x8000)) {
                std::string keyCombo = buildKeyCombo(vk);
                std::cout << keyCombo << std::endl;

                if (vk == VK_ESCAPE && isShiftPressed()) {
                    std::cout << "\nExiting...\n";
                    return 0;
                }
            }
            prevState[vk] = keyState;
        }
        Sleep(10);
    }
}
