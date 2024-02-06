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

//	static uint8_t mode[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//	static uint8_t out;
		uint8_t out;
//	static uint8_t temp;	
//******************************************************************	0	
		if (wordSet[nCh+40] == 0)  // Режим работы канала 
    {
			Alarm[nCh+50] = 1; 
          
      tempPush = timePush (4000, nCh);    // Селектор длительности нажатия
	
			switch (tempPush) // режим пуска
        {
          case 0:
          wordSet[nCh+40] = 0; // monitor off
          break;          
          case 1:
						wordSet[nCh+40] = 1; // redi monitor короткое нажатие
          break;
          case 2:
          wordSet[nCh+40] = 3; // select 1  длинное нажатие
          break;
        }               
				led_rgb[nCh] = _Blue; // Синий мониторинг выключен желтый //Nblinck(2, 0x000f0000, 500);                                          
//				out = 0;

			} 
		//******************************************************************** 1       
      else if (wordSet[nCh+40] == 1)
      {
				Alarm[nCh+50] = 1; 
        tempPush = timePush (4000, nCh);    

        switch (tempPush)
        {
          case 0:
          wordSet[nCh+40] = 1; // monitor off
          break;          
          case 1:
          wordSet[nCh+40] = 2; // redi monitor
          break;
          case 2:
          wordSet[nCh+40] = 1; // select 1
          break;
        }

        led_rgb[nCh] = Nblinck( wordSet[nCh], _Green, 500, nCh); // 
        
//					out = 0;
        }
 //******************************************************************** 2            
      else if (wordSet[nCh+40] == 2)
      {
        tempPush = timePush (4000, nCh); 
				
        switch (tempPush)
        {
          case 0:
          wordSet[nCh+40] = 2; // monitor off
          break;          
          case 1:
          wordSet[nCh+40] = 2; // redi monitor
//					Alarm[nCh+50] = 1; // ручное квитирование обрыва кабеля
          break;
          case 2:
          wordSet[nCh+40] = 0; // select 1
//					Alarm[nCh+50] = 1; // ручное квитирование обрыва кабеля
          break;
        }
				if (Alarm[nCh+100] !=0)
				{
					led_rgb[nCh] = Alarm_blinck (_Red,_Yellow, 500, nCh);			//_Yellow	напряжение в кабеле более 10 В
				}
        else if (	(Alarm[nCh+20] & Alarm[nCh+30] & Alarm[nCh+40])==0)																	//(Alarm[nCh+60] == 0)
        {      
							wordSet[nCh+40] = 7; // Авария
//								wordSet[nCh+40] = 6;
								led_rgb[nCh] =  _Red; 
        }	
				else if ((Alarm[110 + nCh]& Alarm[120 + nCh] & Alarm [130 + nCh])==0)
				{
					wordSet[nCh + 40] = 6; // Предупреждение
					led_rgb[nCh] = _Yellow;
				}
				else	
				{
				led_rgb[nCh] = _Green; //Nblinck( wordSet[nCh], _Green, 500); // 
//				if (nCh == adc_current)
//					{
//						led_rgb[nCh] = _GreenON;
//					}
//					else
//					{
//						led_rgb[nCh] = _Green;
//					}
				}
//				out = 1;	 
			}
//******************************************************************** 3       
       else if (wordSet[nCh+40] == 3)
        {
        tempPush = timePush (4000, nCh);    

        switch (tempPush)
        {
          case 0:
          wordSet[nCh+40] = 3; //set mode 1
          break;          
          case 1:
          wordSet[nCh+40] = 4; // redi monitor  FLASH
         
          break;
          case 2: 											// Monitoring Grn
						wordSet[nCh] = _mode10P; // уходим
						wordSet[250+nCh] = _mode10P>>1;
					
						if ((wordSet[nCh]) == _mode10P)
						{
							wordSet[nCh] = _mode10P | (_mode10P<<8);
							wordSet[250+nCh] = (_mode10P>>1) | (_mode10P<<7);
						}
						wordSet[nCh+40] = 2; // Переходим на мониторинг с уставкой 10%					
						
          break;
        }  
				
					led_rgb[nCh] = Nblinck( (_mode10P|_mode10P<<8) , _Blue, 500, nCh); // _Yellow 
        
        }
//********************************************************************  4      
				else if (wordSet[nCh+40] == 4) // выбираем уставку
      {
                 tempPush = timePush (4000, nCh);    

        switch (tempPush)
        {
          case 0:
          wordSet[nCh+40] = 4; // monitor off
          break;          
          case 1:
          wordSet[nCh+40] = 5; // redi monitor
          break;
          case 2:
          wordSet[nCh] = _mode20P; // уходим
					wordSet[250 + nCh] = _mode20P>>1;
						if ((wordSet[nCh]) == _mode20P)
						{
							wordSet[nCh] = _mode20P | (_mode20P<<8);
							wordSet[250 + nCh] = (_mode20P>>1)  | (_mode20P<<7); // (_mode20P<<8)>2 = <<7
						}
						
						
					wordSet[nCh+40] = 2;
									
          break;
        }  
					led_rgb[nCh] = Nblinck( (_mode20P|_mode20P<<8) , _Blue, 500, nCh); // 
        
//			out =0;
      }
//******************************************************************** 5        
        
      
      else if (wordSet[nCh+40] == 5)
      {
        tempPush = timePush (4000, nCh);    

        switch (tempPush)
        {
          case 0:
          wordSet[nCh+40] = 5; // monitor off
          break;          
          case 1:
          wordSet[nCh+40] = 3; // redi monitor          
          break;
          case 2:
          wordSet[nCh] = _mode30P; // уходим 
					wordSet[250+nCh] = _mode30P>>1;
					
					if ((wordSet[nCh]) == _mode30P)
						
						{
							wordSet[nCh] = _mode30P | (_mode30P<<8);
							wordSet[250 + nCh] = (_mode30P>>1) | (_mode30P<<7);
						}
						 
					wordSet[nCh+40] = 2; // select 1
					
          break;
        }  

       led_rgb[nCh] = Nblinck(( _mode30P| _mode30P<<8) , _Blue, 500, nCh); //2
//				out = 0;
      }
			//********************************************************************  6      
				else if (wordSet[nCh+40] == 6)
				{	
					out = 0;
					
					tempPush = timePush (4000, nCh);    

					switch (tempPush)
					{
					case 0:
          wordSet[nCh+40] = 6; // monitor off
          break;          
          case 1:
          wordSet[nCh+40] = 61; // redi monitor          
          break;
          case 2:
          Alarm[nCh+20] = 1;
					Alarm[nCh+30] = 1;
					Alarm[nCh+40] = 1;
					Alarm[nCh+50] = 1;
					Alarm[nCh +110] = 1;
					Alarm[nCh +120] = 1;
					Alarm[nCh +130] = 1;
          wordSet[nCh+40] = 0; // select 1		
					}
						
					
					
					if (Alarm[110 + nCh]& Alarm[120 + nCh] & Alarm [130 + nCh])
						{
							wordSet[nCh+40] = 2;
						}

					else
						{
						wordSet[nCh+40] = 0; 
						led_rgb[nCh]= _Blue;
						}
				
					
				}
	//********************************************************************   7     
			else if (wordSet[nCh+40] == 7)
				{
					
					out = 0;

					if ((Alarm[nCh+20] | Alarm[nCh+30] | Alarm[nCh+40]) ==0)
					{
						wordSet[nCh+40] = 2;
					}
					
        tempPush = timePush (4000, nCh);    

        switch (tempPush)
        {
          case 0:
          wordSet[nCh+40] = 7; // monitor off
          break;          
          case 1:
						wordSet[nCh+40] = 71; // redi monitor  				
          break;
          case 2:
					Alarm[nCh+20] = 1;
					Alarm[nCh+30] = 1;
					Alarm[nCh+40] = 1;
					Alarm[nCh+50] = 1;
					Alarm[nCh +110] = 1;
					Alarm[nCh +120] = 1;
					Alarm[nCh +130] = 1;
          wordSet[nCh+40] = 0; // select 1					
          break;
        }		
				led_rgb[nCh] =  _Red;       
//        out =1;   																								  
			}

			
			return out;
			
		} 

	// test git    