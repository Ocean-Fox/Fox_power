#include "function.h"
#include "power_control_fox.h"
#include "tim.h"
#include "fdcan_Init.h"
uint8_t Status_=0;
uint8_t Status_mem;
uint8_t Mode_Change=0;
uint8_t PwmEnFlag=0;
uint8_t SMFlag=0;
uint16_t Cap_Error=0;//1为error，0为正常状态
SState_T STState = SSInit;
uint8_t Flag_Boost_Error=0;
float outputDuty;
void StateMRise(void);
void StateMWait(void);
void StateMRun(void);
void StateMErr(void);
/** ===================================================================
 **     Function Name :void StateM(void)
 **     Description :   状态机函数
 **                     state machine function
 **     初始化状态
 **     initial state
 **     等外启动状态
 **     wait state
 **     启动状态
 **     rise state
 **     运行状态
 **     run state
 **     故障状态
 **     error state
 **     Parameters  :
 **     Returns     :
 ** ===================================================================*/
void StateM(void)
{
    //判断状态类型
    //judge state type
	if(rxData.enableDCDC==0)SMFlag=Init;
	if(rxData.systemRestart ==1){SMFlag=Init;Cap_Error =0;}
    switch (SMFlag)
    {
		case Init:
			SMFlag=Wait;
			break;
        //等待状态
    case Wait:
        StateMWait();
        break;
        //软启动状态
    case Rise:
        StateMRise();
        break;
        //运行状态
    case Run:
        StateMRun();
        break;
        //故障状态
    case Err:
        StateMErr();
        break;
    }
}
/** ===================================================================
 **     Funtion Name :void BBMode(void)
 **     Description :运行模式判断
 **      Buck模式：输出参考电压<0.8倍输入电压
 **      MIX模式：1.2倍输入电压>输出参考电压>0.8倍输入电压
 **      当进入MIX（buck-boost）模式后，退出到 Buck时需要滞缓，防止在临界点来回振荡
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
 **     Description :软启阶段
 **     软启初始化
 **     软启等待
 **     开始软启
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
#define MAX_CNT			20
void StateMRise(void)
{
	static uint16_t Cnt=0;
	static uint16_t BuckMaxDutyCnt=0;

	
	//判断软启状态
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
				 STState = SSRun;    //跳转至软启状态
			}
			break;
		}
		//软启动状态
        //soft start state
		case SSRun:
		{
			if(PwmEnFlag ==0)
			{
				All_pid_clean();
					HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //通道打开
 //开启PWM输出和PWM计时器
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
 **     Description :   报警
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
 **     Description :   等待状态机，等待1S后无故障则软启
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
 **     Description :正常运行，主处理函数在中断中运行
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
 **     Description :故障状态
 **                  error state
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
void StateMErr(void)
{
	PwmEnFlag =0;
	HAL_HRTIM_WaveformOutputStop(&hhrtim1 , HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭
	if(Cap_Error ==0)
	{
		SMFlag=Wait;
	}
		  
}

/*
 ** ===================================================================
 **     Function Name :void ShortOff(void)
 **     Description :短路保护，可以重启10次
 **                  shortage protect, can restart 10 times
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
#define MAX_SHORT_I     14.0f//短路电流判据
//shortage current judgment
#define MIN_SHORT_V     0.1f//短路电压判据
//shortage voltage judgment
void ShortOff(void)
{
	static uint16_t RSCnt =0;
	static uint16_t RSNum=0;
	//当output current大于 15A，且output voltage小于0.2V时，可判定为发生短路保护
	if((I_Cap>MAX_SHORT_I )&&(V_Cap<MIN_SHORT_V ))
	{
		PwmEnFlag =0;
		HAL_HRTIM_WaveformOutputStop(&hhrtim1 ,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭
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
				HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭
			}
			else
			{
				//短路重启计数器累加
                //shortage protect counter add
                RSNum++;
				//清除过流保护故障标志位
                //clean shortage protect flag
                clrRegBits(Cap_Error, F_SW_SHORT);
			}
		}
	}
}
/*
 ** ===================================================================
 **     Function Name :void SwOCP(void)
 **     Description :软件过流保护，可重启
 **                  software shortage protect, can restart
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
#define MAX_OCP_VAL     13.0f//13A过流保护点
void SwOCP(void)
{
	//过流保护判据保持计数器定义
    static uint16_t OCPCnt = 0;
    //故障清楚保持计数器定义
    static uint16_t RSCnt = 0;
    //保留保护重启计数器
    static uint16_t RSNum = 0;
	
	if(((I_Cap >MAX_OCP_VAL )||(I_Cap <-MAX_OCP_VAL))&&(SMFlag==Run))
	{
		//条件保持计时
        OCPCnt++;
        //条件保持50ms，则认为过流发生
        if (OCPCnt > 10)
        {
            //计数器清0
            OCPCnt = 0;
            //关闭PWM
			PwmEnFlag =0;
			HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭
			//故障标志位
			setRegBits (Cap_Error ,F_SW_IOUT_OCP);
			SMFlag=Err;
		}
		else 
		{
			 //计数器清0
			OCPCnt = 0;
		}
		//输出过流后恢复
		//当发生输出软件过流保护，关机后等待4S后清除故障信息，进入等待状态等待重启
		if(getRegBits(Cap_Error ,F_SW_IOUT_OCP ))
		{
			 RSCnt++;
        //等待2S
        if (RSCnt > 400)
        {
            //计数器清零
            RSCnt = 0;
            //过流重启计数器累加
            RSNum++;
            //过流重启只重启10次，10次后不重启（严重故障）
            if (RSNum > 10)
            {
                //确保不清除故障，不重启
                RSNum = 11;
                //关闭PWM
                PwmEnFlag  = 0;
                HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭
            }
            else
            {
                //清除过流保护故障标志位
                clrRegBits(Cap_Error, F_SW_IOUT_OCP);
            }
        }
		}
	}
}
/*
 ** ===================================================================
 **     Function Name :void SwOVP(void)
 **     Description :软件输出过压保护，不重启
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */

void VoutSwOVP(void)
{
    //过压保护判据保持计数器定义
    static uint16_t OVPCnt = 0;
    //故障清楚保持计数器定义

    //当output voltage大于24V，且保持10ms
    if (V_Cap  > MAX_VOUT_OVP_VAL)
    {
        //条件保持计时
        OVPCnt++;
        //条件保持10ms
        if (OVPCnt > 2)
        {
            //计时器清零
            OVPCnt = 0;
            //关闭PWM
            PwmEnFlag  = 0;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭
            //故障标志位
            setRegBits(Cap_Error , F_SW_VOUT_OVP);
            //跳转至故障状态
            SMFlag = Err;
        }
    }
    else
        OVPCnt = 0;
}

/*
 ** ===================================================================
 **     Function Name :void VinSwUVP(void)
 **     Description :输入软件欠压保护，低压输入保护,可恢复
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
#define MIN_UVP_VAL    19.0f//20V欠压保护
#define MIN_UVP_VAL_RE 20.0f//21.0V欠压保护恢复
void VinSwUVP(void)
{
    //过压保护判据保持计数器定义
    static uint16_t UVPCnt = 0;
    static uint16_t RSCnt = 0;

    //当input voltage小于20V，且保持10ms
    if ((V_In < MIN_UVP_VAL) && (SMFlag != Init) && (!Mode_Change))
    {
        //条件保持计时
        UVPCnt++;
        //条件保持10ms
        if (UVPCnt > 2)
        {
            //计时器清零
            UVPCnt = 0;
            RSCnt = 0;
            //关闭PWM
            PwmEnFlag  = 0;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭
            //故障标志位
            setRegBits(Cap_Error , F_SW_VIN_UVP);
            //跳转至故障状态
            SMFlag = Err;
        }
    }
    else
        UVPCnt = 0;

    //输入欠压保护恢复
    //当发生输入欠压保护，等待输入电压恢复至正常水平后清楚故障标志位，重启
    if (getRegBits(Cap_Error , F_SW_VIN_UVP))
    {
        if (V_In > MIN_UVP_VAL_RE)
        {
            //等待故障清楚计数器累加
            RSCnt++;
            //等待1S
            if (RSCnt > 200)
            {
                RSCnt = 0;
                UVPCnt = 0;
                //清楚故障标志位
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
 **     Description :输出软件欠压保护，低压输入保护,可恢复
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
#define MIN_OVP_VAL    11.0f//20V欠压保护
#define MIN_OVP_VAL_RE 12.0f//21.0V欠压保护恢复
void VoutSwUVP(void)
{
    //过压保护判据保持计数器定义
    static uint16_t UVPCnt = 0;
    static uint16_t RSCnt = 0;

    //当input voltage小于20V，且保持10ms
    if ((V_Cap < MIN_OVP_VAL) && (SMFlag != Init) && (!Mode_Change)&&(V_In *I_In>= REFEREE_SESTEM_POWER))
    {
        //条件保持计时
        UVPCnt++;
        //条件保持10ms
        if (UVPCnt > 2)
        {
            //计时器清零
            UVPCnt = 0;
            RSCnt = 0;
            //关闭PWM
            PwmEnFlag  = 0;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭
            //故障标志位
            setRegBits(Cap_Error , F_SW_VOUT_UVP);
            //跳转至故障状态
            SMFlag = Err;
        }
    }
    else
        UVPCnt = 0;

    //输入欠压保护恢复
    //当发生输入欠压保护，等待输入电压恢复至正常水平后清楚故障标志位，重启
    if (getRegBits(Cap_Error , F_SW_VOUT_UVP))
    {
        if (V_In > MIN_OVP_VAL_RE||(V_In *I_In<= REFEREE_SESTEM_POWER))
        {
            //等待故障清楚计数器累加
            RSCnt++;
            //等待1S
            if (RSCnt > 200)
            {
                RSCnt = 0;
                UVPCnt = 0;

                //清楚故障标志位
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
 **     Description :软件输入过压保护，不重启
 **     Parameters  : none
 **     Returns     : none
 ** ===================================================================
 */
#define MAX_VIN_OVP_VAL    28.0f//28V过压保护
void VinSwOVP(void)
{
    //过压保护判据保持计数器定义
    static uint16_t OVPCnt = 0;

    //当input voltage大于28V，且保持100ms
    if (V_In > MAX_VIN_OVP_VAL)
    {
        //条件保持计时
        OVPCnt++;
        //条件保持10ms
        if (OVPCnt > 2)
        {
            //计时器清零
            OVPCnt = 0;
            //关闭PWM
            PwmEnFlag  = 0;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2|HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //关闭
            //故障标志位
            setRegBits(Cap_Error , F_SW_VIN_OVP);
            //跳转至故障状态
            SMFlag = Err;
        }
    }
    else
        OVPCnt = 0;
}
