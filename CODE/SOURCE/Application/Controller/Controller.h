// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#pragma once


#include "Config.h"
#include "MovementRequest_Interface.h"

typedef enum Uart_Test_CMD_e
{
  UartCommand_NULL,
  UartCommand_UP,
  UartCommand_DOWN,
  UartCommand_STOP,
  UartCommand_DOWN5TURN,
  UartCommand_SOFTWARENU, 
  UartCommand_REST,
  UartCommand_RESTART,  
}Uart_Test_CMD_e;

typedef enum ActuatorState_e
{
  ActuatorState_Boot,
  ActuatorState_StartEMS,
  ActuatorState_EMS_Mode,
  ActuatorState_EMS_RTS_Sensitivity,
  ActuatorState_StopEMS,
  ActuatorState_Autotest,
  ActuatorState_RunPowerOnAction,
  ActuatorState_HighPower,
  ActuatorState_LowPower,
}ActuatorState_e;

typedef enum Initial_Set_Mode
{
  Initial_Set_Null,
  Initial_Set_Flage,
  Initial_SetAndNotPaired,
  Initial_SetAndTemporary,
  Initial_Set_Finish,
  Initial_DeliveryMode1,
  Initial_DeliveryMode2,
  Initial_DeliveryMode3,  
}Initial_Set_Mode;

typedef enum FatoryPower_MoveMode
{
 FatoryColdPower,
 FatoryHotPower,
 FatoryBeReset,
}FatoryPower_MoveMode;

void Controller_SetPdataToFactory();
void Controller_Init(void);
void Controller_Create(void);
void Controller_SetState(ActuatorState_e State);
void Controller_Idle(void);
ActuatorState_e Controller_GetConntrollerGloabalState(void);
void ControllerSetNoRTSFlag();
void Controller_PowerOffSensor();
void Controller_RTSActivityNotify();
void Controller_TestRTSTiming();
void LimitBack(void);


