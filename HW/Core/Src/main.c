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
// *For integers
#include "stdint.h"

// *For boolean
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

  //!---------------------------------------------------------------------------------------
  // !TASK 1: POLLING

  bool switch_state()
  {
    // *check if PA0 is HIGH (pressed) or LOW (not pressed)
    if ((GPIOA->IDR & (1U)) != 0) // 1U = 0000 0001
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  // *led on 
  void LED_ON()
  {
    // *| = OR
    GPIOE->ODR = GPIOE->ODR | (1U << 9); // (1U << 9) = 0010 0000 0000
  }

  // *led off
  void LED_OFF()
  {
    GPIOE->ODR = GPIOE->ODR & ~(1U << 9);
  }


  //!---------------------------------------------------------------------------------------
  //!TASK 02: INTERRUPT

  // void EXTI0_IRQHandler(void)
  // {
  //   // *Check if EXTI line 0 pending bit is set
  //   if ((EXTI->PR & (1U)) == 1)
  //   {
  //     // *Clear pending flag by writing 1
  //     EXTI->PR = (1U);

  //     // *Toggle PE9 state
  //     if ((GPIOE->ODR & (1U << 9)))
  //     {
  //       LED_OFF();
  //     }
  //     else
  //     {
  //       LED_ON();
  //     }

  //     NVIC_ClearPendingIRQ(EXTI0_IRQn);
  //   }
  //}
  

  //!---------------------------------------------------------------------------------------
  //!TASK 3: FSM

  // *0 = RGB sequence, 1 = Flash mode
  // *flash mode initally 0 so rgb mode runs
  // static volatile uint8_t global_flash_mode = 0; 

  // //* timer to change state like from R to G to B even
  // static volatile uint8_t global_timer_expired = 0;

  // void EXTI0_IRQHandler(void)
  // {
  //   // *check if EXTI line 0 create an interrupt
  //   if (EXTI->PR & (1U))
  //   {
  //     // *clear the pending flag by writing 1 to it otherwise it loops forever
  //     //* & CPU thinks interrupt is still active
  //     EXTI->PR = (1U); //* EXTI->PR = 0000

  //     // *enable mode on button press to change state to flash LED or go to RGB state as switch pressed
  //     //* ^ = xor, TT: 0^0=1, 0^1=1, 1^1=0
  //     global_flash_mode = global_flash_mode ^ 1;

  //     // *turning off red LED
  //     LED_OFF();

  //     //* clearing led blue pe8 n green pe11 to 0 so blue,,green OFF
  //     GPIOE->ODR &= ~((1U << 8) | (1U << 11));

  //     // *change state / add delay of 1s
  //     global_timer_expired = 1;   
      
  //     //* clears pending interrupt in NVIC
  //     NVIC_ClearPendingIRQ(EXTI0_IRQn);
  //   }
  // }

  // // *states for RGB sequence
  // typedef enum 
  // {
  //   STATE_RED = 0,
  //   STATE_GREEN = 1,
  //   STATE_BLUE = 2
  // } 
  // RGB_State;

  // // *states for flash sequence
  // typedef enum 
  // {

  //   STATE_ALL_ON = 0,
  //   STATE_ALL_OFF = 1
  // } 
  // Flash_State;

  // //* start state when in rgb mode = red LED
  // static volatile RGB_State global_rgb_state = STATE_RED;

  // //* start state when in flash mode = all LED ON
  // static volatile Flash_State global_flash_state = STATE_ALL_ON;

  // //* runs autommatically every second
  // void TIM2_IRQHandler(void)
  // {
  //   //* check timer2 status if it expired or no n if UIF bit (update interrupt flag) = 1
  //   //* if UIF=1 that means 1s has passed we need to change state
  //   if ((TIM2->SR & TIM_SR_UIF) == 1)
  //   {
  //     // *Clear UIF flag
  //     TIM2->SR = TIM2->SR & ~TIM_SR_UIF;

  //     // *one second has passed so change the timer case     
  //     global_timer_expired = 1;             

  //     //* clean slate
  //     NVIC_ClearPendingIRQ(TIM2_IRQn);
  //   }
  // }

  // // *1sec hardware timer init (TIM2)
  // void Timer2_Init(void)
  // {
  //   //* enable timer clock
  //   RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  //   // *clock = 8 MHz / 8000 = 1000 timer ticks per sec => 1 tick = 1 ms
  //   TIM2->PSC = 8000 - 1;               

  //   // *count 1000 ms = 1 second 
  //   TIM2->ARR = 1000 - 1;             

  //   //* enable timer interrupt
  //   TIM2->DIER |= TIM_DIER_UIE;

  //   //* set timer priority
  //   NVIC_SetPriority(TIM2_IRQn, 2);

  //   //* allow timer interrupt
  //   NVIC_EnableIRQ(TIM2_IRQn);

  //   //* clear old flags
  //   TIM2->SR &= ~TIM_SR_UIF;

  //   //* start timer again
  //   TIM2->CR1 |= TIM_CR1_CEN;
  // }

  // *RGB mode
  // void Task_RGB_FSM(void)
  // {
  //   if ((global_flash_mode != 0) || (global_timer_expired == 0))
  //   {
  //     return;
  //   }

  //   global_timer_expired = 0;

  //   // *Clear all 3 LED
  //   LED_OFF(); //for red
  //   GPIOE->ODR &= ~((1U << 8) | (1U << 11)); //for green n blue

  //   switch (global_rgb_state)
  //   {
  //     case STATE_RED:
  //     {
  //       // *Red ON (PE9)
  //       LED_ON();                
  //       global_rgb_state = STATE_GREEN; //next state
  //       break;
  //     }

  //     case STATE_GREEN:
  //     {
  //       // *Green ON (PE11)
  //       GPIOE->ODR = GPIOE->ODR | (1U << 11); //LED ON
  //       global_rgb_state = STATE_BLUE; // next state
  //       break;
  //     }

  //     case STATE_BLUE:
  //     {
  //       // *Blue ON (PE8)
  //       GPIOE->ODR = GPIOE->ODR | (1U << 8); //LED ON   
  //       global_rgb_state = STATE_RED; // Next state
  //       break;
  //     }

  //     default:
  //     {
  //       global_rgb_state = STATE_RED;
  //       break;
  //     }
  //   }
  // }

  // // *flash mode
  // void Task_Flash_FSM(void)
  // {
  //   //* if rgb running n timer = 0, do nothing
  //   if ((global_flash_mode != 1) || (global_timer_expired != 1))
  //   {
  //     return;
  //   }

  //   //* else change timer state
  //   global_timer_expired = 0;

  //   switch (global_flash_state)
  //   {
  //     case STATE_ALL_ON:
  //     {
  //       //* turn on all LEDS
  //       LED_ON();                
  //       GPIOE->ODR |= (1U << 8) | (1U << 11); 
  //       //* next state
  //       global_flash_state = STATE_ALL_OFF;
  //       break;
  //     }

  //     case STATE_ALL_OFF:
  //     {
  //       //* turn off all LEDS
  //       LED_OFF();     
  //       GPIOE->ODR &= ~((1U << 8) | (1U << 11)); 
  //       //* next state
  //       global_flash_state = STATE_ALL_ON;
  //       break;
  //     }

  //     default:
  //     {
  //       global_flash_state = STATE_ALL_ON;
  //       break;
  //     }
  //   }
  // }

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

  // !------------------------------------------------------------------------------
  // !TASK 1: POLLING
  
  // *enabling gpioA and gpioE clock
  RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN);

  // *setting the mode of PE9 to output (01)
  GPIOE->MODER = GPIOE->MODER & ~(3U << (9 * 2)); //* ~(3U << (9 * 2)) = 0011 1111 1111 1111 1111
  GPIOE->MODER = GPIOE->MODER | (1U << (9 * 2)); // * (1U << (9 * 2)) = 0100 0000 0000 0000 0000

  // **setting the mode of PA0 to input (00)
  GPIOA->MODER = GPIOA->MODER & ~(3U); // * ~(3U) = 1111 1111 1111 1111 1100


  // !------------------------------------------------------------------------------
  // !TASK 2: INTERRUPT
  
  // // *enabling gpioA and gpioE clock
  // RCC->AHBENR = RCC->AHBENR | (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN);

  // // //* enabling peripheral clock for interrupt
  // RCC->APB2ENR = RCC->APB2ENR | RCC_APB2ENR_SYSCFGEN;

  // // *setting the mode of PE9 to output (01)
  // GPIOE->MODER = GPIOE->MODER & ~(3U << (9 * 2));
  // GPIOE->MODER = GPIOE->MODER | (1U << (9 * 2));

  // // **setting the mode of PA0 to input (00)
  // GPIOA->MODER = GPIOA->MODER & ~(3U);

  // //* select port A for EXTI line 0: EXTI0 field = 0000 → PA0
  // SYSCFG->EXTICR[0] = SYSCFG->EXTICR[0] & ~SYSCFG_EXTICR1_EXTI0;

  // //* tells the interrupt line which that interrupt line 0 is allowed to create interrupt
  // EXTI->IMR = EXTI->IMR | (1U);

  // //* rising trigger selection register = 1 
  // //* low to high will trigger interrupt
  // EXTI->RTSR = EXTI->RTSR | (1U);

  // // *falling trigger selection register = 0
  // // //* high to low will not trigger interrupt
  // EXTI->FTSR = EXTI->FTSR & ~(1U);

  // //*configure NVIC for EXTI0 to set interrupt priority
  // NVIC_SetPriority(EXTI0_IRQn, 0);

  // // *enable EXTI0 interrupt in the NVIC so the cpu can respond to the interrupt
  // NVIC_EnableIRQ(EXTI0_IRQn);
 
  // !------------------------------------------------------------------------------
  // !TASK 3: FSM  

  // // *enable gpio A n gpio E clocks
  // RCC->AHBENR  |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN);

  // // *enable peripheral clock
  // RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  // // *configure PE8 (blue), PE9 (red) & PE11 (green) as outputs (01)
  // GPIOE->MODER = GPIOE->MODER & ~((3U << (8*2)) | (3U << (9*2)) | (3U << (11*2)));
  // GPIOE->MODER = GPIOE->MODER | ((1U << (8*2)) | (1U << (9*2)) | (1U << (11*2)));

  // // *configure PA0 as input (00)
  // GPIOA->MODER &= ~(3U);

  // // *route port A to EXTI0
  // SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;

  // // *like a door for interrupt to unmask EXTI0 and create an interrupt
  // EXTI->IMR = EXTI->IMR | (1U);

  // // *rising edge trigger register = 1
  // EXTI->RTSR = EXTI->RTSR | (1U);

  // //* falling edge trigger register = 0
  // EXTI->FTSR = EXTI->FTSR & ~(1U);

  // //* setting priority for interrupt
  // NVIC_SetPriority(EXTI0_IRQn, 0);

  // //* enabling irq for cpcu's interrupt controller to check if interrupt is allowed
  // NVIC_EnableIRQ(EXTI0_IRQn);

  // // *Start Hardware Timer 2
  // Timer2_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // !-----------------------------------------------------------------------------
    // !TASK 1: POLLING

    bool switch_pressed = switch_state();
    if (switch_pressed == true) 
    {
      LED_ON();
    }
    else
    {
      LED_OFF();
    }

    // !-----------------------------------------------------------------------------
    // !TASK 2: INTERRUPT


    // !-----------------------------------------------------------------------------                                                                                                                             hiiiiiii------------------------------------
    // !TASK 3: FSM
    // *Runs RGB sequence when switch is not pressed
  //   Task_RGB_FSM();   

  //   // *Flashes all 3 LEDs together when switch is pressed
  //   Task_Flash_FSM(); 
  // }
  /* USER CODE END 3 */
  }
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
