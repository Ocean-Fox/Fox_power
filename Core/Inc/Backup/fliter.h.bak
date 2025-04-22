#ifndef __FLITER_H_
#define __FLITER_H_
#include "adc.h"
#include "arm_math.h"
#define low_filter_default {0, 0, 0, 0, 0, 0}
#define CCMRAM __attribute__((section("ccmram")))
typedef struct
{
	float  Input;
	float  Output;
	float  Fc;
	float  Fs;
	float  Ka;
	float  Kb;
	
}LOWPASS_FILTER_STRUCT;
void low_filter_init(LOWPASS_FILTER_STRUCT *p, float sample_f, float cutoff_f);
CCMRAM void low_filter_calc(LOWPASS_FILTER_STRUCT *p);
#define PI2  2 * PI

#endif
