// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#include "Config.h"
#include "stm8s_gpio.h"
#include "stm8s_adc2.h"
#include "stm8s_clk.h"

#include "Stm8_Adc.h"


typedef struct Stm8_Adc_s
{
  ADC_Mode_e CurrentMode;
}Stm8_Adc_s;



Stm8_Adc_s Stm8_Adc;
#define me Stm8_Adc


void ADC_Init()
{
  ADC_Setup(ADC_Mode_Off);
}


void ADC_Setup(ADC_Mode_e Mode)
{
  me.CurrentMode = Mode;
  switch(Mode)
  {
  case ADC_Mode_PowerCharge :
    //////////////////////////////////////////////////////////////////////////////////  
    ADC2_DeInit();
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC,ENABLE);
    GPIO_Init(POWERCH_PORT,POWERCH_PIN,POWERCH_MODE);
    ADC2_Cmd(ENABLE);
    ADC2_Init(ADC2_CONVERSIONMODE_SINGLE,
              POWERCH_MEASURE_CHANNEL,
              ADC2_PRESSEL_FCPU_D4,
              ADC2_EXTTRIG_TIM, DISABLE,
              ADC2_ALIGN_RIGHT,
              ADC2_SCHMITTTRIG_ALL,DISABLE);

    ADC2_ClearFlag();
  break;
    //////////////////////////////////////////////////////////////////////////////////  
  case ADC_Mode_PowerMonitor_Pline :
    //////////////////////////////////////////////////////////////////////////////////  
    ADC2_DeInit();
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC,ENABLE);
    GPIO_Init(BATMeasure_PORT,BATMeasure_PIN,BATMeasure_MODE);
    ADC2_Cmd(ENABLE);
    ADC2_Init(ADC2_CONVERSIONMODE_SINGLE,
              BATMeasure_CHANNEL,
              ADC2_PRESSEL_FCPU_D4,
              ADC2_EXTTRIG_TIM, DISABLE,
              ADC2_ALIGN_RIGHT,
              ADC2_SCHMITTTRIG_ALL,DISABLE);

    ADC2_ClearFlag();
    break;
    
    //////////////////////////////////////////////////////////////////////////////////  
  case ADC_Mode_RSSI :
    //////////////////////////////////////////////////////////////////////////////////  
    ADC2_DeInit();
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC,ENABLE);
    
    GPIO_Init(Radio_RSSI_MEASURE_PORT,Radio_RSSI_MEASURE_PIN,Radio_RSSI_MEASURE_MODE);
    
    ADC2_Init(ADC2_CONVERSIONMODE_SINGLE,
              Radio_RSSI_MEASURE_CHANNEL,
              ADC2_PRESSEL_FCPU_D12,
              ADC2_EXTTRIG_TIM, DISABLE,
              ADC2_ALIGN_RIGHT,
              ADC2_SCHMITTTRIG_ALL,DISABLE);
    
    ADC2_Cmd(ENABLE);
    ADC2_ClearFlag();
    break;
    
    //////////////////////////////////////////////////////////////////////////////////  
  case ADC_Mode_Off :
    //////////////////////////////////////////////////////////////////////////////////  
    ADC2_Cmd(DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC,DISABLE);
    GPIO_Init(Radio_RSSI_MEASURE_PORT,Radio_RSSI_MEASURE_PIN,Radio_RSSI_MEASURE_MODE);
    GPIO_Init(BATMeasure_PORT,BATMeasure_PIN,BATMeasure_MODE);
    GPIO_Init(POWERCH_PORT,POWERCH_PIN,POWERCH_MODE);
    break;
  }
}


TU16 ADC_GetValue_Polling(ADC_Mode_e Mode)
{
  TU16 Rtn = 0U;
  if(me.CurrentMode != Mode)
  {
    ADC_Setup(Mode);
  }
  
  if(me.CurrentMode != ADC_Mode_Off)
  {
    ADC2_ClearFlag();
    ADC2_StartConversion();
    while(ADC2_GetFlagStatus() == RESET);
    Rtn = ADC2_GetConversionValue();
    ADC2_ClearFlag();
  }
  
  return Rtn;
}


//TU16 ADC_GetValue_IT(ADC_Mode_e Mode, void (*Fptr_IT_ADC_CallBack(TU16 AdcMeasure))
//{
//}