/*
 * CanCom.h
 *
 *  Created on: Sep 20, 2022
 *      Author: vanti
 */

#ifndef CANCOM_H_
#define CANCOM_H_

#include "main.h"

void CAN_ConfigFilter(CAN_HandleTypeDef* hcan, uint16_t RcvID, uint16_t FilterBank, uint8_t FilterFIFO);
void CAN_Config(CAN_HandleTypeDef* hcan);

HAL_StatusTypeDef CAN_Receive(CAN_HandleTypeDef* hcan, char* RcvData);


HAL_StatusTypeDef CAN_Transmit(CAN_HandleTypeDef* hcan, uint16_t TxID ,uint8_t* Data, uint8_t DataLen);
#endif /* CANCOM_H_ */
