@echo off
cd /d "%~dp0"

set PATH=C:\msys64\mingw32\bin;%PATH%

if not exist "build" mkdir "build"
if not exist "build\32bit" mkdir "build\32bit"

windres resources\resources.rc resources\resources.o

gcc ^
resources\resources.o ^
code\main.c ^
code\framework\*.c -Icode\framework ^
code\generators\*.c -Icode\generators ^
-std=c99 ^
-Wall -Wextra -Wshadow ^
-O2 -flto -s ^
-fstack-protector-all -D_FORTIFY_SOURCE=2 ^
-o build\32bit\cmg.exe

del resources\resources.o

copy /Y "resources\config.txt" "build\32bit\config.txt"

pause
