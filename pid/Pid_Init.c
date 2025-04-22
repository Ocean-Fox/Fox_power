#include "Pid_Init.h"

const float imu_PID_power_buck[3] = {0.01,35,4e-06};
const float imu_PID_power_buck_boost_mix[3] = {0.0065,9,3.3e-07};
const float imu_PID_vel_buck[3] = {0.03,200,8e-06};
const float imu_PID_vel_buck_boost_mix[3] = {0.03,200,8e-06};
const float imu_PID_cur_buck[3] = {0.007,64,8.7e-07};
const float imu_PID_cur_buck_boost_mix[3] = {0.01,68,1.3e-06};

CCMRAM pid_type_def imu_pid_power_buck;
CCMRAM pid_type_def imu_pid_power_boost;
CCMRAM pid_type_def imu_pid_power_buck_boost_mix;
CCMRAM pid_type_def imu_pid_vel_buck;
CCMRAM pid_type_def imu_pid_vel_boost;
CCMRAM pid_type_def imu_pid_vel_buck_boost_mix;
CCMRAM pid_type_def imu_pid_cur_buck;
CCMRAM pid_type_def imu_pid_cur_boost;
CCMRAM pid_type_def imu_pid_cur_buck_boost_mix;

/**
	*@brief pid参数初始化
	*@param void
*/
void Pid_Init(void)
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
	PID_init(&imu_pid_power_buck,			0.009	,35 / 200000.0	,4e-06* 2000.0	,10,10		,-10,-10);
//	PID_init(&imu_pid_power_boost,			0.01	,35 / 200000.0	,4e-06* 2000.0	,10,10		,-10,-10);
	PID_init(&imu_pid_power_buck_boost_mix, 0.0065	,9/ 200000.0	,3.3e-07* 2000.0	,7,7		,-7,-7);
	PID_init(&imu_pid_vel_buck ,			0.03	,200 / 200000.0	,8e-06* 2000.0	,0.75,0.7		,0.1,-1);
//	PID_init(&imu_pid_vel_boost ,			0.03	,200 / 200000.0	,8e-06* 2000.0	,0.75,0.7		,0.1,-1);
	PID_init(&imu_pid_vel_buck_boost_mix ,	0.03	,200 / 200000.0	,8e-06* 2000.0	,0.75,0.7		,0.05,-1);
	PID_init(&imu_pid_cur_buck ,			0.007	,64 / 200000.0	,8.7e-07* 2000.0	,0.75,0.7		,0.2,-1);
//	PID_init(&imu_pid_cur_boost ,			0.007	,64 / 200000.0	,8.7e-07* 2000.0	,0.75,0.7		,0.2,-1);
	PID_init(&imu_pid_cur_buck_boost_mix ,	0.01	,68 / 200000.0	,1.3e-06* 2000.0	,0.75,0.7		,0.2,-1);
}
