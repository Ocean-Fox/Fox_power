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
#define MAX_VOUT_OVP_VAL    22.0f//28V��ѹ����
/*****************************��������*************************/
#define     F_NOERR      		0x0000//�޹���
#define     F_SW_VIN_UVP  		0x0001//����Ƿѹ
#define     F_SW_VIN_OVP    	0x0002//�����ѹ
#define     F_SW_VOUT_UVP  		0x0004//���Ƿѹ
#define     F_SW_VOUT_OVP    	0x0008//�����ѹ
#define     F_SW_IOUT_OCP    	0x0010//�������
#define     F_SW_SHORT  		0x0020//�����·


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
} SState_T;//������ö�ٱ���

//״̬��ö����
typedef enum
{
    Init,   //��ʼ��
    Wait,   //���еȴ�
    Rise,   //����
    Run,    //��������
    Err     //����
} STATE_M;

#define setRegBits(reg, mask)   (reg |= (unsigned int)(mask))
#define clrRegBits(reg, mask)  	(reg &= (unsigned int)(~(unsigned int)(mask)))
#define getRegBits(reg, mask)   (reg & (unsigned int)(mask))
#define getReg(reg)           	(reg)

#endif
