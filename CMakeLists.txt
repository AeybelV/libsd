cmake_minimum_required(VERSION 3.15..3.25.1)
project(
  libsd
  VERSION 0.1
  DESCRIPTION "An open source portable SD card stack for embedded systems."
  LANGUAGES C CXX)

option(cppcheck "Run CppCheck static code analysis" ON)

# Choose the MCU (e.g., -DTARGET_MCU=rp2040)
set(TARGET_MCU
    "rp2040"
    CACHE STRING "Target MCU family (rp2040, stm32f4, nrf52, ...)")

message(STATUS "sdlib")
message(STATUS "TARGET_MCU = ${TARGET_MCU}")

add_library(libsd STATIC src/sd_core.c)

if(cppcheck)
  find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck REQUIRED)
  if(CMAKE_CXX_CPPCHECK)
    list(APPEND CMAKE_CXX_CPPCHECK "--enable=all")
  endif()
endif()

target_include_directories(libsd PUBLIC include)
