// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


/**
* @file MovementManager.h
* @brief blabla
* @details blabla
* @author Gabriel Mula
* @version xx
* @date Septembre 2015
*
*/

#pragma once
#include "Config.h"
#include "MotorControl.h"
#include "Stack.h"
#include "Encoder_Driver.h"
#include "MovementRequest_Interface.h"

//Do not CHANGE
// This enum is used by function pointer array in moveplanner
typedef enum MovementManager_MoveType_e
{
  MovementManager_MoveType_SecurityStop = 0,
  MovementManager_MoveType_None,
  MovementManager_MoveType_Manual_PWM,
  MovementManager_MoveType_Move,
  MovementManager_MoveType_MoveForTime,
  MovementManager_MoveType_Stop,
  MovementManager_MoveType_StopForTime,
  MovementManager_MoveType_MoveTo,
//  MovementManager_MoveType_MoveOff,
  MovementManager_MoveType_LASTENUM,
}MovementManager_MoveType_e;



typedef enum MoveOption_e
{
  MoveOption_NoOption,          //Default ACC/DCC/Speed
  MoveOption_ShortMove,         //短时运行
  MoveOption_UseCustom,         //使用习惯
  MoveOption_NormalUserStop,    //正常的用户停止
  MoveOption_HardStop,          //硬停
  MoveOption_ForceStop,         //强制停止
}MoveOption_e;


typedef struct MovementManager_Move_s
{
  MovementManager_MoveType_e MoveType;
  MotorDirection_e           MoveDirection;
  MoveOption_e               MoveOption;
  
  TU16 CustomACC;
  TU16 CustomDCC;
  TU16 CustomSpeed;
  
  union
  {
    TS32 MoveOffLenght;
    TU32 AbsFinalPlanedPosition;
    TU32 MoveTime_ms;
    TS16 PWM;
  };

}MovementManager_Move_s;







void MotorControlReset(void);
void MovementManager_Init();
void MovementManager_Create(TBool WatchdogReset);

void MovementManagerLoop();

TBool MovementManager_NoneSet();
TBool MovementManager_IsDirectionLock(MotorDirection_e DirToTest);
void MovementManager_UnLockOppositeDirection(MotorDirection_e MoveDir);
//
void MovementManager_SetNextAction();
void MovementManager_PlanAndSetNextMove();
void MovementManager_SetNextSegment();


