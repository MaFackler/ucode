cmake_minimum_required(VERSION 3.13)
project(ucode)
set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    add_compile_options(/W3 /WX)
else()
    add_compile_options(-W -Wall -Werror)
endif()

add_executable(${PROJECT_NAME} src/main.cpp src/ucode_terminal.cpp)

