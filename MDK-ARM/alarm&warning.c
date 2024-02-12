#include "main.h"
#include "cmsis_os.h"

uint8_t sort(uint8_t* status)
{
	uint8_t chSet; // номер канала по которому идет запрос аварии
	uint8_t sum = 0;// количество каналов по которым идет запрос
	
	
	for(int i=0;i<=9;i++)
  {

			if (( status[0] !=0)&( status[1] ==0)&( status[2] ==0)&
					( status[3] ==0)&( status[4] ==0)&( status[5] ==0)&
					( status[6] ==0)&( status[7] ==0)&( status[8] ==0)&
					( status[9] ==0))
					{return status[0];}
					
			else if (( status[0] ==0)&( status[1] !=0)&( status[2] ==0)&
					( status[3] ==0)&( status[4] ==0)&( status[5] ==0)&
					( status[6] ==0)&( status[7] ==0)&( status[8] ==0)&
					( status[9] ==0))
					{return status[1];}
					
			else if (( status[0] ==0)&( status[1] ==0)&( status[2] !=0)&
					( status[3] ==0)&( status[4] ==0)&( status[5] ==0)&
					( status[6] ==0)&( status[7] ==0)&( status[8] ==0)&
					( status[9] ==0))
					{return status[2];}
					
			else if (( status[0] ==0)&( status[1] ==0)&( status[2] ==0)&
					( status[3] !=0)&( status[4] ==0)&( status[5] ==0)&
					( status[6] ==0)&( status[7] ==0)&( status[8] ==0)&
					( status[9] ==0))
					{return status[3];}
					
			else if (( status[0] ==0)&( status[1] ==0)&( status[2] ==0)&
					( status[3] ==0)&( status[4] !=0)&( status[5] ==0)&
					( status[6] ==0)&( status[7] ==0)&( status[8] ==0)&
					( status[9] ==0))
					{return status[4];}
					
			else if (( status[0] ==0)&( status[1] ==0)&( status[2] ==0)&
					( status[3] ==0)&( status[4] ==0)&( status[5] !=0)&
					( status[6] ==0)&( status[7] ==0)&( status[8] ==0)&
					( status[9] ==0))
					{return status[5];}
					
			else if (( status[0] ==0)&( status[1] ==0)&( status[2] ==0)&
					( status[3] ==0)&( status[4] ==0)&( status[5] ==0)&
					( status[6] !=0)&( status[7] ==0)&( status[8] ==0)&
					( status[9] ==0))
					{return status[6];}
					
			else if (( status[0] ==0)&( status[1] ==0)&( status[2] ==0)&
					( status[3] ==0)&( status[4] ==0)&( status[5] ==0)&
					( status[6] ==0)&( status[7] !=0)&( status[8] ==0)&
					( status[9] ==0))
					{return status[7];}
					
			else if (( status[0] ==0)&( status[1] ==0)&( status[2] ==0)&
					( status[3] ==0)&( status[4] ==0)&( status[5] ==0)&
					( status[6] ==0)&( status[7] ==0)&( status[8] !=0)&
					( status[9] ==0))
					{return status[8];}
					
			else if (( status[0] ==0)&( status[1] ==0)&( status[2] ==0)&
					( status[3] ==0)&( status[4] ==0)&( status[5] ==0)&
					( status[6] ==0)&( status[7] ==0)&( status[8] ==0)&
					( status[9] !=0))
					{return status[9];}
			else 
			{return 0;}			
			
		}

	return 0;
}