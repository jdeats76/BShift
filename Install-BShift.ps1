# Install-BShift.ps1
$targetDir = "C:\Program Files\BShift"
$configDir = "C:\bshift"
$trayRegKey = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run"

# Executables and config
$serviceExe = "BShiftService.exe"
$trayExe = "BShiftTray.exe"
$configFile = "bshift.json"

# 1. Create target directories
New-Item -ItemType Directory -Force -Path $targetDir | Out-Null
New-Item -ItemType Directory -Force -Path $configDir | Out-Null

# 2. Copy files to install locations
Copy-Item -Path $serviceExe -Destination "$targetDir\" -Force
Copy-Item -Path $trayExe -Destination "$targetDir\" -Force
Copy-Item -Path $configFile -Destination "$configDir\" -Force

# 3. Install the Windows Service
Write-Host "Installing BShiftService..."
sc.exe create BShiftService binPath= "\"$targetDir\$serviceExe\"" start= auto

# 4. Start the service
Write-Host "Starting BShiftService..."
Start-Service -Name BShiftService

# 5. Register tray app to auto-run at login
Write-Host "Registering tray app to startup..."
Set-ItemProperty -Path $trayRegKey -Name "BShiftTray" -Value "`"$targetDir\$trayExe`""

Write-Host "Installation complete."
