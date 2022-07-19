@echo off
REM echo %cd%
cl.exe /nologo /Fo./build/ /std:c++17 /W3 /WX /EHsc /D_CRT_SECURE_NO_WARNINGS src\main.cpp /Ilibs/doctest/doctest /Ilibs/mflibs/src user32.lib gdi32.lib opengl32.lib /Fe./build/ucode.exe
