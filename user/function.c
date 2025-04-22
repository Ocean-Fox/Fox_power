#include "function.h"
#include "power_control_fox.h"
#include "tim.h"
#include "fdcan_Init.h"
uint8_t Status_=0;
uint8_t Status_mem;
uint8_t Mode_Change=0;
uint8_t PwmEnFlag=0;
uint8_t SMFlag=0;
uint16_t Cap_Error=0;//1Ϊerror��0Ϊ����״̬
SState_T STState = SSInit;
uint8_t Flag_Boost_Error=0;
float outputDuty;
void StateMRise(void);
void StateMWait(void);
void StateMRun(void);
void StateMErr(void);
/** ===================================================================
 **     Function Name :void StateM(void)
 **     Description :   ״̬������
 **                     state machine function
 **     ��ʼ��״̬
 **     initial state
 **     ��������״̬
 **     wait state
 **     ����״̬
 **     rise state
 **     ����״̬
 **     run state
 **     ����״̬
 **     error state
 **     Parameters  :
 **     Returns     :
 ** ===================================================================*/
void StateM(void)
{
    //�ж�״̬����
    //judge state type
	if(rxData.enableDCDC==0)SMFlag=Init;
	if(rxData.systemRestart ==1){SMFlag=Init;Cap_Error =0;}
    switch (SMFlag)
    {
		case Init:
			SMFlag=Wait;
			break;
        //�ȴ�״̬
    case Wait:
        StateMWait();
        break;
        //������״̬
    case Rise:
        StateMRise();
        break;
        //����״̬
    case Run:
        StateMRun();
        break;
        //����״̬
    case Err:
        StateMErr();
        break;
    }
}
/** ===================================================================
 **     Funtion Name :void BBMode(void)
 **     Description :����ģʽ�ж�
 **      Buckģʽ������ο���ѹ<0.8�������ѹ
 **      MIXģʽ��1.2�������ѹ>����ο���ѹ>0.8�������ѹ
 **      ������MIX��buck-boost��ģʽ���˳��� Buckʱ��Ҫ�ͻ�����ֹ���ٽ��������
 **     Parameters  :
 **     Returns     :
 ** ===================================================================*/
void Buck_Boost_Status(void)
{
	Status_mem=Status_ ;
	switch(Status_)
	{
		case NA0:
		{
			if(V_Cap <0.8f*V_In)
			{
				
					Status_=BUCK;

			}
			else if(V_Cap >0.85f*V_In&&V_Cap <1.25f*V_In)
			{
				Status_=MIX;
			}
			break;
		}
		case BUCK:
		{
			if(V_Cap >0.85f*V_In&&V_Cap <1.25f*V_In)
			{
				Status_ =MIX;
			}
			else if(V_Cap<0.85f*V_In)
			{

					Status_=BUCK;

			}
			break;
		}
		case MIX:
		{
			if(V_Cap <0.8f*V_In)
			{

					Status_=BUCK;

			}

			break;
		}

	}
	if(Status_mem !=Status_)
	{
		Mode_Change=1;
	}
}

/*
 ** ===================================================================
 **     Function Name : void StateMRise(void)
 **     Description :�����׶�
 **     ������ʼ��
 **     �����ȴ�
 **     ��ʼ����
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
#define MAX_CNT			20
void StateMRise(void)
{
	static uint16_t Cnt=0;
	static uint16_t BuckMaxDutyCnt=0;

	
	//�ж�����״̬
	switch(STState)
	{
		case SSInit:
		{
			HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2);
			CtrValue.BuckValue_max = MIN_BUCK_DUTY;
			CtrValue.BoostValue_max = 0.95*27200;
			All_pid_clean();
			STState = SSWait;
			break;
		}
		case SSWait:
		{
			Cnt++;
			if(Cnt>MAX_CNT)
			{
				Cnt = 0;
				All_pid_clean();
				if(V_Cap <=10.0f)
				{
					CtrValue.BuckValue_=V_Cap /V_In*HRTIMAB_Period;
					CtrValue.BuckValue_max=0.3*27200;
					CtrValue.BoostValue_=MAX_BOOST_DUTY;
					CtrValue.BoostValue_max=MAX_BOOST_DUTY;
				}
				else
				{
					CtrValue.BuckValue_=V_Cap /V_In*HRTIMAB_Period;
					CtrValue.BuckValue_max=MAX_BUCK_DUTY;
					CtrValue.BoostValue_=MAX_BOOST_DUTY;
					CtrValue.BoostValue_max=MAX_BOOST_DUTY;
				}
				 STState = SSRun;    //��ת������״̬
			}
			break;
		}
		//������״̬
        //soft start state
		case SSRun:
		{
			if(PwmEnFlag ==0)
			{
				All_pid_clean();
					HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //ͨ����
 //����PWM�����PWM��ʱ��
			}
			PwmEnFlag = 1;
			BuckMaxDutyCnt++;

			CtrValue.BuckValue_max=CtrValue.BuckValue_max+BuckMaxDutyCnt*5;
			if(CtrValue.BuckValue_max>=MAX_BUCK_DUTY)CtrValue.BuckValue_max=MAX_BUCK_DUTY;
			if(CtrValue.BuckValue_max==MAX_BUCK_DUTY)
			{
				SMFlag =Run;
				STState = SSInit;
				BuckMaxDutyCnt=0;
			}
			break;
		}
		
	}
}

/** ===================================================================
 **     Function Name :void Buzzer_error(uint8_t u)
 **     Description :   ����
 **     Parameters  :
 **     Returns     :
 ** ===================================================================*/
void Buzzer_error(uint8_t u)
{
	static uint16_t i=0;
	static uint16_t counter=0;
	counter++;
	if(i<u)
	{
		if(counter ==10*i+1)__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 10000);
		else if(counter == 10*i+5) __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 0);
		else if(counter == 10*i+10)i++;
	}
	if(counter == 100) {i=0;counter=0;}
}

/** ===================================================================
 **     Function Name :void StateMWait(void)
 **     Description :   �ȴ�״̬�����ȴ�1S���޹���������
 **                     wait state, wait for 1s to start when there is no error
 **     Parameters  :
 **     Returns     :
 ** ===================================================================*/
void StateMWait(void)
{
	static uint16_t Cnts=0;
	PwmEnFlag =0;
	Cnts++;
	if(Cnts>256)
	{
		Cnts=256;
	
		if(Cap_Error ==F_NOERR&&rxData.enableDCDC==1)
		{
			Cnts=0;
			SMFlag=Rise;
			STState =SSInit;
		}
	}
}
/*
 ** ===================================================================
 **     Function Name :void StateMRun(void)
 **     Description :�������У������������ж�������
 **                  normal operation, main function is run in interrupt
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
void StateMRun(void)
{
		 
}

/*
 ** ===================================================================
 **     Function Name :void StateMErr(void)
 **     Description :����״̬
 **                  error state
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
void StateMErr(void)
{
	PwmEnFlag =0;
	HAL_HRTIM_WaveformOutputStop(&hhrtim1 , HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�
	if(Cap_Error ==0)
	{
		SMFlag=Wait;
	}
		  
}

/*
 ** ===================================================================
 **     Function Name :void ShortOff(void)
 **     Description :��·��������������10��
 **                  shortage protect, can restart 10 times
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
#define MAX_SHORT_I     14.0f//��·�����о�
//shortage current judgment
#define MIN_SHORT_V     0.1f//��·��ѹ�о�
//shortage voltage judgment
void ShortOff(void)
{
	static uint16_t RSCnt =0;
	static uint16_t RSNum=0;
	//��output current���� 15A����output voltageС��0.2Vʱ�����ж�Ϊ������·����
	if((I_Cap>MAX_SHORT_I )&&(V_Cap<MIN_SHORT_V ))
	{
		PwmEnFlag =0;
		HAL_HRTIM_WaveformOutputStop(&hhrtim1 ,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�
		setRegBits(Cap_Error, F_SW_SHORT);
		SMFlag=Err;
	}
	if(getRegBits(Cap_Error, F_SW_SHORT))
	{
		RSCnt++;
		if(RSCnt >400)
		{
			RSCnt=0;
			if(RSNum >10)
			{
				RSNum=11;
				PwmEnFlag =0;
				HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�
			}
			else
			{
				//��·�����������ۼ�
                //shortage protect counter add
                RSNum++;
				//��������������ϱ�־λ
                //clean shortage protect flag
                clrRegBits(Cap_Error, F_SW_SHORT);
			}
		}
	}
}
/*
 ** ===================================================================
 **     Function Name :void SwOCP(void)
 **     Description :�������������������
 **                  software shortage protect, can restart
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
#define MAX_OCP_VAL     13.0f//13A����������
void SwOCP(void)
{
	//���������оݱ��ּ���������
    static uint16_t OCPCnt = 0;
    //����������ּ���������
    static uint16_t RSCnt = 0;
    //������������������
    static uint16_t RSNum = 0;
	
	if(((I_Cap >MAX_OCP_VAL )||(I_Cap <-MAX_OCP_VAL))&&(SMFlag==Run))
	{
		//�������ּ�ʱ
        OCPCnt++;
        //��������50ms������Ϊ��������
        if (OCPCnt > 10)
        {
            //��������0
            OCPCnt = 0;
            //�ر�PWM
			PwmEnFlag =0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�
			//���ϱ�־λ
			setRegBits (Cap_Error ,F_SW_IOUT_OCP);
			SMFlag=Err;
		}
		else 
		{
			 //��������0
			OCPCnt = 0;
		}
		//���������ָ�
		//�����������������������ػ���ȴ�4S�����������Ϣ������ȴ�״̬�ȴ�����
		if(getRegBits(Cap_Error ,F_SW_IOUT_OCP ))
		{
			 RSCnt++;
        //�ȴ�2S
        if (RSCnt > 400)
        {
            //����������
            RSCnt = 0;
            //���������������ۼ�
            RSNum++;
            //��������ֻ����10�Σ�10�κ����������ع��ϣ�
            if (RSNum > 10)
            {
                //ȷ����������ϣ�������
                RSNum = 11;
                //�ر�PWM
                PwmEnFlag  = 0;
                HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�
            }
            else
            {
                //��������������ϱ�־λ
                clrRegBits(Cap_Error, F_SW_IOUT_OCP);
            }
        }
		}
	}
}
/*
 ** ===================================================================
 **     Function Name :void SwOVP(void)
 **     Description :��������ѹ������������
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */

void VoutSwOVP(void)
{
    //��ѹ�����оݱ��ּ���������
    static uint16_t OVPCnt = 0;
    //����������ּ���������

    //��output voltage����24V���ұ���10ms
    if (V_Cap  > MAX_VOUT_OVP_VAL)
    {
        //�������ּ�ʱ
        OVPCnt++;
        //��������10ms
        if (OVPCnt > 2)
        {
            //��ʱ������
            OVPCnt = 0;
            //�ر�PWM
            PwmEnFlag  = 0;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�
            //���ϱ�־λ
            setRegBits(Cap_Error , F_SW_VOUT_OVP);
            //��ת������״̬
            SMFlag = Err;
        }
    }
    else
        OVPCnt = 0;
}

/*
 ** ===================================================================
 **     Function Name :void VinSwUVP(void)
 **     Description :�������Ƿѹ��������ѹ���뱣��,�ɻָ�
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
#define MIN_UVP_VAL    19.0f//20VǷѹ����
#define MIN_UVP_VAL_RE 20.0f//21.0VǷѹ�����ָ�
void VinSwUVP(void)
{
    //��ѹ�����оݱ��ּ���������
    static uint16_t UVPCnt = 0;
    static uint16_t RSCnt = 0;

    //��input voltageС��20V���ұ���10ms
    if ((V_In < MIN_UVP_VAL) && (SMFlag != Init) && (!Mode_Change))
    {
        //�������ּ�ʱ
        UVPCnt++;
        //��������10ms
        if (UVPCnt > 2)
        {
            //��ʱ������
            UVPCnt = 0;
            RSCnt = 0;
            //�ر�PWM
            PwmEnFlag  = 0;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�
            //���ϱ�־λ
            setRegBits(Cap_Error , F_SW_VIN_UVP);
            //��ת������״̬
            SMFlag = Err;
        }
    }
    else
        UVPCnt = 0;

    //����Ƿѹ�����ָ�
    //����������Ƿѹ�������ȴ������ѹ�ָ�������ˮƽ��������ϱ�־λ������
    if (getRegBits(Cap_Error , F_SW_VIN_UVP))
    {
        if (V_In > MIN_UVP_VAL_RE)
        {
            //�ȴ���������������ۼ�
            RSCnt++;
            //�ȴ�1S
            if (RSCnt > 200)
            {
                RSCnt = 0;
                UVPCnt = 0;
                //������ϱ�־λ
                clrRegBits(Cap_Error , F_SW_VIN_UVP);
            }
        }
        else
            RSCnt = 0;
    }
    else
        RSCnt = 0;
}
/*
 ** ===================================================================
 **     Function Name :void VinSwUVP(void)
 **     Description :������Ƿѹ��������ѹ���뱣��,�ɻָ�
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
#define MIN_OVP_VAL    11.0f//20VǷѹ����
#define MIN_OVP_VAL_RE 12.0f//21.0VǷѹ�����ָ�
void VoutSwUVP(void)
{
    //��ѹ�����оݱ��ּ���������
    static uint16_t UVPCnt = 0;
    static uint16_t RSCnt = 0;

    //��input voltageС��20V���ұ���10ms
    if ((V_Cap < MIN_OVP_VAL) && (SMFlag != Init) && (!Mode_Change)&&(V_In *I_In>= REFEREE_SESTEM_POWER))
    {
        //�������ּ�ʱ
        UVPCnt++;
        //��������10ms
        if (UVPCnt > 2)
        {
            //��ʱ������
            UVPCnt = 0;
            RSCnt = 0;
            //�ر�PWM
            PwmEnFlag  = 0;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�
            //���ϱ�־λ
            setRegBits(Cap_Error , F_SW_VOUT_UVP);
            //��ת������״̬
            SMFlag = Err;
        }
    }
    else
        UVPCnt = 0;

    //����Ƿѹ�����ָ�
    //����������Ƿѹ�������ȴ������ѹ�ָ�������ˮƽ��������ϱ�־λ������
    if (getRegBits(Cap_Error , F_SW_VOUT_UVP))
    {
        if (V_In > MIN_OVP_VAL_RE||(V_In *I_In<= REFEREE_SESTEM_POWER))
        {
            //�ȴ���������������ۼ�
            RSCnt++;
            //�ȴ�1S
            if (RSCnt > 200)
            {
                RSCnt = 0;
                UVPCnt = 0;

                //������ϱ�־λ
                clrRegBits(Cap_Error ,F_SW_VOUT_UVP);
            }
        }
        else
            RSCnt = 0;
    }
    else
        RSCnt = 0;
}

/*
 ** ===================================================================
 **     Funtion Name :void VinSwOVP(void)
 **     Description :��������ѹ������������
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
#define MAX_VIN_OVP_VAL    28.0f//28V��ѹ����
void VinSwOVP(void)
{
    //��ѹ�����оݱ��ּ���������
    static uint16_t OVPCnt = 0;

    //��input voltage����28V���ұ���100ms
    if (V_In > MAX_VIN_OVP_VAL)
    {
        //�������ּ�ʱ
        OVPCnt++;
        //��������10ms
        if (OVPCnt > 2)
        {
            //��ʱ������
            OVPCnt = 0;
            //�ر�PWM
            PwmEnFlag  = 0;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //�ر�
            //���ϱ�־λ
            setRegBits(Cap_Error , F_SW_VIN_OVP);
            //��ת������״̬
            SMFlag = Err;
        }
    }
    else
        OVPCnt = 0;
}
