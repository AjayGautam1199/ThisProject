/**
 * @file st7701s_driver.h
 * @brief Driver for ST7701S TFT display controller with STM32F750
 * @details Supports 480x480 18-bit RGB display via LTDC and initialization via SPI
 */

 #ifndef ST7701S_DRIVER_H
 #define ST7701S_DRIVER_H
 
 #include "stm32f7xx_hal.h"
 
 /* Display Parameters */
 #define ST7701S_WIDTH           480
 #define ST7701S_HEIGHT          480
 #define ST7701S_HSYNC           10      // Horizontal sync
 #define ST7701S_HBP             10      // Horizontal back porch
 #define ST7701S_HFP             10      // Horizontal front porch
 #define ST7701S_VSYNC           10      // Vertical sync
 #define ST7701S_VBP             10      // Vertical back porch
 #define ST7701S_VFP             10      // Vertical front porch
 
 /* Command Definitions */
 #define ST7701S_CMD_NOP         0x00    // No operation
 #define ST7701S_CMD_SWRESET     0x01    // Software reset
 #define ST7701S_CMD_RDDID       0x04    // Read display ID
 #define ST7701S_CMD_SLPIN       0x10    // Sleep in
 #define ST7701S_CMD_SLPOUT      0x11    // Sleep out
 #define ST7701S_CMD_PTLON       0x12    // Partial mode on
 #define ST7701S_CMD_NORON       0x13    // Normal display mode on
 #define ST7701S_CMD_INVOFF      0x20    // Display inversion off
 #define ST7701S_CMD_INVON       0x21    // Display inversion on
 #define ST7701S_CMD_DISPOFF     0x28    // Display off
 #define ST7701S_CMD_DISPON      0x29    // Display on
 #define ST7701S_CMD_CASET       0x2A    // Column address set
 #define ST7701S_CMD_RASET       0x2B    // Row address set
 #define ST7701S_CMD_RAMWR       0x2C    // Memory write
 #define ST7701S_CMD_RAMRD       0x2E    // Memory read
 #define ST7701S_CMD_PTLAR       0x30    // Partial start/end address
 #define ST7701S_CMD_VSCRDEF     0x33    // Vertical scrolling definition
 #define ST7701S_CMD_MADCTL      0x36    // Memory data access control
 #define ST7701S_CMD_COLMOD      0x3A    // Interface pixel format
 #define ST7701S_CMD_VCMOFSET    0xC5    // VCOM offset set
 
 /* SPI Commands */
 #define ST7701S_CMD_MODE        0xFF    // Command2 BKx selection
 
 /* Color definitions */
 #define ST7701S_COLOR_BLACK     0x0000
 #define ST7701S_COLOR_WHITE     0xFFFF
 #define ST7701S_COLOR_RED       0xF800
 #define ST7701S_COLOR_GREEN     0x07E0
 #define ST7701S_COLOR_BLUE      0x001F
 
 /* Function prototypes */
 HAL_StatusTypeDef ST7701S_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *reset_port, uint16_t reset_pin, GPIO_TypeDef *dc_port, uint16_t dc_pin);
 void ST7701S_Reset(void);
 void ST7701S_WriteCommand(uint8_t cmd);
 void ST7701S_WriteData(uint8_t data);
 void ST7701S_InitLTDC(LTDC_HandleTypeDef *hltdc);
 void ST7701S_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
 void ST7701S_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
 void ST7701S_DisplayOn(void);
 void ST7701S_DisplayOff(void);
 
 #endif /* ST7701S_DRIVER_H */