/*
 * TesterDiagnostic.c
 *
 *  Created on: Dec 2, 2022
 *      Author: vanti
 */

#include "TesterDiagnostic.h"
#include "SyncPrintf.h"
#include "KeyCalculate.h"


#define Tester_CANRxFifo 		CAN_FILTER_FIFO1
#define Tester_CANFilterBank	5
#define Tester_HCAN		hcan2

extern CAN_HandleTypeDef Tester_HCAN;
extern osEventFlagsId_t TesterCanTPStatusFlagHandle;
CanTP_HandleTypedef TesterCanTP ={0};

uint8_t Joystick_Pressed = 0;
uint8_t BTN0_IsPress = 0;
uint8_t BTN1_IsPress = 0;

HAL_StatusTypeDef Tester_Init()
{
	TesterCanTP.CanFilter.FilterActivation = CAN_FILTER_ENABLE;
	TesterCanTP.CanFilter.FilterIdHigh = Tester_RxID << 5;
	TesterCanTP.CanFilter.FilterMode = CAN_FILTERMODE_IDLIST;
	TesterCanTP.CanFilter.FilterScale = CAN_FILTERSCALE_32BIT;
	TesterCanTP.CanFilter.FilterBank = Tester_CANFilterBank;
	TesterCanTP.CanFilter.FilterFIFOAssignment = Tester_CANRxFifo;
	TesterCanTP.CanFilter.SlaveStartFilterBank = 0;
	TesterCanTP.hcan = &Tester_HCAN;
	TesterCanTP.Status = TesterCanTPStatusFlagHandle;
	TesterCanTP.CanRxID = Tester_RxID;
	TesterCanTP.CanTxID = Tester_TxID;
//	osDelay(10);
	CanTP_Init(&TesterCanTP);
	return HAL_OK;
}
static HAL_StatusTypeDef Tester_ReadADC(uint16_t* Value)
{
	uint8_t Buffer[3] = {0};
	Buffer[0] = Tester_ReadData_ByID_SID;
	Buffer[1] = Tester_AnalogRead_DID >> 8;
	Buffer[2] = Tester_AnalogRead_DID &0xff;

	CanTP_Packet TxPacket ={
			.Data = Buffer,
			.Len = 3
	};
	CanTP_Packet RxPacket;
	SyncPrintf("Tester Read Data By ID \r\n");
	HAL_CheckERR(CanTP_Transmit(&TesterCanTP, TxPacket));
	HAL_CheckERR(CanTP_Receive(&TesterCanTP, &RxPacket, 500));

	if(RxPacket.Data[0] == 0x62)
	{
		uint16_t DID = RxPacket.Data[1] <<8 | RxPacket.Data[2];
		if(DID == Tester_AnalogRead_DID)
		{
			*Value = RxPacket.Data[3] << 8 | RxPacket.Data[4];

		}
	}
	vPortFree(RxPacket.Data);
	return HAL_OK;
}

static HAL_StatusTypeDef Tester_ReadDataByID()
{
	HAL_StatusTypeDef Status;
	uint16_t ADCValue;
	Status = Tester_ReadADC(&ADCValue);
	if(Status == HAL_OK)
	{
		SyncPrintf("Tester Rcv ADC %d \r\n", ADCValue);
	}
	return HAL_OK;
}

static HAL_StatusTypeDef Tester_WriteDataByID()
{
	uint8_t Data[10] = {0};
	Data[0] = Tester_WriteData_ByID_SID;
	Data[1] = Tester_Write_DID >>8;
	Data[2] = Tester_Write_DID & 0xff;
	if(Joystick_Pressed != 0)
	{
		Joystick_Pressed = 0;
		SyncPrintf("Pressed %d \r\n", Joystick_Pressed);
		if(Joystick_Pressed == 1)
		{
			memset(Data + 3, 0x00, 6);
		}
		if(Joystick_Pressed == 1)
		{
			memset(Data + 3, 0xAA, 6);
		}
		if(Joystick_Pressed == 1)
		{
			memset(Data + 3, 0xFF, 6);
		}
		CanTP_Packet TxPacket = {
				.Data = Data,
				.Len = 10
		};
		CanTP_Packet RxPacket = {0};
		HAL_CheckERR(CanTP_Transmit(&TesterCanTP, TxPacket));
		HAL_CheckERR(CanTP_Receive(&TesterCanTP, &RxPacket, 500));
		if(RxPacket.Data[0] == 0x6E)
		{
			uint16_t DID = RxPacket.Data[1] << 8 | RxPacket.Data[2];
			if(DID == Tester_Write_DID)
			{
				SyncPrintf("Tester: Write Sucess \r\n");
				return HAL_OK;
			}
		}
	}

	return HAL_OK;
};


static HAL_StatusTypeDef Tester_SecurityAccess_RequestSeed(uint32_t* Seed)
{
	uint8_t Buffer[] = {Tester_SecurityAccess_SID, Tester_SecurityAccess_ReqSeedID};
	CanTP_Packet ReqPacket = {
			.Data = Buffer,
			.Len = 2
	};
//	HAL_StatusTypeDef Status =0;
	osDelay(10);
	HAL_CheckERR(CanTP_Transmit(&TesterCanTP, ReqPacket));
	CanTP_Packet RespPacket ={0};
	HAL_CheckERR(CanTP_Receive(&TesterCanTP, &RespPacket,500));
	uint8_t SID = RespPacket.Data[0];
	uint8_t SubFuncID = RespPacket.Data[1];
	uint32_t Seed_BE;
	memcpy(&Seed_BE, RespPacket.Data + 2, 4);
	vPortFree(RespPacket.Data);
	if(SID == Tester_SecurityAccess_RespSID && SubFuncID == Tester_SecurityAccess_ReqSeedID)
	{
		*Seed = __ntohl(Seed_BE);
		return HAL_OK;
	}
	else
	{
		SyncPrintf("Tester: Request Seed Failed \r\n");
		return HAL_ERROR;
	}

}

static HAL_StatusTypeDef Tester_SecurityAccess_ValidKey(uint8_t SubFunctionID, uint32_t Key)
{
	uint8_t Buffer[6] = {0};
	Buffer[0] = Tester_SecurityAccess_SID;
	Buffer[1] = SubFunctionID;
	uint32_t Key_BE = __htonl(Key);
	memmove(Buffer+2, &Key_BE, 4);
	CanTP_Packet SendPacket = {
			.Data = Buffer,
			.Len = sizeof(Buffer)
	};
	osDelay(10);
	HAL_CheckERR(CanTP_Transmit(&TesterCanTP, SendPacket));

	CanTP_Packet RespPacket ={0};
	HAL_CheckERR(CanTP_Receive(&TesterCanTP, &RespPacket, 500));
	uint8_t RespSID = RespPacket.Data[0];
	uint8_t RespSubFuncID = RespPacket.Data[1];
	if(RespSID == Tester_SecurityAccess_RespSID && RespSubFuncID == SubFunctionID)
	{
		SyncPrintf("Tester: Security Access Success \r\n");
		return HAL_OK;
	}
	else
	{
		SyncPrintf("Tester: Security Access Failed \r\n");
		return HAL_ERROR;
	}
}

static HAL_StatusTypeDef Tester_SecurityAccess(uint8_t IsSendTrueKey)
{
	uint32_t SecuritySeed;
	osDelay(10);
	HAL_CheckERR(Tester_SecurityAccess_RequestSeed(&SecuritySeed));
	SyncPrintf("Tester: Seed Receive %ld\r\n", SecuritySeed);
	uint32_t SecurityKey = 0;
	if(IsSendTrueKey == 1)
	{
		SecurityKey = Key_Calculate(SecuritySeed);
	}
	else
	{
		SecurityKey = 12346789;
	}
	osDelay(10);
	SyncPrintf("Tester Send Key %ld \r\n", SecurityKey);
	Tester_SecurityAccess_ValidKey(Tester_SecurityAccess_SendKeyID, SecurityKey);
	return HAL_OK;
}

HAL_StatusTypeDef Tester_Loop()
{
//	Tester_ReadDataByID();
//	osDelay(1000);
//	 Tester_WriteDataByID();
	if(BTN0_IsPress == 1)
	{
		SyncPrintf("------------------------------- \r\n");

		BTN0_IsPress = 0;
		SyncPrintf("Security Access Press 0: False Key, Press 1: True Key \r\n");
		while(BTN0_IsPress == 0 && BTN1_IsPress == 0);
		uint8_t SendTrueKey = 0;
		if(BTN0_IsPress)
		{
			SendTrueKey = 0;
			SyncPrintf("Tester: Send False Key \r\n");
		}else if(BTN1_IsPress)
		{
			SendTrueKey = 1;
			SyncPrintf("Tester Send True Key \r\n");
		}
		Tester_SecurityAccess(SendTrueKey);
		BTN0_IsPress = 0;
		BTN1_IsPress = 0;
	}
	return HAL_OK;
}


void TesterTask_Handler(void *argument)
{
	HAL_StatusTypeDef Status;
	Status = Tester_Init();
	SyncPrintf("Tester Init State %ld\r\n", (uint32_t) Status);
	while(1)
	{
		Tester_Loop();
	}
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	if(hcan->Instance == Tester_HCAN.Instance)
	{
		CAN_RxHeaderTypeDef RcvHeader;
		uint8_t RcvData[8];
		HAL_CAN_GetRxMessage(&Tester_HCAN, Tester_CANRxFifo, &RcvHeader, RcvData);
		if(RcvHeader.StdId == TesterCanTP.CanRxID)
		{
			CanTP_RcvCBHandler(&TesterCanTP, RcvData, RcvHeader.DLC);
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == BTN0_Pin)
	{
		BTN0_IsPress = 1;
	}
	if(GPIO_Pin == BTN1_Pin)
	{
		BTN1_IsPress = 1;
	}
	if(GPIO_Pin == Joystick_A_Pin)
	{
		Joystick_Pressed = 1;
	}
	if(GPIO_Pin == Joystick_B_Pin)
	{
		Joystick_Pressed = 2;
	}
	if(GPIO_Pin == Joystick_Mid_Pin)
	{
		Joystick_Pressed = 3;
	}
}
