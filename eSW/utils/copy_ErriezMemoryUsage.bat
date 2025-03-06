@echo off
REM Define the source and destination paths
set SOURCE_DIR=..\..\external\ErriezMemoryUsage
set LIBRARY_DIR=..\libraries
set DEST_DIR=%USERPROFILE%\Documents\Arduino\libraries\pgmStrToRAM

REM Create the destination directory if it doesn't exist
if not exist "%DEST_DIR%" (
    mkdir "%DEST_DIR%"
)

REM Copy the .h and .cpp files from SOURCE_DIR to the destination directory
copy "%SOURCE_DIR%\*.h" "%DEST_DIR%"
copy "%SOURCE_DIR%\*.cpp" "%DEST_DIR%"

REM Copy all folders and their contents from LIBRARY_DIR to the Documents Arduino libraries
xcopy "%LIBRARY_DIR%\*" "%USERPROFILE%\Documents\Arduino\libraries\" /E /I /Y

echo Files copied to %DEST_DIR% and libraries copied to %USERPROFILE%\Documents\Arduino\libraries\
pause

echo Please restart the Arduino IDE/Programio IDE to see the changes.
