/*
 * ECUDiagnostic.c
 *
 *  Created on: Dec 2, 2022
 *      Author: vanti
 */


#include "ECUDiagnostic.h"
#include "KeyCalculate.h"

#define ECU_CANRxFifo 		CAN_FILTER_FIFO0
#define ECU_CANFilterBank	10
#define ECU_HCAN		hcan1
#define ECU_HADC		hadc1

extern ADC_HandleTypeDef ECU_HADC;
extern CAN_HandleTypeDef ECU_HCAN;
extern osEventFlagsId_t ECUCanTPStatusFlagHandle;
extern RNG_HandleTypeDef hrng;

CanTP_HandleTypedef ECUCanTP ={0};

uint16_t ADCVal[1]={0};

HAL_StatusTypeDef ECU_Init()
{
	ECUCanTP.CanFilter.FilterActivation = CAN_FILTER_ENABLE;
	ECUCanTP.CanFilter.FilterIdHigh = ECU_RxID << 5;
	ECUCanTP.CanFilter.FilterMode = CAN_FILTERMODE_IDLIST;
	ECUCanTP.CanFilter.FilterScale = CAN_FILTERSCALE_16BIT;
	ECUCanTP.CanFilter.FilterBank = ECU_CANFilterBank;
	ECUCanTP.CanFilter.FilterFIFOAssignment = ECU_CANRxFifo;
	ECUCanTP.CanFilter.SlaveStartFilterBank = 15;
//	ECUCanTP.CanRxFifo = ECU_CANRxFifo;
	ECUCanTP.hcan = &ECU_HCAN;
	ECUCanTP.Status = ECUCanTPStatusFlagHandle;
	ECUCanTP.CanRxID = ECU_RxID;
	ECUCanTP.CanTxID = ECU_TxID;
	HAL_CheckERR(CanTP_Init(&ECUCanTP));
	HAL_CheckERR(HAL_ADC_Start_DMA(&ECU_HADC, (uint32_t*)ADCVal, 1));
	return HAL_OK;
}

static HAL_StatusTypeDef ECU_Response_Analog()
{
	uint8_t Data[5];
	uint16_t Value = ADCVal[0];
	Data[0] = 0x62;
	Data[1] = ECU_AnalogRead_DID >>8;
	Data[2] = ECU_AnalogRead_DID & 0xff;
	Data[3] = Value >> 8;
	Data[4] = Value & 0xff;
	CanTP_Packet DataPacket = {
			.Data = Data,
			.Len = 5
	};
	SyncPrintf("ECU Read Value %d \r\n", Value);
	osDelay(5);
	HAL_CheckERR(CanTP_Transmit(&ECUCanTP, DataPacket));
	return HAL_OK;
}


static HAL_StatusTypeDef ECU_ReadData_Service(uint8_t* Buffer, uint8_t Len)
{
	uint16_t DID = Buffer[0]<<8 | Buffer[1];	// Because of the maximum number of DID is limited to 1
	if(DID == ECU_AnalogRead_DID)
	{
		ECU_Response_Analog();
	};

	return HAL_OK;
}

static HAL_StatusTypeDef ECU_WriteData_Service(uint8_t* Buffer, uint8_t Len)
{
	uint8_t DID = Buffer[0] << 8 | Buffer[1];
	if(DID == ECU_Write_DID)
	{
		SyncPrintf("Data ");
		for(int i = 0; i < Len - 3; i++)
		{
			SyncPrintf("%.2x ", Buffer[i]);
		}

		uint8_t ResponseBuffer[] = {0x6E, ECU_Write_DID >> 8, ECU_Write_DID & 0xff};
		CanTP_Packet RespPacket = {
				.Data = ResponseBuffer,
				.Len = sizeof(ResponseBuffer)
		};
		HAL_CheckERR(CanTP_Transmit(&ECUCanTP, RespPacket));
	}
	return HAL_OK;
};


static HAL_StatusTypeDef ECU_SecurityUnlock()
{
	SyncPrintf("ECU: Security Unlock \r\n");
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

	return HAL_OK;
}
static HAL_StatusTypeDef ECU_SecurityLock()
{
	SyncPrintf("ECU: Security Lock \r\n");
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	return HAL_OK;
}


static HAL_StatusTypeDef ECU_SecurityAccess_RespSeed(uint8_t SubFunctionID, uint32_t SecuritySeed)
{
	uint8_t Buffer[6] = {0};
	Buffer[0] = ECU_SecurityAccess_RespSID;
	Buffer[1] = SubFunctionID;
	uint32_t Seed_BE = __htonl(SecuritySeed);
	memcpy(Buffer+2, &Seed_BE, 4);
	CanTP_Packet RespPacket = {
			.Data = Buffer,
			.Len  = sizeof(Buffer)
	};
	osDelay(10);
	HAL_CheckERR(CanTP_Transmit(&ECUCanTP, RespPacket));
	return HAL_OK;
}

static HAL_StatusTypeDef ECU_SecurityAccess_RespKey(uint8_t SubFunctionID)
{
	uint8_t Buffer[2] = {ECU_SecurityAccess_RespSID, SubFunctionID};
	CanTP_Packet RespPacket = {
			.Data = Buffer,
			.Len = 2
	};
	osDelay(10);
	HAL_CheckERR(CanTP_Transmit(&ECUCanTP, RespPacket));
	return HAL_OK;
}

static HAL_StatusTypeDef ECU_SecurityAccess_CheckKey(uint32_t ExpectedKey, uint32_t Timeout)
{
	CanTP_Packet RcvPacket ={0};
	HAL_CheckERR(CanTP_Receive(&ECUCanTP, &RcvPacket, Timeout));
	uint8_t SID = RcvPacket.Data[0];
	uint8_t SubFunctID = RcvPacket.Data[1];
	uint32_t RcvKey_BE, RcvKey;

	memcpy(&RcvKey_BE, RcvPacket.Data + 2, 4);
	vPortFree(RcvPacket.Data);

	if(SID != ECU_SecurityAccess_SID) return HAL_ERROR;
	if(SubFunctID == 0x02)
	{
		RcvKey = __ntohl(RcvKey_BE);
		if(RcvKey == ExpectedKey)
		{
			ECU_SecurityUnlock();
			ECU_SecurityAccess_RespKey(SubFunctID);
		}
		else
		{
			ECU_SecurityLock();
		}
	}
	else
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}


static HAL_StatusTypeDef ECU_SecurityAccess_Service(uint8_t* Data, uint8_t DataLen)
{
	uint32_t SeedValue;
	uint8_t SubFuncID = Data[0];
	HAL_StatusTypeDef Status;
	if(SubFuncID == 0x01)
	{
		HAL_RNG_GenerateRandomNumber(&hrng, &SeedValue);
		SyncPrintf("ECU: Generate Seed %ld \r\n", SeedValue);
		HAL_CheckERR(ECU_SecurityAccess_RespSeed(SubFuncID, SeedValue));
		uint32_t ExpectedKey = Key_Calculate(SeedValue);
		SyncPrintf("ECU: Expected Key %ld \r\n", ExpectedKey);
		Status = ECU_SecurityAccess_CheckKey(ExpectedKey, 500);
		return Status;
	}
	return HAL_OK;
}


HAL_StatusTypeDef ECU_Loop()
{
	CanTP_Packet DataPacket ={0};
	HAL_StatusTypeDef Status = CanTP_Receive(&ECUCanTP, &DataPacket, osWaitForever);
	SyncPrintf("ECU: RcvData ");
	for(int i = 0; i <DataPacket.Len; ++i)
	{
		SyncPrintf("0x%.2x ", DataPacket.Data[i]);
	}
	SyncPrintf("\r\n");

	if(Status == HAL_OK)
	{
		uint8_t SID = DataPacket.Data[0];
		if(SID == ECU_ReadData_ByID_SID)
		{
			ECU_ReadData_Service(DataPacket.Data + 1, DataPacket.Len - 1); // Remove First Byte SID
		}
		else if(SID == ECU_WriteData_ByID_SID)
		{
			ECU_WriteData_Service(DataPacket.Data + 1, DataPacket.Len - 1);
		}
		else if(SID == ECU_SecurityAccess_SID)
		{
			SyncPrintf("ECU: Security Access \r\n");
			ECU_SecurityAccess_Service(DataPacket.Data + 1, DataPacket.Len - 1);

		}
		vPortFree(DataPacket.Data);
	}

	return HAL_OK;
}


void ECUTask_Handler(void *argument)
{
	HAL_StatusTypeDef Status;
	Status = ECU_Init();
	SyncPrintf("ECU Init State %ld\r\n", (uint32_t) Status);
	while(1)
	{
		ECU_Loop();
	}
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan->Instance == ECU_HCAN.Instance)
	{
		CAN_RxHeaderTypeDef RcvHeader;
		uint8_t RcvData[8];
		HAL_CAN_GetRxMessage(hcan, ECU_CANRxFifo, &RcvHeader, RcvData);
		if(RcvHeader.StdId == ECUCanTP.CanRxID)
		{
			CanTP_RcvCBHandler(&ECUCanTP, RcvData, RcvHeader.DLC);
		}
	}

}


