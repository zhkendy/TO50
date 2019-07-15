// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#pragma once

#include "Config.h"
#include "Stack.h"





typedef struct RSSI_s
{
  TU16 RSSI_HP;
  TU16 LastMeasure;
  TU16 Floor;
  TU16 FastAvrerage;
}RSSI_s;


void RtsLowPowerManager_Init();
void RtsLowPowerManager_Create();


void RtsLowPowerManager_SleepLoop();
//Used By indus test
void RtsLowPowerManager_MeasureRSSIFromHighPower();
TU16 RtsLowPowerManager_GetRSSI();



void RtsLowPowerManager_SetUpHardware_LowPower();
void RtsLowPowerManager_SetUpHardware_HighPower();

void RtsLowPowerManager_Set_RTS_Timing_Flag();
void RtsLowPowerManager_Set_UnpairedRTS_Flag();
void RtsLowPowerManager_Set_PairedRTS_Flag();
