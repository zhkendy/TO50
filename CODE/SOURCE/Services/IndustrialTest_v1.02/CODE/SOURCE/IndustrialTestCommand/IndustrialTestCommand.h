/**********************************************************************
This document and/or file is SOMFY’s property. All information it
contains is strictly confidential. This document and/or file shall
not be used, reproduced or passed on in any way, in full or in part
without SOMFY’s prior written approval. All rights reserved.
Ce document et/ou fichier est la propriété de SOMFY. Les informations
qu’il contient sont strictement confidentielles. Toute reproduction,
utilisation, transmission de ce document et/ou fichier, partielle ou
intégrale, non autorisée préalablement par SOMFY par écrit est
interdite. Tous droits réservés.
***********************************************************************/


//---------------------------------------------------------------------
// File "IndustrialTestCommand.h"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef __INDUSTRIALTESTCOMMAND_H__
#define __INDUSTRIALTESTCOMMAND_H__


// External inclusions
//--------------------
#include "TestFrame.h"
#include "IndustrialTestCommandListener.h"
#include "IndustrialTestConfig.h"


// Public types
//--------------

typedef enum
{
  INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR,
  INDUSTRIAL_TEST_CMD_ERROR__UNMANAGED_CMD,
  INDUSTRIAL_TEST_CMD_ERROR__UPDATE_FRAME_ERROR,
  INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR,
  INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_PARAMETER,
  INDUSTRIAL_TEST_CMD_ERROR__CMD_FAILED,
  INDUSTRIAL_TEST_CMD_ERROR__LISTENER_FAILED
} TeIndustrialTestCmdError;

#define COMMAND_CONFIGURATION(cmdIdArg) \
  cmdIdArg, cmdIdArg##_LENGTH

typedef TeIndustrialTestCmdError (*CommandValidateAndFillAck)(TsIndustrialTestCommandListenerInfo* poListenerArg, ToTestFrameId oTransmitFrame, ToTestFrameId oReceiveFrame) ;

typedef TeIndustrialTestCmdError (*CommandExecute)(TsIndustrialTestCommandListenerInfo* poListenerArg, ToTestFrameId oTransmitFrame, ToTestFrameId oReceiveFrame, TU8 u8DataSize, TU8* pu8Data) ;

typedef struct
{
  TU8						u8Command;
  TU8						u8CmdLength;
  CommandValidateAndFillAck	pfnValidateAndFillAck;
  CommandExecute			pfnExecute;
}TsCommandInterface;

#define HEADER_POSITION			        0
#define COMMAND_ID_POSITION		        1
#define COMMAND_DATA_POSITION	                2

#define ACK_OK					0xFF
#define ACK_KO					0x00
#define ACK_UNKNOWN_CMD			        0x01
#define ACK_BAD_CKSUM			        0x02

#define MAX_DATA_FRAME_SIZE		        0x3E



// Public functions declaration
//-----------------------------------

TeIndustrialTestCmdError IndustrialTestCommand_ValidateAndFillAck(TsIndustrialTestCommandListenerInfo* poListenerArg, TU16 u16Cmd, ToTestFrameId oTransmitFrame, ToTestFrameId oReceiveFrame);

TeIndustrialTestCmdError IndustrialTestCommand_Execute(TsIndustrialTestCommandListenerInfo* poListenerArg, TU16 u16Cmd, ToTestFrameId oTransmitFrame, ToTestFrameId oReceiveFrame, TU8 u8DataSize, TU8* pu8Data);

TBool IndustrialTestCommand_CommandIsManaged(TU16 u16CmdId);


// Header end
//--------------
#endif // __INDUSTRIALTESTCOMMAND_H__

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
