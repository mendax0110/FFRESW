@echo off
REM Define the source and destination paths
set SOURCE_DIR=Arduino-MemoryFree
set DEST_DIR=%USERPROFILE%\Documents\Arduino\libraries\pgmStrToRAM

REM Create the destination directory if it doesn't exist
if not exist "%DEST_DIR%" (
    mkdir "%DEST_DIR%"
)

REM Copy the .h and .cpp files to the destination directory
copy "%SOURCE_DIR%\*.h" "%DEST_DIR%"
copy "%SOURCE_DIR%\*.cpp" "%DEST_DIR%"

echo Files copied to %DEST_DIR%
pause

echo Please restart the Arduino IDE/Programio IDE to see the changes.