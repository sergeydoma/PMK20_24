#include "main.h"//

		float testUz[10];
		uint32_t testS[10];
//extern UART_HandleTypeDef huart1;
//extern DMA_HandleTypeDef hdma_usart1_rx;
//extern DMA_HandleTypeDef hdma_usart1_tx;
//extern SPI_HandleTypeDef hspi1;

//extern uint32_t  led_rgb[10];
//extern float RZ[3];
//extern GPIO switch_gpio[10]; 
//extern _Bool arrBool[0x400];  
//extern	uint16_t arrWord[0x400];
//extern uint32_t constBipol;
//extern float Bipol[10];
//extern float sR;

//extern uint8_t switch_state[10]; 
//extern int adc_current_chan; 
//extern uint8_t test_Status;
//extern uint32_t callibrateU[10];
//extern float callFlo[10];

void ADC_measureVolt(uint8_t nCh, uint16_t* aWord, _Bool* aBool) 
	{
	extern uint32_t adc_delay;
	uint32_t n = 16;
	uint32_t s;
	uint16_t limit, delta;
	uint8_t alarm1, alarm2;
	float k;
	float uz;
	_Bool polar;

 	
		// Напряжение утечки 1	**************	
	ADC_set_config(0x0010); //0x1050) смещение внешнее REFin1+ REFin1- буфера нет канал 1
	HAL_Delay(adc_delay);		
	ADC_set_mode(0x8009);  // calibrate zero
	HAL_Delay(adc_delay);			
	ADC_set_mode(0x800a);  // calibrate full-scale
	HAL_Delay(adc_delay);		
	ADC_set_mode(0x0009);  // return to continuous reads
	HAL_Delay(adc_delay);
	ADC_read(1, adc_delay); // buffer flush	
	ADC_read(1, adc_delay); // buffer flush
	s = ADC_read(n, adc_delay);
	//++++++++++++++++++++++++++
	
	if (s > _constBipol )
			{
				s = s - _constBipol;
				polar = 1;
			}
	else 
		{
			s = _constBipol - s;
			polar = 0;
		}	
//////	s=s>>8;
//////	
//////	s=(s*100)/298;
//////	if (s<=280){s=0;}
		
	uz = s;
		
	k = 47 * 8.*1024.*1024; //(1020 * _Uop);
	
	k /= 2447* _Uop;
	
	uz /= k; //34267; //33567;
	
	if (polar)
	{	
		uz = uz +_Uop/2;
	}
	else
	{
		uz = uz - _Uop/2;
	}	
	if (uz<= 2.8)
	{uz = 0;}
	testUz[nCh] = uz;
//	callFlo[nCh]=s;
	
		aWord[190 +nCh] = uz*10;
		
		uz*=100;
		limit  = aWord[200 + nCh];
		delta = limit * 10;
		limit = limit *100;
		if (uz > limit)
		{alarm1 =1;}
		else if (uz <(limit - delta))
		{alarm1 = 0;}	
		
		// Напряжение утечки 2	*******************

	ADC_set_config(0x0011); //0x1051) смещение внешнее REFin1+ REFin1- буфера нет биполярный сигнал 
	HAL_Delay(adc_delay);
		
	ADC_set_mode(0x8009);  // calibrate zero
	HAL_Delay(adc_delay);
	ADC_set_mode(0x800a);  // calibrate full-scale
	HAL_Delay(adc_delay);
	ADC_set_mode(0x0009);  // return to continuous reads
	HAL_Delay(adc_delay);
	ADC_read(1, adc_delay); // buffer flush
	ADC_read(1, adc_delay); // buffer flush
	
	s=0;
	s = ADC_read(n, adc_delay);
	
	if (s > _constBipol )
		{
			s = s - _constBipol;
			polar =1;
		}
	else 
	{
		s = _constBipol - s;
		polar =0;
	}
//	arrWord[nCh+200] = s;
//	s=s>>8;	
//	s=(s*100)/298;
//	if (s<=280){s=0;}
//	aWord[230 +nCh] = s/10;
	testS[nCh] = s;
	uz = s;
	
	k = 47 * 8.*1024.*1024; //(1020 * _Uop);	
	k /= 2447* _Uop;	
	uz /= k; //34267; //33567;	
	if(polar)
	{uz = uz +_Uop/2;}
	else
	{uz = uz -_Uop/2;}
	if (uz<= 2.8)
	{uz = 0;}

	
	
		aWord[230 +nCh] = uz*10;
	
		
		uz *=100;
		limit  = aWord[200 + nCh];
		delta = limit * 10;
		limit = limit *100;
		
		if (uz > limit)
		{alarm2 =1;}
		else if (uz <(limit - delta))
		{alarm2 = 0;}	

		aBool[nCh +100] = alarm1 | alarm2;
		aBool[nCh+110] = !aBool[nCh+100];
}