/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for ECUTask */
osThreadId_t ECUTaskHandle;
const osThreadAttr_t ECUTask_attributes = {
  .name = "ECUTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TesterTask */
osThreadId_t TesterTaskHandle;
const osThreadAttr_t TesterTask_attributes = {
  .name = "TesterTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ECUCanTPStatusFlag */
osEventFlagsId_t ECUCanTPStatusFlagHandle;
const osEventFlagsAttr_t ECUCanTPStatusFlag_attributes = {
  .name = "ECUCanTPStatusFlag"
};
/* Definitions for TesterCanTPStatusFlag */
osEventFlagsId_t TesterCanTPStatusFlagHandle;
const osEventFlagsAttr_t TesterCanTPStatusFlag_attributes = {
  .name = "TesterCanTPStatusFlag"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void ECUTask_Handler(void *argument);
void TesterTask_Handler(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	SyncPrintf_Init();
	ECUCanTPStatusFlagHandle = osEventFlagsNew(&ECUCanTPStatusFlag_attributes);
	TesterCanTPStatusFlagHandle = osEventFlagsNew(&TesterCanTPStatusFlag_attributes);
	TesterTaskHandle = osThreadNew(TesterTask_Handler, NULL, &TesterTask_attributes);

	ECUTaskHandle = osThreadNew(ECUTask_Handler, NULL, &ECUTask_attributes);


  /* USER CODE END Init */
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/**
  * @}
  */

/**
  * @}
  */
}

/* USER CODE BEGIN Header_ECUTask_Handler */
/**
  * @brief  Function implementing the ECUTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_ECUTask_Handler */
__weak void ECUTask_Handler(void *argument)
{
  /* USER CODE BEGIN ECUTask_Handler */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ECUTask_Handler */
}

/* USER CODE BEGIN Header_TesterTask_Handler */
/**
* @brief Function implementing the TesterTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TesterTask_Handler */
__weak void TesterTask_Handler(void *argument)
{
  /* USER CODE BEGIN TesterTask_Handler */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END TesterTask_Handler */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

