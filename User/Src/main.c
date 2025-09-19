/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by Puya under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "myTasks.h"
#include "scan_LED.h"
#include "segment_LED.h"
#include "redstone_line.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

// 扫描 led 的 com 端引脚映射表
scanLED_pin_map_stu_t myScanLED_com_map[] = {
    [SCANLED_COM1] = {
        .port = GPIOB,
        .pin = GPIO_PIN_4,
    },
    [SCANLED_COM2] = {
        .port = GPIOB,
        .pin = GPIO_PIN_3,
    },
    [SCANLED_COM3] = {
        .port = GPIOB,
        .pin = GPIO_PIN_1,
    },
};

// 扫描 led 的 one 端引脚映射表
scanLED_pin_map_stu_t myScanLED_one_map[] = {
    [SCANLED_ONE1] = {
        .port = GPIOB,
        .pin = GPIO_PIN_2,
    },
    [SCANLED_ONE2] = {
        .port = GPIOA,
        .pin = GPIO_PIN_0,
    },
    [SCANLED_ONE3] = {
        .port = GPIOB,
        .pin = GPIO_PIN_0,
    },
    [SCANLED_ONE4] = {
        .port = GPIOA,
        .pin = GPIO_PIN_6,
    },
};

// 扫描 led 缓存
uint8_t myScanLED_buffer[3];

// 扫描 led 对象结构体
struct scanLED_stu myScanLED = {
    .num_com = 3,
    .num_one = 4,

    .now_com = 0,

    .buffer = myScanLED_buffer,

    .scan_com_map = myScanLED_com_map,
    .scan_one_map = myScanLED_one_map,

    .write_pin = myScanLED_write_pin,
};

// 数码管对象结构体，将数码管的写引脚映射到写扫描 led 的缓存上
struct SegmentLED_stu mySegLED = {
    .segment_pin_map = mySegLED_pin_map,

    .write_seg_pin = mySegLED_write_seg_pin,
};


// 四个方向的红石通道对象的列表
struct rs_channel_stu rs_line_chs[] = {
    [RS_LINE_CH1_up] = {
        .status = RS_CHANNEL_STATE_release,
        
        .pin = {
            .port = GPIOA,
            .pin = GPIO_PIN_3,
        },
    },

    [RS_LINE_CH2_down] = {
        .status = RS_CHANNEL_STATE_release,
        
        .pin = {
            .port = GPIOA,
            .pin = GPIO_PIN_5,
        },
    },
    
    [RS_LINE_CH3_left] = {
        .status = RS_CHANNEL_STATE_release,
        
        .pin = {
            .port = GPIOA,
            .pin = GPIO_PIN_4,
        },
    },
    
    [RS_LINE_CH4_right] = {
        .status = RS_CHANNEL_STATE_release,
        
        .pin = {
            .port = GPIOA,
            .pin = GPIO_PIN_7,
        },
    },
};

// 红石链接方向指示灯，将写引脚操作映射到写扫描 led 的缓存上
rs_line_led_t rs_line_leds[] = {
    [RS_LINE_CH1_up] = {
        .port = SCANLED_COM3,
        .pin = SCANLED_ONE1,
    },

    [RS_LINE_CH2_down] = {
        .port = SCANLED_COM3,
        .pin = SCANLED_ONE4,
    },

    [RS_LINE_CH3_left] = {
        .port = SCANLED_COM3,
        .pin = SCANLED_ONE2,
    },

    [RS_LINE_CH4_right] = {
        .port = SCANLED_COM3,
        .pin = SCANLED_ONE3,
    },
};

// 红石线对象结构体
struct rs_line_stu my_rs_line = {

    .leds = rs_line_leds,

    .led_pin_write = my_rs_line_led_pin_write,

    .io = {
        .status = RS_IO_STATE_idle,
        .tick = RS_TICK_40_wire_release,

        .channels = rs_line_chs,
        .channel_num = RS_LINE_CH_NUM,
        .bitmask_channel_change = 0,

        .pin_read = my_rs_io_pin_read,
        .pin_write = my_rs_io_pin_write,

        .callback_channel_change = my_rs_io_callback_channel_change,
        .callback_communication_init = my_rs_io_callback_communication_init,
        .callback_communication_over = my_rs_io_callback_communication_over,
    },
};

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
void my_GPIO_init(void);

/**
  * @brief  Main program.
  * @retval int
  */
int main(void)
{
    /* Reset of all peripherals, Initializes the Systick. */
    HAL_Init();
    
    /* System clock configuration */
    APP_SystemClockConfig();

    SEGGER_RTT_Init();
    LOG_STR("\n\nSYSTEM START\n\n");
    
    HAL_Delay(1);
    my_GPIO_init();


    myTask_init(); // 创建任务
    
    SEG_LED_show_num(&mySegLED, SEG_LED_NUM_0);

    rs_line_init(&my_rs_line);

    LOG_FMT("\nRedstone wire init over in %dms.\n", rtx_Sys_get_tick());

    rtx_Sys_schedule_start(); // 开启调度
    
    rtx_Sys_scheduler(); // 运行调度器

    /* infinite loop */
    while (1)
    {
    }
}

/**
  * @brief  System clock configuration function
  * @param  None
  * @retval None
  */
static void APP_SystemClockConfig(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Oscillator configuration */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE; /* Select oscillator HSE, HSI, LSI, LSE */
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;                           /* Enable HSI */
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                           /* HSI 1 frequency division */
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_24MHz;  /* Configure HSI clock 24MHz */
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DISABLE;               /* Close HSE bypass */
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;                          /* Close LSI */
  /*RCC_OscInitStruct.LSICalibrationValue = RCC_LSICALIBRATION_32768Hz;*/
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;                          /* Close LSE */
  /*RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_MEDIUM;*/
  /* Configure oscillator */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    APP_ErrorHandler();
  }

  /* Clock source configuration */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1; /* Choose to configure clock HCLK, SYSCLK, PCLK1 */
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSISYS; /* Select HSISYS as the system clock */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;        /* AHB clock 1 division */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;         /* APB clock 1 division */
  /* Configure clock source */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    APP_ErrorHandler();
  }
}

/**
  * @brief  Error executing function.
  * @param  None
  * @retval None
  */
void APP_ErrorHandler(void)
{
  while (1)
  {
  }
}

void my_GPIO_init(void){
    GPIO_InitTypeDef          GPIO_InitStruct={0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 红石通信 io
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
    HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);

    // led 扫描显示
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* Users can add their own printing information as needed,
     for example: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
