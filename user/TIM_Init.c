#include "TIM_Init.h"
#include "fdcan_Init.h"
float testa,testb,testc;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{

		VinSwOVP(); //�����ѹ
//		VoutSwUVP();//���Ƿѹ
		VinSwUVP(); //����Ƿѹ
		VoutSwOVP();//�����ѹ
		SwOCP();	//���ݹ���
		ShortOff(); //��·����
		StateM();
		Buck_Boost_Status();
FDCAN3_Send_PowerInfo(V_Cap,I_Chassis*V_In);
	Upper_Computer_Init(&CtrValue.BuckValue_);
	Upper_Computer_Init(&CtrValue.BoostValue_);
////		Upper_Computer_Init(&outputDuty);
////			Upper_Computer_Init(&imu_pid_power_buck.set);
//	Upper_Computer_Init(&imu_pid_power_buck.fdb);
//		Upper_Computer_Init(&imu_pid_power_buck.out);
//		
		Upper_Computer_Init(&I_In);
		Upper_Computer_Init(&V_In);
		Upper_Computer_Init(&V_Cap);
////		Upper_Computer_Init(&I_Cap );
	Upper_Computer_Show_Wave();
		
	}
	if(htim->Instance == TIM3)
	{
		if(getReg(Cap_Error)!=0)
		{
			Buzzer_error(getReg(Cap_Error));
		}
		else
		{
			 __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 0);
		}
	}
//	if(htim->Instance == TIM4)
//	{
//		FDCAN3_Send_PowerInfo(V_Cap,I_Chassis*V_In);
//	}
}


