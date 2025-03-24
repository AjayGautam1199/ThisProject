/**
 * @file st7701s_graphics.c
 * @brief Additional graphics functions for the ST7701S display
 */

 #include "st7701s_driver.h"

 extern LTDC_HandleTypeDef hltdc;
 extern DMA2D_HandleTypeDef hdma2d;
 
 /* External frame buffer */
 extern uint32_t FRAME_BUFFER_ADDR;
 
 /**
  * @brief Draw a single pixel
  * @param x: X coordinate
  * @param y: Y coordinate
  * @param color: Color in RGB666 format
  */
 void ST7701S_DrawPixel(uint16_t x, uint16_t y, uint32_t color)
 {
     /* Check if coordinates are valid */
     if ((x >= ST7701S_WIDTH) || (y >= ST7701S_HEIGHT)) {
         return;
     }
     
     /* Calculate address and write pixel */
     uint32_t *frame_buffer = (uint32_t *)FRAME_BUFFER_ADDR;
     frame_buffer[y * ST7701S_WIDTH + x] = color;
 }
 
 /**
  * @brief Draw a horizontal line
  * @param x: Starting X coordinate
  * @param y: Y coordinate
  * @param width: Line width
  * @param color: Line color
  */
 void ST7701S_DrawHLine(uint16_t x, uint16_t y, uint16_t width, uint32_t color)
 {
     /* Check if coordinates are valid */
     if ((x >= ST7701S_WIDTH) || (y >= ST7701S_HEIGHT)) {
         return;
     }
     
     /* Clip width to display limits */
     if (x + width > ST7701S_WIDTH) {
         width = ST7701S_WIDTH - x;
     }
     
     /* Fill using DMA2D for efficiency */
     DMA2D_FillRectangle(x, y, width, 1, color);
 }
 
 /**
  * @brief Draw a vertical line
  * @param x: X coordinate
  * @param y: Starting Y coordinate
  * @param height: Line height
  * @param color: Line color
  */
 void ST7701S_DrawVLine(uint16_t x, uint16_t y, uint16_t height, uint32_t color)
 {
     /* Check if coordinates are valid */
     if ((x >= ST7701S_WIDTH) || (y >= ST7701S_HEIGHT)) {
         return;
     }
     
     /* Clip height to display limits */
     if (y + height > ST7701S_HEIGHT) {
         height = ST7701S_HEIGHT - y;
     }
     
     /* Fill using DMA2D for efficiency */
     DMA2D_FillRectangle(x, y, 1, height, color);
 }
 
 /**
  * @brief Draw a rectangle outline
  * @param x: X coordinate
  * @param y: Y coordinate
  * @param width: Rectangle width
  * @param height: Rectangle height
  * @param color: Rectangle color
  */
 void ST7701S_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
 {
     /* Draw the four sides of the rectangle */
     ST7701S_DrawHLine(x, y, width, color);                    /* Top */
     ST7701S_DrawHLine(x, y + height - 1, width, color);       /* Bottom */
     ST7701S_DrawVLine(x, y, height, color);                   /* Left */
     ST7701S_DrawVLine(x + width - 1, y, height, color);       /* Right */
 }
 
 /**
  * @brief Draw a circle
  * @param x0: Center X coordinate
  * @param y0: Center Y coordinate
  * @param r: Radius
  * @param color: Circle color
  */
 void ST7701S_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint32_t color)
 {
     int16_t f = 1 - r;
     int16_t ddF_x = 1;
     int16_t ddF_y = -2 * r;
     int16_t x = 0;
     int16_t y = r;
     
     ST7701S_DrawPixel(x0, y0 + r, color);
     ST7701S_DrawPixel(x0, y0 - r, color);
     ST7701S_DrawPixel(x0 + r, y0, color);
     ST7701S_DrawPixel(x0 - r, y0, color);
     
     while (x < y) {
         if (f >= 0) {
             y--;
             ddF_y += 2;
             f += ddF_y;
         }
         x++;
         ddF_x += 2;
         f += ddF_x;
         
         ST7701S_DrawPixel(x0 + x, y0 + y, color);
         ST7701S_DrawPixel(x0 - x, y0 + y, color);
         ST7701S_DrawPixel(x0 + x, y0 - y, color);
         ST7701S_DrawPixel(x0 - x, y0 - y, color);
         ST7701S_DrawPixel(x0 + y, y0 + x, color);
         ST7701S_DrawPixel(x0 - y, y0 + x, color);
         ST7701S_DrawPixel(x0 + y, y0 - x, color);
         ST7701S_DrawPixel(x0 - y, y0 - x, color);
     }
 }
 
 /**
  * @brief Fill a circle
  * @param x0: Center X coordinate
  * @param y0: Center Y coordinate
  * @param r: Radius
  * @param color: Circle color
  */
 void ST7701S_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint32_t color)
 {
     int16_t f = 1 - r;
     int16_t ddF_x = 1;
     int16_t ddF_y = -2 * r;
     int16_t x = 0;
     int16_t y = r;
     
     ST7701S_DrawVLine(x0, y0 - r, 2 * r + 1, color);
     
     while (x < y) {
         if (f >= 0) {
             y--;
             ddF_y += 2;
             f += ddF_y;
         }
         x++;
         ddF_x += 2;
         f += ddF_x;
         
         ST7701S_DrawVLine(x0 + x, y0 - y, 2 * y + 1, color);
         ST7701S_DrawVLine(x0 - x, y0 - y, 2 * y + 1, color);
         ST7701S_DrawVLine(x0 + y, y0 - x, 2 * x + 1, color);
         ST7701S_DrawVLine(x0 - y, y0 - x, 2 * x + 1, color);
     }
 }
 
 /**
  * @brief Draw a character using a built-in font
  * @param x: X coordinate
  * @param y: Y coordinate
  * @param c: Character to draw
  * @param font: Font to use
  * @param color: Text color
  * @param bg_color: Background color
  */
 void ST7701S_DrawChar(uint16_t x, uint16_t y, char c, FontDef *font, uint32_t color, uint32_t bg_color)
 {
     uint32_t i, b, j;
     
     /* Draw pixels */
     for (i = 0; i < font->height; i++) {
         b = font->data[(c - 32) * font->height + i];
         for (j = 0; j < font->width; j++) {
             if ((b << j) & 0x8000) {
                 ST7701S_DrawPixel(x + j, y + i, color);
             } else {
                 ST7701S_DrawPixel(x + j, y + i, bg_color);
             }
         }
     }
 }
 
 /**
  * @brief Draw a string using a built-in font
  * @param x: X coordinate
  * @param y: Y coordinate
  * @param str: String to draw
  * @param font: Font to use
  * @param color: Text color
  * @param bg_color: Background color
  */
 void ST7701S_DrawString(uint16_t x, uint16_t y, const char *str, FontDef *font, uint32_t color, uint32_t bg_color)
 {
     /* Draw characters */
     while (*str) {
         /* Draw character */
         ST7701S_DrawChar(x, y, *str, font, color, bg_color);
         
         /* Increment position */
         x += font->width;
         
         /* Go to new line if needed */
         if (x + font->width > ST7701S_WIDTH) {
             x = 0;
             y += font->height;
             
             /* Break if screen is full */
             if (y + font->height > ST7701S_HEIGHT) {
                 break;
             }
         }
         
         /* Next character */
         str++;
     }
 }
 
 /**
  * @brief Display an image
  * @param x: X coordinate
  * @param y: Y coordinate
  * @param width: Image width
  * @param height: Image height
  * @param data: Pointer to image data in RGB666 format
  */
 void ST7701S_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint32_t *data)
 {
     /* Check if coordinates are valid */
     if ((x >= ST7701S_WIDTH) || (y >= ST7701S_HEIGHT) || 
         (x + width > ST7701S_WIDTH) || (y + height > ST7701S_HEIGHT)) {
         return;
     }
     
     /* Calculate address */
     uint32_t destination = (uint32_t)(FRAME_BUFFER_ADDR + (y * ST7701S_WIDTH + x) * 3);
     
     /* Configure DMA2D for memory-to-memory copy */
     hdma2d.Init.Mode = DMA2D_M2M;
     hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB666;
     hdma2d.Init.OutputOffset = ST7701S_WIDTH - width;
     HAL_DMA2D_Init(&hdma2d);
     
     /* Configure the input */
     hdma2d.LayerCfg[1].InputOffset = 0;
     hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB666;
     hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
     hdma2d.LayerCfg[1].InputAlpha = 0xFF;
     HAL_DMA2D_ConfigLayer(&hdma2d, 1);
     
     /* Start DMA2D transfer */
     HAL_DMA2D_Start(&hdma2d, (uint32_t)data, destination, width, height);
     
     /* Wait for transfer to complete */
     HAL_DMA2D_PollForTransfer(&hdma2d, 100);
 }
 
 /**
  * @brief Convert RGB values to RGB666 format
  * @param r: Red component (0-255)
  * @param g: Green component (0-255)
  * @param b: Blue component (0-255)
  * @return: Color in RGB666 format
  */
 uint32_t ST7701S_RGB(uint8_t r, uint8_t g, uint8_t b)
 {
     /* Convert 8-bit RGB to 6-bit RGB (RGB666) */
     r = (r * 63) / 255;
     g = (g * 63) / 255;
     b = (b * 63) / 255;
     
     /* Create RGB666 color */
     return ((r & 0x3F) << 12) | ((g & 0x3F) << 6) | (b & 0x3F);
 }
 
 /**
  * @brief Set screen rotation (0, 90, 180, 270 degrees)
  * @param rotation: 0=0°, 1=90°, 2=180°, 3=270°
  */
 void ST7701S_SetRotation(uint8_t rotation)
 {
     uint8_t madctl_value = 0;
     
     switch (rotation) {
         case 0: /* Portrait 1 */
             madctl_value = 0x00; /* MY=0, MX=0, MV=0 */
             break;
         case 1: /* Landscape 1 */
             madctl_value = 0x60; /* MY=0, MX=1, MV=1 */
             break;
         case 2: /* Portrait 2 */
             madctl_value = 0xC0; /* MY=1, MX=1, MV=0 */
             break;
         case 3: /* Landscape 2 */
             madctl_value = 0xA0; /* MY=1, MX=0, MV=1 */
             break;
         default:
             return; /* Invalid rotation value */
     }
     
     /* Send command to set rotation */
     ST7701S_WriteCommand(ST7701S_CMD_MADCTL);
     ST7701S_WriteData(madctl_value);
 }