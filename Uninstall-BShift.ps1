# Uninstall-BShift.ps1
$targetDir = "C:\Program Files\BShift"
$configDir = "C:\bshift"
$trayRegKey = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run"

# 1. Stop and delete the service
if (Get-Service -Name BShiftService -ErrorAction SilentlyContinue) {
    Write-Host "Stopping BShiftService..."
    Stop-Service -Name BShiftService -Force
    sc.exe delete BShiftService
}

# 2. Remove application files
if (Test-Path $targetDir) {
    Remove-Item -Path $targetDir -Recurse -Force
    Write-Host "Removed $targetDir"
}

# 3. Remove config folder
if (Test-Path $configDir) {
    Remove-Item -Path $configDir -Recurse -Force
    Write-Host "Removed $configDir"
}

# 4. Remove tray auto-run key
if (Test-Path "$trayRegKey\\BShiftTray") {
    Remove-ItemProperty -Path $trayRegKey -Name "BShiftTray"
    Write-Host "Removed tray app from startup."
}

Write-Host "Uninstallation complete."
