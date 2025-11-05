/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "keyled.h"
#include "queue.h"
#include <stdio.h>
#include "usart.h"
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
/* Definitions for Task_Count */
osThreadId_t Task_CountHandle;
const osThreadAttr_t Task_Count_attributes = {
  .name = "Task_Count",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for Task_ScanKeys */
osThreadId_t Task_ScanKeysHandle;
const osThreadAttr_t Task_ScanKeys_attributes = {
  .name = "Task_ScanKeys",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Queue_Keys */
osMessageQueueId_t Queue_KeysHandle;
const osMessageQueueAttr_t Queue_Keys_attributes = {
  .name = "Queue_Keys"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_Count(void *argument);
void AppTask_ScanKeys(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of Queue_Keys */
  Queue_KeysHandle = osMessageQueueNew (10, sizeof(uint8_t), &Queue_Keys_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_Count */
  Task_CountHandle = osThreadNew(AppTask_Count, NULL, &Task_Count_attributes);

  /* creation of Task_ScanKeys */
  Task_ScanKeysHandle = osThreadNew(AppTask_ScanKeys, NULL, &Task_ScanKeys_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_Count */
/**
  * @brief  Function implementing the Task_Count thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_Count */
void AppTask_Count(void *argument)
{
  /* USER CODE BEGIN AppTask_Count */
	// Read queue information
	const char* qName=pcQueueGetName(Queue_KeysHandle);
	printf("Queue Name = %s.\r\n",qName);

	UBaseType_t qSpaces=uxQueueSpacesAvailable(Queue_KeysHandle);
	printf("Queue Size = %ld.\r\n\r\n",qSpaces);

	UBaseType_t msgCount=0, freeSpace=0;
	uint8_t count_keyleft_pressed=0;
	uint8_t count_keyright_pressed=0;
	uint8_t count_keyup_pressed=0;
	uint8_t count_keydown_pressed=0;
	KEYS  keyCode;
	/* Infinite loop */
	for(;;)
	{
		msgCount=uxQueueMessagesWaiting(Queue_KeysHandle);  	//messages Count
		printf("Number of messages in the queue = %ld.\r\n",msgCount);
		freeSpace=uxQueueSpacesAvailable(Queue_KeysHandle); 		//freeSpace
		printf("free space in the queue = %ld.\r\n",freeSpace);
		// read
		BaseType_t  result=xQueueReceive(Queue_KeysHandle, &keyCode, pdMS_TO_TICKS(50));
		if (result != pdTRUE)
			continue;

		// read messages and process.
		if (keyCode==KEY_LEFT){
			count_keyleft_pressed++;
			printf("count_keyleft_pressed = %d.\r\n",count_keyleft_pressed);}
		else if (keyCode==KEY_RIGHT){
			count_keyright_pressed++;
			printf("count_keyright_pressed = %d.\r\n",count_keyright_pressed);}
		else if (keyCode==KEY_UP){
			count_keyup_pressed++;
			printf("count_keyup_pressed = %d.\r\n",count_keyup_pressed);}
		else if (keyCode==KEY_DOWN){
			count_keydown_pressed++;
			printf("count_keydown_pressed = %d.\r\n",count_keydown_pressed);}

		vTaskDelay(pdMS_TO_TICKS(500));
	}
  /* USER CODE END AppTask_Count */
}

/* USER CODE BEGIN Header_AppTask_ScanKeys */
/**
* @brief Function implementing the Task_ScanKeys thread.
* @Scan the key and write to the queue.
* @After writen, vTaskDelay(300) is called,
* @to be used to de-jitter, and at the same time turn the task into blocking state.
* @Low priority tasks into the running state, read and process messages in the queue timely.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_ScanKeys */
void AppTask_ScanKeys(void *argument)
{
  /* USER CODE BEGIN AppTask_ScanKeys */
	GPIO_PinState keyState=GPIO_PIN_SET;
	KEYS  key=KEY_NONE;
	/* Infinite loop */
	for(;;)
	{
		key=KEY_NONE;
		//S5=KeyRight
		keyState=HAL_GPIO_ReadPin(KeyRight_GPIO_Port, KeyRight_Pin);
		if (keyState==GPIO_PIN_RESET)
			key=KEY_RIGHT;

		//S3=KeyDown
		keyState=HAL_GPIO_ReadPin(KeyDown_GPIO_Port, KeyDown_Pin);
		if (keyState==GPIO_PIN_RESET)
			key=KEY_DOWN;

		//S4=KeyLeft
		keyState=HAL_GPIO_ReadPin(KeyLeft_GPIO_Port, KeyLeft_Pin);
		if (keyState==GPIO_PIN_RESET)
			key=KEY_LEFT;

		//S2=KeyUp
		keyState=HAL_GPIO_ReadPin(KeyUp_GPIO_Port, KeyUp_Pin);
		if (keyState==GPIO_PIN_RESET)
			key=KEY_UP;

		//key pressed.
		if (key != KEY_NONE)
		{
			BaseType_t err= xQueueSendToBack(Queue_KeysHandle, &key, pdMS_TO_TICKS(50));
			if (err == errQUEUE_FULL)  				//Reset once the queue is full.
				xQueueReset(Queue_KeysHandle);
			vTaskDelay(pdMS_TO_TICKS(300)); 	//It will take at least 300 to de-jitter and perform task scheduling.
		}
		else
			vTaskDelay(pdMS_TO_TICKS(5)); 		//vTaskDelay() in for(;;) loop , turned into blocking state.
	}
  /* USER CODE END AppTask_ScanKeys */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart3,(uint8_t*)&ch,1,0xFFFF);
	return ch;
}
/* USER CODE END Application */

