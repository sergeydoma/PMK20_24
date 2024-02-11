#include "main.h"
#include "cmsis_os.h"
static _Bool button[10];
extern uint16_t currentTime;
extern _chlMode cTime[10]; 
extern uint8_t chMode[10];
uint8_t tempPush; 
//_Led_Blinck ledBlinck;
extern led_mon leds[10];
extern GPIO switch_gpio[10];
extern uint32_t  led_rgb[10];
extern uint16_t arrWord[0x400];
// не нажата ни одна кнопка
//extern uint8_t mode;// test
extern int currentPB;

 _Bool All_buttons_NoPush()
{
  _Bool Out;
  Out = 0;	
  for(int i=0;i <10; i++)
  {
		Out = HAL_GPIO_ReadPin(switch_gpio[i].port, switch_gpio[i].pin) == 0;	// out = 1 если кнопка не нажата	
    if (Out == 0){return 0;}	// if (Out == 1){return 0;}
  }
 return 1;  									// return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++


_Bool DeltaBool(uint16_t delay) // Проверка не нажата хотя бы одна кнопка в течении заданного времени
{
  static _Bool Numb;
  static uint16_t curr = 0;
  
		Numb = All_buttons_NoPush();
    curr ++;   //currentTime++;
		if (Numb == 0)
		{
			curr = 0;
		}			
    if (curr >= delay)
    {   
			curr = 0;
			return 1; // на протяжении времени кнопка не была нажата
		}          
	
	return 0;
} 

// Вариант 2
uint8_t timePush (uint16_t delayPush, uint8_t numChannel)
{
		static _Bool begin[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		static int current[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};		 
		_Bool button;
		uint8_t out = 0;
	
		button =  HAL_GPIO_ReadPin(switch_gpio[numChannel].port, switch_gpio[numChannel].pin) == 0; // Состояние кнопки 1 если не нажата
	if (button) 
	{
		begin[numChannel] =1;
	}
	if ((button == 0)&(begin[numChannel]))
	{ current[numChannel]++;
		if (current[numChannel] >= delayPush)
		{			
			out = 2;
			current[numChannel] = 0;
			begin[numChannel] = 0;
			
		}		
	}
	else if ((button == 1)&(begin[numChannel] ==1))
	{
		if (current[numChannel] <= (delayPush>>4))
		{
			out = 0;
			current[numChannel] = 0;
			
		}
		else if (current[numChannel] <= delayPush )
		{
			out = 1;
			current[numChannel] = 0;
			
		}
	}
	return out;
}




_Bool timeNoPush (uint16_t delay, uint8_t numChannel) // 
  {
		static int current = 0;
		_Bool button;
    if (current < delay)
    {
      current ++;
			button =  HAL_GPIO_ReadPin(switch_gpio[numChannel].port, switch_gpio[numChannel].pin) == 0;
      if(button == 0){current = 0; return 0;}         
    }
    else
    {
			current = 0; return 1; // кнопка в течении заданного времени не нажималась
    }
   return 0;  //иначе ругается
}
  
  
  
  
              
  //*****************************************************************************
uint8_t ModeCH (uint8_t nCh, _Bool* Alarm,  uint16_t* wordSet, uint8_t adc_current) // wordSet массив типа arrWord
  {

		uint8_t out;
		static uint16_t mode[400];
//		wordSet[40+nCh] = mode[nCh];
		
		
		switch(wordSet[40+nCh])
		{

		//******************************************************************	0	
		case 0: // Режим работы канала 
			
				Alarm[nCh+50] = 1; 
						
				tempPush = timePush (4000, nCh);    // Селектор длительности нажатия

				switch (tempPush) // режим пуска
					{
						case 0:
						wordSet[40+nCh] = 0; // monitor off
						break;          
						case 1:
							wordSet[40+nCh] = 1; // redi monitor короткое нажатие
						break;
						case 2:
						wordSet[40+nCh] = 3; // select 1  длинное нажатие
						break;
					}               
					led_rgb[nCh] = _Blue; // Синий мониторинг выключен желтый //Nblinck(2, 0x000f0000, 500);                                          
			//				out = 0;
		break;
		case 1:
		//******************************************************************** 1             
				Alarm[nCh+50] = 1; 
        tempPush = timePush (4000, nCh);    

        switch (tempPush)
        {
						case 0:
						wordSet[40+nCh] = 1; // monitor off
						break;          
						case 1:
						wordSet[40+nCh] = 2; // redi monitor
						break;
						case 2:
						wordSet[40+nCh] = 1; // select 1
						break;
					}

					led_rgb[nCh] = Nblinck( wordSet[nCh], _Green, 500, nCh); // 
        
//					out = 0;
      break;
			case 2:		
			//******************************************************************** 2            
					tempPush = timePush (4000, nCh); 
					
					switch (tempPush)
					{
						case 0:
						wordSet[40+nCh] = 2; // monitor off
						break;          
						case 1:
						wordSet[40+nCh] = 2; // redi monitor
	//					Alarm[nCh+50] = 1; // ручное квитирование обрыва кабеля
						break;
						case 2:
						wordSet[40+nCh] = 0; // select 1
	//					Alarm[nCh+50] = 1; // ручное квитирование обрыва кабеля
						break;
					}
					if ((Alarm[140+nCh] & Alarm[150+nCh]) ==0)
						{													 
							wordSet[40+nCh]= 8; // АВАРИЯ ПО НАПРЯЖЕНИЮ					
						}
					else if (	(Alarm[nCh+20] & Alarm[nCh+30] & Alarm[nCh+40] & Alarm[50+nCh])==0)		
						{      
									wordSet[40+nCh] = 7; // Авария

										led_rgb[nCh] =  _Red; 
						}	
					else if (((Alarm[110 + nCh]& Alarm[120 + nCh] & Alarm [130 + nCh])==0)& 
						(wordSet[40+nCh] != 61)
							)
						{
							wordSet[40+nCh] = 6; // Предупреждение
							led_rgb[nCh] = _Yellow;
						}
					
					else	
					{
					led_rgb[nCh] = _Green; //Nblinck( wordSet[nCh], _Green, 500); // 

					}
	 
			break;
				
			case 3:
//******************************************************************** 3       
        tempPush = timePush (4000, nCh);    

        switch (tempPush)
        {
          case 0:
          wordSet[40+nCh] = 3; //set mode 1
          break;          
          case 1:
          wordSet[40+nCh] = 4; // redi monitor  FLASH
         
          break;
          case 2: 											// Monitoring Grn
						wordSet[nCh] = _mode10P; // уходим
						wordSet[250+nCh] = _mode10P>>1;
					
						if ((wordSet[nCh]) == _mode10P)
						{
							wordSet[nCh] = _mode10P | (_mode10P<<8);
							wordSet[250+nCh] = (_mode10P>>1) | (_mode10P<<7);
						}
						wordSet[40+nCh] = 2; // Переходим на мониторинг с уставкой 10%					
						
          break;
        }  
				
					led_rgb[nCh] = Nblinck( (_mode10P|_mode10P<<8) , _Blue, 500, nCh); // _Yellow 
        
       break;
					
			case 4:
//********************************************************************  4      
        tempPush = timePush (4000, nCh);    

        switch (tempPush)
        {
          case 0:
          wordSet[40+nCh] = 4; // monitor off
          break;          
          case 1:
          wordSet[40+nCh] = 5; // redi monitor
          break;
          case 2:
          wordSet[nCh] = _mode20P; // уходим
					wordSet[250 + nCh] = _mode20P>>1;
						if ((wordSet[nCh]) == _mode20P)
						{
							wordSet[nCh] = _mode20P | (_mode20P<<8);
							wordSet[250 + nCh] = (_mode20P>>1)  | (_mode20P<<7); // (_mode20P<<8)>2 = <<7
						}
						
						
					wordSet[40+nCh] = 2;
									
          break;
        }  
					led_rgb[nCh] = Nblinck( (_mode20P|_mode20P<<8) , _Blue, 500, nCh); // 
        
//			out =0;
      break;
				
			case 5:	
//******************************************************************** 5        
        tempPush = timePush (4000, nCh);    

        switch (tempPush)
        {
          case 0:
          wordSet[40+nCh] = 5; // monitor off
          break;          
          case 1:
          wordSet[40+nCh] = 3; // redi monitor          
          break;
          case 2:
          wordSet[nCh] = _mode30P; // уходим 
					wordSet[250+nCh] = _mode30P>>1;
					
					if ((wordSet[nCh]) == _mode30P)
						
						{
							wordSet[nCh] = _mode30P | (_mode30P<<8);
							wordSet[250 + nCh] = (_mode30P>>1) | (_mode30P<<7);
						}
						 
					wordSet[40+nCh] = 2; // select 1
					
          break;
        }  

       led_rgb[nCh] = Nblinck(( _mode30P| _mode30P<<8) , _Blue, 500, nCh); //2
//				out = 0;
      break;
				
			case 6:	
			//********************************************************************  6      	
					led_rgb[nCh]= _Yellow;
					
					out = 0;
					if ((Alarm[140+nCh] & Alarm[150+nCh])==0)
					{
						wordSet[40+nCh] = 8;					
					}
					
					
					else if (	(Alarm[nCh+20] & Alarm[nCh+30] & Alarm[nCh+40] & Alarm[50+nCh])==0)																	//(Alarm[nCh+60] == 0)
						{      
									wordSet[40+nCh] = 7; // Авария

										led_rgb[nCh] =  _Red;
						}
												
					else if (Alarm[110 + nCh]& Alarm[120 + nCh] & Alarm [130 + nCh])
							{
								wordSet[40+nCh] = 2;
								break;
							}
					else
					{														
								tempPush = timePush (4000, nCh);    

								switch (tempPush)
								{
								case 0:
								wordSet[40+nCh] = 6; // monitor off
								break;          
								case 1:
								wordSet[40+nCh] = 61; // redi monitor          
								break;
								case 2:
								Alarm[nCh+20] = 1;
								Alarm[nCh+30] = 1;
								Alarm[nCh+40] = 1;
								Alarm[nCh+50] = 1;
								Alarm[nCh +110] = 1;
								Alarm[nCh +120] = 1;
								Alarm[nCh +130] = 1;
								
								wordSet[40+nCh] = 0; // select 1		
								} 
						}
											
			break;
				
			case 7:	
	//********************************************************************   7  					
			led_rgb[nCh]= _Red;		
			
			out = 0;
			
					if (Alarm[20+nCh]&Alarm[30+nCh]&Alarm[40+nCh]&Alarm[50+nCh])
						{
							wordSet[40+nCh] = 2;										
						}
			
					else if ((Alarm[140+nCh] & Alarm[150+nCh])==0)
						{
							wordSet[40+nCh] = 8;						
						}
//				else
//					{

					else if (Alarm[20+nCh] & Alarm[30+nCh] & Alarm[40+nCh] & Alarm[50+nCh])
						{
							wordSet[40+nCh] = 2;
						}
					else
						{
						
						tempPush = timePush (4000, nCh);    

							switch (tempPush)
							{
								led_rgb[nCh] =  _Red;
								out = 0;
								
								case 0:
								wordSet[40+nCh] = 7; // monitor off
								break;          
								case 1:
									wordSet[40+nCh] = 71; // redi monitor  				
								break;
								case 2:
								Alarm[nCh+20] = 1;
								Alarm[nCh+30] = 1;
								Alarm[nCh+40] = 1;
								Alarm[nCh+50] = 1;
								Alarm[nCh +110] = 1;
								Alarm[nCh +120] = 1;
								Alarm[nCh +130] = 1;
								wordSet[40+nCh] = 0; // select 1					
								break;
							}
						}	
//					}
			break;
					
			case 8:				
//********************************************************************   8    
					led_rgb[nCh] = Alarm_blinck (_Red,_Yellow, 500, nCh);			//_Yellow	напряжение в кабеле	более Уставки 
					
					out = 0;

					if (Alarm[nCh+140] & Alarm[nCh+150])
					{
						wordSet[40+nCh] = 2;
					}
					else
					{
					
						tempPush = timePush (4000, nCh);    

						switch (tempPush)
						{
							case 0:
							wordSet[40+nCh] = 8; // monitor off
							break;          
							case 1:
								wordSet[40+nCh] = 81; // redi monitor  				
							break;
							case 2:
							Alarm[nCh+20] = 1;
							Alarm[nCh+30] = 1;
							Alarm[nCh+40] = 1;
							Alarm[nCh+50] = 1;
							Alarm[nCh +110] = 1;
							Alarm[nCh +120] = 1;
							Alarm[nCh +130] = 1;
							
							wordSet[40+nCh] = 0; // select 1					
							break;
						}
					}
				break;
			default:	
					wordSet[40+nCh] = 0;
			break;
				
	//********************************************************************   61 
		case 61:
				/* прдупр. сопротивление изоляции 1 ниже нормы  (110)*/
		
				out = 	0x01;	
		
		
				if (Alarm[nCh+110] == 1)
				{
					wordSet[40+nCh] = 62;
				}
				led_rgb[nCh] = Alarm_blinck (_Yellow,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
				{
					case 0:
					wordSet[40+nCh] = 61; // monitor off
					break;          
					case 1:
					wordSet[40+nCh] = 62; // redi monitor  				
					break;
					case 2:					
					wordSet[40+nCh] = 6; // select 1					
					break;
				}
				
				if (Alarm[110 + nCh]& Alarm[120 + nCh] & Alarm [130 + nCh] & Alarm[nCh+160])
					{
						wordSet[40+nCh] = 6;
					}
				else if (Alarm[nCh+110] == 1)
				{
					wordSet[40+nCh] = 62;
				}
			
			break;
			

		
//********************************************************************   62
				/* прдупр. сопротивление изоляции 2 ниже нормы  (120)*/
		case 62:
			
			out = 	0x02;	
		
		
				led_rgb[nCh] = Alarm_blinck (_Yellow,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
					{
						case 0:
						wordSet[40+nCh] = 62; // monitor off
						break;          
						case 1:
						wordSet[40+nCh] = 63; // redi monitor  				
						break;
						case 2:					
						wordSet[40+nCh] = 6; // select 1					
						break;
					}			
								
				if (Alarm[110 + nCh]& Alarm[120 + nCh] & Alarm [130 + nCh] & Alarm[nCh+160])
					{
						wordSet[40+nCh] = 6;
					}
				else if (Alarm[nCh+120] == 1)
					{
						wordSet[40+nCh] = 63;
					}
		break;	
					//********************************************************************   63
				/* прдупр. сопротивление шлейфа ниже нормы  (130)*/
		case 63:
			
			out = 	0x03;	
						
				led_rgb[nCh] = Alarm_blinck (_Yellow,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
					{
						case 0:
						wordSet[40+nCh] = 63; // monitor off
						break;          
						case 1:
						wordSet[40+nCh] = 64; // redi monitor  				
						break;
						case 2:					
						wordSet[40+nCh] = 6; // select 1					
						break;
					}
				
				if (Alarm[110 + nCh]& Alarm[120 + nCh] & Alarm [130 + nCh] & Alarm[nCh+160])
					{
						wordSet[40+nCh] = 6;
					}
				else if (Alarm[nCh+130] == 1)
					{
						wordSet[40+nCh] = 64;
					}				
		break;
				
//********************************************************************   64
				/* прдупр. сопротивление шлейфа выше предупредительрой нормы  (140)*/
		case 64:
			
			out = 	0x04;	
		
				led_rgb[nCh] = Alarm_blinck (_Yellow,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
					{
						case 0:
						wordSet[40+nCh] = 64; // monitor off
						break;          
						case 1:
						wordSet[40+nCh] = 61; // redi monitor  				
						break;
						case 2:					
						wordSet[40+nCh] = 6; // select 1					
						break;
					}	
				
				if (Alarm[110 + nCh]& Alarm[120 + nCh] & Alarm [130 + nCh] & Alarm[nCh+160])
					{
						wordSet[40+nCh] = 6;
					}
					
				else if (Alarm[nCh+160] == 1)
					{
						wordSet[40+nCh] = 61;
					}
		break;

//********************************************************************   71
				/* прдупр. сопротивление шлейфа выше аварийной нормы  (20)*/
		case 71:
			
			out = 	0x11;	
		
				led_rgb[nCh] = Alarm_blinck (_Red,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
				{
					case 0:
					wordSet[40+nCh] = 71; // monitor off
					break;          
					case 1:
					wordSet[40+nCh] = 72; // redi monitor  				
					break;
					case 2:					
					wordSet[40+nCh] = 7; // select 1					
					break;
				}	
				
				if (Alarm[20+nCh] & Alarm[30+nCh] & Alarm[40+nCh] & Alarm[50+nCh])
					{
						wordSet[40+nCh] = 7;
					}
					
				else if (Alarm[20+nCh])
					{
						wordSet[40+nCh] = 72;
					}

		break;	
//********************************************************************   72
				/* прдупр. сопротивление шлейфа выше аварийной нормы  (30)*/
		case 72:
			
			out = 	0x12;	
		
				led_rgb[nCh] = Alarm_blinck (_Red,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
				{
					case 0:
					wordSet[40+nCh] = 72; // monitor off
					break;          
					case 1:
					wordSet[40+nCh] = 73; // redi monitor  				
					break;
					case 2:					
					wordSet[40+nCh] = 7; // select 1					
					break;
				}	
				
				if (Alarm[20+nCh] & Alarm[30+nCh] & Alarm[40+nCh] & Alarm[50+nCh])
					{
						wordSet[40+nCh] = 7;
					}
					
				else if (Alarm[30+nCh])
					{
						wordSet[40+nCh] = 73;
					}
			break;
//********************************************************************   73
				/* прдупр. сопротивление шлейфа выше аварийной нормы  (40)*/
		case 73:
			
			out = 	0x13;	
		
				led_rgb[nCh] = Alarm_blinck (_Red,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
				{
					case 0:
					wordSet[40+nCh] = 73; // monitor off
					break;          
					case 1:
					wordSet[40+nCh] = 74; // redi monitor  				
					break;
					case 2:					
					wordSet[40+nCh] = 7; // select 1					
					break;
				}	
				
				if (Alarm[20+nCh] & Alarm[30+nCh] & Alarm[40+nCh] & Alarm[50+nCh])
					{
						wordSet[40+nCh] = 7;
					}
					
				else if (Alarm[40+nCh])
					{
						wordSet[40+nCh] = 74;
					}
		break;		
//********************************************************************   74
				/* прдупр. сопротивление шлейфа выше аварийной нормы  (40)*/
		case 74:
			
			out = 	0x14;	
		
				led_rgb[nCh] = Alarm_blinck (_Red,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
				{
					case 0:
					wordSet[40+nCh] = 74; // monitor off
					break;          
					case 1:
					wordSet[40+nCh] = 75; // redi monitor  				
					break;
					case 2:					
					wordSet[40+nCh] = 7; // select 1					
					break;
				}	
				
				if (Alarm[20+nCh] & Alarm[30+nCh] & Alarm[40+nCh] & Alarm[50+nCh])
					{
						wordSet[40+nCh] = 7;
					}
					
				else if (Alarm[50+nCh])
					{
						wordSet[40+nCh] = 75;
					}
			break;		
					//********************************************************************   75
				/*  прдупр. сопротивление изоляции 1 ниже нормы  (110)*/
		
			case 75:
			
			out = 	0x01;	
		
				led_rgb[nCh] = Alarm_blinck (_Yellow,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
				{
					case 0:
					wordSet[40+nCh] = 75; // monitor off
					break;          
					case 1:
					wordSet[40+nCh] = 76; // redi monitor  				
					break;
					case 2:					
					wordSet[40+nCh] = 7; // select 1					
					break;
				}	
				
				if (Alarm[110 + nCh]& Alarm[120 + nCh] & Alarm [130 + nCh] & Alarm[nCh+160])
					{
						wordSet[40+nCh] = 71;
					}
				else if (Alarm[nCh+110] == 1)
				{
					wordSet[40+nCh] = 76;
				}
				break;
				
									//********************************************************************   76
				/*  прдупр. сопротивление изоляции 2 ниже нормы  (120)*/
		
			case 76:
			
			out = 	0x02;	
		
				led_rgb[nCh] = Alarm_blinck (_Yellow,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
				{
					case 0:
					wordSet[40+nCh] = 76; // monitor off
					break;          
					case 1:
					wordSet[40+nCh] = 77; // redi monitor  				
					break;
					case 2:					
					wordSet[40+nCh] = 7; // select 1					
					break;
				}	
				
				if (Alarm[110 + nCh]& Alarm[120 + nCh] & Alarm [130 + nCh] & Alarm[nCh+160])
					{
						wordSet[40+nCh] = 71;
					}
				else if (Alarm[nCh+120] == 1)
				{
					wordSet[40+nCh] = 77;
				}
				break;
				
//********************************************************************   77
				/*  прдупр. сопротивление изоляции 2 ниже нормы  (120)*/
		
			case 77:
			
			out = 	0x03;	
		
				led_rgb[nCh] = Alarm_blinck (_Yellow,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
				{
					case 0:
					wordSet[40+nCh] = 77; // monitor off
					break;          
					case 1:
					wordSet[40+nCh] = 78; // redi monitor  				
					break;
					case 2:					
					wordSet[40+nCh] = 7; // select 1					
					break;
				}	
				
				if (Alarm[110 + nCh]& Alarm[120 + nCh] & Alarm [130 + nCh] & Alarm[nCh+160])
					{
						wordSet[40+nCh] = 71;
					}
				else if (Alarm[nCh+130] == 1)
				{
					wordSet[40+nCh] = 78;
				}
				break;
				
//********************************************************************   78
				/*  прдупр. сопротивление изоляции 2 ниже нормы  (160)*/
		
			case 78:
			
			out = 	0x04;	
		
				led_rgb[nCh] = Alarm_blinck (_Yellow,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
				{
					case 0:
					wordSet[40+nCh] = 78; // monitor off
					break;          
					case 1:
					wordSet[40+nCh] = 71; // redi monitor  				
					break;
					case 2:					
					wordSet[40+nCh] = 7; // select 1					
					break;
				}	
				
				if (Alarm[110 + nCh]& Alarm[120 + nCh] & Alarm [130 + nCh] & Alarm[nCh+160])
					{
						wordSet[40+nCh] = 71;
					}
				else if (Alarm[nCh+130] == 1)
				{
					wordSet[40+nCh] = 71;
				}	
			break;
				
	//********************************************************************   81
				/*  прдупр. сопротивление изоляции 2 ниже нормы  (140)*/
		
			case 81:
			
			out = 	0x15;	
		
				led_rgb[nCh] = Alarm_blinck (_Red,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
				{
					case 0:
					wordSet[40+nCh] = 81; // monitor off
					break;          
					case 1:
					wordSet[40+nCh] = 82; // redi monitor  				
					break;
					case 2:					
					wordSet[40+nCh] = 8; // select 1					
					break;
				}	
				
				if (Alarm[140 + nCh]& Alarm[150 + nCh])
					{
						wordSet[40+nCh] = 8;
					}
				else if (Alarm[nCh+140] == 1)
				{
					wordSet[40+nCh] = 82;
				}	
			break;
				
			//********************************************************************   82
				/*  прдупр. сопротивление изоляции 2 ниже нормы  (160)*/
		
			case 82:
			
			out = 	0x16;	
		
				led_rgb[nCh] = Alarm_blinck (_Red,_Black, 500, nCh);

				tempPush = timePush (4000, nCh);    

				switch (tempPush)
				{
					case 0:
					wordSet[40+nCh] = 82; // monitor off
					break;          
					case 1:
					wordSet[40+nCh] = 81; // redi monitor  				
					break;
					case 2:					
					wordSet[40+nCh] = 8; // select 1					
					break;
				}	
				
				if (Alarm[140 + nCh]& Alarm[150 + nCh])
					{
						wordSet[40+nCh] = 8;
					}
				else if (Alarm[nCh+150] == 1)
				{
					wordSet[40+nCh] = 81;
				}	
			break;	
				
		}
			return out;
			
	} 
	

	
	