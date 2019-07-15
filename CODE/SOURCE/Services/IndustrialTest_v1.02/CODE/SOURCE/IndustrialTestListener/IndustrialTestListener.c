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
// File "IndustrialTestListener.c"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// External inclusions
//--------------------
#include "SFY_Includes.h"


// Internal inclusion
//------------------
#include "IndustrialTestListener.h"

/*LDRA_HEADER_END*/

// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \fn         TeIndustrialTestListenerError IndustrialTestListener_GetInfo(TsIndustrialTestListenerInfo* poListenerArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Parameters, TU8* pu8DataSize, TU8* pu8Data)
* \param[in]  poListenerArg
* \param[in]  eInfoIdArg
* \param[in]  pu8Parameters
* \param[in]  pu8DataSize
* \param[in]  pu8Data
* \return     TeIndustrialTestListenerError 
***************************************************************************************************/
#ifdef INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
TeIndustrialTestListenerError IndustrialTestListener_GetInfo(TsIndustrialTestListenerInfo* poListenerArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Parameters, TU8* pu8DataSize, TU8* pu8Data)
{  
  /*## BEGIN[GUID 62177388-033f-4e1c-b7f0-556e52a49714] ##*/
  TeIndustrialTestListenerError  loc_eIndusTestListError  = INDUSTRIAL_TEST_LISTENER_ERROR__NO_ERROR;
  
  if (poListenerArg != NULL)
  {
    if (poListenerArg->psInterface->Notify != NULL)
    {
      loc_eIndusTestListError = poListenerArg->psInterface->GetInfo( poListenerArg->oListenerId, eInfoIdArg, pu8Parameters, pu8DataSize, pu8Data);
    }
    else
    {
      loc_eIndusTestListError = INDUSTRIAL_TEST_LISTENER_ERROR__NO_LISTENER;
    }
  }
  else
  {
    loc_eIndusTestListError = INDUSTRIAL_TEST_LISTENER_ERROR__NO_LISTENER;
  }
  
  return loc_eIndusTestListError;
  /*## END[GUID 62177388-033f-4e1c-b7f0-556e52a49714] ##*/
}
#endif // INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES


/*!************************************************************************************************
* \fn         TeIndustrialTestListenerError IndustrialTestListener_Notify(TsIndustrialTestListenerInfo* poListenerArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Data)
* \param[in]  poListenerArg
* \param[in]  eInfoIdArg
* \param[in]  pu8Data
* \return     TeIndustrialTestListenerError 
***************************************************************************************************/
#ifdef INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
TeIndustrialTestListenerError IndustrialTestListener_Notify(TsIndustrialTestListenerInfo* poListenerArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Data)
{  
  /*## BEGIN[GUID 68769354-33a7-4dd9-89b9-e09850b2a69d] ##*/
  TeIndustrialTestListenerError  loc_eIndusTestListError  = INDUSTRIAL_TEST_LISTENER_ERROR__NO_ERROR;
  
  if (poListenerArg != NULL)
  {
    if (poListenerArg->psInterface->Notify != NULL)
    {
      loc_eIndusTestListError = poListenerArg->psInterface->Notify( poListenerArg->oListenerId, eInfoIdArg, pu8Data);
    }
    else
    {
      loc_eIndusTestListError = INDUSTRIAL_TEST_LISTENER_ERROR__NO_LISTENER;
    }
  }
  else
  {
    loc_eIndusTestListError = INDUSTRIAL_TEST_LISTENER_ERROR__NO_LISTENER;
  }
  
  return loc_eIndusTestListError;
  /*## END[GUID 68769354-33a7-4dd9-89b9-e09850b2a69d] ##*/
}
#endif // INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
