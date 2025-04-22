#ifndef __FDCAN_INIT_H_
#define __FDCAN_INIT_H_
#include "fdcan.h"
void FDCAN_Config_Filter(void) ;
void FDCAN3_Send_Msg(uint16_t ID,uint8_t* msg);
typedef struct __attribute__((packed)) {
    uint8_t enableDCDC : 1;
    uint8_t systemRestart : 1;
    uint8_t resv0 : 6;
    uint16_t feedbackRefereePowerLimit;
    uint16_t feedbackRefereeEnergyBuffer;
    uint8_t resv1[3];
} RxData;
extern RxData rxData;
void FDCAN3_Send_PowerInfo(float voltage, float power);
#endif
