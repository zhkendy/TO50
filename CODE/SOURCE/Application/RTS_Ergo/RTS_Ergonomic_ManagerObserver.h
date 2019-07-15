// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once
#include "RTS_Ergonomic_ManagerSharedType.h"

// Malloc is forbid by coding rule, you have to manually add Observer here
typedef enum RTSErgonomicObserver_e
{
  //Caution !! Order matters
  RTSErgonomicObserver_Limits,
  RTSErgonomicObserver_Controller,
  RTSErgonomicObserver_LastEnum,
}RTSErgonomicObserver_e;

typedef enum RTSErgoSpecialRequest_e
{
  RTSErgo_Request_RemoteEepromBackup,
  RTSErgo_Request_InvertRotationDirection,
  RTSErgo_Request_MotorReset,
  RTSErgo_Request_RunAutotest1,
  RTSErgo_Request_RunAutotest3,
  RTSErgo_Request_RunAutotest4,
  RTSErgo_Request_RunAutotest5,
  RTSErgo_Request_RunAutotest6,
  RTSErgo_EMS_SensitivityFrameReceived,
  RTSErgo_Speed_UserModeChange,
  RTSErgo_Initial_SetNull,
  RTSErgo_Initial_SetFlag,
  RTSErgo_Initial_Finish,
  RTSErgo_Initial_SetAndNotPaired,//knedy????
  RTSErgo_Initial_SetAndTemporary,
  RTSErgo_Initial_DeliveryMode1, 
  RTSErgo_Initial_DeliveryMode2, 
  RTSErgo_Initial_DeliveryMode3, 
}RTSErgoSpecialRequest_e;

typedef struct RTSErgonomicObserver_s
{
  void (*Fptr_ErgoChageMode)(ErgoMode_e NewMode);
  void (*Fptr_RTSErgoSpecialRequest)(RTSErgoSpecialRequest_e Sreq);
}RTSErgonomicObserver_s;


void RTSErgonomicObserver_RegisterObserver(RTSErgonomicObserver_e ObserverName,RTSErgonomicObserver_s Observer);


