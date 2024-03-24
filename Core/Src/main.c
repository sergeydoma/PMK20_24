/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MY_FLASH.h"
#include "MD_5.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
 uint16_t current = 0; // счетчик
 uint8_t rer[100];
 uint8_t arr[168];
 uint8_t mbInput[16]; // Массив входных данных modbus
 uint8_t lenMBrx;
 uint8_t mbAddr;
 uint8_t i2cAddr;
 uint16_t crcTx;
 _Bool arrBool[0x400]; 
	_Bool arrBoolTemp[100]; 
	uint16_t arrWord[0x400];
	uint16_t arrWord_minus[0x400];
	uint16_t arrWordBipol[0x400];
	_Bool arrBoolBipol[0x400];
 _Bool uartTx;
  _Bool sem; // сигнал о том что посылка modbud принята без ошибок.
  _Bool blockMode; // блокировка записи по modbus в режиме местного упрвления
  _Bool semF; //  посылка по модбасу принята
	uint8_t timePush_Test;
	uint8_t timePush_Test_N;
	_Bool set_set[10]; // запись уставок по первому сигналу
	uint16_t setPoint[100];
	//MODE
//	uint8_t mode;// test
uint8_t PMU_Mode;
int16_t blink; // для выбора режима упрвления мигание
 _chlMode cTime[10]; // переменная для определения времени нажатия кнопок каждого канала
  uint8_t chMode[10]; // режим мониторинга кабеля
	uint8_t chModeTemp[10];// заглушка
	// CPU ID
	long itr00;
	volatile uint32_t *UniqueID = (uint32_t *)0x1FFF7A10;
	volatile uint32_t __UniqueID[3];
	volatile extern uint8_t test_Status;
	uint16_t errorCounter;
	_Bool ttr = 0; // Блокировка при записи в Флеш память 
		HAL_StatusTypeDef RTF; // test i2c	
		HAL_StatusTypeDef WTF; // test i2c	
		_Bool slaveWrData;
		_Bool slaveRdData;
		 int iik;
		uint8_t arrI2c_R[12];
		
		_Bool i2c_rd = 0;
		_Bool i2c_td = 0;
		uint32_t lanTimer = 0;
		uint8_t hvAllarm = 0;
		
		uint8_t lanCurr;// cтетчик для индикации работы сети
		_Bool lanLed; // индикатор работы сети ?
		uint16_t lanTemer;
		uint32_t rlanTest;
		_Bool blanTest;
		
		_Bool stMbAdd[2];
		
		uint8_t EON_off =1; // когда напряжение смещени 100 В должно быть снято
		_Bool pauseEon;
		_Bool pauseEonM;
		int i_Eon = 0;
		uint8_t modeEon =0;
		uint16_t delayEon = 900;
		uint8_t readyEon;
//		uint8_t modeEon;
		uint8_t timeModeEon = 0;
		_Bool startLokal = 0;
		uint8_t statusMb;
		uint8_t alarm_mes =0;
		
		float callR[10];
		float tempSet[10];
		
		// синхронизация I2c с записью флеш памяти
		uint8_t sinM; //
		uint8_t sinT; //
		uint8_t sinR; //
		uint8_t changSetP[10] = {0,0,0,0,0,0,0,0,0,0};
//	uint16_t currentTime; // переменная для выдержки 30 сек не нажата ни одна кнопка
GPIO switch_gpio[10] = {
	{ BT1_GPIO_Port, BT1_Pin },
	{ BT2_GPIO_Port, BT2_Pin },
	{ BT3_GPIO_Port, BT3_Pin },
	{ BT4_GPIO_Port, BT4_Pin },
	{ BT5_GPIO_Port, BT5_Pin },
	{ BT6_GPIO_Port, BT6_Pin },
	{ BT7_GPIO_Port, BT7_Pin },
	{ BT8_GPIO_Port, BT8_Pin },
	{ BT9_GPIO_Port, BT9_Pin },
	{ BT10_GPIO_Port, BT10_Pin }
};

	
	//plus
uint8_t adc_current =0;
	uint32_t adc_delay = 150;//120;//500; //
	uint32_t  led_rgb[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	float RZ[4];
	int CC; // TEST
	int currentPB =0; //  селекция нажатия кнопки push_Button
	// Flash
		uint8_t data[64];    // test
    uint16_t res[400];		// test
	// monitor CH
	uint8_t ch_monitor[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int ideley=0;
	int kdeley=0;
	uint8_t tempCh[10];
	_Bool startSett; //  считывание при старте
	//For Skada

	char mBuffer[65535];
	
	// Ошибка платы
	_Bool pmkError;
	uint16_t adc_Status; // Состояние АЦП
	
	//gpio
	// md5
	uint32_t CCRC;
	char md5sum;
	char ccrrc;
	
	uint8_t arrI2c_T[12];
	
	uint32_t SetEEprom;
	_Bool LoadNumDev;
	struct I2C_Module i2cm;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;
DMA_HandleTypeDef hdma_i2c1_rx;

IWDG_HandleTypeDef hiwdg;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim9;
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;
TIM_HandleTypeDef htim14;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM7_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM9_Init(void);
static void MX_TIM11_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM10_Init(void);
static void MX_TIM14_Init(void);
static void MX_CRC_Init(void);
static void MX_I2C1_Init(void);
static void MX_IWDG_Init(void);
/* USER CODE BEGIN PFP */

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	
//		HAL_IWDG_Refresh(&hiwdg);
			
		slaveWrData = 0;
	
//							for (int i=0; i<1000; i++){__ASM("nop");}
							if(sinM ==0)
							{
									
									RTF = HAL_I2C_Slave_Receive_DMA(&hi2c1, arrI2c_R,12);						
									for (int i=0; i<100; i++){__ASM("nop");}
										
									arrWord[110] = arrI2c_R[1]<<8	| arrI2c_R[2];
									arrWord[111] = arrI2c_R[3]<<8	| arrI2c_R[4];
										
									if(arrI2c_R[5] == 0xF0){arrWord[114]=0x01FF;}
									else if (arrI2c_R[5] == 0x0F){arrWord[114]=0x05;}
		//							arrWord[114] = arrI2c_R[5]; //<<8	| arrI2c_R[6];
							}
							else
							{
								sinR = 1;
							}
}
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
			
		slaveRdData = 0;

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    
    if ((huart == &huart2)&(!semF))
    {
     			
		// обработка модбас       
     HAL_UART_Receive_DMA(&huart2,(uint8_t*) &rer,1);
    }
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
};
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
		HAL_UART_Receive_DMA(&huart2,(uint8_t*) &rer,1); // Перезапуск при ошибке
}	
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
	i2cm.instance = hi2c1;
	i2cm.sclPin = GPIO_PIN_10;
	i2cm.sclPort = GPIOB;
	i2cm.sdaPin = GPIO_PIN_11;
	i2cm.sdaPort = GPIOB;
	
		uint8_t tbyte;
		uint64_t flas_length = 0;
		_Bool flas_count = 0;
		uint8_t ff_count =0;
		
		
		
//		tbyte =	*(uint8_t*)0x8000000; // прочитать байты;
	char buffer2[9];
	char md5Vstart;		//[flas_length>>1];
	uint32_t md5Vfinish;    //[flas_length>>1];


		
//    mbAddr=0x01;
    startSett = 0;
	  MY_FLASH_SetSectorAddrs(11, 0x080E0000);
//    uint8_t data[2048-8];
//    data[0]=0xA5;//serial_number[0];//0x10;
//    data[1]=0xA6;//serial_number[1];//0x34;

/****** Работа с FLASH *****/
	//предустановки 

	//Проверяем flash
	
	 MY_FLASH_ReadN(0,res,400,DATA_TYPE_16);
	 
	if (res[0] != 0xffff)
	{	
		MY_FLASH_ReadN (0, arrWord, 400, DATA_TYPE_16);	
	}
//    MY_FLASH_WriteN(0,arrWord,40,DATA_TYPE_16); // Запись
//    MY_FLASH_ReadN(0,res,8,DATA_TYPE_8);	
	arrBool[20]=1; arrBool[21]=1; arrBool[22]=1;arrBool[23]=1;arrBool[24]=1; arrBool[25]=1; arrBool[26]=1; arrBool[27]=1; arrBool[28]=1; arrBool[29]=1;
  arrBool[30]=1; arrBool[31]=1; arrBool[32]=1;arrBool[33]=1;arrBool[34]=1; arrBool[35]=1; arrBool[36]=1; arrBool[37]=1; arrBool[38]=1; arrBool[39]=1;
	arrBool[40]=1; arrBool[41]=1; arrBool[42]=1;arrBool[43]=1;arrBool[44]=1; arrBool[45]=1; arrBool[46]=1; arrBool[47]=1; arrBool[48]=1; arrBool[49]=1;
	arrBool[50]=1; arrBool[51]=1; arrBool[52]=1;arrBool[53]=1;arrBool[54]=1; arrBool[55]=1; arrBool[56]=1; arrBool[57]=1; arrBool[58]=1; arrBool[59]=1;
	arrBool[110]=1; arrBool[111]=1; arrBool[112]=1;arrBool[113]=1;arrBool[114]=1; arrBool[115]=1; arrBool[116]=1; arrBool[117]=1; arrBool[118]=1; arrBool[119]=1;
	arrBool[120]=1; arrBool[121]=1; arrBool[122]=1;arrBool[123]=1;arrBool[124]=1; arrBool[125]=1; arrBool[126]=1; arrBool[127]=1; arrBool[128]=1; arrBool[129]=1;
	arrBool[130]=1; arrBool[131]=1; arrBool[132]=1;arrBool[133]=1;arrBool[134]=1; arrBool[135]=1; arrBool[136]=1; arrBool[137]=1; arrBool[138]=1; arrBool[139]=1;
	arrBool[160]=1; arrBool[161]=1; arrBool[162]=1;arrBool[163]=1;arrBool[164]=1; arrBool[165]=1; arrBool[166]=1; arrBool[167]=1; arrBool[168]=1; arrBool[169]=1;
	for(int i = 0; i<10 ; i++)
	{
			if ((arrWord[200+i]==0) | (arrWord[200+i] == 0xFFFF))
			{arrWord[200+i]=_setVolt;}
			if((arrWord[i] == 0) | (arrWord[i] == 0xFFFF))
			{
				arrWord[i]=0x1414;
//				arrWord[250 + i] = 0x0A0A; // уставка предупредительной сигнализации по умолчанию
				
			}
			if((arrWord[250 + i] == 0) | (arrWord[250+i] == 0xFFFF))
				{
					arrWord[250+i] = 0x0A0A;
				}
	}
	arrWord[140]=0;
	arrWord[380]=10;
	arrWord[221] = 0xFC00; // Напряжение -100 В
	arrWord[222] = 0xFC00; // напряжение +100 В
	//ID CPU
	__UniqueID[0] = UniqueID[0];
	__UniqueID[1] = UniqueID[1];
	__UniqueID[2] = UniqueID[2];

	arrWord[100] = __UniqueID[0];
	arrWord[101] = __UniqueID[0]>>16;
	arrWord[102] = __UniqueID[1];
	arrWord[103] = __UniqueID[1]>>16;
	arrWord[104] = __UniqueID[2];
	arrWord[105] = __UniqueID[2]>>16;
	
	// Версия программы
	arrWord[90] = _version>>16;
	arrWord[91] = (uint16_t)_version;
//	arrWord[92] = 20;
//	arrWord[93] = 01;
// md5
	
		uint8_t result[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	 
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	__HAL_RCC_I2C1_CLK_ENABLE();
  HAL_Delay(100);
  __HAL_RCC_I2C1_FORCE_RESET();
  HAL_Delay(100);
  __HAL_RCC_I2C1_RELEASE_RESET();
  HAL_Delay(100);		
//	hi2c1.Init.OwnAddress1 = i2cAddr;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM7_Init();
  MX_SPI1_Init();
  MX_TIM9_Init();
  MX_TIM11_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_TIM1_Init();
  MX_TIM10_Init();
  MX_TIM14_Init();
  MX_CRC_Init();
//  MX_I2C1_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
    HAL_UART_Receive_DMA(&huart2,(uint8_t*) &rer,1);
		
//		HAL_I2C_Slave_Transmit_DMA(&hi2c1,arrI2c, 2);
    HAL_TIM_Base_Start_IT(&htim7);
		HAL_TIM_Base_Start_IT(&htim14);
//		CCRC = pro_CRC();
			stMbAdd[0]=HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0)!=0;
			stMbAdd[1]=HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1)!=0;
			i2cAddr = stMbAdd[0]+stMbAdd[1]+1; //230807	
//			hi2c1.Init.OwnAddress1 = i2cAddr;
			 
			 MX_I2C1_Init();
			 
			 
		CCRC = HAL_CRC_Calculate(&hcrc, (uint32_t *)0x8000000, 0xE208); //0x40000);
//		md5sum = (uint32_t)CCRC;
		sprintf(buffer2, "%X",CCRC);
		md5String(buffer2,result);

		arChar_to_arrWord(result, arrWord);
		
		uint8_t switch_state[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		ADC_CS(-1); // Выбор АЦП отключен

	
//	hi2c1.Init.OwnAddress1 = i2cAddr;
//		hi2c1.Devaddress = i2cAddr;
//	MX_I2C1_Init();
		
	HAL_TIM_PWM_Start_IT(&htim9, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start_IT(&htim9, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start_IT(&htim11, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim1);

	for(int k=0; k<10; k++) 
		{
		int b = HAL_GPIO_ReadPin(switch_gpio[k].port, switch_gpio[k].pin) == 0;
		switch_state[k] = b;
		}
	
	LED_Link(1);
	LED_Fail(1);
	for(int k=0; k<10; k++)
		{
			led_rgb[k] = 0x00001f00;
			HAL_Delay(200);//00);
		}

	LED_Link(2);
	LED_Fail(2);
	for(int k=0; k<10; k++)
		{
			led_rgb[k] = 0x001f0000;
			HAL_Delay(200);//00);
		}

	LED_Link(0);
	LED_Fail(0);
	for(int k=0; k<10; k++)
		{
		led_rgb[k] = 0x0000001f;
		HAL_Delay(200);//00);
		}

	LED_Fail(1);
	for(int k=0; k<10; k++)
		{
			led_rgb[k] = 0x00000000;
			HAL_Delay(100);//00);
		}
			
		pmkError=0; // Сброс ошибки при перезагрузке
		
//		arrWord[220] = ADC_Error(0x4F);
		
		pmkError = 0; // arrWord[220]!= 0;//230721
		
		if (pmkError)
		{arrWord[140]=arrWord[140]|0x1;}
		else
		{arrWord[140]=arrWord[140]&0xFE;}
		
		 HAL_IWDG_Refresh(&hiwdg);
	
		
		
		
		
//		ADC_CS(0);
//		adc_Status = ADC_Status();	
		
		
		
			HAL_TIM_Base_Start_IT(&htim10);
		
		i2c_rd = 1;
//			HAL_TIM_Base_Start_IT(&htim14);
		//Проверка состояния 10 АЦП по ID Общая

//		ADC_CS(0);
//		timePush_Test_N = ADC_readID(); 
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
	{
//		HAL_IWDG_Refresh(&hiwdg);
//		arrWord[145] = modeEon;
		
		for(adc_current=0; adc_current<11; adc_current++) 
		{
			arrWord[220] = ADC_Error(0x4F);
			HAL_IWDG_Refresh(&hiwdg);
			
			if (adc_current < 10)
			{


				
				
				if (PMU_Mode==2)//230722
				{
					preset_V(); // Предустановка с запиью параметров в Флеш
				}
				
				
				
				ADC_CS(adc_current);
				ADC_reset();
//				ADC_measureVolt(adc_current,arrWord,arrBool);
				if (startSett) // запуск прошел
				{
					if(arrWord[40+adc_current]==0)
						{
							arrBool[20+adc_current]=1;
							arrBool[30+adc_current]=1;
							arrBool[40+adc_current]=1;
							arrBool[50+adc_current]=1;
							arrBool[110+adc_current]=1;
							arrBool[120+adc_current]=1;
							arrBool[130+adc_current]=1;
							arrBool[160+adc_current]=1;
							
//							ADC_measure(adc_current, arrWord, arrBoolTemp, startSett);
//							set_set[adc_current] = 1;
						}
//						else if ((arrWord[40+adc_current]!=0)&
//										(arrWord[40+adc_current]!=1)&
//										(arrWord[40+adc_current]!=3)&
//										(arrWord[40+adc_current]!=4)&
//										(arrWord[40+adc_current]!=5))									
//									
//						{
							
							
							{
								arrBool[200] = 1;
								switch (modeEon)
              {
              	case 0:		
									
              		break;
              	case 1:
										ADC_measureVolt(adc_current,arrWord,arrBool);
              		break;
								case 2:										
									break;
								case 3:									
									break;
								case 4:
										ADC_measure_minus(adc_current, arrWordBipol,arrBoolTemp);
									break;
								case 5:	
										for(int i = 0; i<10;i++)
										{									
											arrWord[170+i] = arrWordBipol[170+i]; // сопротивление изоляции 1 при - смещении
											arrWord[180+i] = arrWordBipol[180+i]; // сопротивление изоляции 2 при - смещении
											arrWord[140] = arrWordBipol[140];
											arrWord[221] = arrWordBipol[221];
											arrWord[222] = arrWordBipol[222];
										}
									break;
								case 6:									
								break;
								case 7:										
								break;
								case 8:
									ADC_measure_plus(adc_current,arrWordBipol, arrBoolBipol);
									break;
								case 9:
									for(int i = 0; i<10;i++)
									{
										arrWord[70+i] = arrWordBipol[70+i]; // сопротивление шлейфа
										arrBool[40+i] = arrBoolBipol[40+i]; // авария сопротивления шлейфа вниз
										arrBool[50+i] = arrBoolBipol[50+i]; // авария сопротивления шлейфа вврех

										arrWord[50+i] = arrWordBipol[50+i]; // сопротивление изоляции 1
										arrBool[20+i] = arrBoolBipol[20+i]; // авария изоляции 1
										
										arrWord[60+i] = arrWordBipol[60+i]; // сопротивление изоляции 2
										arrBool[30+i] = arrBoolBipol[30+i]; // авария изоляции 2
										
										arrWord[150+i] = arrWordBipol[150+i];
										arrWord[160+i] = arrWordBipol[160+i];
										
										arrBool[110 +i] = arrBoolBipol[110+i]; // Предупреждение изоляции 1
										arrBool[120 +i] = arrBoolBipol[120+i]; // Предупреждение изоляции 2
										arrBool[130 +i] = arrBoolBipol[130+i]; // Предупреждение шлейфа вниз
										arrBool[160 +i] = arrBoolBipol[160+i]; // Предупреждение шлейфа вверх
										
										arrWord[140] = arrWordBipol[140];
										arrWord[221] = arrWordBipol[221];	
										arrWord[222] = arrWordBipol[222];	
									}
									break;
								case 10:
									break;
              	default:
              		break;
              }
														
							}
//						}
						if ((arrWord[40+adc_current]==1)|(arrWord[40+adc_current]==3)|(arrWord[40+adc_current]==4)|(arrWord[40+adc_current]==5)|(arrWord[40+adc_current]==9))
						{
							arrBool[adc_current+20]=1;
							arrBool[adc_current+30]=1;
							arrBool[adc_current+40]=1;
							arrBool[adc_current+50]=1;
//							arrBool[100 + adc_current]=0;
							arrBool[110 + adc_current] = 1; // Предупреждение Riz1
							arrBool[120 + adc_current] = 1; // Предупреждение Riz2
							arrBool[130 + adc_current] = 1; // Предупреждение Rloop вниз
							arrBool[160 +	adc_current] = 1; // Предупреждение шлейфа вверх

							//							ADC_measure(adc_current, arrWord, arrBoolTemp, startSett); // монитринг только включеных калалов
						}
				}
				else  // идет процесс старта
				{
					if (modeEon==1)
							{
							startLokal=1;							
							}					
					if (startLokal ==1)
					{
						arrBool[200] = 0;
					switch (modeEon)
              {
              	case 0:									
              		break;
              	case 1:										
										ADC_measureVolt(adc_current,arrWord,arrBool);
              		break;
								case 2:										
									break;
								case 3:									
									break;
								case 4:
										ADC_measure_minus(adc_current, arrWordBipol,arrBoolTemp);
									break;
								case 5:
										for(int i = 0; i<10;i++)
										{	
											if(	arrWord[40+i]	==9){arrWord[40+i]=0;}	// 	отмена режима при перезагрузке				
											arrWord[170+i] = arrWordBipol[170+i]; // сопротивление изоляции 1 при - смещении
											arrWord[180+i] = arrWordBipol[180+i]; // сопротивление изоляции 2 при - смещении
											arrWord[140] = arrWordBipol[140];
											arrWord[221] = arrWordBipol[221];
											arrWord[222] = arrWordBipol[222];
										}
									break;
								case 6:									
								break;
								case 7:										
								break;
								case 8:
									ADC_measure_plus(adc_current, arrWordBipol, arrBoolBipol);
									break;
								case 9:
									for(int i = 0; i<10;i++)
									{
										arrWord[70+i] = arrWordBipol[70+i]; // сопротивление шлейфа
										arrBool[40+i] = arrBoolBipol[40+i]; // авария сопротивления шлейфа вниз
										arrBool[50+i] = arrBoolBipol[50+i]; // авария сопротивления шлейфа вврех

										arrWord[50+i] = arrWordBipol[50+i]; // сопротивление изоляции 1
										arrBool[20+i] = arrBoolBipol[20+i]; // авария изоляции 1
										
										arrWord[60+i] = arrWordBipol[60+i]; // сопротивление изоляции 2
										arrBool[30+i] = arrBoolBipol[30+i]; // авария изоляции 2
										
										arrWord[150+i] = arrWordBipol[150+i];
										arrWord[160+i] = arrWordBipol[160+i];
										
										arrBool[110 +i] = arrBoolBipol[110+i]; // Предупреждение изоляции 1
										arrBool[120 +i] = arrBoolBipol[120+i]; // Предупреждение изоляции 2
										arrBool[130 +i] = arrBoolBipol[130+i]; // Предупреждение шлейфа вниз
										arrBool[160 +i] = arrBoolBipol[160+i]; // Предупреждение шлейфа вверх
										
										arrWord[140] = arrWordBipol[140];
										arrWord[221] = arrWordBipol[221];	
										arrWord[222] = arrWordBipol[222];	
										
									}
//									timeModeEon = 1; 
									break;
								case 10:
									timeModeEon = 1; 
									break;
								
              	default:
              		break;
              }
						}
					
				}
				ADC_CS(-1);
			}
			if  ((timeModeEon ==1) & (startSett==0) & (modeEon == 0))    //((adc_current ==0) & (startSett==0) & (modeEon == 3))
			{
				startSett = 1;
				timeModeEon = 0;
			}
			
					
			}
		}








    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = i2cAddr<<1;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Reload = 0xfff;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 5;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 12799;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 15;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 5250;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 1600;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 80;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 5;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 255;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */
  HAL_TIM_MspPostInit(&htim9);

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 15;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 2625;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 5;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 255;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim11, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */
  HAL_TIM_MspPostInit(&htim11);

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 15;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 52500;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA1_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, ADCS2_Pin|ADCS3_Pin|FAIL_RED_Pin|FAIL_GRN_Pin
                          |CH1_Pin|CH2_Pin|ADCS0_Pin|ADCS1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LINK_YEL_Pin|LINK_GRN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, CH3_Pin|CH4_Pin|CH5_Pin|CH6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, CH7_Pin|CH8_Pin|CH9_Pin|CH10_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ADCS2_Pin ADCS3_Pin ADCS0_Pin ADCS1_Pin */
  GPIO_InitStruct.Pin = ADCS2_Pin|ADCS3_Pin|ADCS0_Pin|ADCS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : BRD_ID0_Pin BRD_ID1_Pin */
  GPIO_InitStruct.Pin = BRD_ID0_Pin|BRD_ID1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : LINK_YEL_Pin LINK_GRN_Pin */
  GPIO_InitStruct.Pin = LINK_YEL_Pin|LINK_GRN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : FAIL_RED_Pin FAIL_GRN_Pin */
  GPIO_InitStruct.Pin = FAIL_RED_Pin|FAIL_GRN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : CH1_Pin CH2_Pin */
  GPIO_InitStruct.Pin = CH1_Pin|CH2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : BT1_Pin BT2_Pin BT4_Pin BT3_Pin */
  GPIO_InitStruct.Pin = BT1_Pin|BT2_Pin|BT4_Pin|BT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : BT5_Pin BT6_Pin */
  GPIO_InitStruct.Pin = BT5_Pin|BT6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BT7_Pin BT9_Pin BT8_Pin BT10_Pin
                           BRD_ID0D0_Pin BRD_ID1D1_Pin */
  GPIO_InitStruct.Pin = BT7_Pin|BT9_Pin|BT8_Pin|BT10_Pin
                          |BRD_ID0D0_Pin|BRD_ID1D1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : CH3_Pin CH4_Pin CH5_Pin CH6_Pin */
  GPIO_InitStruct.Pin = CH3_Pin|CH4_Pin|CH5_Pin|CH6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : CH7_Pin CH8_Pin CH9_Pin CH10_Pin */
  GPIO_InitStruct.Pin = CH7_Pin|CH8_Pin|CH9_Pin|CH10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

GPIO led_gpio[10] = {
	{ CH1_GPIO_Port, CH1_Pin },
	{ CH2_GPIO_Port, CH2_Pin },
	{ CH3_GPIO_Port, CH3_Pin },
	{ CH4_GPIO_Port, CH4_Pin },
	{ CH5_GPIO_Port, CH5_Pin },
	{ CH6_GPIO_Port, CH6_Pin },
	{ CH7_GPIO_Port, CH7_Pin },
	{ CH8_GPIO_Port, CH8_Pin },
	{ CH9_GPIO_Port, CH9_Pin },
	{ CH10_GPIO_Port, CH10_Pin }
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	
	_Bool buttonPush;
    if(htim->Instance == TIM7) //check if the interrupt comes from TIM1
        {
					
					
           if (semF)
        {sem = mbError((uint8_t*) arr);
                            if (sem)
                            {

                                  uartTx = mbFunkton( arrBool, arrWord, arr, blockMode);
                                  
														}
                            
    semF = 0;
         }
        }
//				if(htim->Instance == TIM6) 

				/***************************************************/
								if (htim == &htim1) 
	{
		static int channel = 0;
		uint32_t rgb;
//	if (!ttr)
//	{
		HAL_GPIO_WritePin (led_gpio[channel].port, led_gpio[channel].pin, 0);

		channel = (channel + 1) % 10;
		rgb = led_rgb[channel];

		HAL_GPIO_WritePin(led_gpio[channel].port, led_gpio[channel].pin, 1);
//		if (ttr ==1)
//		{
//		 rgb = 0x00;
//		}
		
		htim9.Instance->CCR1 = (rgb >> 16) & 0xff; // RED
		htim9.Instance->CCR2 = (rgb >> 8) & 0xff; 	// GREEN
		htim11.Instance->CCR1 = (rgb >> 0) & 0xff; // BLUE
	
		if (arrWord[140]== 0)
		{LED_Fail(1);}
		else
		{LED_Fail(2);}	
		

		
		
		//******************************************
		 if (blanTest == 1)			 
			 {if (rlanTest < 2000)
				 {rlanTest++;}
				else 
				{blanTest = 0;}
			 }
		else
			{if (rlanTest > 0)
				{rlanTest-- ;}
				else
					{blanTest = 1;}
			}
		
		
	 }
	
	 if (htim == &htim10)
	 {
		 

		 
		 //					/* +++++++++++++++++++++++++++++++++++++++++++++++*/
	 
				//  Режимы работы ПМК 
		 arrWord[145] = modeEon;
      if (PMU_Mode == 0)
      { 
        uartTx=0;      
        blockMode = 1; //2307      
              buttonPush=DeltaBool(10000); //30000
              if (buttonPush ==1)
              {
							PMU_Mode=2;
							}
							LED_Link(0);
               
       }    
              //***********************************
       if (PMU_Mode == 1)
              {
                blockMode = 0;

                  if (uartTx==1)
                  {
                  PMU_Mode = 2;
                  }
//                      if (blink < 500) 
//                      {
												LED_Link(2);

//                      }
//                      else if (blink <= 1000)
//                      {
//												LED_Link(2);
//                      }
//                      else
//                      {
//												blink = 0; // Перезапуск цикла мигания
//                      }
//                      blink ++;
                  
              }
//              //***********************************
       if (PMU_Mode == 2)
              {  blockMode = 0;
								
              if (All_buttons_NoPush()==0)  //==0)
              {
                PMU_Mode = 0;
              }

				if (statusMb ==0)
				{ 
			
				 if (lanTimer > 50000)
						{
							LED_Link(2);
							arrI2c_T[1]=0;				 
						}
					else
						{							
							lanTimer++;					
						}
				}
				else
				{
					statusMb = 0;
					LED_Link(1);
					arrI2c_T[1]=1;
					lanTimer = 0;
				}
							

              }
						
                for (int i=0;i<10;i++)
								{
									if (startSett)
									{
										 changSetP[i] = 0;
										
										 ModeCH(i,arrBool, arrWord, adc_current, 	ch_monitor, modeEon, changSetP);
											
												alarm_mes = sort(ch_monitor);
										
												if (arrWord[40+i]==0)
														{set_set[i]=1;}
														
												if ((arrWord[40+i]==3)|(arrWord[40+i]==4)|(arrWord[40+i]==5))
												{set_set[i] = 0;}
												
												if (((arrWord[40+i]==2)|(arrWord[40+i]==6) | (arrWord[40+i]==9)) & set_set[i] & changSetP[i])
														{
															if (arrWord[50+i]< (_Rins/16)){arrWord[10+i]=(_Rins/16);}
															else {arrWord[10+i] = arrWord[50+i];}	// текущее значение в уставку
															
															if (arrWord[60+i]< (_Rins/16)){arrWord[20+i]=(_Rins/16);}
															else {arrWord[20+i] = arrWord[60+i];}
															
															if (arrWord[70+i]< (_Rloop)){arrWord[30+i]=(_Rloop);}
															else {arrWord[30+i] = arrWord[70+i];}
															
//															if (arrWord[200+i]< (_setVolt)){arrWord[200+i]=(_setVolt);}
//															else {arrWord[200+i] = arrWord[70+i];}
															arrWord[200+i]=(_setVolt);
															
														set_set[i] = 0;
														changSetP[i] = 0;	
														}
									}
									else
									{
										led_rgb[i]= _Blue; // _Violet;
									}
								}
								 
								

							}
	 if (htim == &htim14)
	 {

		 // номер шасси задание
					arrI2c_T[4] = arrWord[112]>>8;
					arrI2c_T[5] = arrWord[112];
					arrI2c_T[6] = arrWord[113]>>8;
					arrI2c_T[7] = arrWord[113];
		 // команда - записать
					arrI2c_T[8] = arrWord[114]>>8;
					arrI2c_T[9] = arrWord[114];
// Управление смещением 100 В
					arrI2c_T[11] = alarm_mes; //0xBB; //EON_off; // 1 когда напряжение должно быть снято
		 
		 		 
		 
//				arrI2c_T[0]= HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6);  //  adc_current; //test
//				arrI2c_T[1] = arr[1];	//
		 
//							if (RTF == HAL_OK)
//							{
							if (sinR ==0)
							{
								sinT =0;
								for (int i=0; i<100; i++){__ASM("nop");}
								WTF = HAL_I2C_Slave_Transmit_DMA(&hi2c1,arrI2c_T,12);// RTF =
	//						HAL_Delay(100);
								for (int i=0; i<100; i++){__ASM("nop");}
							}
							else
							{
								sinT = 1;
							}
//							}
		 

	//git							
									
							mbAddr = arrI2c_R[0]+ i2cAddr - 1;
							
							modeEon = arrI2c_R[10];	
//							hvAllarm = arrI2c_R[11];
								

								
							if((arrWord[140]&2)|(arrWord[140]&4))
								
							{arrI2c_T[0]=1;}
							
							else
								{arrI2c_T[0]=0;}
								
//								arrI2c_T[0] = EON_off;
								
//							WTF = HAL_I2C_Slave_Transmit_DMA(&hi2c1,masterAddr,5); //arrI2c, 3);								
//							for (int i=0; i<100; i++){}
								
//					slaveWrData=1;
//					while(slaveWrData);
				
//					RTF = HAL_I2C_Slave_Transmit_IT(&hi2c1,arrI2c, 3); // RTF = 
//		 
//					RTF = HAL_I2C_Master_Receive(&hi2c1,masterAddr, 3);
					
//					while(1)
//					{
//						if (RTF != HAL_BUSY)
//						{break;}	
							 
//					}						
	}
	}
 
void preset_V(void)
{
		ttr = 0;
	
		
		MY_FLASH_ReadN (0, res, 400, DATA_TYPE_16);	// чтение всего из флеш

		//		int i;
		
		for (int i=0; i<10; i++)
		{
			if ((res[40+i] <= 9) & (arrWord[40+i] >82)) // Нужно учесть режимы 6,7,8, 81, 82
			{arrWord[40+i] = res[40+i];}
			else if ((arrWord[40+i]<=9)&(arrWord[40+i] !=res[40+i]))
			{ttr =1;}
			
			
			// memory
			if (arrWord[i] 		!=res[i])		
				{
					ttr=1;
				}
			if (arrWord[10+i] !=res[10+i])
				{
					ttr=1;
				}
			if (arrWord[20+i] !=res[20+i])
				{
					ttr=1;}
			if (arrWord[30+i] !=res[30+i])
				{
					ttr=1;
				}
				if(arrWord[200+i] != res[200+i])
				{
					ttr=1;
				}	
				if(arrWord[i+250] != res[i+250])
				{
					ttr=1;
				}
					
			
			if (((arrBool[i+20] & arrBool[i+30] & arrBool[i+40])& arrBool[i+50]) == 1)
			{arrBool[i+60] = 1;}
			else
			{arrBool[i+60] = 0;}
			

		}
			if (ttr)		// & (PMU_Mode==2)) 230722
		{	
			sinM = 1;
			if  (sinT)
			{
					__disable_irq();
						
					MY_FLASH_WriteN(0,arrWord,400,DATA_TYPE_16); // Запись измененных значений режима и уставок.	

					__enable_irq();
				sinM = 0;
				sinR = 0;
			}
			
			
		}		

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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
