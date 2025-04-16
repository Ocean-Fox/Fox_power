#ifndef __ADC__H_
#define __ADC__H_
#include "fliter.h"
#include "adc.h"
#include "dma.h"
#define ADC1_MAX		1
#define ADC2_MAX		1
#define ADC3_MAX		2
#define ADC4_MAX		1
#define ADC5_MAX		1

extern  float I_Chassis ;
extern  float V_In ;
extern  float I_In ;
extern  float V_Cap ;
extern  float V_WPT;
extern  float I_WPT;
extern  float I_Cap;
void ADC1_dispose (void);
void ADC2_dispose (void);
void ADC3_dispose (void);
void ADC4_dispose (void);
void ADC5_dispose (void);
void ADC_init(void);
#endif
