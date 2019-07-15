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
// File "IndustrialTestCommandListener.c"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// External inclusions
//--------------------
#include "SFY_Includes.h"
#include "IndustrialTestListener.h"


// Internal inclusion
//------------------
#include "IndustrialTestCommandListener.h"

/*LDRA_HEADER_END*/

// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \fn         TeIndustrialTestCommandListenerError IndustrialTestCommandListener_GetInfo(TsIndustrialTestCommandListenerInfo* poListenerArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Parameters, TU8* pu8DataSize, TU8* pu8Data)
* \param[in]  poListenerArg
* \param[in]  eInfoIdArg
* \param[in]  pu8Parameters
* \param[in]  pu8DataSize
* \param[in]  pu8Data
* \return     TeIndustrialTestCommandListenerError 
***************************************************************************************************/
TeIndustrialTestCommandListenerError IndustrialTestCommandListener_GetInfo(TsIndustrialTestCommandListenerInfo* poListenerArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Parameters, TU8* pu8DataSize, TU8* pu8Data)
{  
  /*## BEGIN[GUID f226a1a7-c1c3-40ac-b08d-c08e81bd7b9c] ##*/
  TeIndustrialTestCommandListenerError  loc_eReturnValue         = INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_ERROR;
  
  if (poListenerArg != NULL)
  {
    if (poListenerArg->psInterface->Notify != NULL)
    {
      loc_eReturnValue = poListenerArg->psInterface->GetInfo( poListenerArg->oListenerId, eInfoIdArg, pu8Parameters, pu8DataSize, pu8Data);
    }
    else
    {
      loc_eReturnValue = INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_LISTENER;
    }
  }
  else
  {
    loc_eReturnValue = INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_LISTENER;
  }
  
  return loc_eReturnValue;
  /*## END[GUID f226a1a7-c1c3-40ac-b08d-c08e81bd7b9c] ##*/
}


/*!************************************************************************************************
* \fn         TeIndustrialTestCommandListenerError IndustrialTestCommandListener_Notify(TsIndustrialTestCommandListenerInfo* poListenerArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Data)
* \param[in]  poListenerArg
* \param[in]  eInfoIdArg
* \param[in]  pu8Data
* \return     TeIndustrialTestCommandListenerError 
***************************************************************************************************/
TeIndustrialTestCommandListenerError IndustrialTestCommandListener_Notify(TsIndustrialTestCommandListenerInfo* poListenerArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Data)
{  
  /*## BEGIN[GUID 2ab75fd5-a8a4-4353-9abf-99a6733065c7] ##*/
  TeIndustrialTestCommandListenerError  loc_eReturnValue         = INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_ERROR;
  
  if (poListenerArg != NULL)
  {
    if (poListenerArg->psInterface->Notify != NULL)
    {
      loc_eReturnValue = poListenerArg->psInterface->Notify( poListenerArg->oListenerId, eInfoIdArg, pu8Data);
    }
    else
    {
      loc_eReturnValue = INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_LISTENER;
    }
  }
  else
  {
    loc_eReturnValue = INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_LISTENER;
  }
  
  return loc_eReturnValue;
  /*## END[GUID 2ab75fd5-a8a4-4353-9abf-99a6733065c7] ##*/
}

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
