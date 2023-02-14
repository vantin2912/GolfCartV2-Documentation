/*
 * Cantp.h
 *
 *  Created on: Nov 30, 2022
 *      Author: vanti
 */

#ifndef CANTP_H_
#define CANTP_H_
#include "CANCom.h"

#define CANTP_SF_ID	00<<4
#define CANTP_FF_ID 01<<4
#define CANTP_CF_ID 10<<4
#define CANTP_FC_ID	11<<4

#define HAL_CheckERR(__Status__) if(__Status__ != HAL_OK ) return __Status__

#define CANTP_RCV_DoneFlag 0x01


typedef struct CanTP_Packet
{
	uint16_t Len;
	uint8_t* Data;
} CanTP_Packet;

typedef struct CanTP_HandleTypedef
{
	uint16_t CanTxID;
	uint16_t CanRxID;
	CAN_HandleTypeDef* hcan;
	CAN_FilterTypeDef CanFilter;
	osEventFlagsId_t Status;
	uint8_t CanRxBuffer[8];
	uint8_t CanRxLen;
} CanTP_HandleTypedef;

typedef struct CanTP_SFFrameTypedef
{
	uint8_t SF_DL;
	uint8_t* Data;
}CanTP_SFFrameTypedef;

typedef struct CanTP_FFFrameTypedef
{
	uint16_t FF_DL;
	uint8_t* Data;
}CanTP_FFFrameTypedef;

typedef struct CanTP_CFFrameTypedef
{
	uint8_t SN;
	uint8_t* Data;
	uint8_t DataLen;
}CanTP_CFFrameTypedef;

typedef struct CanTP_FCFrameTypedef
{
	uint8_t FS;
	uint8_t BS;
	uint8_t STmin;
}CanTP_FCFrameTypedef;

HAL_StatusTypeDef CanTP_Init(CanTP_HandleTypedef* hCanTP);
HAL_StatusTypeDef CanTP_Transmit(CanTP_HandleTypedef* hCanTP, CanTP_Packet Packet);
HAL_StatusTypeDef CanTP_Receive(CanTP_HandleTypedef* hCanTP, CanTP_Packet* Packet, uint32_t Timeout);

HAL_StatusTypeDef CanTP_RcvState(CanTP_HandleTypedef* hCanTP);
HAL_StatusTypeDef CanTP_GetData(CanTP_HandleTypedef* hCanTP);
HAL_StatusTypeDef CanTP_RcvCBHandler(CanTP_HandleTypedef* hCanTP, uint8_t* Data, uint8_t DataLen);
#endif /* CANTP_H_ */
