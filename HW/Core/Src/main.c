/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stdint.h"
#include "stdbool.h"

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
RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// 1. Enable AHB clock for GPIOA (Button) and GPIOE (LEDs)

//---------------------------------------------------------------------------------------
// TASK 1: POLLING
//---------------------------------------------------------------------------------------
bool switch_state()
  {
    if ((GPIOA->IDR & (1U << 0)) != 0)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

//led on function
  void LED_ON()
  {
    GPIOE->ODR |= (1U << 9);
  }

// led off function
  void LED_OFF()
  {
    GPIOE->ODR &= ~(1U << 9);
  }
  //---------------------------------------------------------------------------------------


  //---------------------------------------------------------------------------------------
  //TASK 02: INTERRUPT
  //---------------------------------------------------------------------------------------

  // Mode & timing flags
static volatile uint8_t g_flash_mode = 0;   // 0 = RGB sequence, 1 = Flash mode
static volatile uint8_t g_timer_expired = 0;
  
void EXTI0_IRQHandler(void)
{
  // Check if EXTI line 0 pending bit is set
  if (EXTI->PR & (1U << 0))
  {
    // Clear pending flag by writing 1
    EXTI->PR = (1U << 0);

    // Toggle PE9 state
    if (GPIOE->ODR & (1U << 9))
    {
      LED_OFF(); // Reusing your Task 1 function
    }
    else
    {
      LED_ON();  // Reusing your Task 1 function
    }

    NVIC_ClearPendingIRQ(EXTI0_IRQn);
  }
}


  //---------------------------------------------------------------------------------------

  //------------------------------------------------------------------------------
  //TASK 3: FSM
  //------------------------------------------------------------------------------

//     void EXTI0_IRQHandler(void)
// {
//   // Check if EXTI line 0 pending bit is set
//   if (EXTI->PR & (1U << 0))
//   {
//     // Clear the pending flag by writing '1' to it (critical, otherwise it loops forever)
//     EXTI->PR = (1U << 0);

//     // Toggle mode on button press
//     g_flash_mode ^= 1;

//     // Turn off Green and Blue, and use exact LED_OFF() for Red
//     LED_OFF();
//     GPIOE->ODR &= ~((1U << 8) | (1U << 11));

//     g_timer_expired = 1;                 // Force instant transition
//     NVIC_ClearPendingIRQ(EXTI0_IRQn);
//   }
// }

  // States for RGB Sequence
//   typedef enum 
//   {
//       ST_RED = 0,
//       ST_GREEN,
//       ST_BLUE
//   } RGB_State_t;

//   // States for All-Flash Sequence
//   typedef enum {
//       ST_ALL_ON = 0,
//       ST_ALL_OFF
//   } Flash_State_t;

// static volatile RGB_State_t g_rgb_state = ST_RED;
// static volatile Flash_State_t g_flash_state = ST_ALL_ON;

// ---------------------------------------------------------------------------
// HARDWARE TIMER (TIM2) ISR: Generates non-blocking ticks every 1 second
// ---------------------------------------------------------------------------

// TIM2 ISR: fires every 1 second
// TIM2 ISR: Generates the periodic 1-second timeout
// void TIM2_IRQHandler(void)
// {
//     if (TIM2->SR & TIM_SR_UIF)
//     {
//         TIM2->SR &= ~TIM_SR_UIF;         // Clear UIF flag
//         g_timer_expired = 1;             // Signal scheduler
//         NVIC_ClearPendingIRQ(TIM2_IRQn);
//     }
// }

// ---------------------------------------------------------------------------
// SWITCH (EXTI0) ISR: Pressing SW1 resets the FSM immediately back to State 1
// ---------------------------------------------------------------------------
// Switch ISR: Pressing SW1 resets sequence back to Red (RGB start)
// Switch ISR: Updates g_flash_LED according to button press/release


// ---------------------------------------------------------------------------
// Timer2 Init (Using APB1 bus clock = 8 MHz default HSI)
// ---------------------------------------------------------------------------
// 1 Hz Hardware Timer Init (TIM2)
// void Timer2_Init(void)
// {
//     RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
//     TIM2->PSC = 8000 - 1;               // 8 MHz / 8000 = 1 kHz (1 ms tick)
//     TIM2->ARR = 1000 - 1;               // 1000 ms = 1 second overflow
//     TIM2->DIER |= TIM_DIER_UIE;
//     NVIC_SetPriority(TIM2_IRQn, 3);
//     NVIC_EnableIRQ(TIM2_IRQn);
//     TIM2->SR &= ~TIM_SR_UIF;
//     TIM2->CR1 |= TIM_CR1_CEN;
// }


// ---------------------------------------------------------------------------
// FSM Task Dispatcher: Non-blocking, executes quickly and exits
// ---------------------------------------------------------------------------
// Non-blocking Task for RGB Cycling (Book Listing 3.12 adapted)
// void Task_RGB_FSM(void)
// {
//     if (g_flash_mode != 0 || !g_timer_expired)
//     {
//         return;
//     }
//     g_timer_expired = 0;

//     // Clear all 3 LEDs: uses exact LED_OFF() for Red (PE9)
//     LED_OFF();
//     GPIOE->ODR &= ~((1U << 8) | (1U << 11));

//     switch (g_rgb_state)
//     {
//         case ST_RED:
//             LED_ON();                    // Reused exact function from Task 1
//             g_rgb_state = ST_GREEN;
//             break;

//         case ST_GREEN:
//             GPIOE->ODR |= (1U << 11);    // Green ON (PE11)
//             g_rgb_state = ST_BLUE;
//             break;

//         case ST_BLUE:
//             GPIOE->ODR |= (1U << 8);     // Blue ON (PE8)
//             g_rgb_state = ST_RED;
//             break;

//         default:
//             g_rgb_state = ST_RED;
//             break;
//     }
// }

// Non-blocking All-Flash FSM: Reuses exact LED_ON() and LED_OFF()
// void Task_Flash_FSM(void)
// {
//     if (g_flash_mode != 1 || !g_timer_expired)
//     {
//         return;
//     }
//     g_timer_expired = 0;

//     switch (g_flash_state)
//     {
//         case ST_ALL_ON:
//             LED_ON();                    // Reused exact function for Red
//             GPIOE->ODR |= (1U << 8) | (1U << 11); // Turn ON Blue and Green
//             g_flash_state = ST_ALL_OFF;
//             break;

//         case ST_ALL_OFF:
//             LED_OFF();                   // Reused exact function for Red
//             GPIOE->ODR &= ~((1U << 8) | (1U << 11)); // Turn OFF Blue and Green
//             g_flash_state = ST_ALL_ON;
//             break;

//         default:
//             g_flash_state = ST_ALL_ON;
//             break;
//     }
//}

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
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */


  //------------------------------------------------------------------------------
  // TASK 1: POLLING
  //------------------------------------------------------------------------------
  // RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN);

  // // 2. Configure PE9 (LED) as General Purpose Output (MODER bits [19:18] = 01)
  // GPIOE->MODER &= ~(3U << (9 * 2)); // Clear bits 19:18
  // GPIOE->MODER |=  (1U << (9 * 2)); // Set bit 18

  // // 3. Optional: Configure PA0 explicitly as Input (MODER bits [1:0] = 00)
  // GPIOA->MODER &= ~(3U << (0 * 2));
  //------------------------------------------------------------------------------


  //------------------------------------------------------------------------------
  // TASK 2: INTERRUPT
  //------------------------------------------------------------------------------

  //Enable AHB clocks for GPIOA, GPIOE and APB2 clock for SYSCFG
  RCC->AHBENR  |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN);
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  // 2. Configure PE9 (LED) as Output (MODER bits [19:18] = 01)
  GPIOE->MODER &= ~(3U << (9 * 2));
  GPIOE->MODER |=  (1U << (9 * 2));

  // 3. Configure PA0 as Input (MODER bits [1:0] = 00)
  GPIOA->MODER &= ~(3U << (0 * 2));

  // 4. Route PA0 to EXTI0 in SYSCFG (0000 = PA[x] pin)
  SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;

  // 5. Configure EXTI Line 0
  EXTI->IMR  |= (1U << 0);   // Unmask Interrupt Line 0
  EXTI->RTSR |= (1U << 0);   // Rising trigger enabled (active HIGH on press)
  EXTI->FTSR &= ~(1U << 0);  // Falling trigger disabled

  // 6. Configure NVIC for EXTI0
  NVIC_SetPriority(EXTI0_IRQn, 2);
  NVIC_EnableIRQ(EXTI0_IRQn);
  //------------------------------------------------------------------------------

  //------------------------------------------------------------------------------
  // TASK 3: FSM  
  //------------------------------------------------------------------------------

 // 1. Enable AHB Clocks (GPIOA, GPIOE) and APB2 (SYSCFG)
  // RCC->AHBENR  |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN);
  // RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  // // 2. Configure PE8 (Blue), PE9 (Red), and PE11 (Green) as Outputs
  // GPIOE->MODER &= ~((3U << 16) | (3U << 18) | (3U << 22));
  // GPIOE->MODER |=  ((1U << 16) | (1U << 18) | (1U << 22));

  // // 3. Configure PA0 as Input and route to EXTI0
  // GPIOA->MODER &= ~(3U << 0);
  // SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;

  // // 4. Configure EXTI0 on Rising Edge (Press)
  // EXTI->IMR  |= (1U << 0);
  // EXTI->RTSR |= (1U << 0);
  // EXTI->FTSR &= ~(1U << 0);
  // NVIC_SetPriority(EXTI0_IRQn, 2);
  // NVIC_EnableIRQ(EXTI0_IRQn);

  // // 5. Start Hardware Timer 2
  // Timer2_Init();
  //------------------------------------------------------------------------------

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    //-----------------------------------------------------------------------------
    // TASK 1: POLLING
    //------------------------------------------------------------------------------
    // bool switch_pressed = switch_state();
    // if (switch_pressed == true) 
    // {
    //   LED_ON();
    // }
    // else
    // {
    //   LED_OFF();
    // }
    // //------------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    // TASK 2: INTERRUPT
    __WFI(); // Put CPU to sleep until next interrupt arrives
    //------------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    // TASK 3: FSM
    //------------------------------------------------------------------------------
    // Task_RGB_FSM();   // Runs RGB sequence when switch is not pressed
    // Task_Flash_FSM(); // Flashes all 3 LEDs together when switch is pressed
    //-----------------------------------------------------------------------------
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/* USER CODE BEGIN 4 */

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
