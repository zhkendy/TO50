// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

/**
* @file STM8_Archiver.h
* @brief Header de STM8_Archiver.c
* @author Gabriel Mula
* @version xx
* @date Septembre 2015
*
*/

#pragma once
#include "Config.h"
#include "stm8s_flash.h"                //stm8s flash driver

#include "STM8_Archiver_Conf.h"




void Archiver_Create();
void Archiver_Init();


TBool Archiver_RegisterObject( void *obj,void (*pf_SetToFactory)(void),TU8 ObjectSize,TBool IsCritical);
void Archiver_SaveAllObject(void);
void Archiver_SaveOnlyCriticalObject(void);
void Archiver_LoadMe(void *Me);
void Archiver_NoSetToFactoryFunction();

TBool Motor_Parameter_BackUp(TU32 Address, TU8 Me);
TU8   Motor_Parameter_Read(TU32 Address);







