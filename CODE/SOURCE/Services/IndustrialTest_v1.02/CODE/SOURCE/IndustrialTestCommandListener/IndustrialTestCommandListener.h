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
// File "IndustrialTestCommandListener.h"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef __INDUSTRIALTESTCOMMANDLISTENER_H__
#define __INDUSTRIALTESTCOMMANDLISTENER_H__


// External inclusions
//--------------------
#include "TestProtocol.h"
#include "IndustrialTestConfig.h"


// Public types
//--------------

typedef enum
{
  INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_ERROR,
  INDUSTRIAL_TEST_CMD_LISTENER_ERROR__UNDIFINED_FUNCTION,
  INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_LISTENER,
  INDUSTRIAL_TEST_CMD_LISTENER_ERROR__LISTENER_ERROR
} TeIndustrialTestCommandListenerError;

//! Contains the differents APIs to give to be a IndustrialTestCommand Listener.
typedef struct
{
  TeIndustrialTestCommandListenerError  (*GetInfo)	(TIndex oListenerArg,TeIndustrialTestCmd eInfoIdArg,TU8* pu8Parameters, TU8* pu8DataSize, TU8* pu8Data);
  TeIndustrialTestCommandListenerError  (*Notify)	(TIndex oListenerArg,TeIndustrialTestCmd eInfoIdArg,TU8* pu8Data);
}TsIndustrialTestCommandListenerIf;

typedef struct
{
  TIndex oListenerId;
#ifdef INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
  CONST TsIndustrialTestCommandListenerIf * psInterface;
#endif // INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
} TsIndustrialTestCommandListenerInfo;


// Public functions declaration
//-----------------------------------

TeIndustrialTestCommandListenerError IndustrialTestCommandListener_GetInfo(TsIndustrialTestCommandListenerInfo* poListenerArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Parameters, TU8* pu8DataSize, TU8* pu8Data);

TeIndustrialTestCommandListenerError IndustrialTestCommandListener_Notify(TsIndustrialTestCommandListenerInfo* poListenerArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Data);


// Header end
//--------------
#endif // __INDUSTRIALTESTCOMMANDLISTENER_H__

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
