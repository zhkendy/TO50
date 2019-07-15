// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once

static const char *MoveRequestType_TXT[] = 
{
 "SecurityStop \0",
 "None \0",
 "Manual_Stop \0", 
 "Manual_Roll \0",
 "Manual_UnRoll \0",
 "Manual_RollForTime \0",
 "Manual_UnRollForTime \0",
 "Manual_PWM \0",
 "Nolimit_RollForTime\0",
 "Nolimit_UnRollForTime\0",
 "Nolimit_StopForTime\0",
 "Nolimit_RollTo\0",
 "Nolimit_UnRollTo\0",
 "Nolimit_ShortUnRoll\0",
 "Nolimit_ShortRoll\0",
 "Nolimit_Rolloff\0",
 "Nolimit_UnRolloff\0",
 "RollForTime \0",
 "UnRollForTime \0",
 "StopForTime \0",
 "RollTo \0",
 "UnRollTo \0",
 "Stop \0",
 "GoTo \0",
 "GoToAbsPos\0",
 "ShortFeedBack \0",
 "LongFeedBack \0",
 "DoubleShortFeedBack \0",
 "10cycle\0"
 "tiltMyplace\0"
 "ShortUnRoll \0",
 "ShortRoll \0",
 "Rolloff \0",
 "UnRolloff \0",
 "RollToLimits \0",
 "UnRollToLimits \0",
 "GoToLimits \0",
 "RollToPosition \0",
 "UnRollToPosition \0",
 "GoToPosition \0",
};


static const char *MovementManager_MoveType_TXT[] = 
{
  "SecurityStop\0",
  "None\0",
  "Manual_Stop\0",  
  "Manual_Move\0",
  "Manual_MoveForTime\0",
  "Manual_PWM\0",
  "MoveForTime\0",
  "StopForTime\0",
  "MoveTo\0",
  "MoveOff\0",
  "Stop\0",
};

static const char *MovementManager_Result[] = 
{
  "FAILD\0",
  "SUCCESS\0",
};

