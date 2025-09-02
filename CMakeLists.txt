cmake_minimum_required(VERSION 3.15..3.25.1)

# Choose the MCU (e.g., -DTARGET_MCU=rp2040)
set(TARGET_MCU
    "rp2040"
    CACHE STRING "Target MCU family (rp2040, stm32f4, nrf52, ...)")

message(STATUS "libsd")
message(STATUS "TARGET_MCU = ${TARGET_MCU}")

# Sets GEN_DIR, which will contain generated headers for the target platform
set(GEN_DIR "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${GEN_DIR}")

# Includes the CMake file for the target platform
include(${CMAKE_CURRENT_LIST_DIR}/hw/${TARGET_MCU}/family.cmake)
set(TARGET_MCU_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/hw/${TARGET_MCU}/libsd_mcu_defs.h")

project(
  libsd
  VERSION 0.1
  DESCRIPTION "An open source portable SD card stack for embedded systems."
  LANGUAGES C CXX)

option(cppcheck "Run CppCheck static code analysis" ON)

add_library(libsd STATIC src/sd_core.c src/sd_spi.c)

# Link the selected backend + vendor hal into the core
target_sources(libsd PRIVATE $<TARGET_OBJECTS:libsd_backend>)
configure_file("${TARGET_MCU_INCLUDES}" "${GEN_DIR}/libsd_mcu_defs.h" COPYONLY)

if(cppcheck)
  find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck REQUIRED)
  if(CMAKE_CXX_CPPCHECK)
    list(APPEND CMAKE_CXX_CPPCHECK "--enable=all")
  endif()
endif()

target_include_directories(libsd PUBLIC "include" "${GEN_DIR}")

# Remove the lib prefix to prevent duplicate name
set_target_properties(libsd PROPERTIES PREFIX "")
