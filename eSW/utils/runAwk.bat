@echo off

set AVR_SIZE="C:\Sloeber\arduinoPlugin\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino7\bin\avr-size"
set AWK="C:\Sloeber\arduinoPlugin\tools\awk\awk.exe"
set ELF_FILE="C:\Users\Adrian\Documents\git\FFRESW\eSW\FFRESW\FFRESW\Release\FFRESW.elf"
set AWK_SCRIPT="C:\Users\Adrian\Documents\git\FFRESW\eSW\FFRESW\FFRESW\Release\size.awk"

%AVR_SIZE% -A %ELF_FILE% | %AWK% -f %AWK_SCRIPT%

pause
