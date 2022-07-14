@echo off
REM echo %cd%
cl.exe /nologo /std:c++17 /W3 /WX /EHsc src\main.cpp /Ilibs/doctest/doctest
