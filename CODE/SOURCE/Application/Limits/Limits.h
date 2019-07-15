// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once

#include "Config.h"

//1) Add enum
//2) Edit ComputeModeLimitsValue() function
typedef enum LimitMode_e
{
  LimitMode_Normal,
  LimitMode_settingUEL,
  LimitMode_settingDEL,
  LimitMode_ResettingUEL,
  LimitMode_ResettingDEL,
  LimitMode_LastEnum,
}LimitMode_e;

typedef enum ModeLimitName_e
{
  ModeLimitName_UpLimit,
  ModeLimitName_DownLimit,
}ModeLimitName_e;

typedef struct ModeLimit_s
{
  TU32 UpLimit;
  TU32 DownLimit;
}ModeLimit_s;






//Do not change enum order/Value. Used to compare with > and <
typedef enum Limits_State_e
{
  Limits_State_Above,
  Limits_State_OnUp,
  Limits_State_Between,
  Limits_State_OnDown,
  Limits_State_Below,
}Limits_State_e;


void limits_Init();
void limits_Create(TU32 *CurrentPosition);
void Limits_SetMode(LimitMode_e Mode);
TU32 Limits_GetLimitValue(ModeLimitName_e LimitName);
TU32 Limits_GetLimitDistance(const TU32 *CurrentPosition,ModeLimitName_e LimitName);
TBool Limits_IsTargetPositionInsideLimits(const TU32 *TargetPosition,TU8 Tolerance);
void Limits_ComputeLimitsState(const TU32 *Position);
void Limits_Monitor();
void Limits_MonitorCMD(TBool Cmd);
Limits_State_e Limits_GetLimitsState();
