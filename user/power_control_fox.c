#include "power_control_fox.h"
#include "fdcan_Init.h"
float REFEREE_SESTEM_POWER=13.0f;
#define BUCK_BOOST_V			20
CtrValue_t CtrValue;
CCMRAM  void All_pid_clean(void)
{
	PID_clear(&imu_pid_power_buck);
	PID_clear(&imu_pid_power_boost);
	PID_clear(&imu_pid_power_buck_boost_mix);
	PID_clear(&imu_pid_vel_buck);
	PID_clear(&imu_pid_vel_boost);
	PID_clear(&imu_pid_vel_buck_boost_mix);
	PID_clear(&imu_pid_cur_buck);
	PID_clear(&imu_pid_cur_boost);
	PID_clear(&imu_pid_cur_buck_boost_mix);
}

CCMRAM void Buck_Boost_mix_pid(void)
{
	PID_calc(&imu_pid_power_buck_boost_mix,V_In*I_In,rxData.feedbackRefereePowerLimit);
	PID_calc(&imu_pid_cur_buck_boost_mix,I_Cap ,imu_pid_power_buck_boost_mix.out);
	if(V_Cap >0.9f*MAX_VOUT_OVP_VAL)
	{
		PID_calc(&imu_pid_vel_buck_boost_mix,V_Cap,BUCK_BOOST_V);
		if(imu_pid_vel_buck_boost_mix.out<imu_pid_cur_buck_boost_mix.out)
		{
			CtrValue.Pwm_=(float)imu_pid_vel_buck_boost_mix.out;
		}
		if(imu_pid_vel_buck_boost_mix.out>imu_pid_cur_buck_boost_mix.out)
		{
			CtrValue.Pwm_=(float)imu_pid_cur_buck_boost_mix.out;
		}
	}
	else CtrValue.Pwm_=(float)imu_pid_cur_buck_boost_mix.out;
	CtrValue.BuckValue_=(CtrValue.Pwm_+1.0f)*0.4f*HRTIMAB_Period;
	CtrValue.BoostValue_=(1.0f-1.0f/(1.0f+CtrValue.Pwm_))*0.4f*HRTIMAB_Period;
	CtrValue.BoostValue_min=(1.0f-(CtrValue.Pwm_+1.0f)*0.4f*V_In/(V_Cap+2.5f)) *HRTIMAB_Period;
	if(CtrValue.BoostValue_min>CtrValue.BoostValue_max*0.3f)CtrValue.BoostValue_min=CtrValue.BoostValue_max*0.3f;
	if(CtrValue.BoostValue_>CtrValue.BoostValue_max)
	{
		CtrValue.BoostValue_=CtrValue.BoostValue_max;
	}
	if(CtrValue.BuckValue_>CtrValue.BuckValue_max)
	{
		CtrValue.BuckValue_=CtrValue.BuckValue_max;
	}
	if(CtrValue.BoostValue_<CtrValue.BoostValue_min)
	{
		CtrValue.BoostValue_=CtrValue.BoostValue_min;
	}

}

CCMRAM void Buck_pid(void)
{

		PID_calc(&imu_pid_power_buck,V_In*I_In,rxData.feedbackRefereePowerLimit);
		PID_calc(&imu_pid_cur_buck,I_Cap ,imu_pid_power_buck.out);
		if(V_Cap >0.9f*MAX_VOUT_OVP_VAL)
		{
			PID_calc(&imu_pid_vel_buck,V_Cap,BUCK_BOOST_V);
			if(imu_pid_vel_buck.out<imu_pid_cur_buck.out)
			{
				CtrValue.Pwm_=(float)imu_pid_vel_buck.out;
			}
			if(imu_pid_vel_buck.out>imu_pid_cur_buck.out)
			{
				CtrValue.Pwm_=(float)imu_pid_cur_buck.out;
			}
		}	
		else CtrValue.Pwm_=(float)imu_pid_cur_buck.out;

	
	CtrValue.BuckValue_=CtrValue.Pwm_*HRTIMAB_Period;
	CtrValue.BoostValue_=HRTIMAB_Period*0.05f;
	CtrValue.BuckValue_min=(V_Cap /(V_In+2.5f))*HRTIMAB_Period;
	if(CtrValue.BuckValue_min>CtrValue.BuckValue_max*0.3f)CtrValue.BuckValue_min=CtrValue.BuckValue_max*0.3f;
	if(CtrValue.BuckValue_>CtrValue.BuckValue_max)
	{
		CtrValue.BuckValue_=CtrValue.BuckValue_max;
	}
	if(CtrValue.BuckValue_<CtrValue.BuckValue_min)
	{
		CtrValue.BuckValue_=CtrValue.BuckValue_min;
	}
	
}

/*
 ** ===================================================================
 **     Funtion Name : void BuckBoostVLoopCtlPID(void)
 **     Description :  BUCK-BOOST模式下环路计算
 **                    BUCK-BOOST mode loop calculate
 **     检测输出参考电压与输入电压的关系，判断环路工作于BUCK模式，还是BUCK-BOOST模式
 **     detect output reference voltage's relationship with input voltage,
 **     judge loop control mode is buck-boost mode or buck mode
 **     BUCK模式下，BOOST的开关管工作在固定占空比，控制BUCK的占空比控制输出电压
 **     in BUCK mode, boost side mosfet is work in fixed duty, buck side mosfet control output voltage
 **     BUCK-BOOST模式下，BUCK的开关管工作在固定占空比，控制BOOST的占空比控制输出电压
 **     in BUCK-BOOST mode, buck side mosfet is work in fixed duty, boost side mosfet control output voltage
 **     Parameters  :无
 **                 none
 **     Returns     :无
 **                 none
 ** ===================================================================
 */
CCMRAM void BuckBoostVLoopCtlPID(void)
{
	if(Mode_Change ==1)
	{
		All_pid_clean();
		Mode_Change=0;
	}
	switch(Status_ )
	{
		case NA0:
		{
			All_pid_clean();
			break;
		}
		case BUCK:
		{
			Buck_pid();
			break;
		}
		case MIX:
		{
			Buck_Boost_mix_pid();
			break;
		}

	}
	if (PwmEnFlag == 0)
    {
        CtrValue.BuckValue_ = MIN_BUCK_DUTY;
        CtrValue.BoostValue_ = 0.95*27200;
    }

	hhrtim1.Instance->sTimerxRegs[0].CMP1xR = (uint16_t)CtrValue.BuckValue_;//通过修改比较值CMP，从而修改占空比60000
	hhrtim1.Instance->sMasterRegs.MCMP2R=(uint16_t)CtrValue.BuckValue_ >> 1;//ADC sampling trigger
	hhrtim1.Instance->sTimerxRegs[1].CMP1xR= (uint16_t)CtrValue.BoostValue_;//通过修改比较值CMP，从而修改占空比60000
}
