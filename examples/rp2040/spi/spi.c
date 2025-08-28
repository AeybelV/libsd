#include "hardware/spi.h"

#include "libsd_mcu_defs.h"
#include "pico/stdlib.h"
#include "sd.h"
#include "sd_host.h"
#include "sd_types.h"

#include <boards/pico.h>

int main()
{
#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) ||                                 \
    !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_RX_PIN) ||                      \
    !defined(PICO_DEFAULT_SPI_CSN_PIN)
#warning spi example requires a board with SPI pins
    puts("Default SPI pins were not defined");
#else
    sd_host_t host;
    sd_card_t card;

    // Configure the host context for the controller
    sd_host_ctx_t host_ctx = {.spi = spi_default,
                              .rx_pin = PICO_DEFAULT_SPI_RX_PIN,
                              .tx_pin = PICO_DEFAULT_SPI_TX_PIN,
                              .sck_pin = PICO_DEFAULT_SPI_SCK_PIN,
                              .cs_pin = PICO_DEFAULT_SPI_CSN_PIN};

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
