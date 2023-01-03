#include "stm32f2xx_hal.h"

UART_HandleTypeDef Uart3Handle;

extern void Error_Handler(void);

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};
   if(huart->Instance==USART3)
   {
      /* Enable clocks */
      __HAL_RCC_USART3_CLK_ENABLE();
      __HAL_RCC_GPIOD_CLK_ENABLE();
  
      /* UART TX GPIO pin configuration  */
      GPIO_InitStruct.Pin       = GPIO_PIN_8;
      GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull      = GPIO_PULLUP;
      GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
      HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

      /* UART RX GPIO pin configuration  */
      GPIO_InitStruct.Pin = GPIO_PIN_9;
      GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
      HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
   }
}

void uart3_init(void)
{
   Uart3Handle.Instance        = USART3;
   Uart3Handle.Init.BaudRate   = 115200;
   Uart3Handle.Init.WordLength = UART_WORDLENGTH_8B;
   Uart3Handle.Init.StopBits   = UART_STOPBITS_1;
   Uart3Handle.Init.Parity     = UART_PARITY_NONE;
   Uart3Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
   Uart3Handle.Init.Mode       = UART_MODE_TX_RX;
   if (HAL_UART_Init(&Uart3Handle) != HAL_OK)
   {
      Error_Handler();
   }
}

/* Printf() ouputs to uart3 */
int _write(int file, char *ptr, int len)
{
   HAL_UART_Transmit(&Uart3Handle, (uint8_t *)ptr, len, 0xFFFF);
   return len;
}


