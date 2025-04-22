/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define HRTIMB_Period 27200
#define HRTIMA_Period 27200
#define HRTIMMaster_Period 27200
#define CCMRAM __attribute__((section("ccmram")))
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define ccmram_enable (1)
#if ccmram_enable==1
#define CCMRAM __attribute__((section("ccmram")))
#endif
#if ccmram_enable==0
#define CCMRAM /**/
#endif
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RGB_B_Pin GPIO_PIN_0
#define RGB_B_GPIO_Port GPIOA
#define RGB_R_Pin GPIO_PIN_1
#define RGB_R_GPIO_Port GPIOA
#define RGB_G_Pin GPIO_PIN_2
#define RGB_G_GPIO_Port GPIOA
#define POWER_CTRL_Pin GPIO_PIN_15
#define POWER_CTRL_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
#define HRTIMAB_Period	27200
#define MAX_BUCK_DUTY   (uint16_t)(0.82 * HRTIMAB_Period)    //buck最大占空比82%
#define MIN_BUCK_DUTY	(uint16_t)(0.05 * HRTIMAB_Period)    //buck最小占空比5%
#define	MAX_BUCK_DUTY1  (uint16_t)(0.75 * HRTIMAB_Period)    //MIX state buck最大占空比80%
#define MAX_BOOST_DUTY  (uint16_t)(0.50 * HRTIMAB_Period)    //boost最大占空比40%
#define MIN_BOOST_DUTY  (uint16_t)(0.05 * HRTIMAB_Period)    //boost最小占空比5%
#define MIN_REG_VALUE	(uint16_t)100			//HRTIM最小值限额
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
