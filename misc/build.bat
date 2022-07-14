@echo off
REM echo %cd%
cl.exe /nologo /std:c++17 /W3 /WX /EHsc src\main.cpp /Ilibs/doctest/doctest /Ilibs/mflibs/src user32.lib gdi32.lib opengl32.lib
