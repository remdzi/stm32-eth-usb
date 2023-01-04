#include <stdio.h>

#include "stm32f2xx_hal.h"
#include "gpio.h"
#include "eth.h"

/* Global Ethernet handle */
ETH_HandleTypeDef heth;

/* Ethernet Rx MA Descriptor */
__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;
/* Ethernet Tx DMA Descriptor */
__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;
 /* Ethernet Receive Buffer */
__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END;
 /* Ethernet Transmit Buffer */
__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END;

void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspInit 0 */

  /* USER CODE END ETH_MspInit 0 */
    /* Enable Peripheral clock */
    __HAL_RCC_ETH_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitStruct.Pin = RMII_MDC_Pin|RMII_RXD0_Pin|RMII_RXD1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RMII_REF_CLK_Pin|RMII_MDIO_Pin|RMII_CRS_DV_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RMII_TXD1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(RMII_TXD1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RMII_TX_EN_Pin|RMII_TXD0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(ETH_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ETH_IRQn);
  }
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{
  if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspDeInit 0 */

  /* USER CODE END ETH_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ETH_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOC, RMII_MDC_Pin|RMII_RXD0_Pin|RMII_RXD1_Pin);

    HAL_GPIO_DeInit(GPIOA, RMII_REF_CLK_Pin|RMII_MDIO_Pin|RMII_CRS_DV_Pin);

    HAL_GPIO_DeInit(RMII_TXD1_GPIO_Port, RMII_TXD1_Pin);

    HAL_GPIO_DeInit(GPIOG, RMII_TX_EN_Pin|RMII_TXD0_Pin);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(ETH_IRQn);
  }
}

void eth_init(void)
{
  HAL_StatusTypeDef hal_eth_init_status;

/* Init ETH */

   uint8_t MACAddr[6] ;
  heth.Instance = ETH;
  heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_DISABLE;
  heth.Init.Speed = ETH_SPEED_100M;
  heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
  heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x02;
  MACAddr[2] = 0x02;
  MACAddr[3] = 0x03;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.RxMode = ETH_RXPOLLING_MODE;
  heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  hal_eth_init_status = HAL_ETH_Init(&heth);

  if (hal_eth_init_status != HAL_OK)
  {
    printf("ERROR: HAL_ETH_Init has failed\r\n");
  }
  /* Initialize Tx Descriptors list: Chain Mode */
  HAL_ETH_DMATxDescListInit(&heth, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);

  /* Initialize Rx Descriptors list: Chain Mode  */
  HAL_ETH_DMARxDescListInit(&heth, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

  /* Enable MAC and DMA transmission and reception */
  HAL_ETH_Start(&heth);
}

uint8_t * eth_get_tx_buf(void)
{
  __IO ETH_DMADescTypeDef *DmaTxDesc = heth.TxDesc;
  uint8_t *buffer = (uint8_t *)(heth.TxDesc->Buffer1Addr);

  if((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
  {  
     return NULL;
  }
  else
  {
     return buffer;
  }
}

void eth_send(uint32_t length)
{
  HAL_ETH_TransmitFrame(&heth, length);

  /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
  if ((heth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET)
  {
    /* Clear TUS ETHERNET DMA flag */
    heth.Instance->DMASR = ETH_DMASR_TUS;

    /* Resume DMA transmission*/
    heth.Instance->DMATPDR = 0;
  }
}

uint8_t * eth_receive(uint32_t * length)
{
  if (HAL_ETH_GetReceivedFrame(&heth) != HAL_OK)
     return NULL;

  if(heth.RxFrameInfos.SegCount != 1)
     printf("ERROR: heth.RxFrameInfos.SegCount = %ld\r\n", heth.RxFrameInfos.SegCount);
  *length = heth.RxFrameInfos.length;
  return (uint8_t *)heth.RxFrameInfos.buffer;
}

void eth_release_rx_buf(void)
{
  __IO ETH_DMADescTypeDef *dmarxdesc;
  int i;

  /* Release descriptors to DMA */
  /* Point to first descriptor */
  dmarxdesc = heth.RxFrameInfos.FSRxDesc;
  /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
  for (i=0; i< heth.RxFrameInfos.SegCount; i++)
  {
     dmarxdesc->Status |= ETH_DMARXDESC_OWN;
     dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
  }

  /* Clear Segment_Count */
  heth.RxFrameInfos.SegCount =0;

  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
  if ((heth.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)
  {
     /* Clear RBUS ETHERNET DMA flag */
     heth.Instance->DMASR = ETH_DMASR_RBUS;
     /* Resume DMA reception */
     heth.Instance->DMARPDR = 0;
  }
}

bool eth_get_link_status(void)
{
  uint32_t regvalue = 0;
  /* Read PHY_BSR*/
  HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &regvalue);
  if ((regvalue & PHY_LINKED_STATUS) != 0)
     return true;
  else
     return false; 
}

//void ETH_IRQHandler(void)
//{
  /* USER CODE BEGIN ETH_IRQn 0 */

  /* USER CODE END ETH_IRQn 0 */
  //HAL_ETH_IRQHandler(&heth);
  /* USER CODE BEGIN ETH_IRQn 1 */

  /* USER CODE END ETH_IRQn 1 */
//}

void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
  //printf("HAL_ETH_RxCpltCallback\r\n");
}

void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
{
  //printf("HAL_ETH_TxCpltCallback\r\n");
}

