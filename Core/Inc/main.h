/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct rs485
{
uint8_t number;
uint8_t data;
}rs485;

typedef struct rsBool
{
uint8_t number;
_Bool data;
}rsBool;

typedef struct led_mon
{
uint16_t Current_Blinck;
_Bool alarm;
_Bool colRed; 
_Bool colGreen;
_Bool colYel; 
}led_mon;
typedef struct _Led_Blinck
{
uint16_t timeRed;
uint16_t timeGrn;
uint16_t timeYel;
}_Led_Blinck;

#ifndef _CRC
#define _CRC                   								0xD011FDB6// 2020_11_15_//0x74424007 //2020_11_12___//0xCC736395
#endif

typedef struct _chlMode
{
uint16_t timeButton;
uint16_t timeDelay;
}_chlMode;


typedef struct 
	{
	GPIO_TypeDef* port;
	uint16_t pin;
} GPIO;

#define Queue_item_size sizeof(AMessage)
    
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#ifndef _version
#define _version 						24020100 // версия ПРО
#endif

#ifndef _mode10P
#define _mode10P 						0xA
#endif

#ifndef _mode20P
#define _mode20P 						0x14
#endif

#ifndef _mode30P
#define _mode30P 						0x1E
#endif

#ifndef _LoopUpMax
#define _LoopUpMax 							65500  //65535 //6000		//
#endif

#ifndef _LoopUpMin
#define _LoopUpMin 								65400   //65535 5000	//
#endif

#ifndef _ADR_MAX_F0
#define _ADR_MAX_F0					0x0105      //колличество регистров страницы 0
#endif

#ifndef _ADR_MAX_F1
#define _ADR_MAX_F1					0x0105      //колличество регистров страницы 1
#endif

#ifndef _ADR_MAX_F3
#define _ADR_MAX_F3					0x03C2    //колличество регистров страницы 0
#endif

#ifndef _ADR_MAX_F4
#define _ADR_MAX_F4					0x03C2      //колличество регистров страницы 1
#endif

#ifndef _Yellow
#define _Yellow					0x04FF0400		//0x008F0400  //0x004F0400     //Желтый цвет - 
#endif

#ifndef _YellowON
#define _YellowON					0x00FF0000		//0x008F0400  //0x004F0400     //Желтый цвет - 
#endif

#ifndef _Green
#define _Green 					0x00004F00	 //0x00004F00		// Зеленый цвет 0000FF00
#endif

#ifndef _GreenON
#define _GreenON 					0xA0A0FF00	 //0x00004F00		// Зеленый цвет
#endif

#ifndef _Red
#define _Red 					0xFFFF0000	// 0x004F0000		//  Красный
#endif

#ifndef _Blue
#define _Blue 				0x00000005		// Синий
#endif

#ifndef _Violet
#define _Violet 				0x00050001	// Фиолетовый
#endif

#ifndef _Black
#define _Black				0x00000000	// Фиолетовый
#endif

#ifndef _Rins
#define _Rins						1000 // 231206  5000		// Сопротивление изоляции
#endif




#ifndef _Rloop
#define _Rloop				500		// Сопротивление шлейфа
#endif

//#ifndef _vPro
//#define _vPro				23041201		// номер версии
//#endif

#ifndef _v100
#define _v100				50  //0x10000			// Напряжение смещения
#endif

#ifndef _Voltconst
#define _Voltconst	   				0x7D0630 //0x7D0654 // 0x7D3736 //0x7D4505 // 0x7D751B//  смещение для биполярного измерения напряжения 8379896//
#endif

#ifndef _setVolt
#define _setVolt					5				// уставка по умолчению для ошибки по напряжению утечки
#endif

#ifndef _constBipol
#define _constBipol												0x800000 // 231020   8388815 // 0x7D0630 // 0x00694A1D //7634174 //5395843  //0x007EF21D // константа для 0 биполярного сгнала исп. для вычисления сопр. изоляции
#endif

#ifndef _constBipol_minus
#define _constBipol_minus												8388846 // 0x7D0630 // 0x00694A1D //7634174 //5395843  //0x007EF21D // константа для 0 биполярного сгнала исп. для вычисления сопр. изоляции
#endif

#ifndef _constRiz
#define _constRiz								69333 // значение при бесконечном сопротивлении
#endif

// электрические константы

#ifndef _Uop
#define _Uop 										4.8 		// опорное напряжение сопротивленя изоляции и т. в.
#endif

#ifndef _Rinp
#define _Rinp							2447 // входное сопротивление канала измерения сопротивления изоляции (напряжения)...
#endif

#ifndef _Rmeas
#define _Rmeas							47 // сопротивление измерительного шунта
#endif 

#ifndef _korr
#define _korr							20.0 // коррекция значений сопротивления изоляции
#endif

//#ifndef UID_BASE
//#define UID_BASE 0x1FFFF7E8
//#endif

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADCS2_Pin GPIO_PIN_2
#define ADCS2_GPIO_Port GPIOE
#define ADCS3_Pin GPIO_PIN_3
#define ADCS3_GPIO_Port GPIOE
#define LED_R_Pin GPIO_PIN_5
#define LED_R_GPIO_Port GPIOE
#define LED_G_Pin GPIO_PIN_6
#define LED_G_GPIO_Port GPIOE
#define BRD_ID0_Pin GPIO_PIN_0
#define BRD_ID0_GPIO_Port GPIOH
#define BRD_ID1_Pin GPIO_PIN_1
#define BRD_ID1_GPIO_Port GPIOH
#define LINK_YEL_Pin GPIO_PIN_0
#define LINK_YEL_GPIO_Port GPIOB
#define LINK_GRN_Pin GPIO_PIN_1
#define LINK_GRN_GPIO_Port GPIOB
#define FAIL_RED_Pin GPIO_PIN_7
#define FAIL_RED_GPIO_Port GPIOE
#define FAIL_GRN_Pin GPIO_PIN_8
#define FAIL_GRN_GPIO_Port GPIOE
#define CH1_Pin GPIO_PIN_9
#define CH1_GPIO_Port GPIOE
#define CH2_Pin GPIO_PIN_11
#define CH2_GPIO_Port GPIOE
#define BT1_Pin GPIO_PIN_12
#define BT1_GPIO_Port GPIOE
#define BT2_Pin GPIO_PIN_13
#define BT2_GPIO_Port GPIOE
#define BT4_Pin GPIO_PIN_14
#define BT4_GPIO_Port GPIOE
#define BT3_Pin GPIO_PIN_15
#define BT3_GPIO_Port GPIOE
#define BT5_Pin GPIO_PIN_10
#define BT5_GPIO_Port GPIOB
#define BT6_Pin GPIO_PIN_15
#define BT6_GPIO_Port GPIOB
#define BT7_Pin GPIO_PIN_8
#define BT7_GPIO_Port GPIOD
#define BT9_Pin GPIO_PIN_9
#define BT9_GPIO_Port GPIOD
#define BT8_Pin GPIO_PIN_10
#define BT8_GPIO_Port GPIOD
#define BT10_Pin GPIO_PIN_11
#define BT10_GPIO_Port GPIOD
#define CH3_Pin GPIO_PIN_12
#define CH3_GPIO_Port GPIOD
#define CH4_Pin GPIO_PIN_13
#define CH4_GPIO_Port GPIOD
#define CH5_Pin GPIO_PIN_14
#define CH5_GPIO_Port GPIOD
#define CH6_Pin GPIO_PIN_15
#define CH6_GPIO_Port GPIOD
#define CH7_Pin GPIO_PIN_6
#define CH7_GPIO_Port GPIOC
#define CH8_Pin GPIO_PIN_7
#define CH8_GPIO_Port GPIOC
#define CH9_Pin GPIO_PIN_8
#define CH9_GPIO_Port GPIOC
#define CH10_Pin GPIO_PIN_9
#define CH10_GPIO_Port GPIOC
#define BRD_ID0D0_Pin GPIO_PIN_0
#define BRD_ID0D0_GPIO_Port GPIOD
#define BRD_ID1D1_Pin GPIO_PIN_1
#define BRD_ID1D1_GPIO_Port GPIOD
#define LED_B_Pin GPIO_PIN_9
#define LED_B_GPIO_Port GPIOB
#define ADCS0_Pin GPIO_PIN_0
#define ADCS0_GPIO_Port GPIOE
#define ADCS1_Pin GPIO_PIN_1
#define ADCS1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
uint8_t bit_to_byte(uint8_t byte, uint8_t bit);
uint16_t byte_to_word(uint8_t arr1,uint8_t arr2);
uint16_t crc16(uint8_t *adr_buffer, uint32_t byte_cnt);
_Bool mbError(uint8_t *ARR) ;
_Bool mbFunkton(_Bool *arrBool, uint16_t *arrWord,uint8_t *mbInput, _Bool block);
_Bool DeltaBool(uint16_t delay);
//_Bool oneButtomPush();
uint8_t timePush (uint16_t delayPush, uint8_t numChannel);
_Bool timeNoPush (uint16_t delay, uint8_t numChannel);
 uint32_t Nblinck (uint16_t mode, uint32_t color, uint16_t period, uint8_t Ch);
uint8_t ModeCH (uint8_t nCh, _Bool* Alarm, uint16_t* wordSet, uint8_t adc_current);
 //++++++++++++++++++++++++++++++++++++++++++++++++++
 void Blink_LED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
 void LED_Link(uint8_t val) ;
 void LED_Fail(uint8_t val);
 
 //+++++++++++++++++++++++++++++++++++++++++
 void ADC_CS(int n);
 void ADC_reset(void);
uint16_t ADC_readID(void);
 void ADC_set_mode(uint16_t mode);
 void ADC_set_config(uint16_t cfg);
 uint32_t ADC_read(uint32_t n, uint32_t delay);
//void ADC_measure(uint8_t nCh, uint16_t* arrWord, _Bool* arrBool, _Bool calibrate, uint32_t* noise_1, uint32_t* noise_2);
void ADC_measure_Pre(uint8_t nCh, uint16_t* arrWord, uint16_t* arrSetpoint);

void ADC_measureVolt(uint8_t nCh, uint16_t* arrWord, _Bool* arrBool);
uint16_t ADC_Error(uint8_t id);
void LedCH(uint16_t kDelay, uint16_t* ledCh);
_Bool All_buttons_NoPush();
//void adcCycle(uint8_t nCh);
uint16_t ADC_Status(void);
void preset_V(void);
uint32_t pro_CRC(void);
void arChar_to_arrWord( uint8_t *result, uint16_t *arrWord);

void ADC_measure(uint8_t nCh, uint16_t* arrWord, _Bool* arrBool, _Bool calibrate);

_Bool controlEon(uint8_t nCh);
void EON_ready();

uint8_t EONmode(uint8_t nCh, _Bool EONoff, uint16_t delay);

uint32_t Alarm_blinck (uint32_t color1, uint32_t color2, uint16_t period,uint8_t Ch);

_Bool pause(uint16_t delta);

void ADC_measure_plus(uint8_t nCh, uint16_t* aWord, _Bool* aBool);
void ADC_measure_minus(uint8_t nCh, uint16_t* aWord, _Bool* aBool);
uint16_t formula(float rdm, float rdp);

uint16_t getMode(uint8_t nCh, uint16_t* mode);
void setMode(uint8_t nCh, uint16_t* arrWord, uint16_t* mode);

uint8_t sort(uint8_t* status);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
