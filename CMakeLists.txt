cmake_minimum_required(VERSION 3.16)
project(ucode)
set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DDOCTEST_CONFIG_DISABLE)
    message("-- RELEASE BUILD!!!")
endif()

if (MSVC)
    add_compile_options(/W3 /WX)
else()
    add_compile_options(-W -Wall -Werror)
endif()

include_directories(libs/doctest/doctest)

file(GLOB SRC "src/*.cpp")
add_executable(uc ${SRC})

