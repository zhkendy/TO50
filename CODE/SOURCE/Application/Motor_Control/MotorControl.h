// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#pragma once
#include "Encoder_Driver.h"
#include "Motor_Driver.h"

/**
* @file MotorControl.h
* @brief Header of MotorControl.c
* @author Gabriel Mula
* @version xx
* @date Septembre 2015
*/






//@startuml
//[*] --> ValidMotorStopped
//ValidMotorStopped --> ValidMotorMove : MotorState = Un/Rolling during MoveStartUpTime
//ValidMotorStopped --> Locked_BlockedOnStartRolling :   MotorState = ForcedStop during MoveStartUpTime
//ValidMotorStopped --> Locked_BlockedOnStartUnRolling : MotorState = ForcedStop during MoveStartUpTime
//ValidMotorStopped --> Locked_ForcedRoll:   MotorState = ForcedRoll during MoveForcedTime
//ValidMotorStopped --> Locked_ForcedUnroll: MotorState = ForcedRoll during MoveForcedTime
//ValidMotorMove    --> ValidMotorStopped :  MotorState = Stopped during MoveStopTime
//ValidMotorMove    --> Locked_BlockedWhileRolling :   MotorState = ForcedStop during MoveBlockedTime
//ValidMotorMove    --> Locked_BlockedWhileUnRolling : MotorState = ForcedStop during MoveBlockedTime
//ValidMotorMove    --> Locked_ToLowRollingSpeed :  Speed < Minimum Speed during LowSpeedTime
//ValidMotorMove    --> Locked_ToLowUnRollingSpeed: Speed < Minimum Speed during LowSpeedTime
//ValidMotorMove    --> Locked_ForcedRoll :   MotorState = ForcedRoll during MoveForcedTime
//ValidMotorMove    --> Locked_ForcedUnroll : MotorState = ForcedRoll during MoveForcedTime
//@enduml


typedef enum
{
  LoopMode_PWM,        
  LoopMode_Brake, //以上都是直接操作 pwm
  LoopMode_Speed, //需要转换为       pwm      
}MotorControl_LoopMode_e;

typedef struct
{
  MotorDirection_e Direction;
  MotorControl_LoopMode_e LoopMode;
  TU16 AbsTarget;
  TU16 AbsStartUp;
  TU16 AccMax;
  TU16 DccMax;
}MotorControlSetPoint_s;


typedef enum MotorState_e
{
  MotorState_BlockedRoll=0,
  MotorState_ForcedRoll,
  MotorState_Rolling,
  MotorState_Rolling_LowSpeed ,
  MotorState_BlockedUnRoll,
  MotorState_ForcedUnRoll,
  MotorState_UnRolling,
  MotorState_UnRolling_LowSpeed,
  MotorState_Stopped,
}MotorState_e;

typedef struct MotorControl_SegmentSpeedTrigger_s
{
  TBool TriggerEnable;
  TS16  SpeedTriggerValue; 
}MotorControl_SegmentSpeedTrigger_s;

typedef struct MotorControl_SegmentPosTrigger_s
{
  TBool TriggerEnable;
  TU32  PosTriggerValue; 
}MotorControl_SegmentPosTrigger_s;

typedef struct MotorControl_TimeTrigger_s
{
  TBool TriggerEnable;
  TS32  TimeTriggerValue; 
}MotorControl_SegmentTimeTrigger_s;

typedef struct MotorControl_Segment_s
{
  TBool UnInteruptible;
  MotorControl_SegmentSpeedTrigger_s SpeedTrigger;
  MotorControl_SegmentPosTrigger_s   PositionTrigger;
  MotorControl_SegmentTimeTrigger_s  TimeTrigger;
  MotorControlSetPoint_s SetPoint;   //运动参数 方向 循环 数值
}MotorControl_Segment_s;


//

void MotorControl_Init(  void (*Fprt_MotorControlIsLockBySecuManager)(void),void (*Fprt_MotorControlIsUnLock)(void));
void MotorControl_Create(TBool WatchdogReset);

void MotorControl_ControlLoop(const TS16 *Speed,const TU32 *Position);
void MotorControl_HoldCurrentSetSegment();
void MotorControl_SecuLock();
MotorState_e MotorControl_GetSecuLockState();
TBool MotorControl_IsSecuLock();
void MotorControl_ReleaseLock();
MotorState_e MotorControl_GetState();

void MotorControl_SetSegment(const MotorControl_Segment_s *Segment);
void MotorControl_SecurityMonitorCMD(TBool Enable);
const MotorControl_Segment_s* MotorControl_GetCurrentSegment();
MotorDirection_e MotorControl_GetCurrentSetMoveDirection();
MotorDirection_e MotorControl_GetLastMoveDirection();


