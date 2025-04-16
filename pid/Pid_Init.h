#ifndef __PID_INIT_H_
#define __PID_INIT_H_
#include "pid.h"
void Pid_Init(void);
extern CCMRAM pid_type_def imu_pid_power_buck;
extern CCMRAM pid_type_def imu_pid_power_boost;
extern CCMRAM pid_type_def imu_pid_power_buck_boost_mix;
extern CCMRAM pid_type_def imu_pid_vel_buck;
extern CCMRAM pid_type_def imu_pid_vel_boost;
extern CCMRAM pid_type_def imu_pid_vel_buck_boost_mix;
extern CCMRAM pid_type_def imu_pid_cur_buck;
extern CCMRAM pid_type_def imu_pid_cur_boost;
extern CCMRAM pid_type_def imu_pid_cur_buck_boost_mix;
#endif 
