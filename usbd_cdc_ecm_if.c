/**
  ******************************************************************************
  * @file    Src/usbd_cdc_ecm_if_template.c
  * @author  MCD Application Team
  * @brief   Source file for USBD CDC_ECM interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "usbd_cdc_ecm_if.h"
#include "eth.h"
/*

  Include here  LwIP files if used

*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Received Data over USB are stored in this buffer */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif /* ( __ICCARM__ ) */
//__ALIGN_BEGIN static uint8_t UserRxBuffer[CDC_ECM_ETH_MAX_SEGSZE + 100]__ALIGN_END;

/* Transmitted Data over CDC_ECM (CDC_ECM interface) are stored in this buffer */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif /* ( __ICCARM__ ) */
//__ALIGN_BEGIN  static uint8_t UserTxBuffer[CDC_ECM_ETH_MAX_SEGSZE + 100]__ALIGN_END;

static uint8_t CDC_ECMInitialized = 0U;

/* USB handler declaration */
extern USBD_HandleTypeDef  USBD_Device;

/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_ECM_Itf_Init(void);
static int8_t CDC_ECM_Itf_DeInit(void);
static int8_t CDC_ECM_Itf_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_ECM_Itf_Receive(uint8_t *pbuf, uint32_t *Len);
static int8_t CDC_ECM_Itf_TransmitCplt(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);
static int8_t CDC_ECM_Itf_Process(USBD_HandleTypeDef *pdev);

USBD_CDC_ECM_ItfTypeDef USBD_CDC_ECM_fops =
{
  CDC_ECM_Itf_Init,
  CDC_ECM_Itf_DeInit,
  CDC_ECM_Itf_Control,
  CDC_ECM_Itf_Receive,
  CDC_ECM_Itf_TransmitCplt,
  CDC_ECM_Itf_Process,
  (uint8_t *)CDC_ECM_MAC_STR_DESC,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  CDC_ECM_Itf_Init
  *         Initializes the CDC_ECM media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_Init(void)
{
  uint8_t * eth_tx_buf;

  if (CDC_ECMInitialized == 0U)
  {
    CDC_ECMInitialized = 1U;
  }

  /* Set Application Buffers */
  eth_tx_buf = eth_get_tx_buf();
  if (eth_tx_buf != NULL)
  {
     (void)USBD_CDC_ECM_SetRxBuffer(&USBD_Device, eth_tx_buf);
  }
  else
  {
     printf("ERROR: CDC_ECM_Itf_Init no more ETH TX buffer available !!!\r\n");
  }

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_DeInit
  *         DeInitializes the CDC_ECM media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_DeInit(void)
{
#ifdef USE_USBD_COMPOSITE
  USBD_CDC_ECM_HandleTypeDef *hcdc_cdc_ecm = (USBD_CDC_ECM_HandleTypeDef *) \
                                             (USBD_Device.pClassDataCmsit[USBD_Device.classId]);
#else
  USBD_CDC_ECM_HandleTypeDef *hcdc_cdc_ecm = (USBD_CDC_ECM_HandleTypeDef *)(USBD_Device.pClassData);
#endif /* USE_USBD_COMPOSITE */

  /* Notify application layer that link is down */
  hcdc_cdc_ecm->LinkStatus = 0U;

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_Control
  *         Manage the CDC_ECM class requests
  * @param  Cmd: Command code
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length)
{
#ifdef USE_USBD_COMPOSITE
  USBD_CDC_ECM_HandleTypeDef *hcdc_cdc_ecm = (USBD_CDC_ECM_HandleTypeDef *) \
                                             (USBD_Device.pClassDataCmsit[USBD_Device.classId]);
#else
  USBD_CDC_ECM_HandleTypeDef *hcdc_cdc_ecm = (USBD_CDC_ECM_HandleTypeDef *)(USBD_Device.pClassData);
#endif /* USE_USBD_COMPOSITE */

  switch (cmd)
  {
    case CDC_ECM_SEND_ENCAPSULATED_COMMAND:
      /* Add your code here */
      break;

    case CDC_ECM_GET_ENCAPSULATED_RESPONSE:
      /* Add your code here */
      break;

    case CDC_ECM_SET_ETH_MULTICAST_FILTERS:
      /* Add your code here */
      break;

    case CDC_ECM_SET_ETH_PWRM_PATTERN_FILTER:
      /* Add your code here */
      break;

    case CDC_ECM_GET_ETH_PWRM_PATTERN_FILTER:
      /* Add your code here */
      break;

    case CDC_ECM_SET_ETH_PACKET_FILTER:
      /* Check if this is the first time we enter */
      if (hcdc_cdc_ecm->LinkStatus == 0U)
      {
        /*
          Setup the Link up at TCP/IP level
        */
        hcdc_cdc_ecm->LinkStatus = 1U;

        /* Modification for MacOS which doesn't send SetInterface before receiving INs */
        if (hcdc_cdc_ecm->NotificationStatus == 0U)
        {
          /* Send notification: NETWORK_CONNECTION Event */
          (void)USBD_CDC_ECM_SendNotification(&USBD_Device, NETWORK_CONNECTION,
                                              CDC_ECM_NET_CONNECTED, NULL);

          /* Prepare for sending Connection Speed Change notification */
          hcdc_cdc_ecm->NotificationStatus = 1U;
        }
      }
      /* Add your code here */
      break;

    case CDC_ECM_GET_ETH_STATISTIC:
      /* Add your code here */
      break;

    default:
      break;
  }
  UNUSED(length);
  UNUSED(pbuf);

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_Receive
  *         Data received over USB OUT endpoint are sent over CDC_ECM interface
  *         through this function.
  * @param  Buf: Buffer of data to be transmitted
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_Receive(uint8_t *Buf, uint32_t *Len)
{
  /* Get the CDC_ECM handler pointer */
#ifdef USE_USBD_COMPOSITE
  USBD_CDC_ECM_HandleTypeDef *hcdc_cdc_ecm = (USBD_CDC_ECM_HandleTypeDef *) \
                                             (USBD_Device.pClassDataCmsit[USBD_Device.classId]);
#else
  USBD_CDC_ECM_HandleTypeDef *hcdc_cdc_ecm = (USBD_CDC_ECM_HandleTypeDef *)(USBD_Device.pClassData);
#endif /* USE_USBD_COMPOSITE */

  /* Call Eth buffer processing */
  hcdc_cdc_ecm->RxState = 1U;
  //printf("CDC_ECM_Itf_Receive %ld bytes\r\n", *Len);

  UNUSED(Len);
  UNUSED(Buf);

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_TransmitCplt
  *         Data transmitted callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_TransmitCplt(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);

  /* USB has completed the transmission of the ethernet frame received:
  --> Release ethernet buffer */
  eth_release_rx_buf();

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_Process
  *         Data received over USB OUT endpoint are sent over CDC_ECM interface
  *         through this function.
  * @param  pdef: pointer to the USB Device Handle
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_Process(USBD_HandleTypeDef *pdev)
{
  uint8_t * eth_tx_buf;
  uint8_t * eth_rx_buf;
  uint32_t eth_rx_length;

  /* Get the CDC_ECM handler pointer */
#ifdef USE_USBD_COMPOSITE
  USBD_CDC_ECM_HandleTypeDef *hcdc_cdc_ecm = (USBD_CDC_ECM_HandleTypeDef *)(pdev->pClassDataCmsit[pdev->classId]);
#else
  USBD_CDC_ECM_HandleTypeDef *hcdc_cdc_ecm = (USBD_CDC_ECM_HandleTypeDef *)(pdev->pClassData);
#endif /* USE_USBD_COMPOSITE */

  if (hcdc_cdc_ecm == NULL)
  {
    return (-1);
  }

  if (hcdc_cdc_ecm->LinkStatus != 0U)
  {
    if (hcdc_cdc_ecm->RxState == 1U)
    {
       /* USB has received a buffer
       --> Send it to ethernet */
       eth_send(hcdc_cdc_ecm->RxLength);
       hcdc_cdc_ecm->RxState = 2U;
    }
    else if (hcdc_cdc_ecm->RxState == 2U)
    {
       /* Get a new ethernet tx buffer and configure USB to receive on it */
       eth_tx_buf = eth_get_tx_buf();
       if (eth_tx_buf != NULL)
       {
          hcdc_cdc_ecm->RxLength = 0;
          hcdc_cdc_ecm->RxState = 0U;
          (void)USBD_CDC_ECM_SetRxBuffer(&USBD_Device, eth_tx_buf);
          USBD_CDC_ECM_ReceivePacket(&USBD_Device);
       }
       else
       {
          printf("ERROR: CDC_ECM_Itf_Receive no more ETH TX buffer available !!!\r\n");
       }
    }

    if (hcdc_cdc_ecm->TxState == 0U)
    {
       /* Get an ethernet rx buffer and configure USB to send it */
       eth_rx_buf = eth_receive(&eth_rx_length);
       if (eth_rx_buf != NULL)
       {
         //printf("ETH RX %ld\r\n", eth_rx_length);
         (void)USBD_CDC_ECM_SetTxBuffer(&USBD_Device, eth_rx_buf, eth_rx_length);
         USBD_CDC_ECM_TransmitPacket(&USBD_Device);
       }
    }
  }

  return (0);
}

