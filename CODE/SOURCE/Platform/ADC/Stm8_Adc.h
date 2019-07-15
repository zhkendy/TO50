// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#include "Config.h"
#include "Pcb.h"

typedef enum ADC_Mode_e
{
  ADC_Mode_PowerCharge,
  ADC_Mode_PowerMonitor_Pline,
  ADC_Mode_RSSI,
  ADC_Mode_Off,
  ADC_Mode_LastEnum,
}ADC_Mode_e;



void ADC_Init();
void ADC_Setup(ADC_Mode_e Mode);
TU16 ADC_GetValue_Polling(ADC_Mode_e Mode);
//TU16 ADC_GetValue_IT(ADC_Mode_e Mode, void (*Fptr_IT_ADC_CallBack(TU16 AdcMeasure));

