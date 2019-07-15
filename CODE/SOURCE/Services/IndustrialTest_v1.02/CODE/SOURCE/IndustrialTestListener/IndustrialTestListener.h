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
// File "IndustrialTestListener.h"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef __INDUSTRIALTESTLISTENER_H__
#define __INDUSTRIALTESTLISTENER_H__


// External inclusions
//--------------------
#include "IndustrialTestConfig.h"
#include "TestProtocol.h"
#include "IndustrialTestListener.h"


// Public types
//--------------

typedef enum
{
  INDUSTRIAL_TEST_LISTENER_ERROR__NO_ERROR,
  INDUSTRIAL_TEST_LISTENER_ERROR__UNDIFINED_FUNCTION,
  INDUSTRIAL_TEST_LISTENER_ERROR__NO_LISTENER,
  INDUSTRIAL_TEST_LISTENER_ERROR__CMD_FAILED
} TeIndustrialTestListenerError;

//! Contains the differents APIs to give to be a IndustrialTest Listener.
typedef struct
{
  TeIndustrialTestListenerError  (*GetInfo)                  (TIndex oListenerArg,TeIndustrialTestCmd eInfoIdArg,TU8* pu8Parameters, TU8* pu8DataSize, TU8* pu8Data);
  TeIndustrialTestListenerError  (*Notify)                   (TIndex oListenerArg, TeIndustrialTestCmd eInfoIdArg,TU8* pu8Data);
}TsIndustrialTestListenerIf;

typedef struct
{
  TIndex oListenerId;
#ifdef INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
  CONST TsIndustrialTestListenerIf * psInterface;
#endif // INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
} TsIndustrialTestListenerInfo;


// Public functions declaration
//-----------------------------------

#ifdef INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
TeIndustrialTestListenerError IndustrialTestListener_GetInfo(TsIndustrialTestListenerInfo* poListenerArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Parameters, TU8* pu8DataSize, TU8* pu8Data);
#endif // INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES

#ifdef INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
TeIndustrialTestListenerError IndustrialTestListener_Notify(TsIndustrialTestListenerInfo* poListenerArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Data);
#endif // INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES


// Header end
//--------------
#endif // __INDUSTRIALTESTLISTENER_H__

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
