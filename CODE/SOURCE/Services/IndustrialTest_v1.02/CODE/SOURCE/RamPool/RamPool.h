/*********************************************************************
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
// File "RamPool.h"   Generated with SOMFY code generator v[4.6.0.0]
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef RAMPOOL_H
#define RAMPOOL_H


// Public types
//--------------

#ifdef INDEX_ON_8_BITS
  #define RAMPOOL_ERROR                   252U
  #define RAMPOOL_BAD_ID                  253U
  #define RAMPOOL_IS_FULL                 254U
  #define RAMPOOL_NO_OBJECT               255U
#else
  #define RAMPOOL_ERROR                   65532U
  #define RAMPOOL_BAD_ID                  65533U
  #define RAMPOOL_IS_FULL                 65534U
  #define RAMPOOL_NO_OBJECT               65535U
#endif

#define RAMPOOL_ERROR_DETECTED(ret)     (ret >= RAMPOOL_ERROR)

#ifdef INDEX_ON_8_BITS
	#define RAMPOOL_NO_SUBPOOL_ID			      255U
#else
	#define RAMPOOL_NO_SUBPOOL_ID			      65535U
#endif


typedef struct
{
  TIndex oNbOfSubPool;
  CONST TIndex * paoSubPoolSizes;
} TsSubPoolInfo;

typedef enum
{
  RAMPOOL_INSTANCE_IS_FREE = 0, //!< Always 0 if unused !
  RAMPOOL_INSTANCE_IS_BUSY = 1
} TeRamPoolState;

typedef TeRamPoolState (*TGetStateCBack)(TIndex oIndexArg);

typedef TeRamPoolState (*TSetStateCBack)(TIndex oIndexArg, TeRamPoolState eStateArg);

typedef struct
{
  TSetStateCBack SetState;
  TGetStateCBack GetState;
} TspfStateAssessors;

typedef struct
{
  TspfStateAssessors spfAssessors;
  TIndex idxNbOfElements;
  void * pbtRamDataAddress;
  CONST TsSubPoolInfo * psSubPoolInfo;
} TsRamPoolInfo;


// Public functions declaration
//-----------------------------------

void RamPool_Init(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg);

TIndex RamPool_CreateInstance(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg);

TIndex RamPool_CreateInstanceInSubPool(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TByte btSubPoolIdArg);

TBool RamPool_DestroyInstance(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TIndex idxInstanceArg);

TIndex RamPool_GetAbsoluteInstanceIdInSubPool(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TByte btSubPoolIdArg, TIndex idxInstanceArg);

TBool RamPool_IsItFull(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TByte btSubPoolIdArg);

TIndex RamPool_GetNbOfInstancesInSubPool(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TByte btSubPoolIdArg);

TIndex RamPool_GetNbOfInstances(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg);

TIndex RamPool_GetSubPoolForThisInstance(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TIndex idxInstanceArg);

void RamPool_GetSubPoolLimits(CONST_ARG TsRamPoolInfo CONST_AREA* psPoolInfoArg, TIndex oSubPoolIdArg, TIndex* poOffsetArg, TIndex* poSizeArg);


// Header end
//--------------
#endif // RAMPOOL_H

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
