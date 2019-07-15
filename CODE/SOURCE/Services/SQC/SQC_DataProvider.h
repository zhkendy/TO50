// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once
#include "Config.h"

typedef enum SQC_DataProviderName_e
{
  SQC_DataProvider_Controller,  // ������  �����ṩ
  SQC_DataProvider_LowPower,    // �͵�ѹ  �����ṩ
  SQC_DataProvider_Ergonomic,   // ��Ϊ����
  SQC_DataProvider_Encoder,     // ������
  SQC_DataProvider_MotorDriver, // �������
  SQC_DataProvider_Position,    // λ��
  SQC_DataProvider_PowerMonitor,// ��ѹ���
  SQC_DataProvider_Chrono,      // ʱ��ͳ��
  SQC_DataProvider_Archiver,    // EEPROM�洢
  SQC_DataProvider_Event,       // �¼�
  SQC_DataProviderName_LastEnum,
}SQC_DataProviderName_e;


typedef struct SQC_DataProviderNameInterface_s
{
   TU8* (*Fptr_Get)(TU8* DataLenght);
}SQC_DataProviderNameInterface_s;


void SQC_DataProvider_RegisterDataSource(SQC_DataProviderName_e DataProvider,SQC_DataProviderNameInterface_s *p_Interface);
void SQC_Count(void *ptr_Ctr,TU8 sizeofv);

