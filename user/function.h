#ifndef __FUNCTION_H_
#define __FUNCTION_H_
#include "ADC_.h"
#include "hrtim.h"
extern uint8_t PwmEnFlag;
extern uint16_t Cap_Error;
extern uint8_t Mode_Change;
extern uint8_t Status_;
extern uint8_t SMFlag;
void VinSwOVP(void);
void VoutSwUVP(void);
void VinSwUVP(void);
void VoutSwOVP(void);
void SwOCP(void);
void ShortOff(void);
void StateM(void);
void Buck_Boost_Status(void);
void Buzzer_error(uint8_t u);
extern float outputDuty;
extern uint8_t Flag_Boost_Error;
#define MAX_VOUT_OVP_VAL    22.0f//28V过压保护
/*****************************故障类型*************************/
#define     F_NOERR      		0x0000//无故障
#define     F_SW_VIN_UVP  		0x0001//输入欠压
#define     F_SW_VIN_OVP    	0x0002//输入过压
#define     F_SW_VOUT_UVP  		0x0004//输出欠压
#define     F_SW_VOUT_OVP    	0x0008//输出过压
#define     F_SW_IOUT_OCP    	0x0010//输出过流
#define     F_SW_SHORT  		0x0020//输出短路


typedef enum
{
	NA0,
	BUCK,
	MIX,
	BOOST
}MODE_T;

typedef enum
{
    SSInit, //soft start initl
    SSWait, //soft start waitting
    SSRun   //start soft start
} SState_T;//软启动枚举变量

//状态机枚举量
typedef enum
{
    Init,   //初始化
    Wait,   //空闲等待
    Rise,   //软启
    Run,    //正常运行
    Err     //故障
} STATE_M;

#define setRegBits(reg, mask)   (reg |= (unsigned int)(mask))
#define clrRegBits(reg, mask)  	(reg &= (unsigned int)(~(unsigned int)(mask)))
#define getRegBits(reg, mask)   (reg & (unsigned int)(mask))
#define getReg(reg)           	(reg)

#endif
