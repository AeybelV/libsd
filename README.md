# libsd

An open source portable SD card stack for embedded systems.
It implements the full SD card command set, initialization flow,
and provides a block-level data access interface, while remaining 
independent of specific hardware or operating systems. 

## Structure

```
libsd
 ├─ cmake           # CMake helpers
 ├─ include         # Headers
 ├─ docs            # Documentation
 ├─ fs              # Filesytem shims
 ├─ src             # all source files for libsd stack
 ├─ tests           $ Units tests, hardware tests
 └─ hw              # Low level MCU core and peripheral drivers
```

## Overview

At its heart, libsd treats an SD card as a generic block device. Once the card is
initialized and configured, the library exposes a uniform read/write interface
that can be used directly or combined with higher-level filesystems layers.

## Architecture

The library is structured in layers:

- **Block Device Layer:** Provides a simple and uniform block read/write API. This is the interface intended for applications and filesystems.
- **SD Core:** Implements the SD card commnd set and logic.
- **Hardware Abstraction Layer (HAL):** Defines the minimal set of low-level operations needed to communicate with an SD card. Different backends (SPI, SDIO, SDHCI) can be plugged in here without affecting the rest of the stack.

## License

libsd is open source and released under the MIT license.  
Contributions, bug reports, and feature requests are welcome.
