// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#include "Config.h"
#include "UARTComDriver.h"
#include "SQC_DataProvider.h"
#include "STM8_Archiver.h"
#include "SQC.h"
#include "RTS_Ergonomic_Manager.h"
#include "stm8s_itc.h"
#include "stm8s_uart3.h"


__no_init static SQC_Event_e EventArray[SQC_EVENT_LOG_DEPTH];//��Ҫ�޸�sage 

extern TU8   LedMode;
extern TU16  Led_time_last;
extern TU8   Led_repet_time;
extern TU8   NextLedMode;
extern TU16  Ledtime_outsecond;
extern TU8   LowpowerLedMode;



#define END_STRING                      "\0"
#define NEWLINE				"\n"
#define UNDERSCORE			"_"
#define SPACE			        " "
#define SQC_START 			"<"
#define SQC_END				">"

#define SQC_HEADER      (NEWLINE VALUE(REF_5M) SOFTWARE_VERSION SPACE PRODUCT_TYPE NEWLINE SQC_START END_STRING)
#define VERSION         (NEWLINE VALUE(REF_5M) SOFTWARE_VERSION SPACE PRODUCT_TYPE NEWLINE END_STRING)
#define SQC_SOFTVERSION ("Ref:" VALUE(REF_5M) " SOFT version " SOFTWARE_VERSION END_STRING)
static char SoftVersionString[] = SQC_SOFTVERSION;
static  SQC_DataProviderNameInterface_s DataProviderArray[SQC_DataProviderName_LastEnum];
static void SQC_SetToFactory();
static  TU8* Get_SQC_data(TU8* DataSize);


void SQC_Init()
{
  Archiver_RegisterObject(EventArray,
                          SQC_SetToFactory,
                          sizeof(EventArray),
                          FALSE);  
    //SQC 
  SQC_DataProviderNameInterface_s intf;
  intf.Fptr_Get = Get_SQC_data;
  SQC_DataProvider_RegisterDataSource(SQC_DataProvider_Event ,&intf); 
}



void SQC_Create(TBool HotReset)
{
  if(HotReset == FALSE)
  {
    Archiver_LoadMe(EventArray);
  }
}



static TU8* Get_SQC_data(TU8* p_DataSize)
{
  *p_DataSize = sizeof(EventArray);
  return (TU8*) EventArray;
}

void SQC_DataProvider_RegisterDataSource(SQC_DataProviderName_e DataProvider,SQC_DataProviderNameInterface_s *Interface)
{
  DataProviderArray[DataProvider] = *Interface;
}


void Version_Print(void)
{
  UART3ComDriver_PoollingPrint(VERSION);   
}


void SQCmesage_Print(void)
{
  SQC_LogEvent(SQC_Event_SendSQC);
  UART1ComDriver_PoollingPrint(SQC_HEADER);
  for(TU8 i = 0U; i< SQC_DataProviderName_LastEnum; ++i)
  {
    TU8 DataSize = 0U;
    TU8* Data_ptr = DataProviderArray[i].Fptr_Get(&DataSize);
    UART1ComDriver_BufferedPrintByteInASCII(Data_ptr,DataSize,TRUE);
    UART1ComDriver_WaitBufferedWriteFinish();
  }
  UART1ComDriver_PoollingPrint(SQC_END);
}


void SQC_Count(void *ptr_Ctr,TU8 sizeofv)
{
  TU8*  ptr_Ctr8  = (TU8*)  ptr_Ctr;
  TU16* ptr_Ctr16 = (TU16*) ptr_Ctr;
  TU32* ptr_Ctr32 = (TU32*) ptr_Ctr;

  switch(sizeofv)
  {
  case 1u:
    *ptr_Ctr8 = (*ptr_Ctr8 < 250u) ? (*ptr_Ctr8 + 1u) : (250u); 
    break;
  case 2u:
    *ptr_Ctr16 = (*ptr_Ctr16  < 65500u) ? (*ptr_Ctr16 + 1u) : (65500u); 
    break;
  case 4u:
    *ptr_Ctr32 = (*ptr_Ctr32  < 0xFFFFFFF0) ? (*ptr_Ctr32 + 1u) : (0xFFFFFFF0); 
    break;
  default:
    break;
  }
}


void SQC_LogEvent(SQC_Event_e Evt)
{
  for( TU8 i = SQC_EVENT_LOG_DEPTH - 1u; i >= 1u; i--)
  {
    EventArray[i] = EventArray[i-1];
  }
  EventArray[0] = Evt;
}


char* SQC_Get_SoftVersionString()
{
  return SoftVersionString;
}


static void SQC_SetToFactory()
{
   for( TU8 i = 0U; i < SQC_EVENT_LOG_DEPTH; i++)
  {
    EventArray[i] = SQC_Event_None;
  }
}


void LED_LogEvent(LED_Event_e Evt)
{  
        if(Evt==LED_Event_ThermalProtect)
        {
            if(LedMode!=RedSlowBlink)
            {
                LowpowerLedMode=RedSlowBlink;
                LedMode=RedSlowBlink;
                NextLedMode=AllClose; 
                
                Led_time_last =0;
                Led_repet_time=0;
                Ledtime_outsecond=10u;
            }
        }else if(Evt==LED_Event_SpeedINC || Evt==LED_Event_SpeedDEC )      //SpeedAdjustmentMode  
        {
            LedMode=GreenFastBlink;
            NextLedMode=YellowSlowBlink; 
            
            Led_time_last=0; 
            Led_repet_time=5;
            //Ledtime_outsecond=5u;
        }else if(Evt==LED_Event_ResetMY || Evt==LED_Event_TiltSpeedINC || Evt==LED_Event_TiltSpeedDEC)//user mode
        {
            LedMode=GreenFastBlink;
            NextLedMode=AllClose;
            
            Led_time_last =0;
            Led_repet_time=5;
            Ledtime_outsecond=5;  
        }else if( Evt==LED_Event_SetImpossibleUserMode)//���ô��� �޷���� ���3��˸ �ظ�3��
        {
            LedMode=Red_3_Flash;
            NextLedMode=AllClose;

            Led_time_last=0;
            Led_repet_time=3;  
            Ledtime_outsecond=9;
        }else if( Evt==LED_Event_SetImpossibleSetMode)//���ô��� �޷���� ���3��˸ �ظ�3��
        {
            LedMode=Red_3_Flash;
            NextLedMode=YellowSlowBlink;
            
            Led_time_last=0;
            Led_repet_time=3;      
        }else if(Evt==LED_Event_CantAddOrDeletTeleco)     //configure mode �޷�ɾ�����ң�������˻ص�user mode  ���3��˸ �ظ�3��
        {
            LedMode=Red_3_Flash;
            NextLedMode=AllClose; 
            
            Led_time_last=0;
            Led_repet_time=3;
            Ledtime_outsecond=9;
        }else if(Evt==LED_Event_MotorBlocked)
        {
            LowpowerLedMode=RedLight;
            LedMode=RedLight;
            NextLedMode=AllClose; 
            
            Led_repet_time=0;
            Led_time_last=0;
            Ledtime_outsecond=LED_15MIN;
        }else if(Evt==LED_Event_ChangeToUserModeA)//�л����û�ģʽA  �̵�������
        { 
            LedMode=Green_3_Flash;
            NextLedMode=AllClose;
            
            Led_time_last=0;
            Led_repet_time=0;
            Ledtime_outsecond=2;
        }else if(Evt==LED_Event_ChangeToUserModeB)//�л����û�ģʽB  �̵�������
        { 
            LedMode=Green_2_Flash;
            NextLedMode=AllClose;
            
            Led_time_last=0;
            Led_repet_time=0;
            Ledtime_outsecond=2;   
        }else if(Evt==LED_Event_StopAboveLimits||
                 Evt==LED_Event_StopBelowLimits)//������λ ���������
        {  

            LedMode=Red_2_Flash;
            NextLedMode=AllClose;

            Led_repet_time=0;  
            Led_time_last=0;
            Ledtime_outsecond=10;
        }else if(Evt==LED_Event_RadioOff) //�ر�����         ����5 ��ر�
         {
              LowpowerLedMode=AllClose; 
              LedMode=GreenFastBlink;
              NextLedMode=AllClose;
              
              Led_time_last=0;
              Led_repet_time=5;
              Ledtime_outsecond=5;
         }else if(Evt==LED_Event_ToUserMode)//�����û�ģʽ   ����5 ��ر�
         {
            LowpowerLedMode=AllClose;
            LedMode=GreenFastBlink;
            NextLedMode=AllClose; 
            
            Led_time_last=0;
            Led_repet_time=5;
            Ledtime_outsecond=5;
         }else if(Evt==LED_Event_NoBlinkUserMode)//�����û�ģʽ  û�еƵ���˸
         {
            LowpowerLedMode=AllClose;
            LedMode=AllClose;
            NextLedMode=AllClose; 
            
            Led_time_last=0;
            Led_repet_time=0;
            Ledtime_outsecond=0;
         }
         else if(Evt==LED_Event_Ergo_FactoryMode)    //����ģʽ     �Ƴ��� 15min  ok                                                      
         {
            if(LedMode==GreenFastBlink)
            {
              NextLedMode=YellowLight;
            }else
            {
              LedMode=YellowLight;
              NextLedMode=AllClose;
              Led_time_last=0;
            }
              Ledtime_outsecond=LED_15MIN;
         }else if(Evt==LED_Event_Ergo_IndustrialMode) //��������ģʽ �ƿ��� 15min  ok                                                    
         {
              LedMode=YellowFastBlink;
              NextLedMode=AllClose;
              
              Led_time_last=0;
              Led_repet_time=0;
              Ledtime_outsecond=LED_15MIN;
         }
         else if(Evt==LED_Event_RadioOn_FactMode)    //���ع���ģʽ  ��5 ����                                                          
         {
              LedMode=GreenFastBlink;
              NextLedMode=YellowLight;
              
              Led_time_last=0;
              Led_repet_time=5;
              Ledtime_outsecond=LED_15MIN;
         }else if( Evt==LED_Event_Ergo_MaintenanceResetingMode ||
                    Evt==LED_Event_ErgoEnterInLimitResetting   ||
                    Evt==LED_Event_Ergo_ConfigMode             ||
                    Evt==LED_Event_Ergo_SpeedAdjustmentMode) //����ģʽ�Ƶ�����2min
         {
                LedMode=YellowSlowBlink;
                NextLedMode=AllClose;
                
                Led_time_last=0;
                Led_repet_time=0;
                Ledtime_outsecond=LED_2MIN;
         }else if(Evt==LED_Event_Ergo_FirstPairingMode)     //�Ƶ�����15min
         {
                LedMode=YellowSlowBlink;
                NextLedMode=AllClose;
                
                Led_time_last=0;
                Led_repet_time=0;
                Ledtime_outsecond=LED_15MIN;
         }else if( Evt==LED_Event_ResetDEL ||
                   Evt==LED_Event_ResetUEL ||
                   Evt==LED_Event_ResetRotationDir)     //2������ģʽ�� ����5�κ�Ƶ���˸
         {
                LedMode=GreenFastBlink;
                NextLedMode=YellowSlowBlink;
                
                Led_time_last=0;
                Led_repet_time=5;
         }else if(Evt==LED_Event_PowerOn_SetAndNotPaired)//�̵�����֮�� �Ƶ�flash���� 15min  ���ϵ�֮����롱//kendy??
          {   
            if(NextLedMode!=Yellow_2_Economy)
              {
                    LedMode=Yellow_2_Economy;
                    NextLedMode=AllClose;
                    
                    Led_time_last=0;
                    Led_repet_time=0;
                    Ledtime_outsecond=LED_15MIN; 
              }
          }else if(Evt==LED_Event_Set_ToSetNotTemMode||
                   Evt==LED_Event_RadioOn_SetNotMode)//�л�����ģʽ �̵���5�Σ��Ƶ�flash���� 2min
          {
                LedMode=GreenFastBlink;
                NextLedMode=Yellow_2_Economy;
                
                Led_time_last=0;
                Led_repet_time=5;
                Ledtime_outsecond=LED_15MIN; 
          }else if(Evt==LED_Event_Ergo_ToSleepMode) //˯�� �̵���5�Σ��Ƶ�flashһ�� 15min
          {
                LowpowerLedMode=Yellow_1_Flash;
                LedMode=GreenFastBlink;
                NextLedMode=Yellow_1_Flash;
                
                Led_time_last=0;
                Led_repet_time=5;
                Ledtime_outsecond=LED_15MIN;     
          }else if(Evt==LED_Event_BackUserMode)    //�����û�ģʽ�رյ�  ��ʱ�����ϵ����
          {
                LowpowerLedMode=AllClose; 
                LedMode=AllClose; 
                NextLedMode=AllClose; 
                
                Led_repet_time=0;
                Led_time_last=0;
                Ledtime_outsecond=0;
          }else if(Evt==LED_Event_TempPaired)     //��������ģʽ ����5�κ�Ƶ����� 15min
          {
                LedMode=GreenFastBlink;
                NextLedMode=YellowSlowBlink;
                
                Led_time_last=0;
                Led_repet_time=5;
                Ledtime_outsecond=LED_15MIN;
          }else if(Evt==LED_Event_UserLowPower)     //�͵�ѹģʽ ��flash1  5�� 
          {
               if(LedMode!=Red_1_Flash   &&
                  LedMode!=Green_2_Flash &&
                  LedMode!=RedSlowBlink   &&
                  LedMode!=Green_3_Flash)
               {
                  LedMode=Red_1_Flash;
                  NextLedMode=AllClose; 
                  Led_repet_time=5;
                  Led_time_last=0;
                  Ledtime_outsecond=10;
               }
          }else if(Evt==LED_Event_UserRemotecontral)     //������ѹģʽ ��flash1  1�� 
          {     
                if(LedMode!=Green_1_Flash &&
                   LedMode!=Green_2_Flash &&
                   LedMode!=RedSlowBlink   &&
                   LedMode!=Green_3_Flash)
                {
                    LedMode=Green_1_Flash;
                    NextLedMode=AllClose; 
                    
                    Led_repet_time=1;
                    Led_time_last=0;
                    Ledtime_outsecond=1;
                }
          }else if(Evt==LED_Event_UserChargerFull)      //����ѹģʽ ��flash1  1�� 
          {        
              if(LedMode!=GreenLight     && 
                 LedMode!=Green_1_Flash  && 
                 LedMode!=GreenFastBlink &&
                 LedMode!=RedSlowBlink   &&
                 LedMode!=Red_3_Flash    &&
                 LedMode!=RedLight)
                {
                  LowpowerLedMode=GreenLight;
                  LedMode=GreenLight;
                  NextLedMode=AllClose; 
                  
                  Led_repet_time=0;
                  Led_time_last=0;
                  Ledtime_outsecond=30;
                }
          }else if(Evt==LED_Event_UserChargerGoing)     //�͵�ѹģʽ ��flash1  1�� 
          {
                if(LedMode!=Green_2_Economy && 
                   LedMode!=Green_1_Flash   && 
                   LedMode!=GreenFastBlink  &&
                   LedMode!=RedSlowBlink    &&
                   LedMode!=Red_3_Flash     &&
                   LedMode!=RedLight)
                {
                  LowpowerLedMode=Green_2_Economy;
                  LedMode=Green_2_Economy;
                  NextLedMode=AllClose; 
                  
                  Led_repet_time=0;
                  Led_time_last=0;
                  Ledtime_outsecond=30;
                }
          }
       
}


