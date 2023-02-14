/*
 * ECUDiagnostic.h
 *
 *  Created on: Dec 2, 2022
 *      Author: vanti
 */

#ifndef ECUDIAGNOSTIC_H_
#define ECUDIAGNOSTIC_H_

#include "ECUDefine.h"
#include "main.h"
#include "CanTP.h"
typedef struct ECU_Handler
{

} ECU_Handler;
HAL_StatusTypeDef ECU_Init();
HAL_StatusTypeDef ECU_Loop();


#endif /* ECUDIAGNOSTIC_H_ */
