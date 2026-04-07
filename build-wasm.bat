@echo off
setlocal
echo Building Viby Farming Simulator for WebAssembly...

set BUILD_DIR=build
set REACT_PUBLIC_DIR=react-ui\public

where emcmake >nul 2>nul
if errorlevel 1 (
  echo ERROR: emcmake was not found. Install and activate Emscripten SDK first.
  echo See: https://emscripten.org/docs/getting_started/downloads.html
  exit /b 1
)

where emmake >nul 2>nul
if errorlevel 1 (
  echo ERROR: emmake was not found. Install and activate Emscripten SDK first.
  echo See: https://emscripten.org/docs/getting_started/downloads.html
  exit /b 1
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd %BUILD_DIR%

echo Configuring CMake with Emscripten...
call emcmake cmake .. -DBUILD_FOR_WEB=ON
if errorlevel 1 exit /b 1

echo Building...
call emmake make
if errorlevel 1 exit /b 1

if not exist "..\%REACT_PUBLIC_DIR%" mkdir "..\%REACT_PUBLIC_DIR%"

echo Copying WASM output to React public directory...
copy /Y viby-farm.js ..\%REACT_PUBLIC_DIR%\
if errorlevel 1 exit /b 1
copy /Y viby-farm.wasm ..\%REACT_PUBLIC_DIR%\
if errorlevel 1 exit /b 1

echo Build complete! Files copied to %REACT_PUBLIC_DIR%
echo Run 'cd react-ui ^&^& npm run dev' to start the dev server.
endlocal
