#include "pico/stdlib.h"
#include "hardware/spi.h"

// Define Pins
#define SPI_PORT spi1
#define PIN_MISO  -1  // Not used
#define PIN_SCK   14  // Clock
#define PIN_MOSI  15  // Data out
#define PIN_DC    11  // Data/Command
#define PIN_RST   12  // Reset
#define PIN_BL    10  // Backlight (optional)

// Function to send a command to the ST7789
void st7789_send_command(uint8_t command) {
    gpio_put(PIN_DC, 0);  // DC low for command
    spi_write_blocking(SPI_PORT, &command, 1);
}

// Function to send data to the ST7789
void st7789_send_data(uint8_t *data, size_t length) {
    gpio_put(PIN_DC, 1);  // DC high for data
    spi_write_blocking(SPI_PORT, data, length);
}

// Function to initialize the ST7789
void st7789_init() {
    // Reset the display
    gpio_put(PIN_RST, 0);
    sleep_ms(100);
    gpio_put(PIN_RST, 1);
    sleep_ms(100);

    // Software reset
    st7789_send_command(0x01);
    sleep_ms(150);

    // Exit Sleep mode
    st7789_send_command(0x11);
    sleep_ms(500);

    // Set color mode
    st7789_send_command(0x3A);
    uint8_t color_mode = 0x07;
    st7789_send_data(&color_mode, 1);

    // Memory Access Control (Orientation and mode)
    st7789_send_command(0x36);
    uint8_t orientation = 0x08;
    st7789_send_data(&orientation, 1);

    // Inversion
    st7789_send_command(0x21);
    sleep_ms(10);

    // Display
    st7789_send_command(0x29);
    sleep_ms(100);
}

// Function to fill the screen
void st7789_fill_screen_rgb888(uint8_t red, uint8_t green, uint8_t blue) {
    uint16_t x_start = 0;
    uint16_t x_end = 240 - 1;  // Adjust for screen size
    uint16_t y_start = 0;
    uint16_t y_end = 240 - 1;  // Adjust for screen size

    // Set column address
    st7789_send_command(0x2A);  // Column Address Set
    uint8_t col_data[4] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
    st7789_send_data(col_data, 4);

    // Set row address
    st7789_send_command(0x2B);  // Row Address Set
    uint8_t row_data[4] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
    st7789_send_data(row_data, 4);

    // Write to memory
    st7789_send_command(0x2C);  // Memory Write

    uint8_t color_data[240 * 3];  // Buffer for one row (240 pixels)
    for (int i = 0; i < 240; i++) {
        color_data[i * 3 + 0] = blue;
        color_data[i * 3 + 1] = green;
        color_data[i * 3 + 2] = red;
    }

    for (int row = 0; row < 240; row++) {
        st7789_send_data(color_data, sizeof(color_data));
    }
}

int main()
{
    // SPI initialization
    spi_init(SPI_PORT, 30 * 1000 * 1000);  // 30 MHz for faster data transfer
    spi_set_format(SPI_PORT, 8, SPI_CPOL_1, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);

    // GPIO init
    gpio_init(PIN_DC);
    gpio_set_dir(PIN_DC, GPIO_OUT);

    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);

    gpio_init(PIN_BL);
    gpio_set_dir(PIN_BL, GPIO_OUT);
    gpio_put(PIN_BL, 1);  // Turn on backlight

    // Initialize the display
    st7789_init();

    // Test: Fill screen with different
    st7789_fill_screen_rgb888(255, 0, 0);  // Red
    sleep_ms(1000);

    st7789_fill_screen_rgb888(0, 255, 0);  // Green
    sleep_ms(1000);

    st7789_fill_screen_rgb888(0, 0, 255);  // Blue
    sleep_ms(1000);

    st7789_fill_screen_rgb888(255, 255, 255);  // White
    sleep_ms(1000);

    st7789_fill_screen_rgb888(0, 0, 0);  // Black
    sleep_ms(1000);

    return 0;
}