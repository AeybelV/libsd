cmake_minimum_required(VERSION 3.15..3.25.1)
include_guard(GLOBAL)

if(NOT DEFINED PICO_SDK_PATH)
  message(FATAL_ERROR "PICO_SDK_PATH must be set for RP2040 backend")
endif()

message(STATUS "PICO_SDK_PATH = ${PICO_SDK_PATH}")

include(${CMAKE_CURRENT_LIST_DIR}/pico_sdk_import.cmake)

pico_sdk_init()

add_library(libsd_backend OBJECT ${CMAKE_CURRENT_LIST_DIR}/sd_rp2040.c)

target_link_libraries(libsd_backend OBJECT pico_stdlib hardware_spi)
