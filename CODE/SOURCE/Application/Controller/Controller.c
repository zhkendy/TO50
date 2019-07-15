// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#include "Config.h"
#include "SFY_OS.h"
#include "STM8_Archiver.h"
#include "RTS_Ergonomic_ManagerObserver.h"
#include "UARTComDriver.h"
#include "Controller.h"
#include "IndustrialTest.h"
#include "stm8s_gpio.h"
#include "Motor_Driver.h"
#include "Pcb.h"
#include "Stm8_Adc.h"
#include "PowerMonitor.h"
#include "LongTimer.h"
#include "IndustrialTest.h"
#include "STM8_Archiver.h"
#include "RTS_Ergonomic_Manager.h"
#include "MovementManager.h"
#include "MovementRequest_Interface.h"
#include "stm8s_iwdg.h"
#include "stm8s_wwdg.h"
#include "stm8s_awu.h"
#include "Platform_Config.h"
#include "SQC.h"
#include "stm8s_rst.h"
#include "Chrono.h"
#include "ApplicationMain.h"
#include "RtsLowPowerManager.h"
#include "RTSDataSupply.h"
#include "SQC_DataProvider.h"
#include "Banker.h"
#include "RTS_Ergonomic_ManagerObservable.h"


bool RADIO_OUTLINE  = false;
bool Motor_Reset    = false;
bool WakeUP_FeedBack= false;
bool Motor_HotProtect    = false;
bool Motor_KeymovCommand = true;
bool Switch_Button_Enable= false;

TU8  Idle_changer_Count = 0u;
TU8  TestFinish = 0u;
TU8  FeedBackMode = FatoryColdPower;
extern TU8   Charger_LED;
extern TU8   User_LedMode;
extern TU8   Butt_Press;
extern TU16  Butt_Press_last;
extern TU8   Charger_mode;
extern TU8   Prior_charger;
extern TU8   Current_charger;
extern TU8   Charger_mode;
extern TU16  Led_time_last;
extern TU8   Ledreload;
extern bool   Speed_MoveCycleRelod;

extern TBool SleepWakeEnable;
extern TBool SleepModeChange;
extern TU32  Motor_Protect_Count;
extern TU8   User_LedMode;
extern bool  SpeedChange;
extern bool  Speed_MoveCycle;

extern TU16  SpeedADJCycle_timems;
extern TU16  SpeedCycle_last;
extern TU16  Bool_tim_last;
extern TBool bStayInBootProgram;
extern TU8   UartEvent_Command;
extern TU8   Pre_UartEvent_Command;
extern bool  LowPowermodeTest;
extern TBool Led_flash_once;
extern bool  MotorBlocked;

extern void  MotorControlIsLockBySecuManager(void);
extern void  MotorControlIsUnLock(void);
extern void  Archiver_ResetCreate(void);



typedef struct Controller_Pdata_SQC_s
{
  TU32 StopOnUL;
  TU32 StopOnDL;
  TU32 StopOnMy;
  TU32 StopBetweenLimits;
  TU32 StopAboveLimits;
  TU32 StopBelowLimits;
  TU16 PwrCut;
  TU16 PwrCutWhileMoving; 
  TU8  IWatchdogReset;
  TU8  WWatchdogReset;
  TU8  ILLOPF_Reset;
  TU8  EMCF_Reset;
  TU8  SWIMF_Reset;
  TU16 AllReset;
  TU8  FreeSpaceInStack;  
  TU8  FreeSpaceInHeap;
  TU8  Curr_SpeedMod;     
  TU8  User_Mod;          
}Controller_Pdata_SQC_s;



#ifdef CONTROLLER_PRINT_DEBUG	
#include "Controller_DebugTXT.h"
#include "DebugPrint.h"
#endif

#ifdef CONTROLLER_PRINT_INFO
#include "Controller_DebugTXT.h"
#include "DebugPrint_info.h"
#endif

#ifdef CONTROLLER_PRINT_LOG	
#include "Controller_DebugTXT.h"
#include "Movement_DebugTXT.h"
#include "DebugPrint_Log.h"
#endif

#include "Controller.h"

extern TU8    LedMode;


typedef struct
{
  TU8   InitialSetFinish;
  TU8   IndustrialTestFinish;
  TU8   AllReset;
}Controller_Pdata_s;

TU8 InitialSetDone = Initial_Set_Null;

typedef struct
{
  Controller_Pdata_SQC_s  SQC;
  ActuatorState_e         State;
  Controller_Pdata_s      Pdata;
  TBool AcceptMoveRequest;
  TBool NoRTS;
  TBool PendingEepromWrite;
  AWU_Timebase_TypeDef EMS_LP_Test_AWU;
}Controller_t;


#define me Controller
Controller_t Controller;


static void  RTSErgoChangeModeHandler(ErgoMode_e NewMode);
static void  RTSErgoSpecialRequestHandler(RTSErgoSpecialRequest_e Sreq);
static void  Controller_PowerFailHandler();
static void  ControllerEventHandler_SaveAllToEeprom();
static void  ControllerEventHandler_InvertRotation();
static void  TryToLowpower();
static TBool AllwoMoveRequestOnLowBattery(const MoveRequest_s *Mvrq);
static TU8*  Get_SQC_data(TU8* DataSize);
static void  SetSQCToFactoryValue();

static FlagStatus G_RST_FLAG_EMCF   = RESET;   
static FlagStatus G_RST_FLAG_SWIMF  = RESET;  
static FlagStatus G_RST_FLAG_ILLOPF = RESET;  
static FlagStatus G_RST_FLAG_IWDGF  = RESET;   
static FlagStatus G_RST_FLAG_WWDGF  = RESET; 



void Controller_Init(void)
{
  me.PendingEepromWrite = FALSE;                       
  G_RST_FLAG_EMCF   = RST_GetFlagStatus(RST_FLAG_EMCF);
  G_RST_FLAG_SWIMF  = RST_GetFlagStatus(RST_FLAG_SWIMF);
  G_RST_FLAG_ILLOPF = RST_GetFlagStatus(RST_FLAG_ILLOPF);
  G_RST_FLAG_IWDGF  = RST_GetFlagStatus(RST_FLAG_IWDGF);
  G_RST_FLAG_WWDGF  = RST_GetFlagStatus(RST_FLAG_WWDGF);
  
  RST_ClearFlag(RST_FLAG_EMCF);                       
  RST_ClearFlag(RST_FLAG_SWIMF);
  RST_ClearFlag(RST_FLAG_ILLOPF);
  RST_ClearFlag(RST_FLAG_IWDGF);
  RST_ClearFlag(RST_FLAG_WWDGF);
  
  Archiver_Init();            
  ADC_Init();              
  LongTimer_Init();         
  Chrono_Init();            
  RtsLowPowerManager_Init();
  UARTComDriver_Init();    
  SQC_Init();              
  PowerMonitor_Init();      
  MovementManager_Init();   
  ErgonomicManager_Init();  
  
  me.NoRTS = FALSE;       
  Controller_SetState(ActuatorState_Boot);              
  Archiver_RegisterObject( &me.Pdata,                   
                          Controller_SetPdataToFactory, 
                          sizeof(me.Pdata),
                          TRUE);            
  
  Archiver_RegisterObject(&me.SQC,                         
                          SetSQCToFactoryValue,         
                          sizeof(me.SQC),
                          FALSE);           
  
  if(SFY_OS_TimerCreate(CONTROLLER_EMS_START_FRAME_TIMER_ID,OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Controller,TASK_Controller_EMS_TIMEOUT_EVENT) != OS_SUCCESS)
  {
    trap();
  }
  
  if(SFY_OS_TimerCreate(CONTROLLER_LOWPOWER_TIMER_ID,OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Controller,TASK_Controller_LOWPOWER_TIMEOUT_EVENT) != OS_SUCCESS)
  {
    trap();
  }  
  //Register to RTS Observer
  RTSErgonomicObserver_s ErgoObserver;
  ErgoObserver.Fptr_ErgoChageMode         = RTSErgoChangeModeHandler;    
  ErgoObserver.Fptr_RTSErgoSpecialRequest = RTSErgoSpecialRequestHandler;
  RTSErgonomicObserver_RegisterObserver(RTSErgonomicObserver_Controller,ErgoObserver);
  
  //SQC 
  SQC_DataProviderNameInterface_s intf;
  intf.Fptr_Get = Get_SQC_data;
  SQC_DataProvider_RegisterDataSource(SQC_DataProvider_Controller ,&intf);
     
  LongTimer_Setup(LongTimer_AutoSave,AUTOSAVE_PERIOD_MS,FALSE,TRUE,ControllerEventHandler_SaveAllToEeprom);
  LongTimer_Setup(LongTimer_30MinCharger,CHARGER_PERIOD_MS,TRUE,TRUE,Controller_Idle);

  DEBUG_PRINT("[DBG] Controller_Init\n");
}


static TU8* Get_SQC_data(TU8* DataSize)
{
  *DataSize = sizeof(me.SQC);
  
  return (TU8*) &me.SQC;
}


void Controller_Create(void)
{
  
  TBool HotReset = (G_RST_FLAG_ILLOPF == SET || G_RST_FLAG_IWDGF == SET || G_RST_FLAG_WWDGF == SET || G_RST_FLAG_EMCF == SET) ? (TRUE) : (FALSE);
  IWDG_ReloadCounter();       
  Archiver_Create();           
  Archiver_LoadMe(&me.Pdata); 
  Archiver_LoadMe(&me.SQC);    

  
  if(me.Pdata.AllReset==FatoryHotPower)
  { 
      FeedBackMode = FatoryHotPower;
      if(me.Pdata.IndustrialTestFinish != Initial_Set_Null ) 
        TestFinish  =Initial_Set_Flage;
      else
        TestFinish  =Initial_Set_Null;
      
      Archiver_ResetCreate();
      Archiver_LoadMe(&me.Pdata);  
      Archiver_LoadMe(&me.SQC);   
      me.Pdata.IndustrialTestFinish  = TestFinish; 
  }else if(me.Pdata.AllReset==FatoryBeReset)
  { 
       FeedBackMode = FatoryBeReset;
      if(me.Pdata.IndustrialTestFinish != Initial_Set_Null )
        TestFinish  =Initial_Set_Flage;
      else
        TestFinish  =Initial_Set_Null;
      
      Archiver_ResetCreate();
      Archiver_LoadMe(&me.Pdata);  
      Archiver_LoadMe(&me.SQC);   
      me.Pdata.IndustrialTestFinish  = TestFinish; 
  } 
  else
  { 
      FeedBackMode = FatoryColdPower;
  }

  SQC_Count(&me.SQC.AllReset,sizeof(me.SQC.AllReset));   
  
  if(G_RST_FLAG_IWDGF == SET)
  {
    SQC_Count(&me.SQC.IWatchdogReset,sizeof(me.SQC.IWatchdogReset));
  } 
  if(G_RST_FLAG_WWDGF == SET)
  {
    SQC_Count(&me.SQC.WWatchdogReset,sizeof(me.SQC.WWatchdogReset));
  } 
  if(G_RST_FLAG_ILLOPF == SET)
  {
    SQC_Count(&me.SQC.ILLOPF_Reset,sizeof(me.SQC.ILLOPF_Reset));
  }  
  if(G_RST_FLAG_EMCF == SET)
  {
    SQC_Count(&me.SQC.EMCF_Reset,sizeof(me.SQC.EMCF_Reset));
  } 
  if(G_RST_FLAG_SWIMF == SET)
  { 
    SQC_Count(&me.SQC.SWIMF_Reset,sizeof(me.SQC.SWIMF_Reset));
  }
  InitialSetDone = me.Pdata.InitialSetFinish;
  
  Chrono_Create();                
  SQC_Create(HotReset);            
  SQC_LogEvent(SQC_Event_Reset);
  Chrono_Start(Chrono_TotalTime);  
  
  RtsLowPowerManager_Create();     
  PowerMonitor_Create(Controller_PowerFailHandler);
  MovementManager_Create(HotReset);
  ErgonomicManager_Create();       
  LongTimer_StartTick();          
  
  UARTComDriver_Cmd(UARTComDriver_Enable_RxTxToDebug,UART_BAUD_RATE_DEBUG);
  DEBUG_PRINT("[DBG] Controller_Create\n");
  DEBUG_PRINT1("[DBG]; HotReset  (%d)\n",HotReset);
  Controller_SetState(ActuatorState_Autotest); 

  ControllerEventHandler_SaveAllToEeprom();   
}

//me Controller
static void SetSQCToFactoryValue()
{
  me.SQC.StopOnUL= 0U;         
  me.SQC.StopOnDL= 0U;         
  me.SQC.StopOnMy= 0U;        
  me.SQC.StopBetweenLimits= 0U;
  me.SQC.StopAboveLimits= 0U; 
  me.SQC.StopBelowLimits= 0U;  
  me.SQC.PwrCut= 0U;           
  me.SQC.PwrCutWhileMoving= 0U;
  me.SQC.IWatchdogReset= 0U;   
  me.SQC.WWatchdogReset= 0U;   
  me.SQC.ILLOPF_Reset= 0U;
  me.SQC.EMCF_Reset= 0U;       
  me.SQC.SWIMF_Reset= 0U;      
  me.SQC.AllReset= 0U;         
  me.SQC.FreeSpaceInStack= 0U; 
  me.SQC.FreeSpaceInHeap= 0U;  
  me.SQC.Curr_SpeedMod= SPEED_20RPM;//默认转速20 
  me.SQC.User_Mod= User_ModeB_Roller;      
}


// 电机prog按键按下时的电机抖动反馈
void MotorProg_FeedBack(void)
{
    ErgoMode_e ErgoMode = ErgonomicManager_GetCurrentMode();//获取当前模式
    MoveRequest_s Mvrq;  
    
    if(Butt_Press_last == BUTTON_150Ms && (
       ErgoMode == ErgoMode_SetAndTemporaryRemote ||
       ErgoMode == ErgoMode_UserMode              ||
       ErgoMode == ErgoMode_SetAndNotPaired       ||
       ErgoMode == ErgoMode_SpeedAdjustmentMode   ||
       ErgoMode == ErgoMode_Tiltingspeedadjustment   ||  
       ErgoMode == ErgoMode_SettingPart1Mode      ||
       ErgoMode == ErgoMode_SettingPart1Modebis ))
     {
         if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
         {
           if((ErgoMode == ErgoMode_SpeedAdjustmentMode)||(ErgoMode == ErgoMode_Tiltingspeedadjustment))///kendy ???tilting
             Speed_MoveCycle=false;//电机周期运动模式结束

            
            Motor_KeymovCommand = true;//kendy1????
            Mvrq.MoveType = MoveRequestType_Nolimit_Stop; //调速模式 按键按下电机 停止
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
         }else
            Motor_KeymovCommand = false;   
         
     }else if(Butt_Press_last == BUTTON_2SEC    && 
             ErgoMode != ErgoMode_Delivery1Mode &&     
             ErgoMode != ErgoMode_Delivery2Mode && 
             ErgoMode != ErgoMode_Delivery3Mode)
     {
            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;//2s 一次反馈抖动
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
     }else if(Butt_Press_last == BUTTON_7SEC    && 
             ErgoMode != ErgoMode_Delivery1Mode &&    
             ErgoMode != ErgoMode_Delivery2Mode && 
             ErgoMode != ErgoMode_SetNoTemporaryRemote &&  //kendy
             ErgoMode != ErgoMode_Delivery3Mode)
    {
            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;//7s 一次反馈抖动
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
            
            Speed_MoveCycle=false;//电机周期运动模式结束

    }else if(Butt_Press_last == BUTTON_12SEC    &&    
             ErgoMode != ErgoMode_Delivery1Mode &&      
             ErgoMode != ErgoMode_Delivery2Mode && 
               ErgoMode != ErgoMode_SetNoTemporaryRemote &&  //kendy
             ErgoMode != ErgoMode_Delivery3Mode)
    {    
            RADIO_OUTLINE=TRUE;

            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;//12s 一次反馈抖动
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
    }else if(Butt_Press_last >= BUTTON_13SEC    &&    //kendy???????
             ErgoMode != ErgoMode_Delivery1Mode &&      
             ErgoMode != ErgoMode_Delivery2Mode &&
                 ErgoMode != ErgoMode_SetNoTemporaryRemote &&  //kendy
             ErgoMode != ErgoMode_Delivery3Mode)
    {
             me.AcceptMoveRequest = FALSE; 
             Butt_Press_last = BUTTON_13SEC;
    }else if(Butt_Press_last >= BUTTON_2SEC && (   //kendy?????
             ErgoMode == ErgoMode_Delivery1Mode ||    
             ErgoMode == ErgoMode_Delivery2Mode || 
             ErgoMode == ErgoMode_Delivery3Mode))
    {
             Butt_Press_last = 0;
    }
}


void ErgoMode_Switch_Button(void)

{
  MoveRequest_s Mvrq;  
  ErgoMode_e ErgoMode = ErgonomicManager_GetCurrentMode();
  DEBUG_PRINT1("[DBG] Butt_Press_last = %d\n",Butt_Press_last);
  
  if(Butt_Press_last >= BUTTON_150Ms && Butt_Press_last < BUTTON_2SEC && 
    (ErgoMode == ErgoMode_SettingPart1Mode    || 
     ErgoMode == ErgoMode_SettingPart1Modebis || 
     ErgoMode == ErgoMode_Tiltingspeedadjustment || 
     ErgoMode == ErgoMode_SpeedAdjustmentMode)) 
   {
      if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
      {
        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
        Mvrq.MoveRequestParam = NoMoveParam;
        MoveRequest_Post(&Mvrq);
      }

      if(SpeedChange==true)
      {
        Speed_MoveCycle=true;
        SpeedCycle_last=SpeedADJCycle_timems-1000u;///kendy1???
      }
  }else if(Butt_Press_last >= BUTTON_150Ms && Butt_Press_last < BUTTON_2SEC && ErgoMode == ErgoMode_SleepMode)
   {
      RADIO_OUTLINE=FALSE;
      
      Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;      
      Mvrq.MoveRequestParam = NoMoveParam;
      MoveRequest_Post(&Mvrq);
      
      LED_LogEvent(LED_Event_ToUserMode);
      
      ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);  
  }else if(Butt_Press_last >= BUTTON_150Ms && Butt_Press_last < BUTTON_2SEC && (ErgoMode == ErgoMode_Delivery1Mode))
  {
      RADIO_OUTLINE=FALSE;
      
      Mvrq.MoveType = MoveRequestType_Nolimit_ThreeShortFeedBack;  
      Mvrq.MoveRequestParam = NoMoveParam;
      MoveRequest_Post(&Mvrq);
      
      LED_LogEvent(LED_Event_RadioOn_FactMode); 
      
      ErgonomicManager_ChangeErgoMode(ErgoMode_FactoryMode);  
  }else if(Butt_Press_last >= BUTTON_150Ms && Butt_Press_last < BUTTON_2SEC && ErgoMode == ErgoMode_Delivery2Mode)
  {  
      RADIO_OUTLINE=FALSE;
      
      Mvrq.MoveType = MoveRequestType_Nolimit_DoubleShortFeedBack; 
      Mvrq.MoveRequestParam = NoMoveParam;
      MoveRequest_Post(&Mvrq);
      
      LED_LogEvent(LED_Event_RadioOn_SetNotMode); 
      
      ErgonomicManager_ChangeErgoMode(ErgoMode_SetAndNotPaired); 
  }else if(Butt_Press_last >= BUTTON_150Ms && Butt_Press_last < BUTTON_2SEC && ErgoMode == ErgoMode_Delivery3Mode)
   {
      RADIO_OUTLINE=FALSE;
      
      Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;      
      Mvrq.MoveRequestParam = NoMoveParam;
      MoveRequest_Post(&Mvrq);
      
      LED_LogEvent(LED_Event_ToUserMode);
      
      ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);  
  }else if(Butt_Press_last >= BUTTON_150Ms && Butt_Press_last < BUTTON_2SEC && (ErgoMode == ErgoMode_ConfigMode))
  {
      RADIO_OUTLINE =TRUE;
      
      Mvrq.MoveType = MoveRequestType_Nolimit_DoubleShortFeedBack; 
      Mvrq.MoveRequestParam = NoMoveParam;
      MoveRequest_Post(&Mvrq);
      
      SleepWakeEnable = true;
      ErgonomicManager_ChangeErgoMode(ErgoMode_SleepMode);
  }else if(Butt_Press_last >= BUTTON_150Ms && Butt_Press_last < BUTTON_2SEC && (ErgoMode == ErgoMode_IndustrialMode))
  {    
//      RADIO_OUTLINE=TRUE;  
//      
//      Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
//      Mvrq.MoveRequestParam = NoMoveParam;
//      MoveRequest_Post(&Mvrq); 
//         
//      LowPowermodeTest=false;
//      ErgonomicManager_ChangeErgoMode(ErgoMode_Delivery1Mode);
          RADIO_OUTLINE=FALSE;  
      
      Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
      Mvrq.MoveRequestParam = NoMoveParam;
      MoveRequest_Post(&Mvrq); 
        LED_LogEvent(LED_Event_RadioOn_FactMode);  
      LowPowermodeTest=true;
      ErgonomicManager_ChangeErgoMode(ErgoMode_FactoryMode);
  }else if(Butt_Press_last >= BUTTON_150Ms && Butt_Press_last < BUTTON_2SEC && 
  (ErgoMode == ErgoMode_UserMode              || 
   ErgoMode == ErgoMode_SetAndTemporaryRemote ||
   ErgoMode == ErgoMode_SetAndNotPaired))
  {
    if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
    {
        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
        Mvrq.MoveRequestParam = NoMoveParam;
        MoveRequest_Post(&Mvrq);
    }else if(Motor_KeymovCommand == false)//kendy3???????
    {  //需要增加位置判断，超时回到限位的运动需要考虑
        TU32 CurrentPosition = EncoderDriver_GetPosition();  //获取当前位置
        Limits_ComputeLimitsState(&CurrentPosition);         //当前位置对比限位状态
        Limits_State_e LimisState = Limits_GetLimitsState(); //获取限位位置区间状态
        
        //GPIO_WriteHigh(BRAKECONTROL_PORT,BRAKECONTROL_PIN); 
        
        MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
        
          if(LimisState == Limits_State_OnUp)
          {
              Mvrq.MoveType = MoveRequestType_UnRollToPosition;
              Mvrq.MoveRequestParam.Position = PositionName_DEL;
              MoveRequest_Post(&Mvrq);
          }else if(LimisState == Limits_State_OnDown)
          {
              Mvrq.MoveType = MoveRequestType_RollToPosition;
              Mvrq.MoveRequestParam.Position = PositionName_UEL;
              MoveRequest_Post(&Mvrq); 
          }else if(LimisState ==Limits_State_Between)
          {
              if(LastDir == MotorDirection_UnRoll)
              {
                Mvrq.MoveType = MoveRequestType_RollToPosition;
                Mvrq.MoveRequestParam.Position = PositionName_UEL;
                MoveRequest_Post(&Mvrq); 
              }else if(LastDir == MotorDirection_Roll){
                Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                Mvrq.MoveRequestParam.Position = PositionName_DEL;
                MoveRequest_Post(&Mvrq);
              }
         }
    }
  }else if(Butt_Press_last >= BUTTON_2SEC && Butt_Press_last < BUTTON_7SEC)
  {
       switch(ErgoMode)
       { 
           case ErgoMode_FactoryMode :
               ErgonomicManager_ChangeErgoMode(ErgoMode_FirstPairingMode);
            break;  
           case ErgoMode_SetNoTemporaryRemote ://kendy 不支持短按动作
              RADIO_OUTLINE=FALSE;
//                   Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
//      Mvrq.MoveRequestParam = NoMoveParam;
//      MoveRequest_Post(&Mvrq);
//      
      LED_LogEvent(LED_Event_RadioOn_SetNotMode); //kendy??????
               ErgonomicManager_ChangeErgoMode(ErgoMode_SetAndNotPaired);
            break;             
           case ErgoMode_UserMode :
               ErgonomicManager_ChangeErgoMode(ErgoMode_ConfigMode);
            break; 
             
           case ErgoMode_SpeedAdjustmentMode :  //kendy???????
           case ErgoMode_Tiltingspeedadjustment: //kendy1???????
               if(SpeedChange==true)
                {
                  Speed_MoveCycle=true;//kendy1??????
                  SpeedCycle_last=SpeedADJCycle_timems-1000u;
                }
            break; 
           
           default:
            break;   
       }
  }else if(Butt_Press_last >= BUTTON_7SEC && Butt_Press_last < BUTTON_12SEC)
  {    
      if(ErgoMode == ErgoMode_UserMode                ||
         ErgoMode == ErgoMode_MaintenanceResetingMode ||
         ErgoMode == ErgoMode_ConfigMode              ||
         ErgoMode == ErgoMode_SleepMode               ||
         ErgoMode == ErgoMode_ReSettingSDP            ||
         ErgoMode == ErgoMode_Tiltingspeedadjustment  ||
         ErgoMode == ErgoMode_SpeedAdjustmentMode)
      {
          if(ErgoMode == ErgoMode_SleepMode)
             RADIO_OUTLINE=FALSE;
            
          if(SpeedChange==true)
          {
             SpeedChange=false;
             Curr_SpeedMode=Pre_SpeedMode;//kendy2??? 
          }
//                    if(SpeedChange==true)  //kendy????tilting
//          {
//             SpeedChange=false;
//             Curr_SpeedMode=Pre_SpeedMode; 
//          }
          SQC_LogEvent(SQC_Event_RemoveAllTeleco);
          Banker_DeleteOnlyRemotes();       
          
          ErgonomicManager_ChangeErgoMode(ErgoMode_SetAndNotPaired);
      }else if(ErgoMode == ErgoMode_ReSettingDEL  ||
               ErgoMode == ErgoMode_ReSettingUEL)
      {
              Limits_SetMode(LimitMode_Normal);
              TU32 CurrentPosition = EncoderDriver_GetPosition();  //获取当前位置
              Limits_ComputeLimitsState(&CurrentPosition);         //当前位置对比限位状态
              Limits_State_e LimisState = Limits_GetLimitsState(); //获取限位位置区间状态
              if(LimisState == Limits_State_Above ||LimisState == Limits_State_Below)    
              {
                  ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);  
                  LED_LogEvent(LED_Event_StopAboveLimits);
                  
                  IWDG_ReloadCounter();
                  SFY_OS_TaskDelay(OS_TIME_500_MS);
                  IWDG_ReloadCounter();
                  SFY_OS_TaskDelay(OS_TIME_500_MS);
                  IWDG_ReloadCounter();
                  SFY_OS_TaskDelay(OS_TIME_500_MS);
                  
                  Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBackMoveTo;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq);    
      
              }else
              {  
                  ErgonomicManager_ChangeErgoMode(ErgoMode_SetAndNotPaired);
                  Banker_DeleteOnlyRemotes();  
              }
      }else if(ErgoMode == ErgoMode_FactoryMode)
      {
             Bool_tim_last=BUTTON_7SEC;
      }   
  }else if(Butt_Press_last >= BUTTON_12SEC && Butt_Press_last < BUTTON_13SEC && 
           ErgoMode != ErgoMode_Delivery1Mode && 
           ErgoMode != ErgoMode_Delivery2Mode && 
            ErgoMode != ErgoMode_SetNoTemporaryRemote && //kendy 不复位
           ErgoMode != ErgoMode_Delivery3Mode) 
  {    
           Motor_Reset=TRUE;
  }else if(Butt_Press_last >= BUTTON_13SEC && 
          ErgoMode != ErgoMode_Delivery1Mode && 
          ErgoMode != ErgoMode_Delivery2Mode && 
             ErgoMode != ErgoMode_SetNoTemporaryRemote && //kendy 不复位
          ErgoMode != ErgoMode_Delivery3Mode) 
  {  
          Banker_ApplyFactorySettings();
          RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_MotorReset);
  }
     Butt_Press_last = 0;               
}

  
void ErgoMode_Switch_Charger(void)// kendy 没有充电状态
{
//  MoveRequest_s Mvrq; 
//  ErgoMode_e ErgoMode = ErgonomicManager_GetCurrentMode(); 
//  
//  if(Charger_mode == true)
//   {
//     Charger_mode = false;
//     switch(ErgoMode)
//     { 
//        case ErgoMode_FirstPairingMode :                
//        case ErgoMode_SettingPart1Mode :
//        case ErgoMode_SettingPart1Modebis:     
//            Mvrq.MoveType = MoveRequestType_Nolimit_ThreeShortFeedBack; 
//            Mvrq.MoveRequestParam = NoMoveParam;
//            MoveRequest_Post(&Mvrq); 
//            
//            Motor_Reset=TRUE;               
//          break;  
//         
//        case ErgoMode_IndustrialMode:  //new xiugai
//        case ErgoMode_Delivery1Mode:
//            RADIO_OUTLINE=FALSE;
//            ErgonomicManager_ChangeErgoMode(ErgoMode_FactoryMode);  
//            
//            Mvrq.MoveType = MoveRequestType_Nolimit_ThreeShortFeedBack; 
//            Mvrq.MoveRequestParam = NoMoveParam;
//            MoveRequest_Post(&Mvrq);   
//          break; 
//              
//        case ErgoMode_FactoryMode :   
//            Mvrq.MoveType = MoveRequestType_Nolimit_ThreeShortFeedBack; 
//            Mvrq.MoveRequestParam = NoMoveParam;
//            MoveRequest_Post(&Mvrq); 
//          break; 
//         
//        case ErgoMode_SetAndTemporaryRemote: 
//        case ErgoMode_EMS_Sensitivity_On :
//        case ErgoMode_EMS_Sensitivity_Off :
//          break;  
//         
//        case ErgoMode_Delivery2Mode :
//            RADIO_OUTLINE=FALSE;
//            ErgonomicManager_ChangeErgoMode(ErgoMode_SetAndNotPaired);
//        case ErgoMode_SetAndNotPaired ://kendy ????
////            Mvrq.MoveType = MoveRequestType_Nolimit_DoubleShortFeedBack; 
////            Mvrq.MoveRequestParam = NoMoveParam;
////            MoveRequest_Post(&Mvrq);
//          break;
//          
//        case ErgoMode_SpeedAdjustmentMode :
//             if(SpeedChange==true)
//              {
//                 SpeedChange=false;
//                 Curr_SpeedMode=Pre_SpeedMode; 
//              }
//        case ErgoMode_ConfigMode :
//        case ErgoMode_SleepMode : 
//        case ErgoMode_MaintenanceResetingMode : 
//        case ErgoMode_Delivery3Mode : 
//        case ErgoMode_UserMode :
//            RADIO_OUTLINE=FALSE;
//            ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);
//            
//            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack; 
//            Mvrq.MoveRequestParam = NoMoveParam;
//            MoveRequest_Post(&Mvrq);
//          break;
//          
//        case ErgoMode_ReSettingDEL :            
//        case ErgoMode_ReSettingUEL :    
//            ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);
//            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack; 
//            Mvrq.MoveRequestParam = NoMoveParam;
//            MoveRequest_Post(&Mvrq);
//            
//            IWDG_ReloadCounter();
//            SFY_OS_TaskDelay(OS_TIME_500_MS);
//            
//            LimitBack(); 
//          break;
//          
//       default :
//         // trap();
//          break;   
//     } 
//   }
  
}

     



void Controller_Idle(void)
{
    MoveRequest_s Mvrq;
    ErgoMode_e ErgoMode = ErgonomicManager_GetCurrentMode();
    if(me.PendingEepromWrite == TRUE)
    {
      ControllerEventHandler_SaveAllToEeprom();
    }
    PowerMonitor_MeasureVbat(TRUE);
    IWDG_ReloadCounter();          
//kendy
//    if(ADC_GetValue_Polling(ADC_Mode_PowerCharge) > 500u) //kendy
//    {
//        Current_charger = 0;  
//        Charger_LED=Close;
//    }else if(ADC_GetValue_Polling(ADC_Mode_PowerCharge) < 100u)
//    {
//        Current_charger = 1; 
//        if(User_LedMode == PowerLevelHigh)//插上电之后 电量满足90%
//          Charger_LED=GreenStatic;
//        else                              //插上电之后 电量不满足90%  
//          Charger_LED=Green_Economy;
//    }
    
     if(Motor_HotProtect == true)
      {
          LED_LogEvent(LED_Event_ThermalProtect);
          if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
          {
            Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
          }
      }else if(MotorBlocked!=true)
      {
          if(Charger_LED==GreenStatic && ErgoMode==ErgoMode_UserMode)
          {
               LED_LogEvent(LED_Event_UserChargerFull);
          }else if(Charger_LED==Green_Economy && ErgoMode==ErgoMode_UserMode)
          {
               LED_LogEvent(LED_Event_UserChargerGoing);
          }
      }else
          LED_LogEvent(LED_Event_MotorBlocked);   
     
     
    if(Bool_tim_last==BUTTON_7SEC)
    {
        // Update flag to enter in bootloader mode at startup
        FLASH_Unlock(FLASH_MEMTYPE_DATA);
        FLASH_ProgramByte((uint32_t)&bStayInBootProgram, (uint8_t)TRUE);  
        __disable_interrupt();

        RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_MotorReset);
    }
    
    if( UartEvent_Command!=Pre_UartEvent_Command)
    {
        Pre_UartEvent_Command=UartEvent_Command;
        Current_Speed = STANDARD_28_SPEED; 
        
        if( ErgoMode ==  ErgoMode_FactoryMode)
        {
          ErgonomicManager_ChangeErgoMode(ErgoMode_IndustrialMode);
        }
        
        if(UartEvent_Command==UartCommand_UP)
        {
            if( ErgoMode ==  ErgoMode_FactoryMode)
            {
              ErgonomicManager_ChangeErgoMode(ErgoMode_IndustrialMode);
            }
            RTSProtocol_Disable();
            
            Mvrq.MoveType = MoveRequestType_Manual_Roll;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);        
        }else if(UartEvent_Command==UartCommand_DOWN)
        {
            Mvrq.MoveType = MoveRequestType_Manual_UnRoll;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);        
        }else if(UartEvent_Command==UartCommand_STOP)
        {
            Mvrq.MoveType = MoveRequestType_Manual_Stop; 
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
        }else if(UartEvent_Command==UartCommand_DOWN5TURN)
        {
            TU32 CurrentPosition = EncoderDriver_GetPosition();  //获取当前位置
            Mvrq.MoveType = MoveRequestType_GoToAbsPos;
            Mvrq.MoveRequestParam.AbsPos = (CurrentPosition+790u);  //QT30
//            Mvrq.MoveRequestParam.AbsPos = (CurrentPosition+480u);    //RU24
            MoveRequest_Post(&Mvrq);        
        }else if(UartEvent_Command==UartCommand_REST)
        {
            RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_MotorReset);
        }else if(UartEvent_Command==UartCommand_RESTART)
        {
             RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Initial_SetFlag);
             RTSProtocol_Enable();
            // ErgonomicManager_ChangeErgoMode(ErgoMode_Delivery1Mode);  //kendy 
             ErgonomicManager_ChangeErgoMode(ErgoMode_FactoryMode);
        }
    }
      
      Idle_changer_Count++;
    if(Idle_changer_Count<=4)
      Prior_charger = Current_charger;
    else if(Idle_changer_Count>100)
      Idle_changer_Count=5;
           
    
//    if(Current_charger != Prior_charger)      //充电器状态改变
//    {
//       Prior_charger = Current_charger;       //update
//       
//       if(Current_charger ==1)                //插上充电器
//         Charger_mode  = true;
//       else if(ErgoMode==ErgoMode_UserMode && Motor_HotProtect != true && MotorBlocked !=true)
//         LED_LogEvent(LED_Event_BackUserMode);//拔出充电器 用户模式关闭灯  
//    }  
    
    if(ErgoMode ==  ErgoMode_SpeedAdjustmentMode)///kendy????????
    { 
      if((Speed_MoveCycleRelod == true)&&(EncoderDriver_GetMoveDirection() == EncoderDriver_Stop))
      {
        Speed_MoveCycleRelod=false;
        MoveRequest_s Mvrq = {   
          .MoveType = MoveRequestType_Nolimit_10sCycleMove,
          .MoveRequestParam = NoMoveParam};
        MoveRequest_Post(&Mvrq);	
      }
    }
    else if(ErgoMode ==  ErgoMode_Tiltingspeedadjustment)///kendy1????????
    { 
      if((Speed_MoveCycleRelod == true)&&(EncoderDriver_GetMoveDirection() == EncoderDriver_Stop))
      {
        Speed_MoveCycleRelod=false;
        MoveRequest_s Mvrq = {   
          .MoveType = MoveRequestType_Nolimit_TiltingCycleMove,
          .MoveRequestParam = NoMoveParam};
        MoveRequest_Post(&Mvrq);	
      }
    } 
    
   if(((GPIO_ReadInputData(BUTTON_PORT) & BUTTON_PIN) != 0x00) || Charger_mode==true)  //只有在按键没有按下时尝试低功耗模式进入
   {   
       if(Butt_Press == TRUE)
        {
           WakeUP_FeedBack = FALSE;
           Butt_Press = FALSE;
           ErgoMode_Switch_Button();            
        }else if(Charger_mode == true)
        {
          //ErgoMode_Switch_Charger();   //kendy  
          ;
        }else if(SleepModeChange == true)
        {
          SleepModeChange = false;
          
          LED_LogEvent(LED_Event_ToUserMode);
          
          ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);
          
          Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq); 
        }else if(Led_flash_once == true)
        {
        
        }
        else 
        {
          TryToLowpower();                    
        }            
   }else
    {
      MotorProg_FeedBack();                
    } 
}


//RTS timing detected, stay wake up for at least RTS_ACTIVITY_TIMEOUT
void Controller_RTSActivityNotify()
{
  me.NoRTS = FALSE; //有RTS帧
  SFY_OS_TimerStop(CONTROLLER_LOWPOWER_TIMER_ID);
  SFY_OS_EventReset(OS_TASK_Controller,TASK_Controller_LOWPOWER_TIMEOUT_EVENT);
  //重设OS_TASK_Controller 的超时事件
   //60ms
  SFY_OS_TimerStart(CONTROLLER_LOWPOWER_TIMER_ID,RTS_ACTIVITY_TIMEOUT,OS_ONE_SHOT_TIMER);
}



//Set by TASK_Controller_LOWPOWER_TIMEOUT_EVENT
void ControllerSetNoRTSFlag()
{
  if(me.NoRTS == FALSE)
  {
    //DEBUG_PRINT("\n[DBG] NO RTS");
    me.NoRTS = TRUE;
  }
}


void Controller_PowerOffSensor()
{
  EncoderDriver_SensorCmd(FALSE);
  ControllerEventHandler_SaveAllToEeprom();
}


void Controller_TestRTSTiming()
{
  if( RTSDataSupply_PresenceOfRTSFragment() == TRUE)//有RTS数据帧
  {
    //Used for SQC purpose only
    RtsLowPowerManager_Set_RTS_Timing_Flag();
    Controller_RTSActivityNotify();
  }
}

//判断是否可以进入低功耗模式
static void TryToLowpower()
{
  TBool EncoderDriverIsPowerOn = EncoderDriver_IsPowerOn();                     //编码器电源需要关闭
  TBool MovementManagerNoneSet = MovementManager_NoneSet();                     //堆栈中没有运动数据
  TBool ErgonomicManagerAllowLowPower = ErgonomicManager_AllowLowPower();       //当前模式允许进入低功耗
  Controller_TestRTSTiming();//没有RTS数据帧
   
  if(FORBID_LOWPOWER == FALSE              && 
     EncoderDriverIsPowerOn == FALSE       && 
     me.NoRTS == TRUE                      &&          
     MovementManagerNoneSet == TRUE        &&
     me.State == ActuatorState_HighPower   &&
     ErgonomicManagerAllowLowPower == TRUE &&
     me.PendingEepromWrite == FALSE)           
  {
    DEBUG_PRINT("\n[DBG]LP OK");
    Controller_SetState(ActuatorState_LowPower); //进入低功耗模式
  }
}



//控制器设置状态
void Controller_SetState(ActuatorState_e State)
{
  MoveRequest_s Mvrq;  
  me.State = State; 
  ErgoMode_e ErgoMode = ErgonomicManager_GetCurrentMode();
  
  switch(me.State)
  {
    case ActuatorState_Boot :            
      Chrono_Start(Chrono_HighPowerTime);
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_IDLE, TASK_IDLE_EVENT);
      me.AcceptMoveRequest = FALSE;     
      UARTComDriver_Cmd(UARTComDriver_Enable_RxTxToDebug,UART_BAUD_RATE_DEBUG);
      break;
      
    case ActuatorState_StartEMS :             
      SFY_OS_EventReset(OS_TASK_IDLE,TASK_IDLE_EVENT);
      me.AcceptMoveRequest = FALSE;                   
      UARTComDriver_Cmd(UARTComDriver_Enable_RxTxToIndus,UART_BAUD_RATE_INDUS);
      SFY_OS_TimerStart(CONTROLLER_EMS_START_FRAME_TIMER_ID,INDUSPROTOCOL_TIMEOUT_ms,OS_ONE_SHOT_TIMER);
      break;
      
    case ActuatorState_EMS_Mode :                                     
      SFY_OS_EventReset(OS_TASK_IDLE,TASK_IDLE_EVENT);
      PowerMonitor_SetupHardware_LowPower();
      me.AcceptMoveRequest = FALSE;
      ErgonomicManager_ChangeErgoMode(ErgoMode_EMS_Sensitivity_Off);
      RTSProtocol_Disable();
      break;
      
    case ActuatorState_EMS_RTS_Sensitivity :
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_IDLE, TASK_IDLE_EVENT);
      me.AcceptMoveRequest = FALSE;
      ErgonomicManager_ChangeErgoMode(ErgoMode_EMS_Sensitivity_On);
      RTSProtocol_Enable();
      break;
      
    case ActuatorState_StopEMS :                                 
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_IDLE, TASK_IDLE_EVENT);
      me.AcceptMoveRequest = FALSE;
      UARTComDriver_Cmd(UARTComDriver_Enable_RxTxToDebug,UART_BAUD_RATE_DEBUG);
      Controller_SetState(ActuatorState_Autotest);
      break;
      
    case ActuatorState_Autotest :                
      me.AcceptMoveRequest = FALSE; 

      
      UARTComDriver_Cmd(UARTComDriver_Enable_TxSQC,UART_BAUD_RATE_SQC);
      Version_Print();
      UARTComDriver_Cmd(UARTComDriver_Disable,0);//串口数据输出之后 关闭串口    xinde
      Controller_SetState(ActuatorState_RunPowerOnAction);
      break;
      
    case ActuatorState_RunPowerOnAction :  //执行器上电模式         
      Setup_And_Start_Watchdog();          //设置开启独立看门狗 500ms 
      //Start Idle task
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_IDLE, TASK_IDLE_EVENT);
      //系统事件信号
      me.AcceptMoveRequest = TRUE;         
      //LED　　   
      GPIO_Init(LED_GREEN_PORT,LED_GREEN_PIN,LED_GREEN_MODE); 
      GPIO_Init(LED_RED_PORT,LED_RED_PIN,LED_RED_MODE); 
      GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
      GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
      //BUTTON　   
      GPIO_Init(BUTTON_PORT,BUTTON_PIN,BUTTON_MODE);
      
      GPIO_Init(EN_SC_DET_PORT,EN_SC_DET_PIN,EN_SC_DET_MODE); 
      GPIO_WriteLow(EN_SC_DET_PORT,EN_SC_DET_PIN);


         Curr_SpeedMode =me.SQC.Curr_SpeedMod;
         User_Mode      =me.SQC.User_Mod; 
         ErgonomicManager_ReadParameter();   //速度和用户模式 读取 
         
         if( ErgoMode ==  ErgoMode_FactoryMode)
         {
              UARTComDriver_Cmd(UARTComDriver_Enable_RxTxToDebug,UART_BAUD_RATE_DEBUG);
              if(FeedBackMode == FatoryColdPower)
              {
                if(me.Pdata.IndustrialTestFinish  == Initial_Set_Flage)
                {
                    Mvrq.MoveType = MoveRequestType_Nolimit_ThreeShortFeedBack; 
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq); 
                }else
                 {
                    //Mvrq.MoveType = MoveRequestType_Nolimit_FourShortFeedBack; //kendy
                    Mvrq.MoveType = MoveRequestType_Nolimit_ThreeShortFeedBack; 
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq); 
                 }
              }else if(FeedBackMode == FatoryHotPower)
              {
                    Mvrq.MoveType = MoveRequestType_Nolimit_ThreeShortFeedBack; 
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq); 
              }
              
              ErgonomicManager_ChangeErgoMode(ErgoMode_FactoryMode);        
         }
//         else if(ErgoMode ==  ErgoMode_SetAndNotPaired) //kendy?????
//         { 
//              ErgonomicManager_ChangeErgoMode(ErgoMode_SetNoTemporaryRemote); //kendy????
//         }else if(ErgoMode ==  ErgoMode_SetNoTemporaryRemote) //kendy?????
//         { 
//              ErgonomicManager_ChangeErgoMode(ErgoMode_SetNoTemporaryRemote); 
//         }
         else if(ErgoMode ==  ErgoMode_SetAndTemporaryRemote||
                 ErgoMode ==ErgoMode_SetAndNotPaired||
                   ErgoMode ==  ErgoMode_SetNoTemporaryRemote)
         {
              Mvrq.MoveType = MoveRequestType_Nolimit_DoubleShortFeedBack; 
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
              
              Banker_DeleteOnlyRemotes(); 
             // ErgonomicManager_ChangeErgoMode(ErgoMode_SetAndNotPaired); 
              ErgonomicManager_ChangeErgoMode(ErgoMode_SetNoTemporaryRemote); //kendy
         } 
         else if(ErgoMode ==  ErgoMode_UserMode)
         {
              LimitBack(); //add to return to the limit range 
              ////////////kendy//
                     Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack; 
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq); 
                    /////////////
              ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);  
         }
         else if(ErgoMode ==  ErgoMode_Delivery1Mode)
         {
              RADIO_OUTLINE=TRUE;
 
              ErgonomicManager_ChangeErgoMode(ErgoMode_Delivery1Mode);  
         }
         else if(ErgoMode ==  ErgoMode_Delivery2Mode)
         {
               UARTComDriver_Cmd(UARTComDriver_Enable_RxTxToDebug,UART_BAUD_RATE_DEBUG);
               RADIO_OUTLINE=TRUE;
               ErgonomicManager_ChangeErgoMode(ErgoMode_Delivery2Mode);  
         }
         else if(ErgoMode ==  ErgoMode_Delivery3Mode)
         {
               RADIO_OUTLINE=TRUE;
               ErgonomicManager_ChangeErgoMode(ErgoMode_Delivery3Mode);  
         }
      Switch_Button_Enable= true;
      Controller_SetState(ActuatorState_HighPower);//设置控制器状态 全功耗模式
      Controller_RTSActivityNotify();              //检测RTS 60ms
      break;
      
      
    case ActuatorState_HighPower :
      //退出休眠模式
      PlatformConfig_SetUpHardWareForHighPower();//硬件外设恢复
      RTSProtocol_Enable();                      //RTS 使能     
      Chrono_Start(Chrono_HighPowerTime);        //开始醒来计时
      Chrono_Stop(Chrono_LowPowerTime);          //关闭休眠计时

      me.AcceptMoveRequest = TRUE;               //接受电机运行请求
      //Set no RTS Flag to false, start LP timer 150 ms
      Controller_RTSActivityNotify();
      Controller_Idle();//[DBG]LP OK  Run idle action   数据保存 电压检测 尝试低功耗 
      
  #if DEBUGPRINT == 1
      UARTComDriver_Cmd(UARTComDriver_Enable_RxTxToDebug,UART_BAUD_RATE_DEBUG);
  #endif  
      DEBUG_PRINT("\n[DBG]LP OUT 2 ") ; 
      break;
      
    case ActuatorState_LowPower :
      DEBUG_PRINT("\n[DBG]LP IN 2\n");              //进入休眠模式
      DEBUG_PRINT1("\n[DBG] Motor_Protect_Count(%ld)\n",Motor_Protect_Count);

      Chrono_Start(Chrono_LowPowerTime);            //开始休眠计时
      Chrono_Stop(Chrono_HighPowerTime);            //关闭醒来计时
      me.AcceptMoveRequest = FALSE;                 //不接受运动请求
      PlatformConfig_SetUpHardWareForLowPower();    //硬件进入低功耗
      RtsLowPowerManager_SleepLoop();               //无线睡眠模式
      Controller_SetState(ActuatorState_HighPower); //下一个模式 醒来  
      break;
      
    default :
      trap();
      break;
  }
  
}


ActuatorState_e Controller_GetConntrollerGloabalState(void)
{
  return me.State;
}


void Controller_SetPdataToFactory()
{
  me.Pdata.InitialSetFinish      = Initial_Set_Null;  //与模式有关
  me.Pdata.IndustrialTestFinish  = Initial_Set_Null;  //与是否串口串口测试有关
  me.Pdata.AllReset=FatoryColdPower;
}


static void Controller_PowerFailHandler()
{
  static EncoderPosition_s LastSavedPos,CurrentPosition; 
  ErgoMode_e ErgoMode = ErgonomicManager_GetCurrentMode();//获取当前模式
  
  IWDG_ReloadCounter();          //独立看门狗喂狗
  //Shut off radio
  RTSProtocol_Disable();         //关闭RTS数据
  MotorDriver_SwitchToLowPower();//电机驱动进入低功耗
  
  SQC_Count(&me.SQC.PwrCut,sizeof(me.SQC.PwrCut));
  LED_LogEvent(LED_Event_PwrCut);//断电事件更新
  SQC_LogEvent(SQC_Event_PwrCut);
  if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)//运行中断电
  {
    SQC_Count(&me.SQC.PwrCutWhileMoving,sizeof(me.SQC.PwrCutWhileMoving));//设置标志位
  }
  if( 
     //ErgoMode ==  ErgoMode_FactoryMode      ||
      ErgoMode ==  ErgoMode_FirstPairingMode ||
      ErgoMode ==  ErgoMode_SettingPart1Mode ||
      ErgoMode ==  ErgoMode_SettingPart1Modebis)
  {
        me.Pdata.AllReset=FatoryHotPower;
  }
  ////  //Complete save => 8*6ms = 48ms   完成保存时间
  ////  //estimate capa time [150ms - 500ms] (old capa at 9v, new capa at 12v)
  SQC_LogEvent(SQC_Event_SaveAll);
  LastSavedPos = EncoderDriver_GetFinePosition();//获取位置信息
  Archiver_SaveAllObject();       //保存所有数据
  
  while(PowerMonitor_MeasureVbat(FALSE) < POWERFAIL_ADCLEVEL_TRIGGER)
  {//电压低于最小值时一直运行 
    IWDG_ReloadCounter();//喂狗
    CurrentPosition =  EncoderDriver_GetFinePosition();//获取位置信息
    if(LastSavedPos.MotorPosition != CurrentPosition.MotorPosition ||
       LastSavedPos.MotorTurn     != CurrentPosition.MotorTurn)  //数据在更新
    {
      Archiver_SaveOnlyCriticalObject();             //只保存关键数据
      LastSavedPos = EncoderDriver_GetFinePosition();//获取位置信息
    }
    else
    {
      SFY_OS_TaskDelay(OS_TIME_10_MS);              //延时10ms 继续检测
    }
  }
  WWDG_SWReset(); 
}

 void LimitBack(void)
{
    Limits_SetMode(LimitMode_Normal);
    TU32 CurrentPosition = EncoderDriver_GetPosition();  //获取当前位置
    Limits_ComputeLimitsState(&CurrentPosition);         //当前位置对比限位状态
    Limits_State_e LimisState = Limits_GetLimitsState(); //获取限位位置区间状态
    
    if(LimisState == Limits_State_Above)        
    {
         //LED_LogEvent(LED_Event_StopAboveLimits);
         
         MoveRequest_s Mvrq = {
        .MoveType = MoveRequestType_GoToPosition,
        .MoveRequestParam.Position = PositionName_UEL};
         MoveRequest_Post(&Mvrq);                  
    }else if(LimisState == Limits_State_Below)  
    {
         //LED_LogEvent(LED_Event_StopBelowLimits);
         
         MoveRequest_s Mvrq = {
        .MoveType = MoveRequestType_GoToPosition,
        .MoveRequestParam.Position = PositionName_DEL};
         MoveRequest_Post(&Mvrq);                  
    }
}




//RTS控制 模式改变
static void RTSErgoChangeModeHandler(ErgoMode_e NewMode)
{
// LOG_PRINT1("[LOG] Controller received event NewRTSErgoMode( %s ) \n",ErgoMode_TXT[NewMode]) ;
  
//  if(NewMode == ErgoMode_UserMode)  //在用户模式下 如果位置不在限位之间 电机需要先回去
//  {
//     LimitBack(); 
//  }   
}

//RTS 特殊请求处理
static void RTSErgoSpecialRequestHandler(RTSErgoSpecialRequest_e Sreq)
{
  switch(Sreq)
  {
    case RTSErgo_Request_RemoteEepromBackup:       //RTS请求遥控器数据保存
      ControllerEventHandler_SaveAllToEeprom();    //保存遥控器信息
      break;
      
    case RTSErgo_Request_InvertRotationDirection: //RTS请求改变运行方向  需要这里执行
      ControllerEventHandler_InvertRotation();
      ControllerEventHandler_SaveAllToEeprom();
      break;
      
    case RTSErgo_Request_MotorReset:              //遥控器器复位,电机上面的按键12S 执行复位 
      me.Pdata.AllReset=FatoryBeReset;
      Archiver_SaveAllObject();                   //保存所有数据
      IWDG_ReloadCounter();
      SFY_OS_TaskDelay(OS_TIME_100_MS);           //延时100ms 继续检测
      WWDG_SWReset();                             
      break;
         
    case RTSErgo_Request_RunAutotest6:
      Version_Print();
      break;
      
    case RTSErgo_EMS_SensitivityFrameReceived:
      break;
    
    case RTSErgo_Speed_UserModeChange:
      me.SQC.Curr_SpeedMod=Curr_SpeedMode;//复位存储标志
      me.SQC.User_Mod=User_Mode; 
      ControllerEventHandler_SaveAllToEeprom();
      break;
     
   case RTSErgo_Initial_SetNull:         //factory
      me.Pdata.InitialSetFinish  = Initial_Set_Null; //
      ControllerEventHandler_SaveAllToEeprom();
      break;
      
    case RTSErgo_Initial_SetFlag:         //设置3次抖动标志
      me.Pdata.IndustrialTestFinish = Initial_Set_Flage; //
      ControllerEventHandler_SaveAllToEeprom();
      break;
     
    case RTSErgo_Initial_Finish:         //完成设置进入用户模式  user_mode 
      me.Pdata.InitialSetFinish  = Initial_Set_Finish; //完成初始化设置
      ControllerEventHandler_SaveAllToEeprom();
      break;
           
    case RTSErgo_Initial_SetAndNotPaired:   
      me.Pdata.InitialSetFinish  = Initial_SetAndNotPaired;
      ControllerEventHandler_SaveAllToEeprom();
      break;
      
   case RTSErgo_Initial_SetAndTemporary:   
      me.Pdata.InitialSetFinish  = Initial_SetAndTemporary;
      ControllerEventHandler_SaveAllToEeprom();
      break;
     
    case RTSErgo_Initial_DeliveryMode1:  
      me.Pdata.InitialSetFinish  = Initial_DeliveryMode1;     
      ControllerEventHandler_SaveAllToEeprom();
      break;
      
    case RTSErgo_Initial_DeliveryMode2:  
      me.Pdata.InitialSetFinish  = Initial_DeliveryMode2;     
      ControllerEventHandler_SaveAllToEeprom();
      break;
      
    case RTSErgo_Initial_DeliveryMode3:   
      me.Pdata.InitialSetFinish  = Initial_DeliveryMode3;     
      ControllerEventHandler_SaveAllToEeprom();
      break;
         
    default :
      break;
  }
}

//低电压时允许的电机运行指令请求
static TBool AllwoMoveRequestOnLowBattery(const MoveRequest_s *Mvrq)
{
  TBool Rtn = FALSE;
  switch(Mvrq->MoveType)
  {
      case MoveRequestType_SecurityStop : 
      case MoveRequestType_None:
      case MoveRequestType_Manual_Stop:   
      case MoveRequestType_Nolimit_Stop:
      case MoveRequestType_Manual_PWM:
      case MoveRequestType_CompensationMove :
        Rtn = TRUE;                       
        break;
        
      case MoveRequestType_Manual_Roll:
      case MoveRequestType_Manual_UnRoll:
      case MoveRequestType_Manual_RollForTime:
      case MoveRequestType_Manual_UnRollForTime:
      case MoveRequestType_Nolimit_RollForTime:
      case MoveRequestType_Nolimit_UnRollForTime:
      case MoveRequestType_Nolimit_StopForTime:
      case MoveRequestType_Nolimit_RollTo:
      case MoveRequestType_Nolimit_UnRollTo:
      case MoveRequestType_Nolimit_ShortUnRoll:
      case MoveRequestType_Nolimit_ShortRoll:
      case MoveRequestType_Nolimit_Rolloff:
      case MoveRequestType_Nolimit_UnRolloff:
      case MoveRequestType_Nolimit_ShortFeedBack:
      case MoveRequestType_Nolimit_ErrFeedBack:
      case MoveRequestType_Nolimit_ErrFeedBackMoveTo:
      case MoveRequestType_Nolimit_DoubleShortFeedBack:
      case MoveRequestType_RollForTime:
      case MoveRequestType_UnRollForTime:
      case MoveRequestType_RollTo:
      case MoveRequestType_UnRollTo:
      case MoveRequestType_GoToPercent:
      case MoveRequestType_GoToAbsPos:
      case MoveRequestType_ShortUnRoll:
      case MoveRequestType_ShortRoll:
      case MoveRequestType_Rolloff:
      case MoveRequestType_UnRolloff:
      case MoveRequestType_RollToLimits:
      case MoveRequestType_UnRollToLimits:
      case MoveRequestType_GoToLimits:
      case MoveRequestType_RollToPosition:
      case MoveRequestType_UnRollToPosition:
      case MoveRequestType_GoToPosition:
      case MoveRequestType_TiltMyPosition:
        Rtn = FALSE;                   
        break;
        
      default :
        break;
  }
  
  return Rtn;
}



//电机运行请求传递
void MoveRequest_Post(const MoveRequest_s *Mvrq)
{
  TBool RequestReject = FALSE;     //请求拒绝 FALSE
   DEBUG_PRINT("[DBG] MoveRequest_Post1 \n");
  if(me.AcceptMoveRequest == TRUE) //接受运行请求
  {
    if(PowerMonitor_BatteryLevelAllowMove() == TRUE || AllwoMoveRequestOnLowBattery(Mvrq) == TRUE)
    {//电压判断 命令判断
        //LOG_PRINT1("[LOG] Controller received MvReq( %s ) \n",MoveRequestType_TXT[Mvrq->MoveType]);
         DEBUG_PRINT("[DBG] MoveRequest_Post2 \n");
        //Industrial test bench is pertubed by return move.  工业测试机
        //So, the return move is disable when no remote paired (motor can only be driven by industrial remote)
        if(ErgonomicManager_GetCurrentMode() !=ErgoMode_UserMode && Mvrq->MoveType == MoveRequestType_CompensationMove)
        { //不在ErgoMode_UserMode 模式下 CompensationMove请求 电机运动不执行请求
          //Reject Request => remplace by a NONE
          DEBUG_PRINT("[DBG] Post3 \n");
          MoveRequest_s NoneReq;
          NoneReq.MoveType = MoveRequestType_None;
          NoneReq.MoveRequestParam = NoMoveParam;
          MoveRequest_Execute(&NoneReq);           //电机运动请求执行  无请求
          RequestReject = TRUE;
        }else if(Motor_HotProtect == true)
        {       
          MoveRequest_s NoneReq;
          NoneReq.MoveType = MoveRequestType_None;
          NoneReq.MoveRequestParam = NoMoveParam;
          MoveRequest_Execute(&NoneReq);           //电机运动请求执行  无请求
          RequestReject = TRUE;
        }else
        {
          DEBUG_PRINT("[DBG] Post4 \n");
          MoveRequest_Execute(Mvrq);//执行请求
        }
    }
    
    if(Mvrq->MoveType == MoveRequestType_None || RequestReject == TRUE)
    { //没有电机运行请求   请求拒绝
      LongTimer_Stop(LongTimer_6MinMaxMoveTimer);        //停止3分钟计时
      ErgoMode_e EM = ErgonomicManager_GetCurrentMode(); //获取当前人机模式
      if(EM == ErgoMode_UserMode)                        //如果是用户模式
      {
        const TU32 *DEL = Position_GetValue(PositionName_DEL);
        const TU32 *UEL = Position_GetValue(PositionName_UEL);
        const TU32 *My  = Position_GetValue(PositionName_My);
        
        TU32 CurrentPosition = EncoderDriver_GetPosition();
        PositionCompare_e CompareToUp   = Position_CompareTwoAbsolutePosition(UEL,&CurrentPosition,POSITION_TOLERANCE);
        PositionCompare_e CompareToDown = Position_CompareTwoAbsolutePosition(DEL,&CurrentPosition,POSITION_TOLERANCE);
        PositionCompare_e CompareToMy   = Position_CompareTwoAbsolutePosition(My,&CurrentPosition, POSITION_TOLERANCE);
        
        if(CompareToUp == CurrentPositionIsBelowRef && CompareToDown == CurrentPositionIsAboveRef)
        {//在中间位置
          if(CompareToMy == CurrentPositionIsOnRef)
          { 
            SQC_LogEvent(SQC_Event_StopOnMy);
            SQC_Count(&me.SQC.StopOnMy,sizeof(me.SQC.StopOnMy));
          }else
          {
            SQC_LogEvent(SQC_Event_StopBetweenLimits);
            SQC_Count(&me.SQC.StopBetweenLimits,sizeof(me.SQC.StopBetweenLimits));
          }
        }else if(CompareToUp == CurrentPositionIsOnRef && CompareToDown == CurrentPositionIsAboveRef)
        {//在上限位位置
          SQC_LogEvent(SQC_Event_StopOnUL);
          SQC_Count(&me.SQC.StopOnUL,sizeof(me.SQC.StopOnUL));
        }else if(CompareToUp == CurrentPositionIsBelowRef && CompareToDown == CurrentPositionIsOnRef)
        {//在下限位位置
          SQC_LogEvent(SQC_Event_StopOnDL);
          SQC_Count(&me.SQC.StopOnDL,sizeof(me.SQC.StopOnDL));
        }else if(CompareToUp == CurrentPositionIsAboveRef && CompareToDown == CurrentPositionIsAboveRef)
        {//超过上限位
          SQC_LogEvent(SQC_Event_StopAboveLimits);
          SQC_Count(&me.SQC.StopAboveLimits,sizeof(me.SQC.StopAboveLimits));
        }else if(CompareToUp == CurrentPositionIsBelowRef && CompareToDown == CurrentPositionIsBelowRef)
        {//低于下限位
          SQC_LogEvent(SQC_Event_StopBelowLimits);
          SQC_Count(&me.SQC.StopBelowLimits,sizeof(me.SQC.StopBelowLimits));
        }
      }  
      EncoderPosition_s Pos = EncoderDriver_GetFinePosition();
      ControllerEventHandler_SaveAllToEeprom();
      
      INFO_PRINT2("Pos %ld,%d\n",Pos.MotorTurn,Pos.MotorPosition);
    }
  }
}



//控制器事件处理保存EEPROM数据
static void ControllerEventHandler_SaveAllToEeprom()
{  
  if( EncoderDriver_GetSpeed() == 0 && MovementManager_NoneSet() == TRUE && ErgonomicManager_IsRTSFlowFlagSet() == FALSE)
   {  //电机不在运行 没有运行请求 没有RTS数据帧  则保存数据
      DEBUG_PRINT("[DBG] Controller SaveAllToEeprom\n");
      me.SQC.FreeSpaceInHeap  = Application_Check_Heap_SpaceLeft();
      me.SQC.FreeSpaceInStack = Application_Check_Stack_SpaceLeft();
      SQC_LogEvent(SQC_Event_SaveAll);
      EncoderDriver_UpdateFinePosition();  //获取位置信息
      Archiver_SaveAllObject();            //保存数据
      me.PendingEepromWrite = FALSE;       //eeprom上锁
      LongTimer_Reload(LongTimer_AutoSave);//重置自动保存计时时间
   }else
    {
      if( me.PendingEepromWrite == FALSE)
      {
        DEBUG_PRINT("\n[DBG] Controller SaveAllToEeprom DELAYED\n");//延时更新
        me.PendingEepromWrite = TRUE;       //eeprom解锁
      }
    }
}



//控制器事件处理翻转电机方向  当前位置翻转 
static void ControllerEventHandler_InvertRotation()
{
    MotorDriver_InvertRotation();            //电机方向翻转
    EncoderDriver_InvertCountingDirection(); //编码轮方向翻转
  
  if(me.Pdata.InitialSetFinish  == Initial_Set_Finish)   //IP 位置需要调整
  {
    EncoderPosition_s CurrentPos = EncoderDriver_GetFinePosition();//获取当前位置
    TU32 DownUL = *Position_GetValue(PositionName_DEL);//获取下限位位置
    TU32 upUL   = *Position_GetValue(PositionName_UEL);//获取上限位位置
    TU32 oldmyUL= *Position_GetValue(PositionName_My); //获取中间位置 
    
    TU32 delta  = DownUL - CurrentPos.MotorTurn;       //距离下限位位置
    TU32 distancemy = DownUL - oldmyUL; 
    TU32 newmyUL = upUL + distancemy;
    PositionMY_Set(PositionName_My,newmyUL);
    
    EncoderPosition_s NewPosition; 
    NewPosition.MotorTurn = upUL + delta;                                                    //圈数 
    NewPosition.MotorPosition = (TU16) (TICK_PER_MOTOR_TURN - 1u - CurrentPos.MotorPosition);//角度  sage
    EncoderDriver_SetPositionFine(NewPosition);                                              //设置time1 初始相位
  }
}


