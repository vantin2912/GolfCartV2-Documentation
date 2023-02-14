/*
 * Cantp.c
 *
 *  Created on: Nov 30, 2022
 *      Author: vanti
 */


#include "CanTP.h"
#include "main.h"

static HAL_StatusTypeDef CanTP_WaitData(CanTP_HandleTypedef* hCanTP, uint32_t Timeout)
{
	int Status = osEventFlagsWait(hCanTP->Status, CANTP_RCV_DoneFlag, osFlagsWaitAll, Timeout);
	if (Status == osFlagsErrorTimeout) return HAL_TIMEOUT;
	else if( Status < 0 )return HAL_ERROR;
	return HAL_OK;
}

HAL_StatusTypeDef CanTP_RcvCBHandler(CanTP_HandleTypedef* hCanTP, uint8_t* Data, uint8_t DataLen)
{
	memcpy(hCanTP->CanRxBuffer, Data, DataLen);
	hCanTP->CanRxLen = DataLen;
	osEventFlagsSet(hCanTP->Status, CANTP_RCV_DoneFlag);
	return HAL_OK;
}

static HAL_StatusTypeDef CanTP_TxSF(CanTP_HandleTypedef* hCanTP, CanTP_SFFrameTypedef FrameInfo)
{
	uint8_t Payload[8] = {0};
	if(FrameInfo.SF_DL > 7) return HAL_ERROR;
	Payload[0] = CANTP_SF_ID | FrameInfo.SF_DL;
	memcpy(Payload + 1, FrameInfo.Data, FrameInfo.SF_DL);
	memset(Payload + 1 + FrameInfo.SF_DL, 0x55, 8 - FrameInfo.SF_DL - 1);
	return CAN_Transmit(hCanTP->hcan, hCanTP->CanTxID, Payload, 8);
}

static HAL_StatusTypeDef CanTP_TxFF(CanTP_HandleTypedef* hCanTP, CanTP_FFFrameTypedef FrameInfo)
{
	uint8_t Payload[8] = {0};
	Payload[0] = CANTP_FF_ID | (FrameInfo.FF_DL>>8);
	Payload[1] = FrameInfo.FF_DL;
	memcpy(Payload + 2, FrameInfo.Data, 6);
	return CAN_Transmit(hCanTP->hcan, hCanTP->CanTxID, Payload, 8);
}

static HAL_StatusTypeDef CanTP_TxCF(CanTP_HandleTypedef* hCanTP, CanTP_CFFrameTypedef FrameInfo)
{
	uint8_t Payload[8] = {0};
	Payload[0] = CANTP_CF_ID | FrameInfo.SN;
	memcpy(Payload + 1, FrameInfo.Data, FrameInfo.DataLen);
	if(FrameInfo.DataLen < 7)
	{
		memset(Payload + 1 + FrameInfo.DataLen, 0x55, 7 - FrameInfo.DataLen);
	}
	return CAN_Transmit(hCanTP->hcan, hCanTP->CanTxID, Payload, 8);
}

static HAL_StatusTypeDef CanTP_TxFC(CanTP_HandleTypedef* hCanTP, CanTP_FCFrameTypedef FrameInfo)
{
	uint8_t Payload[8] = {0};
	Payload[0] = CANTP_FC_ID | FrameInfo.FS;
	Payload[1] =  FrameInfo.BS;
	Payload[2] = FrameInfo.STmin;
	memset(Payload + 3, 0x55, 5);
	return CAN_Transmit(hCanTP->hcan, hCanTP->CanTxID, Payload, 8);
}
static uint8_t CanTP_GetFrameType(uint8_t Data[])
{
	return Data[0] & 0xf0;
}


static HAL_StatusTypeDef CanTP_WaitFC(CanTP_HandleTypedef* hCanTP, CanTP_FCFrameTypedef* FrameInfo, uint32_t Timeout)
{
	CanTP_WaitData(hCanTP,Timeout);
	uint8_t RcvData[8] = {0};
	memcpy(RcvData, hCanTP->CanRxBuffer, hCanTP->CanRxLen);
	memset(hCanTP->CanRxBuffer, 0, 8);
	hCanTP->CanRxLen = 0;
	if((RcvData[0]& 0xf0) != CANTP_FC_ID) return HAL_ERROR;
	FrameInfo->BS = RcvData[1];
	FrameInfo->STmin = RcvData[2];
	FrameInfo->FS = RcvData[0] &0x0f;
	return HAL_OK;
}
static HAL_StatusTypeDef CanTP_WaitCF(CanTP_HandleTypedef* hCanTP, CanTP_CFFrameTypedef* FrameInfo, uint32_t Timeout)
{
	CanTP_WaitData(hCanTP,Timeout);
	uint8_t RcvData[8] = {0};
	uint8_t RcvLen = hCanTP->CanRxLen;
	memcpy(RcvData, hCanTP->CanRxBuffer, hCanTP->CanRxLen);
	memset(hCanTP->CanRxBuffer, 0, 8);
	hCanTP->CanRxLen = 0;
	if((RcvData[0]& 0xf0) != CANTP_CF_ID) return HAL_ERROR;
	FrameInfo->SN = RcvData[0]&0x0f;
	FrameInfo->DataLen = RcvLen - 1;
	memcpy(FrameInfo->Data, RcvData+1, RcvLen);
	return HAL_OK;
}

static HAL_StatusTypeDef CanTP_TxSingleFrame(CanTP_HandleTypedef* hCanTP, CanTP_Packet Packet)
{
	CanTP_SFFrameTypedef MsgFrame;
	MsgFrame.SF_DL = Packet.Len;
	MsgFrame.Data = Packet.Data;
	HAL_CheckERR(CanTP_TxSF(hCanTP, MsgFrame));
	return HAL_OK;
}


static HAL_StatusTypeDef CanTP_TxMultiFrame(CanTP_HandleTypedef* hCanTP, CanTP_Packet Packet)
{
	CanTP_FFFrameTypedef FFFrame = {0};
	CanTP_CFFrameTypedef CFFrame = {0};
	CanTP_FCFrameTypedef FCFrame = {0};

	FFFrame.FF_DL = Packet.Len;
	FFFrame.Data = Packet.Data;
	HAL_CheckERR(CanTP_TxFF(hCanTP, FFFrame));
	HAL_CheckERR(CanTP_WaitFC(hCanTP, &FCFrame, 200));
	uint16_t DataSent = 6;
	while (DataSent < Packet.Len)
	{
		CFFrame.Data = Packet.Data + DataSent;
		CFFrame.DataLen = (Packet.Len - DataSent < 7) ? Packet.Len - DataSent : 7;
		CFFrame.SN++;
		osDelay(5);
//		SyncPrintf("CF Tx Len %d, SN %d \r\n", CFFrame.DataLen, CFFrame.SN);
		HAL_CheckERR(CanTP_TxCF(hCanTP, CFFrame));
		DataSent += CFFrame.DataLen;
	}
	return HAL_OK;
}


HAL_StatusTypeDef CanTP_Init(CanTP_HandleTypedef* hCanTP)
{
	HAL_CheckERR(HAL_CAN_ConfigFilter(hCanTP->hcan, &(hCanTP->CanFilter)));
	if(hCanTP->CanFilter.FilterFIFOAssignment == CAN_FilterFIFO0)
	{
		HAL_CheckERR(HAL_CAN_ActivateNotification(hCanTP->hcan, CAN_IT_RX_FIFO0_MSG_PENDING));
	}
	else
	{
		HAL_CheckERR(HAL_CAN_ActivateNotification(hCanTP->hcan, CAN_IT_RX_FIFO1_MSG_PENDING));
	}
	HAL_CAN_ActivateNotification(hCanTP->hcan, CAN_IT_ERROR | CAN_IT_LAST_ERROR_CODE);
	HAL_CheckERR(HAL_CAN_Start(hCanTP->hcan));
	return HAL_OK;
}

HAL_StatusTypeDef CanTP_Transmit(CanTP_HandleTypedef* hCanTP, CanTP_Packet Packet)
{

	if(Packet.Len <= 7)
	{
		HAL_CheckERR(CanTP_TxSingleFrame(hCanTP, Packet));
	}
	else
	{
		HAL_CheckERR(CanTP_TxMultiFrame(hCanTP, Packet));
	}
	return HAL_OK;
}


HAL_StatusTypeDef CanTP_Receive(CanTP_HandleTypedef* hCanTP, CanTP_Packet* Packet, uint32_t Timeout)
{
	HAL_CheckERR(CanTP_WaitData(hCanTP,Timeout));
	uint8_t FrameType = CanTP_GetFrameType(hCanTP->CanRxBuffer);
	if(FrameType == CANTP_SF_ID)
	{
		uint8_t* RcvData = hCanTP->CanRxBuffer;
		Packet->Len = RcvData[0] & 0x0f;
		Packet->Data = pvPortMalloc(Packet->Len);
		memcpy(Packet->Data, RcvData + 1, Packet->Len);
	}
	else if(FrameType == CANTP_FF_ID)
	{
		CanTP_CFFrameTypedef CFFrame = {0};
		CanTP_FCFrameTypedef FCFrame = {0};

		uint8_t* RcvData = hCanTP->CanRxBuffer;
		uint16_t FF_DL = ((RcvData[0] & 0x0f)<<8) | RcvData[1] ;
		Packet->Len = FF_DL;
		Packet->Data = pvPortMalloc(Packet->Len + 7);
		memcpy(Packet->Data, RcvData + 2, 6);
		FCFrame.BS = 8;
		FCFrame.FS = 0;
		FCFrame.STmin = 25;
		CanTP_TxFC(hCanTP, FCFrame);

		uint16_t DataReceived = 6;
		uint8_t CurrentSN = 1;
		CFFrame.Data = (uint8_t*)pvPortMalloc(8);
		while(DataReceived < FF_DL)
		{
			HAL_CheckERR(CanTP_WaitCF(hCanTP, &CFFrame, Timeout));
			if(CurrentSN != CFFrame.SN ) return HAL_ERROR;
			memcpy(Packet->Data + DataReceived, CFFrame.Data, 7);
			DataReceived += CFFrame.DataLen;
			CurrentSN++;
		}
		vPortFree(CFFrame.Data);
		Packet->Data[Packet->Len] = 0;
		SyncPrintf("Done Rcv \r\n");
	}
	else
	{
		return HAL_ERROR;
	}
	return HAL_OK;

}
HAL_StatusTypeDef CanTP_RcvState(CanTP_HandleTypedef* hCanTP);
HAL_StatusTypeDef CanTP_GetData(CanTP_HandleTypedef* hCanTP);



