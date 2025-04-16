/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fmac.c
  * @brief   This file provides code for the configuration
  *          of the FMAC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "fmac.h"

/* USER CODE BEGIN 0 */
#include "ADC_.h"
uint16_t I_IN_Fmac;
/* USER CODE END 0 */

FMAC_HandleTypeDef hfmac;
DMA_HandleTypeDef hdma_fmac_read;

/* FMAC init function */
void MX_FMAC_Init(void)
{

  /* USER CODE BEGIN FMAC_Init 0 */

  /* USER CODE END FMAC_Init 0 */

  /* USER CODE BEGIN FMAC_Init 1 */

  /* USER CODE END FMAC_Init 1 */
  hfmac.Instance = FMAC;
  if (HAL_FMAC_Init(&hfmac) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FMAC_Init 2 */

  /* USER CODE END FMAC_Init 2 */

}

void HAL_FMAC_MspInit(FMAC_HandleTypeDef* fmacHandle)
{

  if(fmacHandle->Instance==FMAC)
  {
  /* USER CODE BEGIN FMAC_MspInit 0 */

  /* USER CODE END FMAC_MspInit 0 */
    /* FMAC clock enable */
    __HAL_RCC_FMAC_CLK_ENABLE();

    /* FMAC DMA Init */
    /* FMAC_READ Init */
    hdma_fmac_read.Instance = DMA2_Channel1;
    hdma_fmac_read.Init.Request = DMA_REQUEST_FMAC_READ;
    hdma_fmac_read.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_fmac_read.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_fmac_read.Init.MemInc = DMA_MINC_DISABLE;
    hdma_fmac_read.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_fmac_read.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_fmac_read.Init.Mode = DMA_CIRCULAR;
    hdma_fmac_read.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    if (HAL_DMA_Init(&hdma_fmac_read) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(fmacHandle,hdmaOut,hdma_fmac_read);

  /* USER CODE BEGIN FMAC_MspInit 1 */

  /* USER CODE END FMAC_MspInit 1 */
  }
}

void HAL_FMAC_MspDeInit(FMAC_HandleTypeDef* fmacHandle)
{

  if(fmacHandle->Instance==FMAC)
  {
  /* USER CODE BEGIN FMAC_MspDeInit 0 */

  /* USER CODE END FMAC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FMAC_CLK_DISABLE();

    /* FMAC DMA DeInit */
    HAL_DMA_DeInit(fmacHandle->hdmaOut);
  /* USER CODE BEGIN FMAC_MspDeInit 1 */

  /* USER CODE END FMAC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
static int16_t iirA[2] = { 31754, 0 };
static int16_t iirB[3] = { 507, 507, 0 };

void HAL_FMAC_FilterConfig_my(void)
{
    /* declare a filter configuration structure */
    FMAC_FilterConfigTypeDef sFmacConfig;
    /* Set the coefficient buffer base address */
    sFmacConfig.CoeffBaseAddress = 0;
    /* Set the coefficient buffer size to the number of coeffs */
    sFmacConfig.CoeffBufferSize = 5;
    /* Set the Input buffer base address to the next free address */
    sFmacConfig.InputBaseAddress = 5;
    /* Set the input buffer size greater than the number of coeffs */
    sFmacConfig.InputBufferSize = 7;
    /* Set the input watermark to zero since we are using DMA */
    sFmacConfig.InputThreshold = FMAC_THRESHOLD_1;
    /* Set the Output buffer base address to the next free address */
    sFmacConfig.OutputBaseAddress = 12;
    /* Set the output buffer size */
    sFmacConfig.OutputBufferSize = 6;
    /* Set the output watermark to zero since we are using DMA */
    sFmacConfig.OutputThreshold = FMAC_THRESHOLD_1;
    /* No A coefficients since FIR */
    sFmacConfig.pCoeffA = iirA;
    /* Number of A coefficients */
    sFmacConfig.CoeffASize = 2;
    /* Pointer to the coefficients in memory */
    sFmacConfig.pCoeffB = iirB;
    /* Number of coefficients */
    sFmacConfig.CoeffBSize = 3;
    /* Select FIR filter function */
    sFmacConfig.Filter = FMAC_FUNC_IIR_DIRECT_FORM_1;
    /* Enable DMA input transfer */
    sFmacConfig.InputAccess = FMAC_BUFFER_ACCESS_NONE;
    /* Enable DMA output transfer */
    sFmacConfig.OutputAccess = FMAC_BUFFER_ACCESS_DMA;
    /* Enable clipping of the output at 0x7FFF and 0x8000 */
    sFmacConfig.Clip = FMAC_CLIP_ENABLED;
    /* P parameter contains number of B coefficients */
    sFmacConfig.P = 3;
    /* Q parameter contains number of A coefficients */
    sFmacConfig.Q = 2;
    /* R parameter contains the post-shift value */
    sFmacConfig.R = 0;
    /* Configure the FMAC */
    HAL_FMAC_FilterConfig(&hfmac, &sFmacConfig);
    uint16_t ExpectedCalculatedOutputSize = 1;
    HAL_FMAC_FilterStart(&hfmac, (int16_t*) &I_IN_Fmac, &ExpectedCalculatedOutputSize);

}
/* USER CODE END 1 */
