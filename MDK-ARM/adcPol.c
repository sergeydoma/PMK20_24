#include "main.h"//
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern SPI_HandleTypeDef hspi1;

extern uint32_t  led_rgb[10];
extern float RZ[3];
extern GPIO switch_gpio[10]; 
extern _Bool arrBool[0x400];  
extern	uint16_t arrWord[0x400];
extern uint32_t constBipol;
extern float Bipol[10];
extern float sR;

extern uint8_t switch_state[10]; 
extern int adc_current_chan; 
extern uint8_t test_Status;
extern uint32_t callibrateU[10];
extern float callFlo[10];
extern float callR[10];
float k; // временный коэффициент для расчета напряжения 
float riz_m1[10];
float riz_m2[10];
float setRz[10];
setPointLoop[10];



void ADC_measure_minus(uint8_t nCh, uint16_t* aWord, _Bool* aBool) 
	{
	extern uint32_t adc_delay;
	uint32_t n = 16;//16;
	uint32_t s;
	float v100; // Временно напряжение 100 вольт
	float R = 20000.0;
	int ok = 1;
//	float delta[nCh] = 0.2;



		//*******************************************
	// Проверка наличия 100 В
	v100 = 0;
			
	ADC_set_config(0x0013);  // биполярный смещение от IN1 без буфера ....(0x1093); //0x1051) смещение Внутреннее - буфер канал 4
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
	
	s = ADC_read(n, adc_delay); // Измерение 100 В.
	
	// ADC_measure_minus
	if (s >=  _constBipol) //_constBipol)
	{
		s = s - _constBipol; //_constBipol;	
	}
	else
	{
	 s = 0; // s - _constBipol ;
	}

	
//	v100 = ((_Rinp/_Rmeas)*(_Uop/1024*1024*8)*s);
	
	v100 = s; //33567;
	
	
	//k = (1020/20)*(_Uop/(8.*1024.1024))
	k = 20 * 8.*1024.*1024; //(1020 * _Uop);
	
	k /= 1020* _Uop;
	
	v100 /= k; //34267; //33567;
	
	HAL_Delay(adc_delay);
	
	
	v100 = v100 + _Uop/2;
	
	
	
	if (v100 < _v100)
	{
		aWord[140] 	= aWord[140] | 0x2;
		aWord[221 ] = aWord[221] | 1 << nCh;
	}
	else
	{
		aWord[140] &=  0xFD;
		aWord[221] &= ~ (1<<nCh);
	}
	
// сопротивление изоляции 1 минус
	
	ADC_set_config(0x0010); //(0x0080); 	//	(0x1050); //0x1050) смещение внешнее REFin2+ REFin2- буфер канал 1 
	HAL_Delay(adc_delay);		
	ADC_set_mode(0x8009);  // calibrate zero
	HAL_Delay(adc_delay);			
	ADC_set_mode(0x800a);  // calibrate full-scale
	HAL_Delay(adc_delay);
	
	ADC_set_mode(0x0009);  // return to continuous reads
	HAL_Delay(adc_delay);
			
//	HAL_Delay(adc_delay);
	ADC_read(1, adc_delay); // buffer flush	
	ADC_read(1, adc_delay); // buffer flush
	s = ADC_read(n, adc_delay);
	if (s < _constBipol	){s = 0;} //s - _Voltconst;}
	else {s = s -_constBipol;}	
		
	R = (100 - _Uop/2) *_Rmeas/_Uop;		
	R =  R*8.*1024.*1024/s;	
	R -= 2447.0;
	R += _korr; // коррекция
//	RZ[0] = R;	// Сопротивление изоляции 1 минус
	R = R/16;	
		if (R>0xFFFF){R=0xFFFF;}
		riz_m1[nCh] = R;
		
		aWord[170 + nCh] = R; // измеренное значение
	
// Сопротивление изолияции 2	минус
		//led_rgb[adc_current_chan] = 0x1f;

	ADC_set_config (0x0011); //(0x0081); //0x1051) Смещение внутреннее ....смещение внешнее REFin2+ REFin2- буфер канал 1 
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

	if (s < _constBipol)
		{
			s = 0;
		} //s - _Voltconst;}
	else 
		{
			s = s -_constBipol;
		}	
//	R=s;

	R = (100 - _Uop/2) *_Rmeas/_Uop;	
	R =  R*8.*1024.*1024/s;	
	R -= 2447.0;
	R += _korr; // коррекция	
	R = R/16;
	if (R>0xFFFF){R=0xFFFF;}
	riz_m2[nCh] = R;
	aWord[180 + nCh] = R; // измеренное значение сопротивления изоляции 2	


}
	
	void ADC_measure_plus(uint8_t nCh, uint16_t* aWord, _Bool* aBool) //, uint16_t* arrWordTemp, _Bool* arrBoolTemp ) 
	{
	extern uint32_t adc_delay;
	uint32_t n = 16;//16;
	uint32_t s;
	float v100; // Временно напряжение 100 вольт
	float R = 20000.0;
	int ok = 1;
	float delta[10]; // = [0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2];
	float kbipol=0;
		
		// Сопротивление Шлейфа

	ADC_set_config(0x1012); // Внешний REFin1+ REFin2+  Ain 3 (+) буферный усилитель униполярный режим
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
	
	
	R *= s;
	
	if (s <=0x00FFFAD7)
	{R /= (0x00FFFAD7 - s);}
	else 
	{R /= (s - 0x00FFFAD7);}	
	if (R > 31.0)
	{R -= 31.0;}  // TBU effective serial resistance
//	printf("Loop R: %.2f Ohm\n------------------\n", R);
 RZ[2] = R;	

	// Сопротивление Шлейфа
if (R>65535){R=65535;}

//if((aWord[70+nCh]<_LoopUpMin)& (R>_LoopUpMax)) // Скачек пока не надо
//{ // таким образом фиксируем обрыв кабеля
//			{
//				arrBool[nCh+50]=0;
//			}
//}	
//if (R<_LoopUpMax)
//			{
//				arrBool[nCh+50]=1;
//			}

		aWord[70 + nCh] = R; // измеренное значение
			
		//диапазон 	
			setPointLoop[nCh] = arrWord[nCh]>>8;
			delta[nCh] = ((uint8_t)setPointLoop[nCh])*0.01; //230717
		// Сопротивление шлейфа
			setRz[nCh] = arrWord[nCh+30];

			if (R < (setRz[nCh]-setRz[nCh]*delta[nCh])) // | (R > (arrWord[nCh +30]+arrWord[nCh +30]*delta)))
				{ 
					aBool[nCh +40] = 0;	// выход за пределы для канала вниз
				}
			else
				{
					aBool[nCh +40] = 1;
				}
				
		
				
			if (R > (setRz[nCh]+setRz[nCh]*delta[nCh])) // | (R > (arrWord[nCh +30]+arrWord[nCh +30]*delta)))
				{ 
					aBool[nCh +50] = 0;	// выход за пределы для канала вверх
				}
			else
				{
					aBool[nCh +50] = 1;
				}
			
			
			
																		/***** Предупреждение *****/
		// вниз
		delta[nCh] = (uint8_t)(arrWord[250 + nCh])*0.01;
		
		setRz[nCh] = arrWord[30+nCh];
		
		if (R < (setRz[nCh]-setRz[nCh]*delta[nCh]))
		{ 
			aBool[nCh +130] = 0;	// выход за пределы для канала сопротивления шлейфа предупредительно
		}
		else
		{
			aBool[nCh +130] = 1;
		}
		
		// вверх шлейфа

		
		if (R > (setRz[nCh]+setRz[nCh]*delta[nCh]))
		{ 
			aBool[nCh +160] = 0;	// выход за пределы для канала сопротивления шлейфа предупредительно
		}
		else
		{
			aBool[nCh +160] = 1;
		}


//////////	//led_rgb[adc_current_chan] = 0x1f;
		//*******************************************
	// Проверка наличия 100 В плюс
	v100 = 0;
			
	ADC_set_config(0x0013); //0x1051) смещение Внутреннее - буфер канал 4
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
	s = ADC_read(n, adc_delay); // Измерение 100 В.
	HAL_Delay(adc_delay);
	// measure_plus
	
	
	
	if (s <= _constBipol)
	{
		s = _constBipol - s;	
	}
	else
	{
	 s = 0;
	}
	
	v100 = s;
		
	k = 20 * 8.*1024.*1024; //(1020 * _Uop);
	
	k /= 1020* _Uop;
	
	v100 /= k; //34267; //33567;
	
	HAL_Delay(adc_delay);
	
	v100 = v100 -_Uop/2;
	
	
	if (v100 < _v100)
	{
		aWord[140] 	= aWord[140] | 0x4;
		aWord[222 ] = aWord[222] | 1 << nCh;
	}
	else
	{
		aWord[140] &=  0xFB;
		aWord[222] &= ~ (1<<nCh);
	}

	
// сопротивление изоляции 1 плюс
	
	ADC_set_config(0x0010);//(0x0080); 	//	(0x1050); //0x1050) смещение внешнее REFin2+ REFin2- буфер канал 1 
	HAL_Delay(adc_delay);		
	ADC_set_mode(0x8009);  // calibrate zero
	HAL_Delay(adc_delay);			
	ADC_set_mode(0x800a);  // calibrate full-scale
	HAL_Delay(adc_delay);
			

			
	ADC_set_mode(0x0009);  // return to continuous reads
	HAL_Delay(adc_delay);
			
//	HAL_Delay(adc_delay);
	ADC_read(1, adc_delay); // buffer flush	
	ADC_read(1, adc_delay); // buffer flush
	s = ADC_read(n, adc_delay);
	callibrateU[nCh] = s;
	if (s > _constBipol	){s = 0;} //s - _Voltconst;}
	else {s = _constBipol - s;}	

	R = (100 + _Uop/2) *_Rmeas/_Uop;
	R =  R*8.*1024.*1024/s;
	R -= 2447.0;
	R += _korr; // коррекция
	R = R/16;		
	if (R>0xFFFF){R=0xFFFF;}
		float RzM= 0;
		float RzP =0;
		uint16_t Rcr;
		
		RzP = R;
		aWord[150+nCh] = R;		
		RzM =riz_m1[nCh] ;//aWord[170+nCh];		
		
		Rcr = formula(RzM,RzP);
			
		aWord[50+nCh]= Rcr;
//		
//		arrWord[50+nCh] = (arrWord[150+nCh]+arrWord[170+nCh])/2;
//		
//		R=arrWord[50+nCh];
		
//		kbipol = arrWord[170+nCh]
																/***** Авария *****/		
		delta[nCh] = (uint8_t)(arrWord[nCh]>>8)*0.01; //230717
		
		setRz[nCh] = arrWord[nCh+10];
		
		
	
		if (Rcr < (setRz[nCh]-setRz[nCh]*delta[nCh]))// | (R > (arrWord[nCh +10]+arrWord[nCh +10]*delta[nCh])))
		{ 
			aBool[nCh +20] = 0;	// выход за пределы для канала сопротивления изоляции 1
		}
		else
		{
			aBool[nCh +20] = 1;
		}

																/***** Предупреждение *****/
		delta[nCh] = (uint8_t)(arrWord[250 + nCh]>>8)*0.01;
		setRz[nCh] = arrWord[nCh+10];
	
		if (Rcr < (setRz[nCh]-setRz[nCh]*delta[nCh]))
		{ 
			aBool[nCh +110] = 0;	// выход за пределы для канала сопротивления изоляции 1 предупредительно
		}
		else
		{
			aBool[nCh +110] = 1;
		}
		
// Сопротивление изолияции 2 plus	
		//led_rgb[adc_current_chan] = 0x1f;

	ADC_set_config(0x0011); //(0x0081); //0x1051) Смещение внутреннее ....смещение внешнее REFin2+ REFin2- буфер канал 1 
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
	
	if (s > _constBipol	){s = 0;} //s - _Voltconst;}
	else {s = _constBipol - s;}	
	R=s;	
	R = (100 + _Uop/2) *_Rmeas/_Uop;
	R =  R*8.*1024.*1024/s;
	R -= 2447.0;
	R += _korr; // коррекция
	R = R/16;
	if (R>0xFFFF){R=0xFFFF;}		
		RzP = R;
		aWord[160 + nCh] = R; 
		RzM =riz_m2[nCh] ;//aWord[180+nCh];
		Rcr = formula(RzM, RzP);
		
		aWord[60+nCh]=Rcr;
		
		// измеренное значение Сопротивление изолияции 2
	
	
		delta[nCh] = (uint8_t)(arrWord[nCh]>>8)*0.01;// 230717
		
		setRz[nCh] = arrWord[nCh+20];
	
		callFlo[nCh]=(setRz[nCh]-setRz[nCh]*delta[nCh]);
		callR[nCh]= Rcr;
		
		if (Rcr < (setRz[nCh]-setRz[nCh]*delta[nCh]))//| (R > (arrWord[nCh +20]+arrWord[nCh +20]*delta[nCh])))
	{ 
		aBool[nCh +30] = 0;	// выход за пределы для канала сопротивления изоляции 1 ps фиксируется только вниз
	}
	else
	{
		aBool[nCh +30] = 1;	
	}

																/***** Предупреждение *****/
	
		delta[nCh] = (uint8_t)(arrWord[250 + nCh]>>8)*0.01;
		
		setRz[nCh] = arrWord[nCh+20];
	
		if (Rcr < (setRz[nCh]-setRz[nCh]*delta[nCh]))
		{ 
			aBool[nCh +120] = 0;	// выход за пределы для канала сопротивления изоляции 1 предупредительно
		}
		else
		{
			aBool[nCh +120] = 1;
		}


}
	uint16_t formula(float rdm, float rdp)
	{
		float uv = 0;
		float k = 1;
		float upmk = 100.0; // Наряжение смещения
		float rcm = 0;
		float rcp = 0;
		float rc = 0;		
		uint16_t out;
		
//		rcm = rcm*16;
//		rcp = rcp*16;
		
		if (rdp>rdm)
		{
			k = (rdp + 2447/16)/(rdm +2447/16);	
			uv = -(upmk*(k-1))/(k+1);
		}
		else
		{
			k = (rdm + 2447/16)/(rdp +2447/16);	
			uv = (upmk*(k-1))/(k+1);
		}
		rcm = (((upmk - uv)/upmk)*(rdm + 2447/16)) - 2447/16;
		rcp = (((upmk + uv)/upmk)*(rdp + 2447/16)) - 2447/16;
		rc = (rcp+rcm)/2;
		
//		rc = rc/16;
		out = (uint16_t) rc;
		return out;
	
	}		