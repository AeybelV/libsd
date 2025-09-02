# RP2040 Platform Port

This folder contains the RP2040 hardware port for the SD stack.

## Bus Driver Support

| Bus        | Support | Notes                                                     |
| ---------- | :-----: | --------------------------------------------------------- |
| SPI        |    ✅    | Uses the RP2040 SPI peripherals (provided by Pico SDK) with software-controlled CS |
| SDMMC/SDIO |    ❌    | RP2040 has no SDMMC/SDIO controller. Plans to implement a bitbanged implementation                       |
| SDHCI      |    ❌    | Not applicable on RP2040                                  |

## CMake Options

| Option          | Type   | Required | Example                          | Purpose                                        |
| --------------- | ------ | :------: | -------------------------------- | ---------------------------------------------- |
| `TARGET_MCU`    | string |     ✅    | `-DTARGET_MCU=rp2040`            | Selects the RP2040 as the MCU to build the library for |
| `PICO_SDK_PATH` | path   |     ✅    | `-DPICO_SDK_PATH=/opt/pico-sdk` | Points CMake to your Pico SDK checkout         |

## Examples

### Building

To build the static library

```sh
# Create build directory
mkdir build
cd build

# Create build files
cmake -DTARGET_MCU=rp2040 -DPICO_SDK_PATH=<SDK PATH> ..

# Build the library (generates a libsd.a)
make libsd -j$(nproc)
```

Otherwise you can include the library in your project using CMake.

Your projects CMakeLists.txt

```cmake

# ... your CMakeLists

# Pico SDK import
set(PICO_SDK_PATH "$ENV{PICO_SDK_PATH}" CACHE PATH "Path to Pico SDK") # Set PICO_SDK_PATH to wherever you have the SDK
include(${pico_sdk_path}/external/pico_sdk_import.cmake) # Use the SDK import cmake file from the pico-sdk if needed
pico_sdk_init() # Initialize the Pico SDK

# Bring in libsd
set(TARGET_MCU rp2040 CACHE STRING "MCU port")
add_subdirectory(<path to libsd>)

# Link the library
target_link_libraries(<target> libsd)

# .. your CMakeLists

```
### SPI

```c
// rp2040 pico-sdk includes
#include "hardware/spi.h"             //  Provides pico-sdk access to SPI peripheral
#include "pico/stdlib.h"
#include <boards/pico.h>

// libsd includes
#include "libsd_mcu_defs.h"           // A header containing anything MCU specific, is generated for every platform, provides sd_host_ctx_t
#include "sd.h"                       // Provides the SD Card API
#include "sd_host.h"                  // Provides functions for setting up a SD Host Controller
#include "sd_types.h"                 // Types and enums used in the library

int main(void) {
// Checks if the default SPI peripheral and default SPI pins are defined
#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) ||                                 \
  !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_RX_PIN) ||                      \
  !defined(PICO_DEFAULT_SPI_CSN_PIN)
#warning spi example requires a board with SPI pins
  puts("Default SPI pins were not defined");
#else

  // Instantiates host and card struct
  sd_host_t host;
  sd_card_t card;

  // Configure the host context for the controller
  sd_host_ctx_t host_ctx = {.spi = spi_default,
                            .rx_pin = PICO_DEFAULT_SPI_RX_PIN,
                            .tx_pin = PICO_DEFAULT_SPI_TX_PIN,
                            .sck_pin = PICO_DEFAULT_SPI_SCK_PIN,
                            .cs_pin = PICO_DEFAULT_SPI_CSN_PIN};

  // Sets the host context on the controller
  SD_HOST_SET_CTX(&host, &host_ctx);

  // Initialize host and controller
  init_host(&host);

  // Initializes sd card
  sd_init(&host, &card);

  while (true)
  {
      ;
  }
#endif
}
```
