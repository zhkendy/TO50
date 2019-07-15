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
  me.Pdata.BatMeasureAvr    = 1500;//��Ҫ�������� sage
}

void PowerMonitor_Init()
{
  me.MonitorVBat = FALSE;
  me.BatMeasureHardwareIsOn = FALSE;
  me.PowerFailCallBack = VoidFPrtTrap; //�ص�������
  
  Archiver_RegisterObject(&me.Pdata,
                          PowerMonitorSetToFactory,
                          sizeof(me.Pdata),
                          FALSE);
     
     //SQC 
   SQC_DataProviderNameInterface_s intf;
   intf.Fptr_Get = Get_SQC_data;
   SQC_DataProvider_RegisterDataSource(SQC_DataProvider_PowerMonitor ,&intf);//��Դ�������
}


static TU8* Get_SQC_data(TU8* DataSize)  //��ȡ����
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
    PowerMonitor_SetupHardware_HighPower();//��ѹ���Ӳ��ʹ��
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
      PowerMonitor_SetupHardware_HighPower();//Ӳ��ʹ��
    }
    
    me.LastMeasure = ADC_GetValue_Polling(ADC_Mode_PowerMonitor_Pline);//����Դ��ѹ
    
    if(me.LastMeasure <= POWERFAIL_ADCLEVEL_TRIGGER && PWRCallBack == TRUE)
    {
      me.PowerFailCallBack();//Ƿѹ����
    }
    
    me.Pdata.BatMeasureAvr  = (me.LastMeasure + (5*(me.Pdata.BatMeasureAvr))) / 6;//ƽ��ֵ
    LastMeasure =  me.LastMeasure;

    if(PowerMonitorCount < 50)
    {
          PowerMonitorCount++;
        if(me.Pdata.BatMeasureAvr > me.Pdata.MaxBatMeasureAvr)
        {
          me.Pdata.MaxBatMeasureAvr = me.Pdata.BatMeasureAvr;  //�ı����ֵ
        }
        
        if(me.Pdata.BatMeasureAvr < me.Pdata.MinBatMeasureAvr)
        {
          me.Pdata.MinBatMeasureAvr = me.Pdata.BatMeasureAvr; //�ı���Сֵ
        }
    }else
    { 
         PowerMonitorCount=0;
         if(me.Pdata.MinBatMeasureAvr >= FULL_VOLTAGE_FOR_MOVE)
        {
          User_LedMode=PowerLevelHigh;   //��Դ��>90%   voltage >= 12.3V
        }else if((me.Pdata.MaxBatMeasureAvr >= MIDDLE_VOLTAGE_FOR_MOVE) && (me.Pdata.MinBatMeasureAvr < FULL_VOLTAGE_FOR_MOVE))
        {
          User_LedMode=PowerLevelMiddle; //��Դ�е�     12.3V > voltage >= 10.3V
        }else if (me.Pdata.MaxBatMeasureAvr <  MIDDLE_VOLTAGE_FOR_MOVE)
        {
          User_LedMode=PowerLevelLow;    //��Դ��       voltage <10.3V
        }
        me.Pdata.MaxBatMeasureAvr = me.Pdata.BatMeasureAvr;  //��ֵ��һ
        me.Pdata.MinBatMeasureAvr = me.Pdata.BatMeasureAvr;  //
    }
  }
  return LastMeasure;
}


//��ѹ���Ӳ��ʹ��
void PowerMonitor_SetupHardware_HighPower()
{
  GPIO_Init(BATMeasure_ChipSelect_PORT,BATMeasure_ChipSelect_PIN,BATMeasure_ChipSelect_MODE);
  GPIO_Init(BATMeasure_PORT,BATMeasure_PIN,BATMeasure_MODE);
  GPIO_Init(POWERCH_PORT,POWERCH_PIN,POWERCH_MODE);
  GPIO_WriteHigh(BATMeasure_ChipSelect_PORT,BATMeasure_ChipSelect_PIN);
  me.BatMeasureHardwareIsOn = TRUE;
}

//��ѹ���Ӳ������͹���
void PowerMonitor_SetupHardware_LowPower()
{
  GPIO_Init(BATMeasure_ChipSelect_PORT,BATMeasure_ChipSelect_PIN,BATMeasure_ChipSelect_MODE);
  GPIO_WriteLow(BATMeasure_ChipSelect_PORT,BATMeasure_ChipSelect_PIN);  
  GPIO_Init(BATMeasure_PORT,BATMeasure_PIN,GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_WriteLow(BATMeasure_PORT,BATMeasure_PIN); 
  me.BatMeasureHardwareIsOn = FALSE;
}


//��ص�ѹ��� �ж��Ƿ�����������
TBool PowerMonitor_BatteryLevelAllowMove()
{
  for(TU8 i = 0u; i< 10u; ++i)
  {
    PowerMonitor_MeasureVbat(TRUE);
  }//��Դ��� 10��
  return (me.Pdata.BatMeasureAvr > MINIMUM_VOLTAGE_FOR_MOVE) ? (TRUE) : (FALSE);
}

//��ȡ��ѹƽ��ֵ
TU16 PowerMonitor_GetVBatAvr()
{
  return me.Pdata.BatMeasureAvr;
}

//��ȡ��ѹ���ֵ
TU16 PowerMonitor_GetMaxVBatAvr()
{
  return me.Pdata.MaxBatMeasureAvr;
}

//��ȡ��ѹ��Сֵ
TU16 PowerMonitor_GetMinVBatAvr()
{
  return me.Pdata.MinBatMeasureAvr;
}



