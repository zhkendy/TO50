// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once


#include "Config.h"

#define CLOSE_PERCENTAGE_VALUE           (200)

typedef enum PositionName_e
{
  PositionName_UEL,
  PositionName_DEL,
  PositionName_My,
  PositionName_Origin,
  PositionName_EndOfWorld,
  PositionName_LastEnum,
}PositionName_e;

typedef enum PositionCompare_e
{
  CurrentPositionIsAboveRef,
  CurrentPositionIsBelowRef,
  CurrentPositionIsOnRef,
  RefPositionIsNotSet,
}PositionCompare_e;


void Position_Init();
void Position_Create();
void Position_Reset();


const TU32* Position_GetValue(PositionName_e PosName);

TBool PositionMY_Set(PositionName_e PosName, TU32 PosValue);
TBool Position_IsPositionSet(PositionName_e PosName);
TBool Position_Set(PositionName_e PosName, TU32 PosValue);
TBool Position_Disable(PositionName_e PosName);
TBool IsOnPosition(PositionName_e PosNameRef,const TU32* CurrentPosition);
PositionCompare_e Position_CompareTwoAbsolutePosition(const TU32* PosRef,const TU32* CurrentPosition,TU8 Tolerance);
PositionCompare_e Position_Compare(PositionName_e PosNameRef,const TU32* CurrentPosition,TU8 Tolerance);
TU32 Position_ConvertPercentageCloseToAbsolutePosition(TU8 PercentageClose);
TU32 Position_MyPlace();
TU32 Position_DEL();
TU32 Position_UEL();
