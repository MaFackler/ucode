cmake_minimum_required(VERSION 3.13)
project(ucode)
set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    add_compile_options(/W3 /WX)
else()
    add_compile_options(-g -W -Wall -Werror)
endif()

include_directories(libs/doctest/doctest)
add_executable(${PROJECT_NAME} src/main.cpp src/ucode_terminal.cpp src/ucode_editor.cpp src/ucode_command.cpp src/ucode_input.cpp)

