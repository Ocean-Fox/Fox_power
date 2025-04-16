#include "TIM_Init.h"
float testa,testb,testc;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
//		testa=adc_Data_hadc1[0];
//		testb=adc_Data_hadc2[0];
//		testc=adc_Data_hadc2[1];
		VinSwOVP(); //输入过压
//		VoutSwUVP();//输出欠压
		VinSwUVP(); //输入欠压
		VoutSwOVP();//输出过压
		SwOCP();	//电容过流
		ShortOff(); //短路保护
		StateM();
		Buck_Boost_Status();
//		outputDuty=CtrValue.BuckValue_/(27200.0f-CtrValue.BoostValue_);
	Upper_Computer_Init(&CtrValue.BuckValue_);
	Upper_Computer_Init(&CtrValue.BoostValue_);
		Upper_Computer_Init(&outputDuty);
//			Upper_Computer_Init(&imu_pid_power_buck.set);
//	Upper_Computer_Init(&imu_pid_power_buck.fdb);
//		Upper_Computer_Init(&imu_pid_power_buck.out);
		
		Upper_Computer_Init(&I_In);
	Upper_Computer_Init(&V_In);
		Upper_Computer_Init(&I_Cap );
	Upper_Computer_Show_Wave();
		
	}
}
