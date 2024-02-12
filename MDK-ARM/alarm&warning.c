#include "main.h"
#include "cmsis_os.h"

uint8_t sort(uint8_t* status)
{
	uint8_t chSet; // номер канала по которому идет запрос аварии
	uint8_t sum = 0;// количество каналов по которым идет запрос
	
	
	for(int i=0;i<=9;i++)
  {
		if (status[i]) 
		{
			sum ++;
			chSet = status[i];
		}
		if (sum ==1)
		{return chSet;}
	
	return 0;
  }

//	if (status[0])
//			{status[1] =0;
//				status[2] =0;
//				status[3] =0;
//				status[4] =0;
//				status[5] =0;
//				status[6] =0;
//				status[7] =0;
//				status[8] =0;
//				status[9] =0;
//			}


}