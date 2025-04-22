#include "ADC_.h"
#include "fmac.h"
#define VREFINT_CAL_ADDR1  ((uint16_t*) (0x1FFF75AA)) // 校准值存储地址

#define VREF	3.3f
/**=================================================================
ADC1	V_Cap	I_In	Vrefint
ADC2		I_WPT
ADC3	V_In	I_Chassis	
ADC4	V_WPT
ADC5		
====================================================================
I_Chassis 相关函数：	真实值=20.4274*ADC-33.5493
I_in相关函数：	    真实值=21.1332*ADC-34.3196
V_in相关函数：		真实值=11.0500f*ADC+0.1040f
V_Cap相关函数：		真实值=10.9600f*ADC+0.1740f（目前未测试，暂用V_in的函数)

注：电流ADC我没有经过各种芯片参数的处理，只是用数据拟合出来了函数。效果本质上来说应该是一样的
*/
uint16_t vrefint_cal ;
uint16_t adc_Data_hadc1[ADC1_MAX];
uint16_t adc_Data_hadc2[ADC2_MAX];
uint16_t adc_Data_hadc3[ADC3_MAX];
uint16_t adc_Data_hadc4[ADC4_MAX];
uint16_t adc_Data_hadc5[ADC5_MAX];
float I_Chassis ;
float I_In ;
float V_In ;
float V_Cap ;
float V_WPT;
float I_WPT;
float I_Cap;
float vrefint;
LOWPASS_FILTER_STRUCT I_Chassis_lowpass;
LOWPASS_FILTER_STRUCT I_In_lowpass;
LOWPASS_FILTER_STRUCT V_In_lowpass;
LOWPASS_FILTER_STRUCT V_Cap_lowpass;
LOWPASS_FILTER_STRUCT I_Cap_lowpass;
//float I_IN_K=10.2590f,I_IN_D=-16.0650f;
//float I_CHASSIS_K=10.2590f,I_CHASSIS_D=-16.0650f;
//===========================================
//float V_IN_K=11.0500f,V_IN_D=-0.18f;
//float V_Cap_K=11.0500f,V_Cap_D=-0.18f;
//float I_IN_K=17.0f,I_IN_D=-27.78f;
//float I_CHASSIS_K=17.0f,I_CHASSIS_D=-27.78f;
//float I_CAP_K=21.4797f,I_CAP_D=-35.5317f;
//===========================================
//float V_IN_K=10.0f,V_IN_D=-0.2627f;//4号
//float V_Cap_K=10.0f,V_Cap_D=-0.5f;
//float I_IN_K=30.0429f,I_IN_D=-48.08f;
//float I_CAP_K=21.4797f,I_CAP_D=-35.5317f;
//float I_CHASSIS_K=-25.9067f,I_CHASSIS_D=42.3979f;
//===============================================
float V_IN_K=10.00f,V_IN_D=-0.0f;//2号
float V_Cap_K=10.0f,V_Cap_D=-0.0f;
float I_IN_K=10.83764f,I_IN_D=-35.51574f;
float I_CHASSIS_K=-20.4545f,I_CHASSIS_D=33.6841f;
float I_CAP_K=26.087f,I_CAP_D=-43.4347f;


void ADC_init(void)
{
	low_filter_init(&I_Chassis_lowpass,200e3,1e3);
	low_filter_init(&I_In_lowpass,200e3,1e3);
	low_filter_init(&V_In_lowpass,200e3,2e3);
	low_filter_init(&V_Cap_lowpass,200e3,2e3);
	low_filter_init(&I_Cap_lowpass,200e3,1e3);
//	 vrefint_cal =*VREFINT_CAL_ADDR1; // 读取校准值
	    uint32_t *Fmac_Wdata;
    Fmac_Wdata = (uint32_t *) 0x40021418;

//	HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED );//adc校准
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED );//adc校准
	HAL_ADCEx_Calibration_Start(&hadc3,ADC_SINGLE_ENDED );//adc校准
	HAL_ADCEx_Calibration_Start(&hadc4,ADC_SINGLE_ENDED );//adc校准
	HAL_ADCEx_Calibration_Start(&hadc5,ADC_SINGLE_ENDED );//adc校准
	HAL_ADC_Start_DMA(&hadc5, Fmac_Wdata, ADC1_MAX); 
	HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adc_Data_hadc2, ADC2_MAX);
	HAL_ADC_Start_DMA(&hadc3, (uint32_t *)adc_Data_hadc3, ADC3_MAX);
	HAL_ADC_Start_DMA(&hadc4, (uint32_t *)adc_Data_hadc4, ADC4_MAX);
//	HAL_ADC_Start_DMA(&hadc5, (uint32_t *)adc_Data_hadc5, ADC5_MAX);
	vrefint =2355.0f;
}

/*
	*adc数据处理
	*
	*/
void ADC1_dispose (void)
{	
//	I_In_lowpass.Input =I_IN_K*I_IN_Fmac*VREF/4095.0f+I_IN_D;
//	low_filter_calc(&I_In_lowpass);
//	I_In = I_In_lowpass.Output ;
	I_In =I_IN_K*I_IN_Fmac*VREF/4095.0f+I_IN_D;
}
void ADC2_dispose (void)
{

	I_Cap_lowpass.Input =I_CAP_K*adc_Data_hadc2[0]*VREF/4095.0f+I_CAP_D;
	low_filter_calc(&I_Cap_lowpass);
	I_Cap = I_Cap_lowpass.Output ;
//	if(I_Cap <0.5f)
//		I_Cap=(I_In-I_Chassis)*V_In/V_Cap *1.1f;
}

void ADC3_dispose (void)
{
	
//	I_Chassis =adc_Data_hadc3[1]*VREF/4095.0f ;
	V_In_lowpass.Input =V_IN_K*adc_Data_hadc3[0]*VREF/4095.0f+V_IN_D ;
	low_filter_calc(&V_In_lowpass );
	V_In = V_In_lowpass.Output ;
//	I_Cap=(I_In-I_Chassis)*V_In/V_Cap *1.1f;
	
	I_Chassis_lowpass.Input =I_CHASSIS_K*adc_Data_hadc3[1]*VREF/4095.0f+I_CHASSIS_D;
	low_filter_calc(&I_Chassis_lowpass);
	I_Chassis = I_Chassis_lowpass.Output ;

}
void ADC4_dispose (void)
{
	V_Cap_lowpass.Input =V_Cap_K*adc_Data_hadc4[0]*VREF/4095.0f+V_Cap_D ;
	low_filter_calc(&V_Cap_lowpass );
	V_Cap = V_Cap_lowpass.Output ;
 
}
void ADC5_dispose (void)
{

	
	
}



//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//    if (hadc->Instance == ADC1) { // 只处理 ADC1 的数据
//		HAL_ADC_Stop_DMA(&hadc1);
//        ADC1_dispose();  // 处理 ADC 数据
//        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_Data_hadc1, ADC1_MAX); // 重新开启 ADC 采样
//    }
//	if (hadc->Instance == ADC2) { // 只处理 ADC2 的数据
//		HAL_ADC_Stop_DMA(&hadc2);
//        ADC2_dispose();  // 处理 ADC 数据
//        HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adc_Data_hadc2, ADC2_MAX); // 重新开启 ADC 采样
//    }
//	if (hadc->Instance == ADC3) { // 只处理 ADC3 的数据
//		HAL_ADC_Stop_DMA(&hadc3);
//        ADC3_dispose();  // 处理 ADC 数据
//        HAL_ADC_Start_DMA(&hadc3, (uint32_t *)adc_Data_hadc3, ADC3_MAX); // 重新开启 ADC 采样
//    }
//	if (hadc->Instance == ADC4) { // 只处理 ADC4 的数据
//		HAL_ADC_Stop_DMA(&hadc4);
//        ADC4_dispose();  // 处理 ADC 数据
//        HAL_ADC_Start_DMA(&hadc4, (uint32_t *)adc_Data_hadc4, ADC4_MAX); // 重新开启 ADC 采样
//    }
//	if (hadc->Instance == ADC5) { // 只处理 ADC4 的数据
//		HAL_ADC_Stop_DMA(&hadc5);
//        ADC5_dispose();  // 处理 ADC 数据
//        HAL_ADC_Start_DMA(&hadc5, (uint32_t *)adc_Data_hadc5, ADC5_MAX); // 重新开启 ADC 采样
//    }
//}

