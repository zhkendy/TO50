// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#include "Config.h"
#include "Stm8_Adc.h"
#include "stm8s_gpio.h"
#include "pcb.h"

#include "STM8_Archiver.h"
#include "SQC_DataProvider.h"
#include "PowerMonitor.h"

TU8 User_LedMode=PowerLevelNull;
TU8 PowerMonitorCount=0;

extern TU16 Ledtime_outsecond;

typedef struct PowerMonitor_Pdata_s
{
  TU16 BatMeasureAvr;
  TU16 MaxBatMeasureAvr;
  TU16 MinBatMeasureAvr;
}PowerMonitor_Pdata_s;

typedef struct PowerMonitor_s
{
  TBool MonitorVBat;
  void (*PowerFailCallBack)(void);
  TU16 LastMeasure;
  PowerMonitor_Pdata_s Pdata;
  TBool BatMeasureHardwareIsOn;
}PowerMonitor_s;

typedef struct PowerMonitor_SQC_s
{
  TU16 Max;
  TU16 Min;
  TU16 Avr;
}PowerMonitor_SQC_s;


static PowerMonitor_s PwrMonitor;
static PowerMonitor_SQC_s SQC;
#define me (PwrMonitor)
static void VoidFPrtTrap();
static void PowerMonitorSetToFactory();
static TU8* Get_SQC_data(TU8* DataSize);




static void PowerMonitorSetToFactory()
{  
  me.Pdata.MaxBatMeasureAvr = 0U;
  me.Pdata.MinBatMeasureAvr = 1500;
  me.Pdata.BatMeasureAvr    = 1500;//需要调整计算 sage
}

void PowerMonitor_Init()
{
  me.MonitorVBat = FALSE;
  me.BatMeasureHardwareIsOn = FALSE;
  me.PowerFailCallBack = VoidFPrtTrap; //回调函数空
  
  Archiver_RegisterObject(&me.Pdata,
                          PowerMonitorSetToFactory,
                          sizeof(me.Pdata),
                          FALSE);
     
     //SQC 
   SQC_DataProviderNameInterface_s intf;
   intf.Fptr_Get = Get_SQC_data;
   SQC_DataProvider_RegisterDataSource(SQC_DataProvider_PowerMonitor ,&intf);//电源检测数据
}


static TU8* Get_SQC_data(TU8* DataSize)  //获取数据
{
  //Take and return snapshot
  *DataSize = sizeof(SQC);
  SQC.Avr = me.Pdata.BatMeasureAvr;
  SQC.Max = me.Pdata.MaxBatMeasureAvr;
  SQC.Min = me.Pdata.MinBatMeasureAvr;
  return (TU8*) &SQC;
}

void PowerMonitor_Create(void (*PowerFailCallBack)(void))
{
  if(PowerFailCallBack != NULL)
  {
    Archiver_LoadMe(&me.Pdata);
    me.PowerFailCallBack = PowerFailCallBack;
    PowerMonitor_CMD(TRUE);
    me.LastMeasure = me.Pdata.BatMeasureAvr;
    me.Pdata.BatMeasureAvr = ADC_GetValue_Polling(ADC_Mode_PowerMonitor_Pline);
  }
  else
  {
    trap();
  }
}

static void VoidFPrtTrap()
{
  trap();
}

void PowerMonitor_CMD(TBool OnOFF)
{
  me.MonitorVBat = OnOFF;
  if(me.MonitorVBat == TRUE)
  {
    PowerMonitor_SetupHardware_HighPower();//电压检测硬件使能
  }
  else
  {
    PowerMonitor_SetupHardware_LowPower();
  }
}
  

TU16 PowerMonitor_MeasureVbat(TBool PWRCallBack)
{
  TU16 LastMeasure = 0;
  if(me.MonitorVBat == TRUE)
  {
    if(me.BatMeasureHardwareIsOn == FALSE )
    {
      PowerMonitor_SetupHardware_HighPower();//硬件使能
    }
    
    me.LastMeasure = ADC_GetValue_Polling(ADC_Mode_PowerMonitor_Pline);//检测电源电压
    
    if(me.LastMeasure <= POWERFAIL_ADCLEVEL_TRIGGER && PWRCallBack == TRUE)
    {
      me.PowerFailCallBack();//欠压处理
    }
    
    me.Pdata.BatMeasureAvr  = (me.LastMeasure + (5*(me.Pdata.BatMeasureAvr))) / 6;//平均值
    LastMeasure =  me.LastMeasure;

    if(PowerMonitorCount < 50)
    {
          PowerMonitorCount++;
        if(me.Pdata.BatMeasureAvr > me.Pdata.MaxBatMeasureAvr)
        {
          me.Pdata.MaxBatMeasureAvr = me.Pdata.BatMeasureAvr;  //改变最大值
        }
        
        if(me.Pdata.BatMeasureAvr < me.Pdata.MinBatMeasureAvr)
        {
          me.Pdata.MinBatMeasureAvr = me.Pdata.BatMeasureAvr; //改变最小值
        }
    }else
    { 
         PowerMonitorCount=0;
         if(me.Pdata.MinBatMeasureAvr >= FULL_VOLTAGE_FOR_MOVE)
        {
          User_LedMode=PowerLevelHigh;   //电源满>90%   voltage >= 12.3V
        }else if((me.Pdata.MaxBatMeasureAvr >= MIDDLE_VOLTAGE_FOR_MOVE) && (me.Pdata.MinBatMeasureAvr < FULL_VOLTAGE_FOR_MOVE))
        {
          User_LedMode=PowerLevelMiddle; //电源中等     12.3V > voltage >= 10.3V
        }else if (me.Pdata.MaxBatMeasureAvr <  MIDDLE_VOLTAGE_FOR_MOVE)
        {
          User_LedMode=PowerLevelLow;    //电源低       voltage <10.3V
        }
        me.Pdata.MaxBatMeasureAvr = me.Pdata.BatMeasureAvr;  //两值归一
        me.Pdata.MinBatMeasureAvr = me.Pdata.BatMeasureAvr;  //
    }
  }
  return LastMeasure;
}


//电压检测硬件使能
void PowerMonitor_SetupHardware_HighPower()
{
  GPIO_Init(BATMeasure_ChipSelect_PORT,BATMeasure_ChipSelect_PIN,BATMeasure_ChipSelect_MODE);
  GPIO_Init(BATMeasure_PORT,BATMeasure_PIN,BATMeasure_MODE);
  GPIO_Init(POWERCH_PORT,POWERCH_PIN,POWERCH_MODE);
  GPIO_WriteHigh(BATMeasure_ChipSelect_PORT,BATMeasure_ChipSelect_PIN);
  me.BatMeasureHardwareIsOn = TRUE;
}

//电压检测硬件进入低功耗
void PowerMonitor_SetupHardware_LowPower()
{
  GPIO_Init(BATMeasure_ChipSelect_PORT,BATMeasure_ChipSelect_PIN,BATMeasure_ChipSelect_MODE);
  GPIO_WriteLow(BATMeasure_ChipSelect_PORT,BATMeasure_ChipSelect_PIN);  
  GPIO_Init(BATMeasure_PORT,BATMeasure_PIN,GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_WriteLow(BATMeasure_PORT,BATMeasure_PIN); 
  me.BatMeasureHardwareIsOn = FALSE;
}


//电池电压检测 判断是否允许电机运行
TBool PowerMonitor_BatteryLevelAllowMove()
{
  for(TU8 i = 0u; i< 10u; ++i)
  {
    PowerMonitor_MeasureVbat(TRUE);
  }//电源检测 10次
  return (me.Pdata.BatMeasureAvr > MINIMUM_VOLTAGE_FOR_MOVE) ? (TRUE) : (FALSE);
}

//获取电压平均值
TU16 PowerMonitor_GetVBatAvr()
{
  return me.Pdata.BatMeasureAvr;
}

//获取电压最大值
TU16 PowerMonitor_GetMaxVBatAvr()
{
  return me.Pdata.MaxBatMeasureAvr;
}

//获取电压最小值
TU16 PowerMonitor_GetMinVBatAvr()
{
  return me.Pdata.MinBatMeasureAvr;
}



