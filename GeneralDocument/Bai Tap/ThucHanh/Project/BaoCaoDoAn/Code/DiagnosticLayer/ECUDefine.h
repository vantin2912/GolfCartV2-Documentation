/*
 * ECUDefine.h
 *
 *  Created on: Dec 2, 2022
 *      Author: vanti
 */

#ifndef ECUDEFINE_H_
#define ECUDEFINE_H_

#define ECU_TxID 0x712
#define ECU_RxID 0x7A2

#define ECU_ReadData_ByID_SID 	0x22
#define ECU_WriteData_ByID_SID	0x2E
#define ECU_SecurityAccess_SID	0x27

#define ECU_ReadData_ByID_RespSID 	0x62
#define ECU_WriteData_ByID_RespSID	0x6E
#define ECU_SecurityAccess_RespSID	0x67


#define ECU_AnalogRead_DID	0xE015
#define ECU_Write_DID		0xF002

#endif /* ECUDEFINE_H_ */
