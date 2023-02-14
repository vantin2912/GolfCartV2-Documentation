/*
 * CanCom.c
 *
 *  Created on: Sep 20, 2022
 *      Author: vanti
 */
#include <stdio.h>
#include <string.h>

#include "CANCom.h"



#define CANTransmitID	0x012
#define CANRcvID		0x0A2

uint8_t crcTable[256];

uint8_t CAN_FrameData[8]={0};


void CRCInit(void) {
	uint8_t _crc;
	for (int i = 0; i < 0x100; i++) {
		_crc = i;

		for (uint8_t bit = 0; bit < 8; bit++) _crc = (_crc & 0x80) ? ((_crc << 1) ^ 0x1D) : (_crc << 1);

		crcTable[i] = _crc;
	}
}
uint8_t CalcCRC(uint8_t * buf, uint8_t len) {
        const uint8_t * ptr = buf;
        uint8_t _crc = 0xFF;

        while(len--) _crc = crcTable[_crc ^ *ptr++];

        return ~_crc;
}

void CAN_ConfigFilter(CAN_HandleTypeDef* hcan, uint16_t RcvID, uint16_t FilterBank, uint8_t FilterFIFO)
{
	CAN_FilterTypeDef Filter;
	Filter.FilterActivation = CAN_FILTER_ENABLE;
	Filter.FilterIdHigh = RcvID<<5;
	Filter.FilterMode = CAN_FILTERMODE_IDLIST;
	Filter.FilterScale = CAN_FILTERSCALE_16BIT;
	Filter.FilterBank = FilterBank;
	Filter.FilterFIFOAssignment = CAN_FilterFIFO0;
	Filter.SlaveStartFilterBank =20;
	HAL_CAN_ConfigFilter(hcan, &Filter);
}

//HAL_StatusTypeDef CAN_Receive(CAN_HandleTypeDef* hcan, uint8_t* RcvData, uint8_t* RcvLen)
//{
//
//}

HAL_StatusTypeDef CAN_Transmit(CAN_HandleTypeDef* hcan, uint16_t TxID ,uint8_t* Data, uint8_t DataLen)
{
	CAN_TxHeaderTypeDef Can_Header = {0};
	HAL_StatusTypeDef Status;
	uint32_t Mailbox = 0;
	uint32_t FreeLevel = HAL_CAN_GetTxMailboxesFreeLevel(hcan);
	if(FreeLevel == 0) return HAL_ERROR;

	Can_Header.DLC = DataLen;
	Can_Header.StdId = TxID;
	Status = HAL_CAN_AddTxMessage(hcan, &Can_Header,(uint8_t*) Data, &Mailbox);

	return Status;
}


