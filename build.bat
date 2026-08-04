@echo off
setlocal enabledelayedexpansion

echo ================================================
echo    SHAR ModLoader - Automated Build Script
echo ================================================
echo.

REM Check if CMake is installed
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found in PATH!
    echo Please install CMake from https://cmake.org/download/
    echo.
    pause
    exit /b 1
)

echo [1/5] Checking CMake version...
cmake --version | findstr /C:"version"
echo.

REM Detect Visual Studio version
echo [2/5] Detecting Visual Studio...

set VS_VERSION=
set VS_GENERATOR=

REM Check for VS 2022
if exist "C:\Program Files\Microsoft Visual Studio\2022" (
    set VS_VERSION=2022
    set VS_GENERATOR=Visual Studio 17 2022
    echo Found: Visual Studio 2022
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022" (
    set VS_VERSION=2022
    set VS_GENERATOR=Visual Studio 17 2022
    echo Found: Visual Studio 2022
)

REM Check for VS 2019 if 2022 not found
if not defined VS_VERSION (
    if exist "C:\Program Files\Microsoft Visual Studio\2019" (
        set VS_VERSION=2019
        set VS_GENERATOR=Visual Studio 16 2019
        echo Found: Visual Studio 2019
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019" (
        set VS_VERSION=2019
        set VS_GENERATOR=Visual Studio 16 2019
        echo Found: Visual Studio 2019
    )
)

if not defined VS_VERSION (
    echo ERROR: Visual Studio 2019 or 2022 not found!
    echo Please install Visual Studio with C++ Desktop Development workload.
    echo.
    pause
    exit /b 1
)

echo.

REM Create build directory
echo [3/5] Creating build directory...
if exist build (
    echo Build directory already exists. Cleaning...
    rmdir /S /Q build
)
mkdir build
cd build

echo.

REM Generate project files
echo [4/5] Generating Visual Studio project (Win32 architecture)...
echo Running: cmake .. -G "%VS_GENERATOR%" -A Win32
echo.

cmake .. -G "%VS_GENERATOR%" -A Win32

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: CMake generation failed!
    echo Check the error messages above.
    cd ..
    pause
    exit /b 1
)

echo.
echo CMake generation successful!
echo.

REM Build the project
echo [5/5] Building Release configuration...
echo This may take 30-60 seconds...
echo.

cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Build failed!
    echo Check the error messages above.
    cd ..
    pause
    exit /b 1
)

echo.
echo ================================================
echo    BUILD SUCCESSFUL!
echo ================================================
echo.
echo Output files:
echo   - build\bin\SHARModLoader.dll
echo   - build\bin\SHARInjector.exe
echo.
echo Next steps:
echo   1. Copy both files to your SHAR game directory
echo   2. Create a 'mods' folder in the game directory
echo   3. Run the game, then inject with: SHARInjector.exe SHARModLoader.dll
echo.

cd ..

REM Create example mod structure in build output
if not exist "build\bin\mods\01_example_mod" (
    echo Creating example mod structure...
    mkdir "build\bin\mods\01_example_mod"
    echo {"name": "Example Mod", "version": "1.0.0", "author": "You"} > "build\bin\mods\01_example_mod\mod.json"
    echo Example mod.json created in build\bin\mods\01_example_mod\
    echo.
)

echo Build script completed!
echo.
pause