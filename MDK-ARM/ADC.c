#include "main.h"//
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern SPI_HandleTypeDef hspi1;

extern uint32_t  led_rgb[10];
extern float RZ[3];
extern GPIO switch_gpio[10]; 
//extern _Bool arrBool[0x400];  
//extern	uint16_t arrWord[0x400];
uint32_t constBipol;
float Bipol[10];
float sR;

uint8_t switch_state[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int adc_current_chan = -1;
uint8_t test_Status;
uint32_t callibrateU[10];
float callFlo[10];

void ADC_CS(int n)
{
	if ((n>=0) && (n < 10)) {
		adc_current_chan = n;
		GPIOE->BSRR = 15;
		if (n < 5) {
			GPIOE->BSRR = (8 | ((~n) & 7)) << 16;
		} else {
			n -= 5;
			GPIOE->BSRR = (((~n) & 7)) << 16;
		}
	} else {
		GPIOE->BSRR = 15;
		adc_current_chan = -1;
	}
}


void ADC_reset(void)
{
	uint8_t reset[4] = { 0xff, 0xff, 0xff, 0xff };

	HAL_SPI_Transmit(&hspi1, reset, 4, 2);

//	puts("ADC reset");
}


uint16_t ADC_readID(void)
{
	uint8_t id[2] = { 0x60, 0xff };
	uint16_t r;
	uint16_t out;

	r = HAL_SPI_TransmitReceive(&hspi1, id, id, 2, 2);
	HAL_Delay(100);
	out = id[1];// || (id[1]);
	return out;

//	printf("ADC ID: 0x%02X (%d)\n", id[1], r);
}

uint16_t ADC_Status(void)
{
	uint8_t id[2] = { 0x40, 0xff };
	uint16_t r;
	uint16_t out;
//	HAL_Delay(100);
	r = HAL_SPI_TransmitReceive(&hspi1, id, id, 2, 2);
	out = id[1];// || (id[1]);
	return out;

}


void ADC_set_mode(uint16_t mode)
{
	uint8_t md[4] = { 0x08, 0xff, 0xff, 0xff };

	// protect adc system bits
	mode &= ~0x0D20;


	md[1] = mode >> 8;
	md[2] = mode & 0xff;

	HAL_SPI_TransmitReceive(&hspi1, md, md, 3, 2);
}


void ADC_set_config(uint16_t cfg)
{
	uint8_t cf[4] = { 0x10, 0xff, 0xff, 0xff }; // 0x10 выбор регистра конфигурации

	// protect adc system bits
	cfg &= ~0x2000;


	cf[1] = cfg >> 8;
	cf[2] = cfg & 0xff;

	HAL_SPI_TransmitReceive(&hspi1, cf, cf, 3, 2);
	
	
}


uint32_t ADC_read(uint32_t n, uint32_t delay)
{
	uint8_t rd[4] = { 0x58, 0x80, 0x80, 0x80 }; // 0x58 регистр данных
	uint32_t i, r, s = 0;

	for(i=0; i<n; i++) {
		rd[0] = 0x58;
		HAL_SPI_TransmitReceive(&hspi1, rd, rd, 4, 2);
		r = (rd[1]<<16) | (rd[2]<<8) | rd[3];
//		printf("ADC: 0x%08x\n", (unsigned int) r);
		s += r;
//		HAL_Delay(delay);
			while (1) 
			{
			
			if ((ADC_Status() & 0x80)==0)
			 {break;}
			}	
	}

	if (n > 1) {
		s /= n;
//		printf("Average: 0x%08x\n", (unsigned int)s);
	}

	return s;
}
///******************************************************

	void ADC_measure(uint8_t nCh, uint16_t* arrWord, _Bool* arrBool, _Bool calibrate) 
	{
	extern uint32_t adc_delay;
	uint32_t n = 16;//16;
	uint32_t s;
	uint32_t v100; // Временно напряжение 100 вольт
	float R = 20000.0;
	int ok = 1;
	float delta = 0.2;

	//led_rgb[adc_current_chan] = 0x1f;
		
		// Сопротивление Шлейфа

	ADC_set_config(0x1012); // Внешний REFin1+ REFin2+  Ain 3 (+) буферный усилитель униполярный режим
	HAL_Delay(adc_delay);

	ADC_set_mode(0x8009);  // calibrate zero
	HAL_Delay(adc_delay);
	ADC_set_mode(0x800a);  // calibrate full-scale
	HAL_Delay(adc_delay);
	ADC_set_mode(0x0009);  // return to continuous reads
	HAL_Delay(adc_delay);

	//led_rgb[adc_current_chan] = 0x4f;

	ADC_read(1, adc_delay); // buffer flush
	ADC_read(1, adc_delay); // buffer flush
	s = ADC_read(n, adc_delay);
	
	

	//led_rgb[adc_current_chan] = 0x1f;

	R *= s;
	if (s <=0x00FFFAD7)
	{R /= (0x00FFFAD7 - s);}
	else 
	{R /= (s - 0x00FFFAD7);}	
//	R /= (0x00FFDCEE - s); //(0x00ffeb00 - s); // 0x00ffeb00   (0x00FFFAD7 - s);  //

	if (R > 31.0)
	{R -= 31.0;}  // TBU effective serial resistance

//	printf("Loop R: %.2f Ohm\n------------------\n", R);
 RZ[2] = R;	

	// Сопротивление Шлейфа
if (R>65535){R=65535;}

if((arrWord[70+nCh]<_LoopUpMin)& (R>_LoopUpMax)) // Скачек пока не надо
{ // таким образом фиксируем обрыв кабеля
			{
				arrBool[nCh+50]=0;
			}
}	
if (R<_LoopUpMax)
			{
				arrBool[nCh+50]=1;
			}

arrWord[70 + nCh] = R; // измеренное значение
//диапазон 
//if (((arrWord[nCh+40] == 2)||(arrWord[nCh+40] == 6))& calibrate)
//{
//////			switch ((uint8_t)arrWord[nCh]) //   допустимое отклонение от заданного среднего
//////		{
//////			case	_mode10P:
//////				delta = 0.1;
//////				break;  
//////			case	_mode30P:
//////				delta = 0.3;
//////				break;
//////			default:
//////				delta = 0.2;
//////				break;
//////		}	
			delta = ((uint8_t)arrWord[nCh])*0.01; //230717
		// Сопротивление шлейфа

			if (R < (arrWord[nCh +30]-arrWord[nCh +30]*delta)) // | (R > (arrWord[nCh +30]+arrWord[nCh +30]*delta)))
			{ 
				arrBool[nCh +40] = 0;	// выход за пределы для канала вниз
			}
			else
			{
				arrBool[nCh +40] = 1;
			}

//////////	//led_rgb[adc_current_chan] = 0x1f;
		//*******************************************
	// Проверка наличия 100 В
	v100 = 0;
			
	ADC_set_config(0x1093); //0x1051) смещение Внутреннее - буфер канал 4
	HAL_Delay(adc_delay);
		
	ADC_set_mode(0x8009);  // calibrate zero
	HAL_Delay(adc_delay);
	ADC_set_mode(0x800a);  // calibrate full-scale
		
	HAL_Delay(adc_delay);
	ADC_set_mode(0x0009);  // return to continuous reads
	HAL_Delay(adc_delay);

	//led_rgb[adc_current_chan] = 0x4f;

	ADC_read(1, adc_delay); // buffer flush
	ADC_read(1, adc_delay); // buffer flush
	v100 = ADC_read(n, adc_delay); // Измерение 100 В.
	HAL_Delay(adc_delay);
	if (v100 < _v100)
	{
		arrWord[140] 	= arrWord[140] | 0x2;
		arrWord[221 ] = arrWord[221] | 1 << nCh;
	}
	else
	{
		arrWord[140] &=  0xFD;
		arrWord[221] &= ~ (1<<nCh);
	}
	
// сопротивление изоляции 1
	
	ADC_set_config(0x0080); 	//	(0x1050); //0x1050) смещение внешнее REFin2+ REFin2- буфер канал 1 
	HAL_Delay(adc_delay);		
	ADC_set_mode(0x8009);  // calibrate zero
	HAL_Delay(adc_delay);			
	ADC_set_mode(0x800a);  // calibrate full-scale
	HAL_Delay(adc_delay);
			

			
	ADC_set_mode(0x0009);  // return to continuous reads
	HAL_Delay(adc_delay);

	//led_rgb[adc_current_chan] = 0x4f;
	
//	test_Status = ADC_Status();
//	
//			
//	HAL_Delay(adc_delay);
	ADC_read(1, adc_delay); // buffer flush	
	ADC_read(1, adc_delay); // buffer flush
	s = ADC_read(n, adc_delay);
	if (s > _constBipol	){s = 0;} //s - _Voltconst;}
	else {s = _constBipol - s;}	

	R=s;
	
//	R= 0xffffff - R;

	
	R = 2396*16.*1024.*1024 / s;
	R= R/1.2025;

	

	
//	R *= 2396.;
	R -= 1047.0;
	RZ[1] = R;
//	R -= 0937.93286;

	
//if (s > _constBipol	){s = 0;} //s - _Voltconst;}
//	else {s = _constBipol - s;}
//	s=s; //>>8;

	
//	sR = s/256;
//	sR=(sR)/298;
//	sR=s;

//	// расчет тока
//	float I;
//	I = sR;///80.66;

//	
//	I=I/59.2;

//	
//	
////	sR=s/100;
//	int u100 = 100;
//	I=I/1000000;

//	R=(u100 - I*1047)/(I);
//	
////	R =(100-sR)*1047/(sR+1.14);

//	Bipol[nCh] = R; //*1.775839; //R*1.775839;


//	printf("Ins R: %u KOhm\n------------------\n", (unsigned int)R);
//	RZ[0] = R;	// Сопротивление изоляции 1
	R = R/16;
		if (R>0xFFFF){R=0xFFFF;}
		arrWord[50 + nCh] = R; // измеренное значение

		
		delta = (uint8_t)(arrWord[nCh]>>8)*0.01; //230717
	
		if (R < (arrWord[nCh +10]-arrWord[nCh +10]*delta))// | (R > (arrWord[nCh +10]+arrWord[nCh +10]*delta)))
		{ 
			arrBool[nCh +20] = 0;	// выход за пределы для канала сопротивления изоляции 1
		}
		else
		{
			arrBool[nCh +20] = 1;
		}
//}
//	else
//	{
//	arrBool[nCh +40] = 1;
//	}
//	ok &= (R >= 40000.);
	
// Сопротивление изолияции 2	
		//led_rgb[adc_current_chan] = 0x1f;

	ADC_set_config(0x0081); //0x1051) Смещение внутреннее ....смещение внешнее REFin2+ REFin2- буфер канал 1 
	HAL_Delay(adc_delay);

//	test_Status = ADC_Status();
		
	ADC_set_mode(0x8009);  // calibrate zero
	HAL_Delay(adc_delay);
	ADC_set_mode(0x800a);  // calibrate full-scale
	HAL_Delay(adc_delay);
	ADC_set_mode(0x0009);  // return to continuous reads
	HAL_Delay(adc_delay);

	//led_rgb[adc_current_chan] = 0x4f;

	ADC_read(1, adc_delay); // buffer flush
	ADC_read(1, adc_delay); // buffer flush
	s = ADC_read(n, adc_delay);
	if (s > _constBipol)
	{s = s - _constBipol;}
	else
	{s = _constBipol - s;}	

	
//	R = 16.*1024.*1024 / s;
//	Bipol[nCh] =  R*1.775839; //R*1.775839;
		R=R*1775.839; //1.775839;
	//led_rgb[adc_current_chan] = 0x1f;
//	R = 20000.0; //RR
	
//	R = 16.*1024.*1024 / s;
//	R *= 2396.;
//	R -= 1047.0;

////	printf("Ins R: %u KOhm\n------------------\n", (unsigned int)R);
//	RZ[1] = R;
//R = R/16;
	if (R>0xFFFF){R=0xFFFF;}
//	if (R>65535){R=65535;}
	arrWord[60 + nCh] = R; // измеренное значение сопротивления изоляции 2
//	if (((arrWord[nCh+40] == 2)||(arrWord[nCh+40] == 6))& calibrate)
//	{
//////	switch (arrWord[nCh]) //   допустимое отклонение от заданного среднего
//////{
//////case	_mode10P:
//////	delta = 0.1;
//////	break;  
//////case	_mode30P:
//////	delta = 0.3;
//////	break;
//////default:
//////	delta = 0.2;
//////	break;
//////}	
	delta = (uint8_t)(arrWord[nCh]>>8)*0.01;// 230717
	if (R < (arrWord[nCh +20]-arrWord[nCh +20]*delta))//| (R > (arrWord[nCh +20]+arrWord[nCh +20]*delta)))
	{ 
		arrBool[nCh +30] = 0;	// выход за пределы для канала сопротивления изоляции 1 ps фиксируется только вниз
	}
	else
	{
		arrBool[nCh +30] = 1;	
	}
//}
//	else
//	{
//		arrBool[nCh +40] = 1;
//	}
//	ok &= (R >= 40000.);
//////////	if (arrBool[nCh + 20] & arrBool[nCh +30] & arrBool[nCh +40]&arrBool[nCh+50])
//////////	{ arrBool[nCh+60]=1; } // allarm
	//*******************************************
////////	//*******************************************
////////	// Проверка наличия 100 В
////////	ADC_set_config(0x1093); //0x1051) смещение Внутреннее - буфер канал 4
////////	HAL_Delay(adc_delay);
////////		
////////	ADC_set_mode(0x8009);  // calibrate zero
////////	HAL_Delay(adc_delay);
////////	ADC_set_mode(0x800a);  // calibrate full-scale
////////		
////////	HAL_Delay(adc_delay);
////////	ADC_set_mode(0x0009);  // return to continuous reads
////////	HAL_Delay(adc_delay);

////////	//led_rgb[adc_current_chan] = 0x4f;

////////	ADC_read(1, adc_delay); // buffer flush
////////	ADC_read(1, adc_delay); // buffer flush
////////	s = ADC_read(n, adc_delay);
////////	HAL_Delay(adc_delay);
	
	// AVDD Monitor
	//*********************************************************************
	//*********************************************************************

		ADC_set_config(0x1097); //0x1051) смещение Внутреннее - буфер канал 4
		HAL_Delay(adc_delay);

//	test_Status = ADC_Status();
		
	ADC_set_mode(0x8009);  // calibrate zero
	HAL_Delay(adc_delay);
	ADC_set_mode(0x800a);  // calibrate full-scale
	HAL_Delay(adc_delay);
	ADC_set_mode(0x0009);  // return to continuous reads
	HAL_Delay(adc_delay);

	//led_rgb[adc_current_chan] = 0x4f;

	ADC_read(1, adc_delay); // buffer flush
	ADC_read(1, adc_delay); // buffer flush
	s = ADC_read(n, adc_delay);
	HAL_Delay(adc_delay);
//////	//led_rgb[adc_current_chan] = ok ? 0x001f00 : 0x3f0000;  // RGB
}
//Диагностика АЦП
uint16_t ADC_Error(uint8_t id)
{
uint8_t out;
	uint16_t ret;
	out = 0;
	ret = 0;
	
	for(int k=0; k<10; k++) //for(int k=0; k<10; k++)
			{
				ADC_CS(k);
				HAL_Delay(200);
				out = ADC_readID();
    if (out != id) 
		{
//		return 1;
			ret = ret | 1<< k;
			 
		}
		HAL_Delay(100);
		
		}
		return ret;
}

//void adcCycle(uint8_t nCh)
//{
//	ADC_CS(nCh);
//	ADC_reset();
//	ADC_measure(nCh,arrWord,arrBool);
//	ADC_CS(-1);
//}	
// **************************************************************


	
_Bool pause(uint16_t delta)
{
static uint16_t current = 0; // счетчик
_Bool out;
	
	
if (current < delta)
{
	current ++;
	out = 0;
}
else
{
delta = 0;
out = 1;

}

return out;

}

//void ADC_measureVolt(uint8_t nCh, uint16_t* aWord, _Bool* aBool) 
//	{
//	extern uint32_t adc_delay;
//	uint32_t n = 16;//16;
//	uint32_t s;
//	uint16_t limit, delta;
//	uint8_t alarm1, alarm2;

// 	
//		// Напряжение утечки 1
//		
//	ADC_set_config(0x0000); //0x1050) смещение внешнее REFin1+ REFin1- буфера нет канал 1
//	HAL_Delay(adc_delay);		
//	ADC_set_mode(0x8009);  // calibrate zero
//	HAL_Delay(adc_delay);			
//	ADC_set_mode(0x800a);  // calibrate full-scale
//	HAL_Delay(adc_delay);		
//	ADC_set_mode(0x0009);  // return to continuous reads
//	HAL_Delay(adc_delay);
//	ADC_read(1, adc_delay); // buffer flush	
//	ADC_read(1, adc_delay); // buffer flush
//	s = ADC_read(n, adc_delay);
//	//++++++++++++++++++++++++++
//	
//	if (s > _Voltconst ){s = s - _Voltconst;}
//	else {
//	s = _Voltconst - s;
//	
//	}
//	
//	s=s>>8;
//	
//	s=(s*100)/298;
//	if (s<=280){s=0;}
//	
//	/*callFlo[nCh]=s;*/
//	
//	aWord[190 +nCh] = s/10;
//	
//		limit  = aWord[200 + nCh];
//		delta = limit * 10;
//		limit = limit *100;
//		if (s > limit)
//		{alarm1 =1;}
//		else if (s <(limit - delta))
//		{alarm1 = 0;}
//	
//  

//	
//		// Напряжение утечки 2	

//	ADC_set_config(0x0001); //0x1051) смещение внешнее REFin1+ REFin1- буфера нет биполярный сигнал 
//	HAL_Delay(adc_delay);
//		
//	ADC_set_mode(0x8009);  // calibrate zero
//	HAL_Delay(adc_delay);
//	ADC_set_mode(0x800a);  // calibrate full-scale
//	HAL_Delay(adc_delay);
//	ADC_set_mode(0x0009);  // return to continuous reads
//	HAL_Delay(adc_delay);
//	ADC_read(1, adc_delay); // buffer flush
//	ADC_read(1, adc_delay); // buffer flush
//	
//	s=0;
//	s = ADC_read(n, adc_delay);
//	
//	if (s > _Voltconst ){s = s - _Voltconst;}
//	else {s = _Voltconst - s;}
////	arrWord[nCh+200] = s;
//	s=s>>8;	
//	s=(s*100)/298;
//	if (s<=280){s=0;}
//	aWord[230 +nCh] = s/10;
//	
//		limit  = aWord[200 + nCh];
//		delta = limit * 10;
//		limit = limit *100;
//		if (s > limit)
//		{alarm2 =1;}
//		else if (s <(limit - delta))
//		{alarm2 = 0;}	

//		aBool[nCh +100] = alarm1 | alarm2;
//		aBool[nCh+110] = !aBool[nCh+100];

//////	arrWord[nCh+200] = s; //>>8;
////	arrWord[nCh +201] = s>>8;
//	



//////////	if (arrBool[nCh + 20] & arrBool[nCh +30] & arrBool[nCh +40]&arrBool[nCh+50])
//////////	{ arrBool[nCh+60]=1; } // allarm


//}
////////	void ADC_measure_plus(uint8_t nCh, uint16_t* aWord, _Bool* aBool) //, uint16_t* arrWordTemp, _Bool* arrBoolTemp ) 
////////	{
////////	extern uint32_t adc_delay;
////////	uint32_t n = 16;//16;
////////	uint32_t s;
////////	float v100; // Временно напряжение 100 вольт
////////	float R = 20000.0;
////////	int ok = 1;
////////	float delta = 0.2;
////////	float kbipol=0;
////////		
////////		// Сопротивление Шлейфа

////////	ADC_set_config(0x1012); // Внешний REFin1+ REFin2+  Ain 3 (+) буферный усилитель униполярный режим
////////	HAL_Delay(adc_delay);

////////	ADC_set_mode(0x8009);  // calibrate zero
////////	HAL_Delay(adc_delay);
////////	ADC_set_mode(0x800a);  // calibrate full-scale
////////	HAL_Delay(adc_delay);
////////	ADC_set_mode(0x0009);  // return to continuous reads
////////	HAL_Delay(adc_delay);


////////	ADC_read(1, adc_delay); // buffer flush
////////	ADC_read(1, adc_delay); // buffer flush
////////	s = ADC_read(n, adc_delay);

////////	R *= s;
////////	callibrateU[nCh] = s;
////////	if (s <=0x00FFFAD7)
////////	{R /= (0x00FFFAD7 - s);}
////////	else 
////////	{R /= (s - 0x00FFFAD7);}	
////////	if (R > 31.0)
////////	{R -= 31.0;}  // TBU effective serial resistance
//////////	printf("Loop R: %.2f Ohm\n------------------\n", R);
//////// RZ[2] = R;	

////////	// Сопротивление Шлейфа
////////if (R>65535){R=65535;}

//////////if((arrWord[70+nCh]<_LoopUpMin)& (R>_LoopUpMax)) // Скачек пока не надо
//////////{ // таким образом фиксируем обрыв кабеля
//////////			{
//////////				arrBool[nCh+50]=0;
//////////			}
//////////}	
//////////////////if (R<_LoopUpMax)
//////////////////			{
//////////////////				arrBool[nCh+50]=1;
//////////////////			}

////////aWord[70 + nCh] = R; // измеренное значение
//////////диапазон 	
////////			delta = ((uint8_t)aWord[nCh])*0.01; //230717
////////		// Сопротивление шлейфа

////////			if (R < (aWord[nCh +30]-aWord[nCh +30]*delta)) // | (R > (arrWord[nCh +30]+arrWord[nCh +30]*delta)))
////////			{ 
////////				aBool[nCh +40] = 0;	// выход за пределы для канала вниз
////////			}
////////			else
////////			{
////////				aBool[nCh +40] = 1;
////////			}

//////////////////	//led_rgb[adc_current_chan] = 0x1f;
////////		//*******************************************
////////	// Проверка наличия 100 В
////////	v100 = 0;
////////			
////////	ADC_set_config(0x0003); //0x1051) смещение Внутреннее - буфер канал 4
////////	HAL_Delay(adc_delay);
////////		
////////	ADC_set_mode(0x8009);  // calibrate zero
////////	HAL_Delay(adc_delay);
////////	ADC_set_mode(0x800a);  // calibrate full-scale
////////		
////////	HAL_Delay(adc_delay);
////////	ADC_set_mode(0x0009);  // return to continuous reads
////////	HAL_Delay(adc_delay);

////////	//led_rgb[adc_current_chan] = 0x4f;

////////	ADC_read(1, adc_delay); // buffer flush
////////	ADC_read(1, adc_delay); // buffer flush
////////	v100 = ADC_read(n, adc_delay); // Измерение 100 В.
////////	HAL_Delay(adc_delay);
////////	if (s <=  _constBipol)
////////	{
////////		s = _constBipol -s ;
////////	}
////////	else
////////	{
////////	 s = 0;
////////	}
////////	v100 = s/32897;
////////	
////////	
////////	if (v100 < _v100)
////////	{
////////		aWord[140] 	= aWord[140] | 0x4;
////////		aWord[222 ] = aWord[222] | 1 << nCh;
////////	}
////////	else
////////	{
////////		aWord[140] &=  0xFB;
////////		aWord[222] &= ~ (1<<nCh);
////////	}
////////	
////////// сопротивление изоляции 1
////////	
////////	ADC_set_config(0x0000);//(0x0080); 	//	(0x1050); //0x1050) смещение внешнее REFin2+ REFin2- буфер канал 1 
////////	HAL_Delay(adc_delay);		
////////	ADC_set_mode(0x8009);  // calibrate zero
////////	HAL_Delay(adc_delay);			
////////	ADC_set_mode(0x800a);  // calibrate full-scale
////////	HAL_Delay(adc_delay);
////////			

////////			
////////	ADC_set_mode(0x0009);  // return to continuous reads
////////	HAL_Delay(adc_delay);
////////			
//////////	HAL_Delay(adc_delay);
////////	ADC_read(1, adc_delay); // buffer flush	
////////	ADC_read(1, adc_delay); // buffer flush
////////	s = ADC_read(n, adc_delay);
////////	if (s > _constBipol	){s = 0;} //s - _Voltconst;}
////////	else {s = _constBipol - s;}	

////////	R=s;

////////	
//////////	R = 2396*16.*1024.*1024 / s;
//////////	R= R/1.2025;
////////	
////////	R=458*16.*1024.*1024 / s;  //470*16.*1024.*1024 / s;


////////	
//////////	R *= 2396.;
////////	R -= 1047.0;
////////	RZ[1] = R;
//////////	R -= 0937.93286;


//////////	printf("Ins R: %u KOhm\n------------------\n", (unsigned int)R);
//////////	RZ[0] = R;	// Сопротивление изоляции 1
////////	R = R/16;
////////		if (R>0xFFFF){R=0xFFFF;}
////////		float RzM= 0;
////////		float RzP =0;
////////		float Ux = 0;
////////		float Rp = 0;
////////		float Rm = 0;
////////		
////////		RzP = R;
////////		
////////		RzM = aWord[170+nCh];
////////		
////////		if (RzM>=RzP)
////////		{
////////			kbipol = RzM/RzP;
////////		}
////////		else
////////		{
////////			kbipol = RzP/RzM;;
////////		}
////////		
////////		Ux = 100*(kbipol-1)/(kbipol+1);
////////		
////////		Rm = (100 -Ux)*RzM/100;
////////				
////////		aWord[150 + nCh] = R; // измеренное значение
////////		
////////		aWord[50+nCh]=Rm;
//////////		
//////////		arrWord[50+nCh] = (arrWord[150+nCh]+arrWord[170+nCh])/2;
//////////		
//////////		R=arrWord[50+nCh];
////////		
//////////		kbipol = arrWord[170+nCh]
////////		
////////		delta = (uint8_t)(aWord[nCh]>>8)*0.01; //230717
////////	
////////		if (Rm < (aWord[nCh +10]-aWord[nCh +10]*delta))// | (R > (arrWord[nCh +10]+arrWord[nCh +10]*delta)))
////////		{ 
////////			aBool[nCh +20] = 0;	// выход за пределы для канала сопротивления изоляции 1
////////		}
////////		else
////////		{
////////			aBool[nCh +20] = 1;
////////		}
////////	
////////// Сопротивление изолияции 2	
////////		//led_rgb[adc_current_chan] = 0x1f;

////////	ADC_set_config(0x0001); //(0x0081); //0x1051) Смещение внутреннее ....смещение внешнее REFin2+ REFin2- буфер канал 1 
////////	HAL_Delay(adc_delay);

//////////	test_Status = ADC_Status();
////////		
////////	ADC_set_mode(0x8009);  // calibrate zero
////////	HAL_Delay(adc_delay);
////////	ADC_set_mode(0x800a);  // calibrate full-scale
////////	HAL_Delay(adc_delay);
////////	ADC_set_mode(0x0009);  // return to continuous reads
////////	HAL_Delay(adc_delay);

////////	//led_rgb[adc_current_chan] = 0x4f;

////////	ADC_read(1, adc_delay); // buffer flush
////////	ADC_read(1, adc_delay); // buffer flush
////////	s = ADC_read(n, adc_delay);
////////	
////////	if (s > _constBipol	){s = 0;} //s - _Voltconst;}
////////	else {s = _constBipol - s;}	

////////	R=s;


////////	
//////////	R = 2396*16.*1024.*1024 / s;
//////////	R= R/1.2025;
////////	
////////	R=458 *16.*1024.*1024 / s;			//470*16.*1024.*1024 / s;


////////	
//////////	R *= 2396.;
////////	R -= 1047.0;
////////	RZ[1] = R;
//////////	R -= 0937.93286;


//////////	printf("Ins R: %u KOhm\n------------------\n", (unsigned int)R);
//////////	RZ[0] = R;	// Сопротивление изоляции 1
////////	R = R/16;
////////	if (R>0xFFFF){R=0xFFFF;}		
////////		RzP = R;
////////		aWord[160 + nCh] = R; 
////////		RzM = aWord[180+nCh];
////////		if (RzM>=RzP)
////////		{
////////			kbipol = RzM/RzP;
////////		}
////////		else
////////		{
////////			kbipol = RzP/RzM;;
////////		}
////////		
////////		Ux = 100*(kbipol-1)/(kbipol+1);
////////		Rm = (100 -Ux)*RzM/100;
////////		
////////		aWord[60+nCh]=Rm;
////////		
////////		// измеренное значение
////////	
//////////	arrWord[60+nCh] = (arrWord[160+nCh]+arrWord[180+nCh])/2;
//////////	R=arrWord[60+nCh];

//////////	arrWord[60 + nCh] = R; // измеренное значение сопротивления изоляции 2	
////////	
////////	delta = (uint8_t)(aWord[nCh]>>8)*0.01;// 230717
////////	if (Rm < (aWord[nCh +20]-aWord[nCh +20]*delta))//| (R > (arrWord[nCh +20]+arrWord[nCh +20]*delta)))
////////	{ 
////////		aBool[nCh +30] = 0;	// выход за пределы для канала сопротивления изоляции 1 ps фиксируется только вниз
////////	}
////////	else
////////	{
////////		aBool[nCh +30] = 1;	
////////	}

//////////	ok &= (R >= 40000.);
//////////////////	if (arrBool[nCh + 20] & arrBool[nCh +30] & arrBool[nCh +40]&arrBool[nCh+50])
//////////////////	{ arrBool[nCh+60]=1; } // allarm

////////}
////	void ADC_measure_minus(uint8_t nCh, uint16_t* aWord, _Bool* aBool) 
////	{
////	extern uint32_t adc_delay;
////	uint32_t n = 16;//16;
////	uint32_t s;
////	float v100; // Временно напряжение 100 вольт
////	float R = 20000.0;
////	int ok = 1;
////	float delta = 0.2;
////		
////		// Сопротивление Шлейфа

//////	ADC_set_config(0x1012); // Внешний REFin1+ REFin2+  Ain 3 (+) буферный усилитель униполярный режим
//////	HAL_Delay(adc_delay);

//////	ADC_set_mode(0x8009);  // calibrate zero
//////	HAL_Delay(adc_delay);
//////	ADC_set_mode(0x800a);  // calibrate full-scale
//////	HAL_Delay(adc_delay);
//////	ADC_set_mode(0x0009);  // return to continuous reads
//////	HAL_Delay(adc_delay);


//////	ADC_read(1, adc_delay); // buffer flush
//////	ADC_read(1, adc_delay); // buffer flush
//////	s = ADC_read(n, adc_delay);

//////	R *= s;
//////	if (s <=0x00FFFAD7)
//////	{R /= (0x00FFFAD7 - s);}
//////	else 
//////	{R /= (s - 0x00FFFAD7);}	
//////	if (R > 31.0)
//////	{R -= 31.0;}  // TBU effective serial resistance
////////	printf("Loop R: %.2f Ohm\n------------------\n", R);
////// RZ[2] = R;	

//////	// Сопротивление Шлейфа
//////if (R>65535){R=65535;}

////////if((arrWord[70+nCh]<_LoopUpMin)& (R>_LoopUpMax)) // Скачек пока не надо
////////{ // таким образом фиксируем обрыв кабеля
////////			{
////////				arrBool[nCh+50]=0;
////////			}
////////}	
////////if (R<_LoopUpMax)
////////			{
////////				arrBool[nCh+50]=1;
////////			}

//////arrWord[70 + nCh] = R; // измеренное значение
////////диапазон 	
//////			delta = ((uint8_t)arrWord[nCh])*0.01; //230717
//////		// Сопротивление шлейфа

//////			if (R < (arrWord[nCh +30]-arrWord[nCh +30]*delta)) // | (R > (arrWord[nCh +30]+arrWord[nCh +30]*delta)))
//////			{ 
//////				arrBool[nCh +40] = 0;	// выход за пределы для канала вниз
//////			}
//////			else
//////			{
//////				arrBool[nCh +40] = 1;
//////			}

//////////////	//led_rgb[adc_current_chan] = 0x1f;
////		//*******************************************
////	// Проверка наличия 100 В
////	v100 = 0;
////			
////	ADC_set_config(0x0003);  // биполярный смещение от IN1 без буфера ....(0x1093); //0x1051) смещение Внутреннее - буфер канал 4
////	HAL_Delay(adc_delay);
////		
////	ADC_set_mode(0x8009);  // calibrate zero
////	HAL_Delay(adc_delay);
////	ADC_set_mode(0x800a);  // calibrate full-scale
////		
////	HAL_Delay(adc_delay);
////	ADC_set_mode(0x0009);  // return to continuous reads
////	HAL_Delay(adc_delay);

////	//led_rgb[adc_current_chan] = 0x4f;

////	ADC_read(1, adc_delay); // buffer flush
////	ADC_read(1, adc_delay); // buffer flush
////	
////	s = ADC_read(n, adc_delay); // Измерение 100 В.
////	
////	if (s >=  _constBipol)
////	{
////		s = s -_constBipol;
////	}
////	else
////	{
////	 s = 0;
////	}
////	v100 = s/32897;
////	

////	HAL_Delay(adc_delay);
////	
////	if (v100 < _v100)
////	{
////		aWord[140] 	= aWord[140] | 0x2;
////		aWord[221 ] = aWord[221] | 1 << nCh;
////	}
////	else
////	{
////		aWord[140] &=  0xFD;
////		aWord[221] &= ~ (1<<nCh);
////	}
////	
////// сопротивление изоляции 1
////	
////	ADC_set_config(0x0000); //(0x0080); 	//	(0x1050); //0x1050) смещение внешнее REFin2+ REFin2- буфер канал 1 
////	HAL_Delay(adc_delay);		
////	ADC_set_mode(0x8009);  // calibrate zero
////	HAL_Delay(adc_delay);			
////	ADC_set_mode(0x800a);  // calibrate full-scale
////	HAL_Delay(adc_delay);
////			

////			
////	ADC_set_mode(0x0009);  // return to continuous reads
////	HAL_Delay(adc_delay);
////			
//////	HAL_Delay(adc_delay);
////	ADC_read(1, adc_delay); // buffer flush	
////	ADC_read(1, adc_delay); // buffer flush
////	s = ADC_read(n, adc_delay);
////	if (s < _constBipol_minus	){s = 0;} //s - _Voltconst;}
////	else {s = s -_constBipol_minus;}	
////	
////	
////	R=s;

////	
////	
//////	R = 2396*16.*1024.*1024 / s;
//////	R= R/1.2625;

////	R= 481*16.*1024.*1024 / s; //470*16.*1024.*1024 / s;
////	
////	
//////	R *= 2396.;
////	R -= 1047.0;
////	RZ[1] = R;
//////	R -= 0937.93286;


//////	printf("Ins R: %u KOhm\n------------------\n", (unsigned int)R);
//////	RZ[0] = R;	// Сопротивление изоляции 1
////	R = R/16;
////		if (R>0xFFFF){R=0xFFFF;}
////		aWord[170 + nCh] = R; // измеренное значение
////		
//////		delta = (uint8_t)(arrWord[nCh]>>8)*0.01; //230717
//////	
//////		if (R < (arrWord[nCh +10]-arrWord[nCh +10]*delta))// | (R > (arrWord[nCh +10]+arrWord[nCh +10]*delta)))
//////		{ 
//////			arrBool[nCh +20] = 0;	// выход за пределы для канала сопротивления изоляции 1
//////		}
//////		else
//////		{
//////			arrBool[nCh +20] = 1;
//////		}
////	
////// Сопротивление изолияции 2	
////		//led_rgb[adc_current_chan] = 0x1f;

////	ADC_set_config (0x0001); //(0x0081); //0x1051) Смещение внутреннее ....смещение внешнее REFin2+ REFin2- буфер канал 1 
////	HAL_Delay(adc_delay);

//////	test_Status = ADC_Status();
////		
////	ADC_set_mode(0x8009);  // calibrate zero
////	HAL_Delay(adc_delay);
////	ADC_set_mode(0x800a);  // calibrate full-scale
////	HAL_Delay(adc_delay);
////	ADC_set_mode(0x0009);  // return to continuous reads
////	HAL_Delay(adc_delay);

////	//led_rgb[adc_current_chan] = 0x4f;

////	ADC_read(1, adc_delay); // buffer flush
////	ADC_read(1, adc_delay); // buffer flush
////	s = ADC_read(n, adc_delay);

////	if (s < _constBipol_minus	){s = 0;} //s - _Voltconst;}
////	else {s = s -_constBipol_minus;}	

////	
////	R=s;

////	
////	
//////	R = 2396*16.*1024.*1024 / s;
//////	R= R/1.2625;
////	
////	R= 481*16.*1024.*1024 / s;	//470*16.*1024.*1024 / s;

////	
//////	callFlo[nCh] = R;
//////	
//////	R *= 2396.;
////	R -= 1047.0;
////	RZ[1] = R;
//////	R -= 0937.93286;


//////	printf("Ins R: %u KOhm\n------------------\n", (unsigned int)R);
//////	RZ[0] = R;	// Сопротивление изоляции 2
////	R = R/16;
////	if (R>0xFFFF){R=0xFFFF;}

////	aWord[180 + nCh] = R; // измеренное значение сопротивления изоляции 2	
//////	delta = (uint8_t)(arrWord[nCh]>>8)*0.01;// 230717
//////	if (R < (arrWord[nCh +20]-arrWord[nCh +20]*delta))//| (R > (arrWord[nCh +20]+arrWord[nCh +20]*delta)))
//////	{ 
//////		arrBool[nCh +30] = 0;	// выход за пределы для канала сопротивления изоляции 1 ps фиксируется только вниз
//////	}
//////	else
//////	{
//////		arrBool[nCh +30] = 1;	
//////	}

//////	ok &= (R >= 40000.);
////////////	if (arrBool[nCh + 20] & arrBool[nCh +30] & arrBool[nCh +40]&arrBool[nCh+50])
////////////	{ arrBool[nCh+60]=1; } // allarm

////}