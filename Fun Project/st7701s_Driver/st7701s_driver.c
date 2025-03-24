/**
 * @file st7701s_driver.c
 * @brief Driver implementation for ST7701S TFT display controller with STM32F750
 */

 #include "st7701s_driver.h"

 /* Private variables */
 static SPI_HandleTypeDef *ST7701S_SPI;
 static GPIO_TypeDef *ST7701S_ResetPort;
 static uint16_t ST7701S_ResetPin;
 static GPIO_TypeDef *ST7701S_DCPort;
 static uint16_t ST7701S_DCPin;
 static LTDC_HandleTypeDef *ST7701S_LTDC;
 
 /**
  * @brief Initialize the ST7701S display
  * @param hspi: SPI Handle for command transmission
  * @param reset_port: GPIO port for reset pin
  * @param reset_pin: GPIO pin for reset
  * @param dc_port: GPIO port for data/command selection
  * @param dc_pin: GPIO pin for data/command selection
  * @return HAL status
  */
 HAL_StatusTypeDef ST7701S_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *reset_port, uint16_t reset_pin, GPIO_TypeDef *dc_port, uint16_t dc_pin)
 {
     /* Store interface parameters */
     ST7701S_SPI = hspi;
     ST7701S_ResetPort = reset_port;
     ST7701S_ResetPin = reset_pin;
     ST7701S_DCPort = dc_port;
     ST7701S_DCPin = dc_pin;
     
     /* Reset the display */
     ST7701S_Reset();
     
     /* Initialize display with specific commands */
     ST7701S_WriteCommand(ST7701S_CMD_SLPOUT);    // Exit sleep mode
     HAL_Delay(120);                              // Required delay after sleep out
     
     /* Enter command mode 2 */
     ST7701S_WriteCommand(0xFF);
     ST7701S_WriteData(0x77);
     ST7701S_WriteData(0x01);
     ST7701S_WriteData(0x00);
     ST7701S_WriteData(0x00);
     ST7701S_WriteData(0x10);
     
     /* Configure display settings */
     ST7701S_WriteCommand(0xC0);                  // Display Control 1
     ST7701S_WriteData(0x3B);                     // 480 pixels
     ST7701S_WriteData(0x00);
     
     ST7701S_WriteCommand(0xC1);                  // Display Control 2
     ST7701S_WriteData(0x0D);                     // VBP
     ST7701S_WriteData(0x02);
     
     ST7701S_WriteCommand(0xC2);                  // Display Control 3
     ST7701S_WriteData(0x31);                     // Line inversion, 31h = 2-dot
     ST7701S_WriteData(0x05);
     
     /* Gamma settings */
     ST7701S_WriteCommand(0xB0);                  // Positive Voltage Gamma Control
     ST7701S_WriteData(0x00);
     ST7701S_WriteData(0x11);
     ST7701S_WriteData(0x18);
     ST7701S_WriteData(0x0E);
     ST7701S_WriteData(0x11);
     ST7701S_WriteData(0x06);
     ST7701S_WriteData(0x07);
     ST7701S_WriteData(0x08);
     ST7701S_WriteData(0x07);
     ST7701S_WriteData(0x22);
     ST7701S_WriteData(0x04);
     ST7701S_WriteData(0x12);
     ST7701S_WriteData(0x0F);
     ST7701S_WriteData(0xAA);
     ST7701S_WriteData(0x31);
     ST7701S_WriteData(0x18);
     
     ST7701S_WriteCommand(0xB1);                  // Negative Voltage Gamma Control
     ST7701S_WriteData(0x00);
     ST7701S_WriteData(0x11);
     ST7701S_WriteData(0x19);
     ST7701S_WriteData(0x0E);
     ST7701S_WriteData(0x12);
     ST7701S_WriteData(0x07);
     ST7701S_WriteData(0x08);
     ST7701S_WriteData(0x08);
     ST7701S_WriteData(0x08);
     ST7701S_WriteData(0x22);
     ST7701S_WriteData(0x04);
     ST7701S_WriteData(0x11);
     ST7701S_WriteData(0x11);
     ST7701S_WriteData(0xA9);
     ST7701S_WriteData(0x32);
     ST7701S_WriteData(0x18);
     
     /* Exit command mode 2 */
     ST7701S_WriteCommand(0xFF);
     ST7701S_WriteData(0x77);
     ST7701S_WriteData(0x01);
     ST7701S_WriteData(0x00);
     ST7701S_WriteData(0x00);
     ST7701S_WriteData(0x11);
     
     /* Vcom settings */
     ST7701S_WriteCommand(0xB0);
     ST7701S_WriteData(0x60);
     
     /* Exit command mode */
     ST7701S_WriteCommand(0xFF);
     ST7701S_WriteData(0x77);
     ST7701S_WriteData(0x01);
     ST7701S_WriteData(0x00);
     ST7701S_WriteData(0x00);
     ST7701S_WriteData(0x00);
     
     /* Set 18-bit RGB color mode (666) */
     ST7701S_WriteCommand(ST7701S_CMD_COLMOD);
     ST7701S_WriteData(0x66);                     // 18-bit/pixel (666 RGB)
     
     /* Set normal display mode */
     ST7701S_WriteCommand(ST7701S_CMD_NORON);
     
     /* Turn display on */
     ST7701S_WriteCommand(ST7701S_CMD_DISPON);
     
     return HAL_OK;
 }
 
 /**
  * @brief Reset the ST7701S display
  */
 void ST7701S_Reset(void)
 {
     /* Hardware reset sequence */
     HAL_GPIO_WritePin(ST7701S_ResetPort, ST7701S_ResetPin, GPIO_PIN_RESET);
     HAL_Delay(10);
     HAL_GPIO_WritePin(ST7701S_ResetPort, ST7701S_ResetPin, GPIO_PIN_SET);
     HAL_Delay(120);  // Recommended delay after reset
 }
 
 /**
  * @brief Send command to the display via SPI
  * @param cmd: Command byte to send
  */
 void ST7701S_WriteCommand(uint8_t cmd)
 {
     /* Set DC pin to command mode (low) */
     HAL_GPIO_WritePin(ST7701S_DCPort, ST7701S_DCPin, GPIO_PIN_RESET);
     
     /* Send command byte */
     HAL_SPI_Transmit(ST7701S_SPI, &cmd, 1, HAL_MAX_DELAY);
 }
 
 /**
  * @brief Send data to the display via SPI
  * @param data: Data byte to send
  */
 void ST7701S_WriteData(uint8_t data)
 {
     /* Set DC pin to data mode (high) */
     HAL_GPIO_WritePin(ST7701S_DCPort, ST7701S_DCPin, GPIO_PIN_SET);
     
     /* Send data byte */
     HAL_SPI_Transmit(ST7701S_SPI, &data, 1, HAL_MAX_DELAY);
 }
 
 /**
  * @brief Initialize LTDC for the display
  * @param hltdc: LTDC handle
  */
 void ST7701S_InitLTDC(LTDC_HandleTypeDef *hltdc)
 {
     LTDC_LayerCfgTypeDef layer_cfg;
     
     /* Store LTDC handle */
     ST7701S_LTDC = hltdc;
     
     /* LTDC Parameter Configuration */
     hltdc->Instance = LTDC;
     hltdc->Init.HSPolarity = LTDC_HSPOLARITY_AL;
     hltdc->Init.VSPolarity = LTDC_VSPOLARITY_AL;
     hltdc->Init.DEPolarity = LTDC_DEPOLARITY_AL;
     hltdc->Init.PCPolarity = LTDC_PCPOLARITY_IPC;
     
     /* Set timings */
     hltdc->Init.HorizontalSync = ST7701S_HSYNC - 1;
     hltdc->Init.VerticalSync = ST7701S_VSYNC - 1;
     hltdc->Init.AccumulatedHBP = ST7701S_HSYNC + ST7701S_HBP - 1;
     hltdc->Init.AccumulatedVBP = ST7701S_VSYNC + ST7701S_VBP - 1;
     hltdc->Init.AccumulatedActiveW = ST7701S_HSYNC + ST7701S_HBP + ST7701S_WIDTH - 1;
     hltdc->Init.AccumulatedActiveH = ST7701S_VSYNC + ST7701S_VBP + ST7701S_HEIGHT - 1;
     hltdc->Init.TotalWidth = ST7701S_HSYNC + ST7701S_HBP + ST7701S_WIDTH + ST7701S_HFP - 1;
     hltdc->Init.TotalHeigh = ST7701S_VSYNC + ST7701S_VBP + ST7701S_HEIGHT + ST7701S_VFP - 1;
     
     /* Background color */
     hltdc->Init.Backcolor.Blue = 0;
     hltdc->Init.Backcolor.Green = 0;
     hltdc->Init.Backcolor.Red = 0;
     
     /* Initialize LTDC */
     HAL_LTDC_Init(hltdc);
     
     /* Layer 0 Configuration */
     layer_cfg.WindowX0 = 0;
     layer_cfg.WindowX1 = ST7701S_WIDTH;
     layer_cfg.WindowY0 = 0;
     layer_cfg.WindowY1 = ST7701S_HEIGHT;
     
     /* Set pixel format to RGB666 (18-bit) */
     layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB666;
     
     /* Default color values */
     layer_cfg.Alpha = 255;
     layer_cfg.Alpha0 = 0;
     layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
     layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
     
     /* Fill with default color */
     layer_cfg.FBStartAdress = 0;  // To be set by the application
     
     /* Frame buffer pitch in bytes */
     layer_cfg.ImageWidth = ST7701S_WIDTH;
     layer_cfg.ImageHeight = ST7701S_HEIGHT;
     
     /* Configure Layer 0 */
     HAL_LTDC_ConfigLayer(hltdc, &layer_cfg, 0);
 }
 
 /**
  * @brief Set window for drawing
  * @param x0: Starting X coordinate
  * @param y0: Starting Y coordinate
  * @param x1: Ending X coordinate
  * @param y1: Ending Y coordinate
  */
 void ST7701S_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
 {
     /* Set column address */
     ST7701S_WriteCommand(ST7701S_CMD_CASET);
     ST7701S_WriteData((x0 >> 8) & 0xFF);
     ST7701S_WriteData(x0 & 0xFF);
     ST7701S_WriteData((x1 >> 8) & 0xFF);
     ST7701S_WriteData(x1 & 0xFF);
     
     /* Set row address */
     ST7701S_WriteCommand(ST7701S_CMD_RASET);
     ST7701S_WriteData((y0 >> 8) & 0xFF);
     ST7701S_WriteData(y0 & 0xFF);
     ST7701S_WriteData((y1 >> 8) & 0xFF);
     ST7701S_WriteData(y1 & 0xFF);
     
     /* Memory write */
     ST7701S_WriteCommand(ST7701S_CMD_RAMWR);
 }
 
 /**
  * @brief Fill rectangle with color (using LTDC)
  * @param x: X coordinate
  * @param y: Y coordinate
  * @param width: Width of rectangle
  * @param height: Height of rectangle
  * @param color: Color value
  */
 void ST7701S_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
 {
     uint32_t *frame_buffer = (uint32_t*)ST7701S_LTDC->LayerCfg[0].FBStartAdress;
     uint32_t index;
     
     for(uint16_t i = y; i < (y + height); i++)
     {
         for(uint16_t j = x; j < (x + width); j++)
         {
             index = i * ST7701S_WIDTH + j;
             frame_buffer[index] = color;
         }
     }
 }
 
 /**
  * @brief Turn display on
  */
 void ST7701S_DisplayOn(void)
 {
     ST7701S_WriteCommand(ST7701S_CMD_DISPON);
 }
 
 /**
  * @brief Turn display off
  */
 void ST7701S_DisplayOff(void)
 {
     ST7701S_WriteCommand(ST7701S_CMD_DISPOFF);
 }