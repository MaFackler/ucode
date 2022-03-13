@echo off

REM cl.exe /nologo src\win32_main.cpp /link user32.lib
cl.exe /std:c++17 /Fe:ucode /EHsc /nologo /Ilibs/doctest/doctest src\main.cpp /link user32.lib gdi32.lib

move ucode.exe build/ >NUL 2>&1
move main.obj build/ >NUL 2>&1
