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
// File "IndustrialTest.h"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef __INDUSTRIALTEST_H__
#define __INDUSTRIALTEST_H__


// External inclusions
//--------------------
#include "IndustrialTestListener.h"
#include "IndustrialTestCommandListener.h"
#include "IndustrialTestCommand.h"


// Public types
//--------------

typedef enum
{
  INDUSTRIAL_TEST_ERROR__NO_ERROR,
  INDUSTRIAL_TEST_ERROR__INDUS_CMD_ERROR,
  INDUSTRIAL_TEST_ERROR__TEST_PROTOCOL_ERROR,
  INDUSTRIAL_TEST_ERROR__TEST_FRAME_ERROR,
  INDUSTRIAL_TEST_ERROR__UNKNOWN_CMD
} TeIndustrialTestError;


// Public functions declaration
//-----------------------------------

void IndustrialTest_Init(TsIndustrialTestListenerInfo* pListenerArg);

void IndustrialTest_Cleanup(void);

TeIndustrialTestError IndustrialTest_CmdProcessed(void);

TeIndustrialTestError IndustrialTest_NewFrameIsReceived(void);

void IndustrialTest_StartEmbeddedTest(void);

void IndustrialTest_StopEmbeddedTest(void);

void IndustrialTest_UnknownCmd(void);

// Method to setup listener info dynamically.
#ifdef INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
void IndustrialTest_SetupListener(TsIndustrialTestListenerInfo psListenerInfoArg);
#endif // INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES

TeIndustrialTestError IndustrialTest_AbortIndustrialTest(void);

TBool IndustrialTest_CheckFrameIsStartFrame(void);

TeIndustrialTestCommandListenerError IndustrialTest_GetInfo(TIndex _idxDummyArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Parameters, TU8* pu8DataSize, TU8* pu8data);

TeIndustrialTestCommandListenerError IndustrialTest_Notify(TIndex _idxDummyArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Data);

TeIndustrialTestCmdError IndustrialTest_SendKeyBoardPressedAnswer(TU8 u8ButtonPressedId);

TeIndustrialTestCmdError IndustrialTest_SendKeyBoardReleasedAnswer(TU8 u8ButtonPressedId);

#ifdef MANAGE_SENSITIVE_TEST_ANS
TeIndustrialTestCmdError IndustrialTest_SendSensitiveTestAnswer(TU8* pu8SensitiveTestValuesArg);
#endif //MANAGE_SENSITIVE_TEST_ANS


// Header end
//--------------
#endif // __INDUSTRIALTEST_H__

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
