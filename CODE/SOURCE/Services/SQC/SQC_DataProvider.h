// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once
#include "Config.h"

typedef enum SQC_DataProviderName_e
{
  SQC_DataProvider_Controller,  // 控制器  数据提供
  SQC_DataProvider_LowPower,    // 低电压  数据提供
  SQC_DataProvider_Ergonomic,   // 人为控制
  SQC_DataProvider_Encoder,     // 编码器
  SQC_DataProvider_MotorDriver, // 电机驱动
  SQC_DataProvider_Position,    // 位置
  SQC_DataProvider_PowerMonitor,// 电压检测
  SQC_DataProvider_Chrono,      // 时间统计
  SQC_DataProvider_Archiver,    // EEPROM存储
  SQC_DataProvider_Event,       // 事件
  SQC_DataProviderName_LastEnum,
}SQC_DataProviderName_e;


typedef struct SQC_DataProviderNameInterface_s
{
   TU8* (*Fptr_Get)(TU8* DataLenght);
}SQC_DataProviderNameInterface_s;


void SQC_DataProvider_RegisterDataSource(SQC_DataProviderName_e DataProvider,SQC_DataProviderNameInterface_s *p_Interface);
void SQC_Count(void *ptr_Ctr,TU8 sizeofv);

