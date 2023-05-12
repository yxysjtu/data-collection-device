#include "myadc.h"

void ADC_Select_CH (uint32_t channel)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	  */
	  sConfig.Channel = channel;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_41CYCLES_5;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

int ADC_GetVal(uint32_t channel){
	/*ADC_Select_CH(channel);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1000);
	int val = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
    return val;*/
		//设置转换序列	  	
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=channel;		  			    
	ADC1->CR2|=1<<22;       //启动规则转换通道 
	
	while(!(ADC1->SR&1<<1));//等待转换结束	 	   
	return ADC1->DR;		//返回adc值	
}

