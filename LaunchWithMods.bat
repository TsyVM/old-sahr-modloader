@echo off
setlocal

echo ================================================
echo    SHAR ModLoader - Game Launcher
echo ================================================
echo.

REM Check if SHARModLoader.dll exists
if not exist "SHARModLoader.dll" (
    echo ERROR: SHARModLoader.dll not found in current directory!
    echo Please copy SHARModLoader.dll to your game folder.
    echo.
    pause
    exit /b 1
)

REM Check if SHARInjector.exe exists
if not exist "SHARInjector.exe" (
    echo ERROR: SHARInjector.exe not found in current directory!
    echo Please copy SHARInjector.exe to your game folder.
    echo.
    pause
    exit /b 1
)

REM Check if Simpsons.exe exists
if not exist "Simpsons.exe" (
    echo ERROR: Simpsons.exe not found!
    echo Please place this script in your SHAR game directory.
    echo.
    pause
    exit /b 1
)

REM Create mods directory if it doesn't exist
if not exist "mods" (
    echo Creating mods directory...
    mkdir mods
    echo.
)

REM Check for Safe Mode
if "%1"=="--safe" (
    echo SAFE MODE ENABLED - Mods will be disabled
    set SHAR_SAFEMODE=1
    echo.
)

echo Starting The Simpsons: Hit ^& Run...
start "" "Simpsons.exe"

echo Waiting for game to initialize...
timeout /t 3 /nobreak >nul

echo.
echo Injecting ModLoader...
SHARInjector.exe "%~dp0SHARModLoader.dll"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ================================================
    echo    ModLoader injected successfully!
    echo ================================================
    echo.
    echo Check ModLoader.log for details.
    echo Have fun with your mods!
) else (
    echo.
    echo ================================================
    echo    Injection failed!
    echo ================================================
    echo.
    echo Troubleshooting:
    echo   - Try running this script as Administrator
    echo   - Check if antivirus is blocking the injector
    echo   - Make sure the game is actually running
)

echo.
pause