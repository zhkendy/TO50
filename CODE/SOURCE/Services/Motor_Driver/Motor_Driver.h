// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#pragma once
#include "Config.h"



/**
* @file Motor_Driver.h
* @brief Motor_Driver.c Header file
* @author Gabriel Mula
* @version xx
* @date Septembre 2015
*/


typedef enum MotorDirection_e
{
  MotorDirection_Stop  =0,
  MotorDirection_Roll  =-1,
  MotorDirection_UnRoll=1
}MotorDirection_e;

void MotorDriver_RotateDirA(TU16 Cst);
void MotorDriver_RotateDirB(TU16 Cst);

void MotorDriver_Move(TS16 DutyCycle,TS16 CurrentSpeed);
void MotorDriver_Move2(TS16 DutyCycle);
void MotorDriver_Brake(TS16 DutyCycle);

void MotorDriver_SwitchToLowPower();
void MotorDriver_RestoreFromLowpower();

void MotorDriver_Create();
void MotorDriver_Init();
void MotorDriver_RestInit();

void MotorDriver_InvertRotation();
void MotorDriver_SetPersistantDataToFactoryValue();
MotorDirection_e MotorDriver_GetLastMoveDirection();
void MotorDriver_CountMotorBloccked();