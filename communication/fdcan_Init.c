#include "fdcan_Init.h"

FDCAN_TxHeaderTypeDef TxHeader;
FDCAN_RxHeaderTypeDef rxHeader;
#include <string.h>

void FDCAN_Config_Filter(void) {
    FDCAN_FilterTypeDef sFilterConfig;

    sFilterConfig.IdType = FDCAN_STANDARD_ID;   // 使用标准 CAN ID
    sFilterConfig.FilterIndex = 0;              // 过滤器索引 0
    sFilterConfig.FilterType = FDCAN_FILTER_MASK; // 掩码过滤器
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // 过滤后进入 RX FIFO0
    sFilterConfig.FilterID1 = 0x061;            // 目标 ID
    sFilterConfig.FilterID2 = 0x7FF;            // 过滤器掩码（全匹配）

    if (HAL_FDCAN_ConfigFilter(&hfdcan3, &sFilterConfig) != HAL_OK) {

    }

    // ? 启用接收通知中断
    if (HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
        Error_Handler();
    }
	HAL_FDCAN_Start(&hfdcan3);
}


/* 初始化 FDCAN3 */
void FDCAN3_Send_Msg(uint16_t ID,uint8_t* msg)
{	
		
	FDCAN_TxHeaderTypeDef TxHeader;
	
	TxHeader.Identifier          = ID;  // 设置 CAN ID (标准或扩展)
	TxHeader.IdType              = FDCAN_STANDARD_ID;  // 标准ID或扩展ID
	TxHeader.TxFrameType         = FDCAN_DATA_FRAME;   // 数据帧 or 远程帧
	TxHeader.DataLength          = FDCAN_DLC_BYTES_8;  // 数据长度 (DLC)
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;  // 错误状态指示
	TxHeader.BitRateSwitch       = FDCAN_BRS_OFF;      // 关闭或开启BRS
	TxHeader.FDFormat            = FDCAN_CLASSIC_CAN;  // 经典CAN 或 CAN FD
	TxHeader.TxEventFifoControl  = FDCAN_NO_TX_EVENTS; // 发送事件FIFO
	TxHeader.MessageMarker       = 0x52;  // 0x00 ~ 0xFF 之间
	
	HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &TxHeader, msg);
}
RxData rxData;

/* 接收 fdcan3 消息 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	
    uint8_t rxBuffer[sizeof(RxData)];
        if (HAL_FDCAN_GetRxMessage(&hfdcan3, FDCAN_RX_FIFO0, &rxHeader,  rxBuffer) == HAL_OK)
        {
			if (rxHeader.Identifier == 0x061 && rxHeader.DataLength == FDCAN_DLC_BYTES_8) {
				
                memcpy(&rxData, rxBuffer, sizeof(RxData));}
        }

}
void FDCAN3_Send_PowerInfo(float voltage, float power)
{
    uint8_t txBuffer1[8];
    
    // 打包 float 到 uint8_t 数组（使用 memcpy）
    memcpy(&txBuffer1[0], &voltage, 4);     // 前4字节是电压
    memcpy(&txBuffer1[4], &power, 4);       // 后4字节是底盘功率

    // 发送第一帧（ID: 0x100）
    FDCAN3_Send_Msg(0x051, txBuffer1);

}
