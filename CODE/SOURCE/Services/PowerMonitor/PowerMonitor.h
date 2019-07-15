// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once
#include "Config.h"


typedef enum Power_Capacity
{
  PowerLevelNull,     //0
  PowerLevelHigh,     //1
  PowerLevelMiddle,   //2
  PowerLevelLow,      //3
}Power_Capacity;

void PowerMonitor_Init();
void PowerMonitor_Create(void (*PowerFailCallBack)(void));
void PowerMonitor_CMD(TBool OnOFF);

void PowerMonitor_SetupHardware_HighPower();
void PowerMonitor_SetupHardware_LowPower();
TU16 PowerMonitor_MeasureVbat(TBool PWRCallBack);
TBool PowerMonitor_BatteryLevelAllowMove();

TU16 PowerMonitor_GetVBatAvr();
TU16 PowerMonitor_GetMaxVBatAvr();
TU16 PowerMonitor_GetMinVBatAvr();



