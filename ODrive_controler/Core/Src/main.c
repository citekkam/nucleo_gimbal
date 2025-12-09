/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ODriveCAN.h"
#include "ODriveHAL.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// CAN bus baudrate. Musí odpovídat nastavení v CubeIDE a na ODrive!
#define CAN_BAUDRATE 250000

// ODrive node_id pro odrv0
#define ODRV0_NODE_ID 0

// Arduino konstanta, kterou STM32 nezná
#define TWO_PI (2.0f * M_PI)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char msg[64];
// Vytvoření STM32 HAL wrapperu pro ODrive knihovnu
// Předáme mu ukazatel na náš HAL CAN handle
ODriveHAL can_intf(&hcan);

// Instance ODrive objektů
ODriveCAN odrv0(can_intf, ODRV0_NODE_ID);
ODriveCAN* odrives[] = {&odrv0}; // Pole všech ODrive pro callback

// Struktura pro uchování stavu z ODrive
struct ODriveUserData {
    Heartbeat_msg_t last_heartbeat;
    volatile bool received_heartbeat = false;
    Get_Encoder_Estimates_msg_t last_feedback;
    volatile bool received_feedback = false;
};
//sasdaasd
ODriveUserData odrv0_user_data;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN_Init(void);
/* USER CODE BEGIN PFP */

/*

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
 */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */

  sprintf(msg, "Start ODriveCAN\r\n");
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);


  // Registrace callbacks pro heartbeat a feedback enkodéru
  odrv0.onFeedback(onFeedback, &odrv0_user_data);
  odrv0.onStatus(onHeartbeat, &odrv0_user_data);

	// Spuštění CAN periferie
	if (HAL_CAN_Start(&hcan) != HAL_OK) {
		sprintf(msg, "ERROR_HAL_CAN_1\r\n");
		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
		Error_Handler();
	}

	// Aktivace CAN RX přerušení
	if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
		sprintf(msg, "ERROR_HAL_CAN_2\r\n");
		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
		Error_Handler();
	}

  sprintf(msg, "Wait_ODRIVE\r\n");
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
  while (!odrv0_user_data.received_heartbeat) {
	  HAL_Delay(100); // Čekáme na první heartbeat (přijde přes přerušení)
  }

  sprintf(msg, "ODRIVE_FIND\r\n");
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

  // Požadavek na napětí a proud (timeout 1000ms)
  sprintf(msg, "POKUS_CTENI\r\n");
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
  Get_Bus_Voltage_Current_msg_t vbus;
  if (!odrv0.request(vbus, 1000)) {
	  sprintf(msg, "ERROR_VBUS\r\n");
	  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
	  while (true); // Zastavíme se
  }

  //printf("DC napeti [V]: %f\r\n", vbus.Bus_Voltage);
  //printf("DC proud [A]: %f\r\n", vbus.Bus_Current);

  //printf("Povolovani closed loop control...\r\n");
  //printf("Povolovani closed loop control...\r\n");
  //printf("Povolovani closed loop control...\r\n");
	while (odrv0_user_data.last_heartbeat.Axis_State != ODriveAxisState::AXIS_STATE_CLOSED_LOOP_CONTROL) {
		odrv0.clearErrors();
		HAL_Delay(1);
		odrv0.setState(ODriveAxisState::AXIS_STATE_CLOSED_LOOP_CONTROL);

		// Čekáme 150ms na potvrzení stavu (přijde v heartbeatu)
		// Není potřeba pumpEvents, přerušení běží na pozadí
		for (int i = 0; i < 15; ++i) {
			HAL_Delay(10);
		}
		sprintf(msg, "ODRIVE_STATE\r\n");
		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
	}

  	  sprintf(msg, "ODRIVE_RUN\r\n");
  	  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  float SINE_PERIOD = 2.0f; // Perioda sinusovky v sekundách

	  // Místo millis() použijeme HAL_GetTick()
	  float t = 0.001f * HAL_GetTick();

	  float phase = t * (TWO_PI / SINE_PERIOD);

	  odrv0.setPosition(
		  sin(phase), // pozice
		  cos(phase) * (TWO_PI / SINE_PERIOD) // feedforward rychlosti (volitelné)
	  );

	  // Tisk pozice a rychlosti pro Serial Plotter
	  if (odrv0_user_data.received_feedback) {
		  Get_Encoder_Estimates_msg_t feedback = odrv0_user_data.last_feedback;
		  odrv0_user_data.received_feedback = false; // Reset flagu

		  // Použijeme printf místo Serial.print
		  //printf("odrv0-pos:%f,odrv0-vel:%f\r\n", feedback.Pos_Estimate, feedback.Vel_Estimate);
	  }

	  HAL_Delay(10); // Malá pauza, aby smyčka neběžela příliš rychle


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN;
  hcan.Init.Prescaler = 12;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// Voláno pokaždé, když dorazí Heartbeat zpráva
void onHeartbeat(Heartbeat_msg_t& msg, void* user_data) {
    ODriveUserData* odrv_user_data = static_cast<ODriveUserData*>(user_data);
    odrv_user_data->last_heartbeat = msg;
    odrv_user_data->received_heartbeat = true;
}

// Voláno pokaždé, když dorazí feedback zpráva
void onFeedback(Get_Encoder_Estimates_msg_t& msg, void* user_data) {
    ODriveUserData* odrv_user_data = static_cast<ODriveUserData*>(user_data);
    odrv_user_data->last_feedback = msg;
    odrv_user_data->received_feedback = true;
}


/**
  * @brief Callback při přijetí zprávy do RX FIFO 0.
  * @param hcan Ukazatel na CAN handle.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];

    // Přečtení zprávy z CAN bufferu
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data) != HAL_OK) {
        Error_Handler();
    }

    // Vytvoření zprávy ve formátu, kterému rozumí ODriveHAL wrapper
    ODriveHAL::CanMsg msg;
    msg.id = rx_header.StdId;
    msg.len = rx_header.DLC;
    memcpy(msg.buffer, rx_data, msg.len);

    // Zpracování zprávy ODrive knihovnou
    // Toto nahrazuje funkci onCanMessage z Arduino kódu
    for (auto odrive: odrives) {
        onReceive(msg, *odrive);
    }

    // Znovu aktivujeme notifikaci, protože přerušení se po přijetí deaktivuje
    // Toto je standardní postup pro HAL CAN přerušení
    if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        Error_Handler();
    }
}


// --- Retargeting printf na UART ---

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
