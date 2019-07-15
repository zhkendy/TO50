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
// File "RamPool.c"   Generated with SOMFY code generator v[4.6.0.0]
//---------------------------------------------------------------------


// External inclusions
//--------------------
#include "SFY_Includes.h"


// Internal inclusion
//------------------
#include "RamPool.h"


// Private types
//-------------

/* LDRA Rule Deactivation : 210 S Macro name is not upper case. */
/*LDRA_INSPECTED 210 S*/
#define me (&oRamPool)

//! Field sizes
#define RAMPOOL_STATE_SIZE  sizeof(TeRamPoolState)


// Private functions declaration
//---------------------------------

static TIndex RamPool_PRV_CreateInstance(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TIndex idxOffsetArg, TIndex idxSizeArg);

/*LDRA_HEADER_END*/

// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \fn         void RamPool_Init(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg)
* \param[in,out] psPoolInfoArg
* \return     void 
***************************************************************************************************/
void RamPool_Init(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg)
{  
  // Implementation
  //---------------
  
  {
    /*## BEGIN[GUID 1e0efdd9-98e9-4962-a5e8-fb5efe8dd08f] ##*/
    TIndex idx;
    
    for(idx= 0U; idx<psPoolInfoArg->idxNbOfElements; idx++)
    {
    	if(psPoolInfoArg->spfAssessors.SetState != NULL)
        {
          psPoolInfoArg->spfAssessors.SetState(idx, RAMPOOL_INSTANCE_IS_FREE);
        }
        else
        {
          RUNTIME_WARNING(); 
        }
    }
    /*## END[GUID 1e0efdd9-98e9-4962-a5e8-fb5efe8dd08f] ##*/
  
  }
}


/*!************************************************************************************************
* \fn         TIndex RamPool_CreateInstance(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg)
* \param[in,out] psPoolInfoArg
* \return     TIndex 
***************************************************************************************************/
TIndex RamPool_CreateInstance(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg)
{  
  {
    /*## BEGIN[GUID 1d7ebbd9-0860-4a48-a7ec-eb385953c4c4] ##*/
    return RamPool_PRV_CreateInstance(psPoolInfoArg, 0, psPoolInfoArg->idxNbOfElements);                
    /*## END[GUID 1d7ebbd9-0860-4a48-a7ec-eb385953c4c4] ##*/
  }
}


/*!************************************************************************************************
* \fn         TIndex RamPool_CreateInstanceInSubPool(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TByte btSubPoolIdArg)
* \param[in,out] psPoolInfoArg
* \param[in]  btSubPoolIdArg
* \return     TIndex 
***************************************************************************************************/
TIndex RamPool_CreateInstanceInSubPool(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TByte btSubPoolIdArg)
{  
  {
    /*## BEGIN[GUID 59a4aaa4-7fca-4009-9ae4-b63ae216671e] ##*/
    TIndex       idxOffset = 0U;
    TIndex       idxSize = 0U;
    
    // Get the Partition Limits
    //-------------------------
    RamPool_GetSubPoolLimits(psPoolInfoArg, btSubPoolIdArg, &idxOffset, &idxSize);
    
    return RamPool_PRV_CreateInstance(psPoolInfoArg, idxOffset, idxSize);
    /*## END[GUID 59a4aaa4-7fca-4009-9ae4-b63ae216671e] ##*/
  }
}


/*!************************************************************************************************
* \fn         TBool RamPool_DestroyInstance(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TIndex idxInstanceArg)
* \param[in,out] psPoolInfoArg
* \param[in]  idxInstanceArg
* \return     TBool 
***************************************************************************************************/
TBool RamPool_DestroyInstance(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TIndex idxInstanceArg)
{  
  {
    /*## BEGIN[GUID 3a1e09fc-87af-491c-b388-614804643b88] ##*/
    TBool bRetValue = FALSE;  
    
    if(idxInstanceArg < psPoolInfoArg->idxNbOfElements)
    {
      if(psPoolInfoArg->spfAssessors.GetState(idxInstanceArg) != RAMPOOL_INSTANCE_IS_FREE)
      {
      	psPoolInfoArg->spfAssessors.SetState(idxInstanceArg, RAMPOOL_INSTANCE_IS_FREE);
      	bRetValue = TRUE;
      }
    }
    return bRetValue; 
    /*## END[GUID 3a1e09fc-87af-491c-b388-614804643b88] ##*/
  }
}


/*!************************************************************************************************
* \fn         TIndex RamPool_GetAbsoluteInstanceIdInSubPool(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TByte btSubPoolIdArg, TIndex idxInstanceArg)
* \param[in,out] psPoolInfoArg
* \param[in]  btSubPoolIdArg
* \param[in]  idxInstanceArg
* \return     TIndex 
***************************************************************************************************/
TIndex RamPool_GetAbsoluteInstanceIdInSubPool(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TByte btSubPoolIdArg, TIndex idxInstanceArg)
{  
  {
    /*## BEGIN[GUID 2a4457f2-6659-45ad-8290-d0ecd69bd53a] ##*/
    TIndex   retValue = 0U;
    TIndex    oIndex;
      
    for(oIndex= 0U; oIndex<btSubPoolIdArg; oIndex++)
    {
      retValue += psPoolInfoArg->psSubPoolInfo->paoSubPoolSizes[oIndex];
    }
    retValue += idxInstanceArg;
    return retValue;
    /*## END[GUID 2a4457f2-6659-45ad-8290-d0ecd69bd53a] ##*/
  }
}


/*!************************************************************************************************
* \fn         TBool RamPool_IsItFull(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TByte btSubPoolIdArg)
* \param[in,out] psPoolInfoArg
* \param[in]  btSubPoolIdArg
* \return     TBool 
***************************************************************************************************/
TBool RamPool_IsItFull(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TByte btSubPoolIdArg)
{  
  {
    /*## BEGIN[GUID 80f925aa-194c-4fcc-a007-d4eca504c905] ##*/
    TIndex      idxSubPoolOffset = 0U;
    TIndex      idxSubPoolSize = 0U;
    
    // Get the Partition Limits
    //-------------------------
    RamPool_GetSubPoolLimits(psPoolInfoArg, btSubPoolIdArg, &idxSubPoolOffset, &idxSubPoolSize);
    
    // search if there an empty place between the limits
    while(idxSubPoolSize--)
    {
    	if(psPoolInfoArg->spfAssessors.GetState(idxSubPoolOffset) == RAMPOOL_INSTANCE_IS_FREE)
    	{
    		return FALSE;
    	}
    	idxSubPoolOffset++;
    }
    return TRUE;                                          
    /*## END[GUID 80f925aa-194c-4fcc-a007-d4eca504c905] ##*/
  }
}


/*!************************************************************************************************
* \fn         TIndex RamPool_GetNbOfInstancesInSubPool(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TByte btSubPoolIdArg)
* \param[in,out] psPoolInfoArg
* \param[in]  btSubPoolIdArg
* \return     TIndex 
***************************************************************************************************/
TIndex RamPool_GetNbOfInstancesInSubPool(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TByte btSubPoolIdArg)
{  
  {
    /*## BEGIN[GUID 3f9a13da-3734-4f35-ab87-590e57a559de] ##*/
    TIndex      idxNbOfInstances = 0U;
    TIndex      idxSubPoolOffset = 0U;
    TIndex      idxSubPoolSize = 0U;
    
    // Get the Partition Limits
    //-------------------------
    RamPool_GetSubPoolLimits(psPoolInfoArg, btSubPoolIdArg, &idxSubPoolOffset, &idxSubPoolSize);
        
    // search if there an empty place between the limits    
    while(idxSubPoolSize--)
    {
    	if(psPoolInfoArg->spfAssessors.GetState(idxSubPoolOffset) != RAMPOOL_INSTANCE_IS_FREE)
      {
        idxNbOfInstances++;
      }
      idxSubPoolOffset++;
    }
    return idxNbOfInstances;
    /*## END[GUID 3f9a13da-3734-4f35-ab87-590e57a559de] ##*/
  }
}


/*!************************************************************************************************
* \fn         TIndex RamPool_GetNbOfInstances(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg)
* \param[in,out] psPoolInfoArg
* \return     TIndex 
***************************************************************************************************/
TIndex RamPool_GetNbOfInstances(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg)
{  
  {
    /*## BEGIN[GUID af33a65d-05cc-49c3-8f3c-c8da997095ee] ##*/
    TIndex      idxNbOfInstances = 0U;
    TIndex      idxOffset = 0U;
    TIndex      idxSize = 0U;
    
    // search if there an empty place between the limits
    idxSize = psPoolInfoArg->idxNbOfElements;
    while(idxSize--)
    {
    	if(psPoolInfoArg->spfAssessors.GetState(idxOffset) != RAMPOOL_INSTANCE_IS_FREE)
      {
        idxNbOfInstances++;
      }
      idxOffset++;
    }
    return idxNbOfInstances;
    /*## END[GUID af33a65d-05cc-49c3-8f3c-c8da997095ee] ##*/
  }
}


/*!************************************************************************************************
* \fn         TIndex RamPool_GetSubPoolForThisInstance(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TIndex idxInstanceArg)
* \param[in,out] psPoolInfoArg
* \param[in]  idxInstanceArg
* \return     TIndex 
***************************************************************************************************/
TIndex RamPool_GetSubPoolForThisInstance(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TIndex idxInstanceArg)
{  
  {
    /*## BEGIN[GUID 00715889-c022-4b41-9cbc-a8a71f2037ac] ##*/
    TIndex oSubPoolId = 0U;
    TIndex idxNextSubPooLimit  = psPoolInfoArg->psSubPoolInfo->paoSubPoolSizes[0];
    while(idxInstanceArg >= idxNextSubPooLimit)
    {
      oSubPoolId++;
      if(oSubPoolId < psPoolInfoArg->psSubPoolInfo->oNbOfSubPool)
      {
        idxNextSubPooLimit += psPoolInfoArg->psSubPoolInfo->paoSubPoolSizes[oSubPoolId];
      }
      else
      {
        oSubPoolId = RAMPOOL_NO_SUBPOOL_ID;
        break;
      }
    }
    return oSubPoolId;
    /*## END[GUID 00715889-c022-4b41-9cbc-a8a71f2037ac] ##*/
  }
}


/*!************************************************************************************************
* \fn         void RamPool_GetSubPoolLimits(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TIndex oSubPoolIdArg, TIndex* poOffsetArg, TIndex* poSizeArg)
* \param[in,out] psPoolInfoArg
* \param[in]  oSubPoolIdArg
* \param[out] poOffsetArg
* \param[out] poSizeArg
* \return     void 
***************************************************************************************************/
void RamPool_GetSubPoolLimits(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TIndex oSubPoolIdArg, TIndex* poOffsetArg, TIndex* poSizeArg)
{  
  {
    /*## BEGIN[GUID 1b8bdc6f-158a-46ff-9864-5b8ded27fc93] ##*/
    TIndex       i;
    TIndex       idxOffset = 0U;
    TIndex       idxSize = 0U;
    
    // Get the pool limits	
    //--------------------
    if((psPoolInfoArg->psSubPoolInfo == NULL) || (oSubPoolIdArg == RAMPOOL_NO_SUBPOOL_ID))
    {
      idxSize = psPoolInfoArg->idxNbOfElements;
    }
    else
    {
      if(oSubPoolIdArg < psPoolInfoArg->psSubPoolInfo->oNbOfSubPool)
      {
        for(i= 0U; i<oSubPoolIdArg; i++)
        {
          idxOffset += psPoolInfoArg->psSubPoolInfo->paoSubPoolSizes[i];
        }
        idxSize = psPoolInfoArg->psSubPoolInfo->paoSubPoolSizes[i]; 
      }
      else
      {
        idxOffset = RAMPOOL_NO_OBJECT;
      }
    }
    *poOffsetArg = idxOffset;
    *poSizeArg = idxSize;
    /*## END[GUID 1b8bdc6f-158a-46ff-9864-5b8ded27fc93] ##*/
  }
}


// Private functions implementation
//------------------------------------


/*!************************************************************************************************
* \fn         static TIndex RamPool_PRV_CreateInstance(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TIndex idxOffsetArg, TIndex idxSizeArg)
* \param[in,out] psPoolInfoArg
* \param[in]  idxOffsetArg
* \param[in]  idxSizeArg
* \return     TIndex 
***************************************************************************************************/
static TIndex RamPool_PRV_CreateInstance(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TIndex idxOffsetArg, TIndex idxSizeArg)
{  
  {
    /*## BEGIN[GUID cb09b380-5cc0-409e-ae14-6346ab40abcc] ##*/
    if(idxSizeArg == 0)
    {
      return RAMPOOL_NO_OBJECT;
    }
    
    while(idxSizeArg)
    {
    	if(psPoolInfoArg->spfAssessors.GetState(idxOffsetArg) == RAMPOOL_INSTANCE_IS_FREE)
    	{
        // Set the instance as busy now
        //---------------------------------------   		
        psPoolInfoArg->spfAssessors.SetState(idxOffsetArg, RAMPOOL_INSTANCE_IS_BUSY);
        return idxOffsetArg;
      }
      idxOffsetArg++;
      idxSizeArg--;
    }
    return RAMPOOL_IS_FULL; 
    /*## END[GUID cb09b380-5cc0-409e-ae14-6346ab40abcc] ##*/
  }
}

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
