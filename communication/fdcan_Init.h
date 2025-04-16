#ifndef __FDCAN_INIT_H_
#define __FDCAN_INIT_H_
#include "fdcan.h"
void FDCAN_Config_Filter(void) ;
void FDCAN3_Send_Msg(uint16_t ID,uint8_t* msg);

#endif
