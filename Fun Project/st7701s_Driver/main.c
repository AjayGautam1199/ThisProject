/**
 * @file main.c
 * @brief Example of using the ST7701S display driver with STM32F750
 */

 #include "main.h"
 #include "st7701s_driver.h"
 
 /* Private variables */
 SPI_HandleTypeDef hspi5;
 LTDC_HandleTypeDef hltdc;
 DMA2D_HandleTypeDef hdma2d;
 
 /* Frame buffer in external SDRAM */
 #define FRAME_BUFFER_ADDR       ((uint32_t)0xC0000000)
 #define FRAME_BUFFER_SIZE       (ST7701S_WIDTH * ST7701S_HEIGHT * 3)  // 3 bytes per pixel for RGB666
 
 /* Function prototypes */
 static void SystemClock_Config(void);
 static void GPIO_Init(void);
 static void SPI5_Init(void);
 static void LTDC_Init(void);
 static void DMA2D_Init(void);
 static void Display_Init(void);
 
 /**
  * @brief Main function
  */
 int main(void)
 {
     /* MCU Configuration */
     HAL_Init();
     SystemClock_Config();
     
     /* Initialize peripherals */
     GPIO_Init();
     SPI5_Init();
     LTDC_Init();
     DMA2D_Init();
     
     /* Initialize the display */
     Display_Init();
     
     /* Main loop */
     while (1)
     {
         /* Fill screen with red color */
         ST7701S_FillRect(0, 0, ST7701S_WIDTH, ST7701S_HEIGHT, ST7701S_COLOR_RED);
         HAL_Delay(1000);
         
         /* Fill screen with green color */
         ST7701S_FillRect(0, 0, ST7701S_WIDTH, ST7701S_HEIGHT, ST7701S_COLOR_GREEN);
         HAL_Delay(1000);
         
         /* Fill screen with blue color */
         ST7701S_FillRect(0, 0, ST7701S_WIDTH, ST7701S_HEIGHT, ST7701S_COLOR_BLUE);
         HAL_Delay(1000);
     }
 }
 
 /**
  * @brief Display initialization
  */
 static void Display_Init(void)
 {
     /* Configure and initialize the display */
     ST7701S_Init(&hspi5, GPIOD, GPIO_PIN_3, GPIOD, GPIO_PIN_13);  // Using PD3 for RESET, PD13 for DC
     
     /* Initialize frame buffer */
     memset((void*)FRAME_BUFFER_ADDR, 0, FRAME_BUFFER_SIZE);
     
     /* Configure LTDC for the display */
     ST7701S_InitLTDC(&hltdc);
     
     /* Set frame buffer address */
     hltdc.LayerCfg[0].FBStartAdress = FRAME_BUFFER_ADDR;
     
     /* Turn display on */
     ST7701S_DisplayOn();
 }
 
 /**
  * @brief System Clock Configuration
  */
 static void SystemClock_Config(void)
 {
     RCC_OscInitTypeDef RCC_OscInitStruct = {0};
     RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
     RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
     
     /* Configure the main internal regulator output voltage */
     __HAL_RCC_PWR_CLK_ENABLE();
     __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
     
     /* Initialize the CPU, AHB and APB buses clocks */
     RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
     RCC_OscInitStruct.HSEState = RCC_HSE_ON;
     RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
     RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
     RCC_OscInitStruct.PLL.PLLM = 25;
     RCC_OscInitStruct.PLL.PLLN = 400;
     RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
     RCC_OscInitStruct.PLL.PLLQ = 9;
     HAL_RCC_OscConfig(&RCC_OscInitStruct);
     
     /* Configure the Systick interrupt time */
     HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
     
     /* Configure the Systick */
     HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
     
     /* SysTick_IRQn interrupt configuration */
     HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
     
     /* Initialize the CPU, AHB and APB busses clocks */
     RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                 |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
     RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
     RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
     RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
     RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
     HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
     
     /* Configure LTDC clock */
     PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
     PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
     PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
     PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
     HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
 }
 
 /**
  * @brief GPIO Initialization
  */
 static void GPIO_Init(void)
 {
     GPIO_InitTypeDef GPIO_InitStruct = {0};
     
     /* GPIO Ports Clock Enable */
     __HAL_RCC_GPIOA_CLK_ENABLE();
     __HAL_RCC_GPIOB_CLK_ENABLE();
     __HAL_RCC_GPIOC_CLK_ENABLE();
     __HAL_RCC_GPIOD_CLK_ENABLE();
     __HAL_RCC_GPIOE_CLK_ENABLE();
     __HAL_RCC_GPIOF_CLK_ENABLE();
     __HAL_RCC_GPIOG_CLK_ENABLE();
     __HAL_RCC_GPIOH_CLK_ENABLE();
     __HAL_RCC_GPIOI_CLK_ENABLE();
     __HAL_RCC_GPIOK_CLK_ENABLE();
     
     /* Configure SPI5 GPIO pins */
     GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
     GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
     HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
     
     /* Configure Reset and DC pins for display */
     GPIO_InitStruct.Pin = GPIO_PIN_3;  // Reset pin
     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
     GPIO_InitStruct.Pull = GPIO_PULLUP;
     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
     HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
     
     GPIO_InitStruct.Pin = GPIO_PIN_13;  // Data/Command pin
     HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
     
     /* Configure LTDC pins */
     /* R0-R7 */
     GPIO_InitStruct.Pin = GPIO_PIN_13;
     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
     GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
     HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
     
     GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15
                         |GPIO_PIN_8|GPIO_PIN_9;
     HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
     
     /* G0-G7 */
     GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_11
                         |GPIO_PIN_12;
     HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
     
     GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_6;
     HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);
     
     GPIO_InitStruct.Pin = GPIO_PIN_6;
     HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
     
     /* B0-B7 */
     GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3
                         |GPIO_PIN_7|GPIO_PIN_2;
     HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
     
     /* LTDC control signals */
     GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
     HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);
 }
 
 /**
  * @brief SPI5 Initialization
  */
 static void SPI5_Init(void)
 {
     hspi5.Instance = SPI5;
     hspi5.Init.Mode = SPI_MODE_MASTER;
     hspi5.Init.Direction = SPI_DIRECTION_2LINES;
     hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
     hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
     hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
     hspi5.Init.NSS = SPI_NSS_SOFT;
     hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
     hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
     hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
     hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
     hspi5.Init.CRCPolynomial = 7;
     hspi5.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
     hspi5.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
     HAL_SPI_Init(&hspi5);
 }
 
 /**
  * @brief LTDC Initialization
  */
 static void LTDC_Init(void)
 {
     /* LTDC initialization is handled by ST7701S_InitLTDC() */
     hltdc.Instance = LTDC;
 }
 
 /**
  * @brief DMA2D Initialization
  */
 static void DMA2D_Init(void)
 {
     hdma2d.Instance = DMA2D;
     hdma2d.Init.Mode = DMA2D_M2M;
     hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB666;
     hdma2d.Init.OutputOffset = 0;
     hdma2d.LayerCfg[1].InputOffset = 0;
     hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB666;
     hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
     hdma2d.LayerCfg[1].InputAlpha = 0xFF;
     HAL_DMA2D_Init(&hdma2d);
     HAL_DMA2D_ConfigLayer(&hdma2d, 1);
 }
 
 /**
  * @brief Drawing function using DMA2D
  * @param x: X coordinate
  * @param y: Y coordinate
  * @param width: Width of the area
  * @param height: Height of the area
  * @param color: Color to fill
  */
 void DMA2D_FillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
 {
     /* Calculate address */
     uint32_t destination = (uint32_t)(FRAME_BUFFER_ADDR + (y * ST7701S_WIDTH + x) * 3);
     
     /* Configure DMA2D */
     hdma2d.Init.Mode = DMA2D_R2M;
     hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB666;
     hdma2d.Init.OutputOffset = ST7701S_WIDTH - width;
     HAL_DMA2D_Init(&hdma2d);
     
     /* Start DMA2D transfer */
     HAL_DMA2D_Start(&hdma2d, color, destination, width, height);
     
     /* Wait for transfer to complete */
     HAL_DMA2D_PollForTransfer(&hdma2d, 100);
 }
 
 /**
  * @brief This function handles DMA2D global interrupt.
  */
 void DMA2D_IRQHandler(void)
 {
     HAL_DMA2D_IRQHandler(&hdma2d);
 }