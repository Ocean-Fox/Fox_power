#include "vofa_float.h"


#define BYTE0(dwTemp)       (*(char *)(dwTemp))
#define BYTE1(dwTemp)       (*((char *)(dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(dwTemp) + 3))
#define RX_BUFFER_SIZE 6

uint8_t rxBuffer[RX_BUFFER_SIZE];  // 接收缓冲区
unsigned char data_to_send[4*MAX_CHANNEL+4] = {0}; //使用DMA发送数据时，由于指向的是地址，需要是全局地址，否则真正发送数据时数据已经没了!!!!!!!!!!
float *UserData[MAX_CHANNEL]={0};//only transmit float
unsigned char Data_Number = 0;

/**
	*@brief 串口消息初始化
	*@param void
*/
void Upper_Computer_Init(float* addr)
{
    if(Data_Number < MAX_CHANNEL)UserData[Data_Number++]=addr;
}

/**
	*@brief 发送串口消息
	*@param void
*/
void Upper_Computer_Show_Wave(void)
{
	unsigned char cnt = 0;

	for(int i=0;i<Data_Number;i++)
	{
		data_to_send[cnt++] = BYTE0(UserData[i]);
		data_to_send[cnt++] = BYTE1(UserData[i]);
		data_to_send[cnt++] = BYTE2(UserData[i]);
		data_to_send[cnt++] = BYTE3(UserData[i]);
	}

	data_to_send[cnt++] = 0x00;
	data_to_send[cnt++] = 0x00;
	data_to_send[cnt++] = 0x80;
	data_to_send[cnt++] = 0x7F;
	//-------------user change-----------------
     HAL_UART_Transmit(&huart1, data_to_send, cnt,100);
	//----------------end----------------------
}

/**
	*@brief 启动串口消息
	*@param void
*/

void vofa_begin(void)
{
	 HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t *)rxBuffer, RX_BUFFER_SIZE);
}
