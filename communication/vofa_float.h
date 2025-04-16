#ifndef __VOFA_FLOAT_H__
#define __VOFA_FLOAT_H__
#include "usart.h"
#define MAX_CHANNEL 6		//定义发送串口通道数
void vofa_begin(void);
void Upper_Computer_Init(float* addr);
void Upper_Computer_Show_Wave(void);
#endif
