#include "fdcan_Init.h"

FDCAN_TxHeaderTypeDef TxHeader;
FDCAN_RxHeaderTypeDef rxHeader;
#include <string.h>

void FDCAN_Config_Filter(void) {
    FDCAN_FilterTypeDef sFilterConfig;

    sFilterConfig.IdType = FDCAN_STANDARD_ID;   // ʹ�ñ�׼ CAN ID
    sFilterConfig.FilterIndex = 0;              // ���������� 0
    sFilterConfig.FilterType = FDCAN_FILTER_MASK; // ���������
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // ���˺���� RX FIFO0
    sFilterConfig.FilterID1 = 0x061;            // Ŀ�� ID
    sFilterConfig.FilterID2 = 0x7FF;            // ���������루ȫƥ�䣩

    if (HAL_FDCAN_ConfigFilter(&hfdcan3, &sFilterConfig) != HAL_OK) {

    }

    // ? ���ý���֪ͨ�ж�
    if (HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
        Error_Handler();
    }
	HAL_FDCAN_Start(&hfdcan3);
}


/* ��ʼ�� FDCAN3 */
void FDCAN3_Send_Msg(uint16_t ID,uint8_t* msg)
{	
		
	FDCAN_TxHeaderTypeDef TxHeader;
	
	TxHeader.Identifier          = ID;  // ���� CAN ID (��׼����չ)
	TxHeader.IdType              = FDCAN_STANDARD_ID;  // ��׼ID����չID
	TxHeader.TxFrameType         = FDCAN_DATA_FRAME;   // ����֡ or Զ��֡
	TxHeader.DataLength          = FDCAN_DLC_BYTES_8;  // ���ݳ��� (DLC)
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;  // ����״ָ̬ʾ
	TxHeader.BitRateSwitch       = FDCAN_BRS_OFF;      // �رջ���BRS
	TxHeader.FDFormat            = FDCAN_CLASSIC_CAN;  // ����CAN �� CAN FD
	TxHeader.TxEventFifoControl  = FDCAN_NO_TX_EVENTS; // �����¼�FIFO
	TxHeader.MessageMarker       = 0x52;  // 0x00 ~ 0xFF ֮��
	
	HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &TxHeader, msg);
}
RxData rxData;

/* ���� fdcan3 ��Ϣ */
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
    
    // ��� float �� uint8_t ���飨ʹ�� memcpy��
    memcpy(&txBuffer1[0], &voltage, 4);     // ǰ4�ֽ��ǵ�ѹ
    memcpy(&txBuffer1[4], &power, 4);       // ��4�ֽ��ǵ��̹���

    // ���͵�һ֡��ID: 0x100��
    FDCAN3_Send_Msg(0x051, txBuffer1);

}
