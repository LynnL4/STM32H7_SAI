/**
  ******************************************************************************
  * File Name          : SAI.c
  * Description        : This file provides code for the configuration
  *                      of the SAI instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "sai.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SAI_HandleTypeDef hsai_BlockA4;

/* SAI4 init function */
void MX_SAI4_Init(void)
{

  /* USER CODE BEGIN SAI4_Init 0 */

  /* USER CODE END SAI4_Init 0 */

  /* USER CODE BEGIN SAI4_Init 1 */

  /* USER CODE END SAI4_Init 1 */

  hsai_BlockA4.Instance = SAI4_Block_A;
  hsai_BlockA4.Init.AudioMode = SAI_MODEMASTER_TX;
  hsai_BlockA4.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA4.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA4.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA4.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_FULL;
  hsai_BlockA4.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_44K;
  hsai_BlockA4.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockA4.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA4.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA4.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  if (HAL_SAI_InitProtocol(&hsai_BlockA4, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, 2) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN SAI4_Init 2 */

  /* USER CODE END SAI4_Init 2 */

}
static uint32_t SAI4_client =0;

void HAL_SAI_MspInit(SAI_HandleTypeDef* hsai)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
/* SAI4 */
    if(hsai->Instance==SAI4_Block_A)
    {
    /* SAI4 clock enable */
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI4A;
    PeriphClkInitStruct.PLL3.PLL3M = 2;
    PeriphClkInitStruct.PLL3.PLL3N = 48;
    PeriphClkInitStruct.PLL3.PLL3P = 25;
    PeriphClkInitStruct.PLL3.PLL3Q = 1;
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_3;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
    PeriphClkInitStruct.Sai4AClockSelection = RCC_SAI4ACLKSOURCE_PLL3;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    if (SAI4_client == 0)
    {
       __HAL_RCC_SAI4_CLK_ENABLE();
    }
    SAI4_client ++;

    /**SAI4_A_Block_A GPIO Configuration
    PE2     ------> SAI4_MCLK_A
    PE3     ------> SAI4_SD_B
    PD11     ------> SAI4_SD_A
    PD13     ------> SAI4_SCK_A
    PD12     ------> SAI4_FS_A
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF8_SAI4;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF10_SAI4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    }
}

void HAL_SAI_MspDeInit(SAI_HandleTypeDef* hsai)
{

/* SAI4 */
    if(hsai->Instance==SAI4_Block_A)
    {
    SAI4_client --;
    if (SAI4_client == 0)
      {
      /* Peripheral clock disable */
       __HAL_RCC_SAI4_CLK_DISABLE();
      }

    /**SAI4_A_Block_A GPIO Configuration
    PE2     ------> SAI4_MCLK_A
    PE3     ------> SAI4_SD_B
    PD11     ------> SAI4_SD_A
    PD13     ------> SAI4_SCK_A
    PD12     ------> SAI4_FS_A
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2|GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_12);

    }
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
