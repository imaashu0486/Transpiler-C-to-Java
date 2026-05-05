@echo off
echo Building C to Java Transpiler...

cd /d %~dp0

if exist build (
    echo Cleaning previous build...
    rmdir /s /q build
)

echo Creating build directory...
mkdir build
cd build

echo Configuring with CMake...
cmake ..
if errorlevel 1 (
    echo CMake configuration failed!
    exit /b 1
)

echo Building project...
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executable: build\Release\transpiler.exe
pause
