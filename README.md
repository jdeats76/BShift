# 🖱️ BShift – Custom Mouse Profile Switcher for MMO Mice

BShift is a lightweight Windows service and system tray app that brings **G-Shift-style functionality** (like on the Logitech G600) to any programmable MMO mouse—especially the Redragon M908 Impact. 🎮

No proprietary software needed. Just plug in, map, and go!

---

## ✨ Features

- 🎮 **12-button MMO mouse support** with G-Shift modifier functionality
- 🔄 **Profile-based button remapping** using configurable shift button
- ⚙️ **JSON-configurable macros** for each button with dual-layer mapping
- 💻 **System Tray App** to toggle service and switch profiles on the fly
- 🔁 **Auto-start Windows service** on boot
- 🛠️ **Developer tools** to identify mouse button codes

---

## 🗂️ Project Structure

| Component | Description |
|-----------|-------------|
| **BShiftService** | Low-level background Windows service that handles button remapping |
| **BShiftTrayApp** | Windows system tray application for profile management |
| **BShiftKeyIdentifier** | Console utility to identify mouse button codes for configuration |
| **bshift.json** | User-defined configuration with profiles and button mappings |
| **Install-BShift.ps1** | PowerShell script to install and register the service |
| **Uninstall-BShift.ps1** | PowerShell script to remove the service |

---

## 🔧 Configuration

BShift uses a JSON configuration file located at `C:\bshift\bshift.json`. The configuration supports multiple profiles with customizable button mappings.

### Sample `bshift.json`

```json
{
  "profiles": [
    {
      "name": "Default",
      "description": "For everyday productivity use.",
      "shiftButton": "13",
      "buttonMappings": {
        "Button1": "Ctrl+C",
        "Button2": "Ctrl+V",
        "Button3": "Ctrl+Z",
        "Button4": "Ctrl+Y",
        "Button5": "Alt+Tab",
        "Button6": "Ctrl+T",
        "Button7": "Ctrl+W",
        "Button8": "Win+D",
        "Button9": "MediaPlayPause",
        "Button10": "MediaNextTrack",
        "Button11": "MediaPrevTrack",
        "Button12": "VolumeMute",

        "Button13": "Ctrl+Shift+C",
        "Button14": "Ctrl+Shift+V",
        "Button15": "Ctrl+Shift+Z",
        "Button16": "Ctrl+Shift+Y",
        "Button17": "Win+Tab",
        "Button18": "Ctrl+Shift+T",
        "Button19": "Alt+F4",
        "Button20": "Win+E",
        "Button21": "MediaStop",
        "Button22": "VolumeDown",
        "Button23": "VolumeUp",
        "Button24": "PrintScreen"
      }
    },
    {
      "name": "MMO Game",
      "description": "For use in World of Warcraft or Final Fantasy XIV.",
      "shiftButton": "g",
      "buttonMappings": {
        "Button1": "1",
        "Button2": "2",
        "Button3": "3",
        "Button4": "4",
        "Button5": "5",
        "Button6": "6",
        "Button7": "Q",
        "Button8": "E",
        "Button9": "R",
        "Button10": "F",
        "Button11": "Z",
        "Button12": "X",

        "Button13": "Shift+1",
        "Button14": "Shift+2",
        "Button15": "Shift+3",
        "Button16": "Shift+4",
        "Button17": "Shift+5",
        "Button18": "Shift+6",
        "Button19": "Shift+Q",
        "Button20": "Shift+E",
        "Button21": "Shift+R",
        "Button22": "Shift+F",
        "Button23": "Shift+Z",
        "Button24": "Shift+X"
      }
    }
  ]
}
```

### 📝 Configuration Notes

- **Buttons 1-12**: Primary button mappings
- **Buttons 13-24**: Alternate mappings when holding the shift button/key
- **shiftButton**: The button number (e.g., "13") or keyboard key (e.g., "g", "F1", "Space") that acts as the G-Shift modifier
- **Supported Shift Keys**: 
  - **Mouse buttons**: Numeric values like "13", "14", etc.
  - **Keyboard keys**: Single letters ("a"-"z"), numbers ("0"-"9")
  - **Special keys**: "Space", "Tab", "Enter", "Escape", "Shift", "Ctrl", "Alt", "Win"
  - **Function keys**: "F1" through "F12"
- **Supported Key Mappings**: Standard keys, key combinations (Ctrl+C), media keys (MediaPlayPause), and Windows keys (Win+D)

---

## 🛠️ Building the Project

### Requirements

- **Visual Studio 2022** with C++ development tools
- **vcpkg** package manager
- **Windows SDK**

### Dependencies

Install the required JSON library using vcpkg:

```bash
vcpkg install nlohmann-json:x64-windows
```

### Build Steps

1. Clone the repository:
   ```bash
   git clone https://github.com/jdeats76/BShift.git
   cd BShift
   ```

2. Open `BShiftService.sln` in Visual Studio 2022

3. Set the build configuration to **Release** and platform to **x64**

4. Build the solution (`Ctrl+Shift+B`)

The compiled executables will be available in the `x64/Release/` directory.

---

## ▶️ Installation & Usage

### Quick Install

1. **Build or download** the compiled executables
2. **Run as Administrator** and execute the install script:
   ```powershell
   .\Install-BShift.ps1
   ```
3. **Configure** your mouse buttons by editing `C:\bshift\bshift.json`
4. **Restart the service** or reboot to apply changes

### Manual Steps

The install script performs these actions:
- Copies executables to `C:\Program Files\BShift\`
- Copies configuration to `C:\bshift\`
- Registers BShiftService as a Windows service
- Adds BShiftTrayApp to Windows startup

### Using the Tray App

- **Right-click** the system tray icon to access the context menu
- **Switch profiles** on the fly
- **Enable/disable** the service
- **Exit** the application

---

## 🧪 Developer Tools

### BShiftKeyIdentifier

Use the `BShiftKeyIdentifier.exe` console tool to discover button codes sent by your mouse:

```bash
cd "C:\Program Files\BShift"
BShiftKeyIdentifier.exe
```

This tool helps you identify the correct button numbers for your `bshift.json` configuration.

---

## 🧼 Uninstalling

To completely remove BShift from your system:

```powershell
.\Uninstall-BShift.ps1
```

This will:
- Stop and remove the Windows service
- Remove startup registry entries
- Clean up installed files

---

## 🎯 Supported Mice

BShift is designed to work with any programmable MMO mouse, with particular focus on:

- **Redragon M908 Impact** ⭐ (Primary target)
- **Logitech G600** (Inspiration for G-Shift functionality)
- **Other 12+ button MMO mice** with programmable side buttons

---

## 🔧 Troubleshooting

### Service Won't Start
- Ensure you're running as Administrator
- Check Windows Event Viewer for service errors
- Verify `bshift.json` syntax is valid

### Buttons Not Responding
- Use `BShiftKeyIdentifier.exe` to verify button codes
- Check that your mouse software isn't conflicting
- Ensure button mappings in JSON are correct

### Configuration Changes Not Applied
- Restart the BShiftService after editing `bshift.json`
- Use the tray app to reload the configuration

---

## 🙏 Credits

Built with ❤️ by [jdeats76](https://github.com/jdeats76) to revive the beloved G600 experience on modern MMO mice.

**Special Thanks:**
- Logitech for the original G-Shift concept
- The MMO gaming community for inspiration
- Open-source contributors who make projects like this possible

---

## 📜 License

MIT License – free to use, modify, and contribute.

See [LICENSE](LICENSE) for full details.

---

## 💬 Contributing

Contributions are welcome! Here's how you can help:

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Commit** your changes (`git commit -m 'Add amazing feature'`)
4. **Push** to the branch (`git push origin feature/amazing-feature`)
5. **Open** a Pull Request

### 🐛 Bug Reports

If you encounter issues:
- Check existing [Issues](https://github.com/jdeats76/BShift/issues)
- Include your mouse model and Windows version
- Provide relevant log files and configuration

### 💡 Feature Requests

Have ideas for new features? Open an issue with:
- Clear description of the feature
- Use case examples
- Potential implementation ideas

---

## 🌟 Star This Project

If BShift helps enhance your gaming or productivity experience, consider giving it a ⭐ star on GitHub!
