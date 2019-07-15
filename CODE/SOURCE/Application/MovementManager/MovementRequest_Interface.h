// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#pragma once
#include "Config.h"
#include "Limits.h"
#include "Position.h"



typedef enum MoveRequestType_e
{
  MoveRequestType_SecurityStop ,
  MoveRequestType_None,
  MoveRequestType_Manual_Stop,  //Manual move are perform without counting position, and without limit monitor
  MoveRequestType_Manual_Roll,
  MoveRequestType_Manual_UnRoll,
  MoveRequestType_Manual_RollForTime,
  MoveRequestType_Manual_UnRollForTime,
  MoveRequestType_Manual_PWM,
  MoveRequestType_Nolimit_RollForTime,
  MoveRequestType_Nolimit_UnRollForTime,
  MoveRequestType_Nolimit_StopForTime,
  MoveRequestType_Nolimit_Stop,
  MoveRequestType_Nolimit_RollTo,
  MoveRequestType_Nolimit_UnRollTo,
  MoveRequestType_Nolimit_ShortUnRoll,
  MoveRequestType_Nolimit_ShortRoll,
  MoveRequestType_Nolimit_Rolloff,
  MoveRequestType_Nolimit_UnRolloff,
  MoveRequestType_Nolimit_ShortFeedBack,
  MoveRequestType_Nolimit_ErrFeedBack,
  MoveRequestType_Nolimit_ErrFeedBackMoveTo,
  MoveRequestType_Nolimit_DoubleShortFeedBack,
  MoveRequestType_Nolimit_ThreeShortFeedBack,
  MoveRequestType_Nolimit_FourShortFeedBack,
  MoveRequestType_Nolimit_10sCycleMove, 
  MoveRequestType_Nolimit_TiltingCycleMove,
  MoveRequestType_TiltMyPosition,
  MoveRequestType_RollForTime,
  MoveRequestType_UnRollForTime,
  MoveRequestType_RollTo,
  MoveRequestType_UnRollTo,
  MoveRequestType_GoToPercent,
  MoveRequestType_GoToAbsPos,
  MoveRequestType_ShortUnRoll,
  MoveRequestType_ShortRoll,
  MoveRequestType_Rolloff,
  MoveRequestType_UnRolloff,
  MoveRequestType_RollToLimits,
  MoveRequestType_UnRollToLimits,
  MoveRequestType_GoToLimits,
  MoveRequestType_RollToPosition,
  MoveRequestType_UnRollToPosition,
  MoveRequestType_GoToPosition,
  MoveRequestType_CompensationMove,
  MoveRequestType_LAST_ENUM,
}MoveRequestType_e;

typedef union Union_MoveRequestParam
{
  TU32 AbsPos;
  TU32 Time_mS;   //原来是16位 但是不能到3分钟
  TU8  GOTO;
  TU8  MoveOff;
  TS16 PWM;
  ModeLimitName_e Limit;
  PositionName_e  Position;
}Union_MoveRequestParam;

static const Union_MoveRequestParam NoMoveParam = {0};

typedef struct MoveRequest_s
{
  MoveRequestType_e      MoveType;
  Union_MoveRequestParam MoveRequestParam;
}MoveRequest_s;




void MoveRequest_Post(const MoveRequest_s *Mvrq);
void MoveRequest_Execute(const MoveRequest_s *Mvrq);

