#ifndef __POWER_CONTROL_FOX_H_
#define __POWER_CONTROL_FOX_H_
#include "ADC_.h"
#include "Pid_Init.h"
#include "function.h"
#include "vofa_float.h"
CCMRAM void All_pid_clean(void);
CCMRAM void Buck_Boost_mix_pid(void);
CCMRAM void Buck_Boostpid(void);
extern float buck_boost_mix_pwm;
extern float buck_boost_pwm;
extern uint16_t HRTIM_DMA_Buffer[3];
typedef struct
{
	float Pwm_;
	float BoostValue_;
	float BoostValue_max;
	float BoostValue_min;
	float BuckValue_;
	float BuckValue_max;
	float BuckValue_min;
}CtrValue_t;
extern CtrValue_t CtrValue;
extern CCMRAM void BuckBoostVLoopCtlPID(void);
extern float REFEREE_SESTEM_POWER;
#endif
