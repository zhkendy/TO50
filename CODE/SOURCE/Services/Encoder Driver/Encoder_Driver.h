// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once
#include "Config.h"
#include "stack.h"
#include "RTS_InputCapture.h"

typedef struct
{
  TU32 MotorTurn;
  TU16 MotorPosition; 
}EncoderPosition_s;


typedef enum EncoderDriverMovingDirection_e
{
  EncoderDriver_Stop   = 0,
  EncoderDriver_Roll   = -1,
  EncoderDriver_UnRoll = 1,
}EncoderDriverMovingDirection_e;



__monitor TU32 EncoderDriver_GetPosition();
__monitor EncoderPosition_s EncoderDriver_GetFinePosition();
__monitor void EncoderDriver_UpdateFinePosition();
__monitor void EncoderDriver_SetPositionCoarse(TU32 NewPos);
__monitor void EncoderDriver_SetPositionFine(EncoderPosition_s NewPos);

__monitor EncoderDriverMovingDirection_e EncoderDriver_GetMoveDirection();

 void EncoderDriver_SetUpHardware_HighPower();
 void EncoderDriver_SetUpHardware_LowPower();
 
__monitor TS16 EncoderDriver_GetSpeed();


void EncoderDriver_IT_Handler_Timer2_Update();
void EncoderDriver_IT_Handler_Timer1_Update();
void EncoderDriver_IT_MotorRotationTimeTimerTimeout();

TBool EncoderDriver_IsPowerOn();
void IT_SpeedTimerTimeout();


void EncoderDriver_Init();
void EncoderDriver_Create(TBool HotReset);
void EncoderDriver_Reset();
void EncoderDriver_SensorCmd(TBool State);
void EncoderDriver_PosCounterCMD(TBool State);
void EncoderDriver_SetPersistantDataToFactoryValue();
void EncoderDriver_InvertCountingDirection();





