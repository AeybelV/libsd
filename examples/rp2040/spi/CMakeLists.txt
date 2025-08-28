add_executable(spi spi.c)

# pull in libraries
target_link_libraries(spi pico_stdlib libsd hardware_spi)

if(PICO_CYW43_SUPPORTED)
  target_link_libraries(spi pico_cyw43_arch_none)
endif()

# create map/bin/hex file etc.
pico_add_extra_outputs(spi)
