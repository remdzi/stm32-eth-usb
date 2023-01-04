#include <stdio.h>

#include "stm32f2xx_hal.h"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_conf.h"
#include "usbd_cdc_ecm.h"
#include "usbd_cdc_ecm_if.h"

USBD_HandleTypeDef USBD_Device;
extern PCD_HandleTypeDef hpcd;

#include "eth.h"
#include "uart.h"

void SystemClock_Config(void);
void Error_Handler(void);

int main(void)
{
   uint32_t u32PreviousTick = 0;

   HAL_Init();

   /* Configure the system clock to 120 MHz */
   SystemClock_Config();

   uart3_init();
   printf("Hello world!\r\n");

   eth_init();

   /* Init Device Library, add supported class and start the library. */
   if (USBD_Init(&USBD_Device, &Class_Desc, 0) != USBD_OK)
   {
     Error_Handler();
   }
   if (USBD_RegisterClass(&USBD_Device, &USBD_CDC_ECM) != USBD_OK)
   {
     Error_Handler();
   }
   if (USBD_CDC_ECM_RegisterInterface(&USBD_Device, &USBD_CDC_ECM_fops) != USBD_OK)
   {
     Error_Handler();
   }
   if (USBD_Start(&USBD_Device) != USBD_OK)
   {
     Error_Handler();
   }

   while(1)
   {
      if (HAL_GetTick() - u32PreviousTick > 1000)
      {
         u32PreviousTick += 1000;
         printf("One second loop!\r\n");
      }
      
      USBD_CDC_ECM_fops.Process(&USBD_Device);
   }
}

void SystemClock_Config(void)
{
   RCC_ClkInitTypeDef RCC_ClkInitStruct;
   RCC_OscInitTypeDef RCC_OscInitStruct;

   /* Enable HSE Oscillator and activate PLL with HSE as source */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 8;
   RCC_OscInitStruct.PLL.PLLN = 240;
   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
   RCC_OscInitStruct.PLL.PLLQ = 5;
   HAL_RCC_OscConfig(&RCC_OscInitStruct);

   /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
      clocks dividers */
   RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
   HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
}

void Error_Handler(void)
{
  while (1)
  {
  }
}

void SysTick_Handler(void)
{
   HAL_IncTick();
}

void OTG_FS_IRQHandler(void)
{
   HAL_PCD_IRQHandler(&hpcd);
}
