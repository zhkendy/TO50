// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#include "Config.h"
#include "SFY_OS.h"
#include "Encoder_Driver.h"
#include "Banker.h"
#include "SQC.h"
#include "STM8_Archiver.h"
#include "stm8s_iwdg.h"
#include "MovementRequest_Interface.h"
#include "LongTimer.h"
#include "SQC_DataProvider.h"
#include "RtsLowPowerManager.h"
#include "Controller.h"
#include "RTS_Ergonomic_ManagerConfig.h"
#include "RTS_Ergonomic_ManagerObservable.h"
#include "RTS_Ergonomic_Manager.h"
#include "MovementPlanner.h"
#include "Chrono.h"
#include "PowerMonitor.h"
#include "RTS_Ergonomic_DebugTXT.h"
#include "Limits_DebugTXT.h"
#include "DebugPrint.h"


#ifdef ERGONOMIC_PRINT_DEBUG	
#include "RTS_Ergonomic_DebugTXT.h"
#include "DebugPrint.h"
#endif

#ifdef ERGONOMIC_PRINT_INFO
#include "Ergonomic_DebugTXT.h"
#include "DebugPrint_info.h"
#endif

#ifdef ERGONOMIC_PRINT_LOG	
#include "Ergonomic_DebugTXT.h"
#include "DebugPrint_Log.h"
#endif

//somfy add
#ifdef ENABLE_BOOTLOADER
#pragma location="APPLI_VERSION_ADD"
#endif
//__root const APPLI_INFO AppliInfo = {APPLI_SOFTWARE_NUMBER, APPLI_SOFTWARE_VERSION, APPLI_SOFTWARE_BUILD_REVISION_HIGH, APPLI_SOFTWARE_BUILD_REVISION_MID, APPLI_SOFTWARE_BUILD_REVISION_LOW, APPLI_SOFTWARE_SIZE_HIGH, APPLI_SOFTWARE_SIZE_MID, APPLI_SOFTWARE_SIZE_LOW};
__root const APPLI_INFO AppliInfo = {APPLI_SOFTWARE_NUMBER, APPLI_SOFTWARE_VERSION, APPLI_SOFTWARE_SIZE_HIGH, APPLI_SOFTWARE_SIZE_MID, APPLI_SOFTWARE_SIZE_LOW};

#ifdef ENABLE_BOOTLOADER
#pragma location="SHARED_DATA_WITH_BOOTLOADER_NO_INIT"
__no_init TBool bStayInBootProgram;
#else 
__no_init TBool bStayInBootProgram;
#endif


bool  Up_moveonce=false;
bool  Down_moveonce=false;
bool  ConfigMode=false;
TU16 Current_Speed=0;
TU16 Tilting_Speed=0;
TU16 Roller_Speed=0;
TU8  Curr_SpeedMode=1;
TU8  Pre_SpeedMode=0;
TU8  User_Mode=0;
TU8  LedMode = AllClose;
TU8  LowpowerLedMode = AllClose;
TU8  NextLedMode = AllClose;
bool Speed_MoveCycleRelod=false;

bool Motor_Protect_Enable=false;
bool SpeedChange=false;
bool Speed_MoveCycle=false;
bool Direction_change=false;
bool Tilting_Move=false;
bool LowPowermodeTest=false;
bool MovingJustOnce=false;//kendy

//bool AcceptedRemote_flag=false;//kendy
//bool AcceptedU56Frame_flag=false;//kendy
//TU8 AcceptedU56Frame_value;
//bool AcceptedU80Function_flag=false;//kendy
bool Tiltingspeed_mode_change=false;
extern TU16  Ledtime_outsecond;
extern TU8   User_LedMode;
extern TU8   InitialSetDone;
extern TU16  SpeedCycle_last;
extern TBool SleepModeChange;
extern bool  RADIO_OUTLINE;
extern TU16  SpeedADJCycle_timems;
extern TU32  SleepWakeTime;
extern TU16  Led_time_last;
extern TU8   Led_repet_time;
extern bool  Motor_Reset;



typedef struct
{
  ErgoRemoteEventType_e Type;
  ErgoRemoteBP_State_e BpState;
  Press_Duration_e PressDuration;
  TU8 RemoteType;
  TU8 PairedState;
  U80_Function_e U80Function;
  TU16 U80_OptionValue;
  TAdresseU56 RemoteAdress;
}ErgoEvent_t;


typedef struct ErgonomicManager_PCounter_SQC_s
{
  TU32 UnPairedRTSFrame;
  TU32 RTS_Up;
  TU32 RTS_Down;
  TU32 RTS_Stop;
  TU32 RTS_My;
  TU32 RTS_ModeUp;
  TU32 RTS_ModeDown;
  TU32 RTS_ModeStop;
  TU32 RTS_Goto;
  TU8  UEL_Reseting;
  TU8  DEL_Reseting;
  TU16 My_Reseting;
  TU8  Motor_Reseting;
  TU8  Motor_RotationResetting;
  TU8  Wheel_RotationResetting;
  TU8  RemotePurge;
}ErgonomicManager_PCounter_SQC_s;

typedef struct ErgonomicManager_SQC_s
{
  ErgonomicManager_PCounter_SQC_s Counter;
  TRemoteControlPersistent CurrentRemote;
  TRemoteControlPersistent RecordedRemote0;
  TRemoteControlPersistent RecordedRemote1;
  TRemoteControlPersistent RecordedRemote2;
  TRemoteControlPersistent RecordedRemote3;
  TRemoteControlPersistent RecordedRemote4;  
  TRemoteControlPersistent RecordedRemote5;
  TRemoteControlPersistent RecordedRemote6;
  TRemoteControlPersistent RecordedRemote7;
  TRemoteControlPersistent RecordedRemote8;
  TRemoteControlPersistent RecordedRemote9;
  TRemoteControlPersistent RecordedRemote10;
  TRemoteControlPersistent RecordedRemote11;
  TRemoteControlPersistent RecordedSensor1;
  TRemoteControlPersistent RecordedSensor2;
  TRemoteControlPersistent RecordedSensor3;
  TRemoteControlPersistent RecordedSensor4;
}ErgonomicManager_SQC_s;

typedef struct Ergonomic_Pdata_s
{
  TU8 Tilt_Size;
  WheelDir_e WheelDir;
}Ergonomic_Pdata_s;

typedef struct
{
  Ergonomic_Pdata_s Pdata;
  ErgonomicManager_SQC_s SQC;
  ErgoMode_e ErgoMode;
  ErgoModeState_e ErgoModeState;
  ErgoMode_e ErgoModeAfterTimeOut;
  ErgoMode_e ErgoModeAfterRelease;
  TBool ChangeStateOnBpRelease;
  TBool RTSFlow;
  TBool DeadmanModeStarted;
  TBool Accepted_U80_CMDArray[U80_LASTID];
  TBool Accepted_U56_CMDArray[ErgoRemoteEventType_LastID];
  TBool AcceptedRemoteArray[REMOTE_TYPE_LENGH];
  
  TAdresseU56 ResettingsRemoteAdress;
}ErgonomicManager_t;

#define me ErgonomicManager
ErgonomicManager_t ErgonomicManager;


static TBool IsItFlowInteruptedStimulus(TStimuli * ptStimuli);
static Press_Duration_e GetStimulusDuration(ErgoEvent_t *Evt,TStimuli *ptStimuli);
static void UpdateU80Field(ErgoEvent_t *ptRemoteEvent,TU32 *U80_Field);
static void UpdateDuration(ErgoEvent_t *ptRemoteEvent, ErgoEvent_t *ptLastEvent,TStimuli *ptStimuli);

static void RememberRemoteUsedToEnterInResetingMode();
static TBool IsCurrentRemoteTheResetingRemote();
static TAdresseU56 GetCurrentRemoteAdress();

static void ErgonomicManager_ErgoTimeOutHandler();
static void RearmTimeout(ErgoEvent_t *Evt);

static void ErgonomicManager_HandleEvent(ErgoEvent_t *Evt);
static void ErgonomicManager_SetAndTemporaryPairedMode_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_FactoryMode_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_IndustrialMode_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_FirstPairingMode_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_SettingPart1Mode_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_SettingPart1Modebis_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_SetAndNotPaired_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_SetNoTemporaryRemote_EventHandler(ErgoEvent_t *evt);//kendy??????
static void ErgonomicManager_ConfigMode_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_MaintenanceResetingMode_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_SpeedAdjustmentMode_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_TiltingspeedadjustmentMode_EventHandler(ErgoEvent_t *evt); //kendy???????
static void ErgonomicManager_ErgoMode_ReSettingSDP_EventHandler(ErgoEvent_t *evt);//kendy3
static void ErgonomicManager_ErgoMode_ReSettingDEL_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_ErgoMode_ReSettingUEL_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_SleepMode_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_Delivery1Mode_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_Delivery2Mode_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_Delivery3Mode_EventHandler(ErgoEvent_t *evt);

static void ErgonomicManager_UserModeA_EventHandler(ErgoEvent_t *evt);
static void ErgonomicManager_UserModeB_EventHandler(ErgoEvent_t *evt);

static TBool CheckU80Checksum(const TU32* U80_Field);

static void ErgonomicManager_SetSQCToFactory();
static void ErgonomicManager_SetToFactory();
static TU8* Get_SQC_data(TU8* DataSize);

static TU8* Get_SQC_data(TU8* DataSize)
{
  *DataSize = sizeof(me.SQC);
//  me.SQC.GlobalMode = me.Pdata.GlobalMode;
  me.SQC.CurrentRemote    = (TransceiverBank_GetRemoteControlBank(INDEX_CURRENT_REMOTE))->RemoteControlPersistent;
  me.SQC.RecordedRemote0  = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED))->RemoteControlPersistent;
  me.SQC.RecordedRemote1  = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 1))->RemoteControlPersistent;
  me.SQC.RecordedRemote2  = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 2))->RemoteControlPersistent;
  me.SQC.RecordedRemote3  = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 3))->RemoteControlPersistent;
  me.SQC.RecordedRemote4  = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 4))->RemoteControlPersistent;
  me.SQC.RecordedRemote5  = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 5))->RemoteControlPersistent;
  me.SQC.RecordedRemote6  = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 6))->RemoteControlPersistent;
  me.SQC.RecordedRemote7  = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 7))->RemoteControlPersistent;
  me.SQC.RecordedRemote8  = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 8))->RemoteControlPersistent;
  me.SQC.RecordedRemote9  = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 9))->RemoteControlPersistent;
  me.SQC.RecordedRemote10 = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 10))->RemoteControlPersistent;
  me.SQC.RecordedRemote11 = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 11))->RemoteControlPersistent;
    me.SQC.RecordedSensor1 = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 12))->RemoteControlPersistent;
      me.SQC.RecordedSensor2 = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 13))->RemoteControlPersistent;
       me.SQC.RecordedSensor3 = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 14))->RemoteControlPersistent;
        me.SQC.RecordedSensor4 = (TransceiverBank_GetRemoteControlBank(INDEX_FIRST_REMOTE_RECORDED + 15))->RemoteControlPersistent;

  return (TU8*) &me.SQC;
}

static void ErgonomicManager_SetSQCToFactory()
{
  me.SQC.Counter.DEL_Reseting = 0u;
  me.SQC.Counter.Motor_Reseting= 0u;
  me.SQC.Counter.RemotePurge= 0u;
  me.SQC.Counter.RTS_Down= 0u;
  me.SQC.Counter.RTS_ModeDown= 0u;
  me.SQC.Counter.RTS_ModeStop= 0u;
  me.SQC.Counter.RTS_ModeUp= 0u;
  me.SQC.Counter.RTS_My= 0u;
  me.SQC.Counter.RTS_Stop= 0u;
  me.SQC.Counter.RTS_Up= 0u;
  me.SQC.Counter.UEL_Reseting= 0u;
  me.SQC.Counter.UnPairedRTSFrame= 0u;
}

static void ErgonomicManager_SetToFactory()
{
  me.Pdata.Tilt_Size  =  DEFAULT_TILT_SIZE;
  me.Pdata.WheelDir   =  WheelDir_Normal;
}

void ErgonomicManager_Reset()
{
  me.Pdata.Tilt_Size = DEFAULT_TILT_SIZE;
  me.Pdata.WheelDir  = WheelDir_Normal;
}


void ErgonomicManager_Init()
{
  TU8 test = ( sizeof(TRemoteControl) ) * (TOTAL_TRANSCEIVER_NBR-SENSOR_NBR);//kendy TOTAL_TRANSCEIVER_NBR
  //TU8 test = ( sizeof(TRemoteControl) ) * (TOTAL_TRANSCEIVER_NBR);
  Archiver_RegisterObject(TransceiverBank_GetRemoteControlBank(0),
                          Banker_ApplyFactorySettings,
                          test,
                          FALSE);
  ////////////kendy//////////
  TU8 test1 = ( sizeof(TRemoteControl) ) * SENSOR_NBR;
    Archiver_RegisterObject(TransceiverBank_GetRemoteControlBank(INDEX_FIRST_SENSOR_RECORDED),
                          Banker_ApplyFactorySettings,
                          test1,
                          FALSE);
  
  //////////////////////
  Archiver_RegisterObject(&me.Pdata,                   //me ErgonomicManager
                          ErgonomicManager_SetToFactory,
                          sizeof(me.Pdata),
                          TRUE);
  
  Archiver_RegisterObject(&me.SQC.Counter,             //me ErgonomicManager
                          ErgonomicManager_SetSQCToFactory,
                          sizeof(me.SQC.Counter),
                          FALSE);
  //SQC 
  SQC_DataProviderNameInterface_s intf;
  intf.Fptr_Get = Get_SQC_data;
  SQC_DataProvider_RegisterDataSource(SQC_DataProvider_Ergonomic ,&intf);
  me.RTSFlow = FALSE;
  me.ChangeStateOnBpRelease = FALSE;
}


RTSErgonomicObserver_s ErgonomicObserverarray[RTSErgonomicObserver_LastEnum];

void RTSErgonomicInterface_NotifyErgoChangeMode(ErgoMode_e NewMode)
{
  for(TU8 i= 0U; i<RTSErgonomicObserver_LastEnum; ++i)
  {
    ErgonomicObserverarray[i].Fptr_ErgoChageMode(NewMode);
  } 
}

TBool ErgonomicManager_IsRTSFlowFlagSet()
{
  return me.RTSFlow;
}


void RTSErgonomicObserver_RegisterObserver(RTSErgonomicObserver_e ObserverName,RTSErgonomicObserver_s Observer)
{
  if(ObserverName < RTSErgonomicObserver_LastEnum)
  {
    ErgonomicObserverarray[ObserverName] = Observer;
  }
}

void RTSErgonomicInterface_SendSpecialRequest(RTSErgoSpecialRequest_e Sreq)
{
  for(TU8 i= 0U; i<RTSErgonomicObserver_LastEnum; ++i)
  {
    ErgonomicObserverarray[i].Fptr_RTSErgoSpecialRequest(Sreq);
  } 
}


void ErgonomicManager_SetGlobalMode()//允许兼容的所有全感器及遥控器，kendy RTS and sensors
{
    SQC_LogEvent(SQC_Event_Indus_Perso_WorldWide); 
    memcpy(me.Accepted_U56_CMDArray,EntryRangeWolrdWide_U56_AcceptedCMDArray,sizeof(me.Accepted_U56_CMDArray));
    memcpy(me.Accepted_U80_CMDArray,EntryRangeWolrdWide_U80_AcceptedCMDArray,sizeof(me.Accepted_U80_CMDArray));
    memcpy(me.AcceptedRemoteArray,EntryRange_AcceptedRemoteArray,sizeof(me.AcceptedRemoteArray));
}


/* 
  5种模式允许进入休眠模式
  其他模式都是临时模式 有超时限制
  ErgoMode_Delivery1Mode
  ErgoMode_Delivery2Mode
  ErgoMode_Delivery3Mode
  ErgoMode_UserMode
  ErgoMode_SleepMode
*/
//允许进入低功耗  
TBool ErgonomicManager_AllowLowPower()
{
  TBool Rtn = FALSE; 
  if(me.RTSFlow  == FALSE &&
    (me.ErgoMode == ErgoMode_Delivery1Mode ||
     me.ErgoMode == ErgoMode_Delivery2Mode ||
     me.ErgoMode == ErgoMode_Delivery3Mode ||
     me.ErgoMode == ErgoMode_UserMode      || 
     me.ErgoMode == ErgoMode_SleepMode     ||
     LowPowermodeTest==true))
    {
      Rtn = TRUE;
    } 
  return Rtn;
}




//人机管理创建
void ErgonomicManager_Create()
{
  RTSProtocol_Create(); //RTS 数据帧初始化
  RTSProtocol_Disable();
  
  Archiver_LoadMe(&me.Pdata);
  Archiver_LoadMe(&me.SQC.Counter);
  ErgonomicManager_SetGlobalMode();
  //导入任务参数和任务
  //遥控器队列导入
  Banker_Restore();
  
  //人机管理选择 模式和状态 上电之后检测是否是设置完成
  if(InitialSetDone == Initial_SetAndNotPaired)//kendy ?????
  {
   // me.ErgoMode = ErgoMode_SetAndNotPaired;//kendy
    me.ErgoMode = ErgoMode_SetNoTemporaryRemote;
  }else if(InitialSetDone == Initial_SetAndTemporary)//???????????kendy
  {
   // me.ErgoMode = ErgoMode_SetAndTemporaryRemote;//kendy
     me.ErgoMode = ErgoMode_SetNoTemporaryRemote; 
  }else if(InitialSetDone == Initial_Set_Finish)
  {
    me.ErgoMode = ErgoMode_UserMode;
  }else if (InitialSetDone ==Initial_DeliveryMode1)
  {
    me.ErgoMode = ErgoMode_Delivery1Mode; 
  }else if (InitialSetDone ==Initial_DeliveryMode2)
  {
    me.ErgoMode = ErgoMode_Delivery2Mode; 
  }else if (InitialSetDone ==Initial_DeliveryMode3)
  {
    me.ErgoMode = ErgoMode_Delivery3Mode; 
  }else if (InitialSetDone ==Initial_Set_Null )
  {
    me.ErgoMode = ErgoMode_FactoryMode; 
  }else 
  {
    me.ErgoMode = ErgoMode_FactoryMode; 
  }
   
  Limits_SetMode(LimitMode_Normal);        
  
  me.ErgoModeState = ErgoModeState_WaitEvent;
}



//记录用于重设的遥控器地址
static void RememberRemoteUsedToEnterInResetingMode()
{
  me.ResettingsRemoteAdress = GetCurrentRemoteAdress();
}

//判断当前遥控器是否为重置遥控器
static TBool IsCurrentRemoteTheResetingRemote()
{
  TAdresseU56 CurrentRemoteAdress = GetCurrentRemoteAdress();
  return memcmp(&CurrentRemoteAdress,&me.ResettingsRemoteAdress,sizeof(TAdresseU56)) == 0  ? (TRUE) : (FALSE);
}

//获取当前遥控器地址
static TAdresseU56 GetCurrentRemoteAdress()
{
  return TransceiverBank_GetRemoteControlBank(TransceiverBank_GiveIndexCurrent())->RemoteControlPersistent.Address;
}


//人机管理模式 改变
void ErgonomicManager_ChangeErgoMode(ErgoMode_e NewMode)
{
  me.ErgoMode = NewMode;
  LOG_PRINT1("[LOG][Ergo] New State = %s\n", ErgoMode_TXT[me.ErgoMode]);

  switch(me.ErgoMode)
  {
    case ErgoMode_Delivery1Mode :
        LED_LogEvent(LED_Event_RadioOff); 
        RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Initial_DeliveryMode1);  // ok
        LongTimer_Stop(LongTimer_ErgoTimer);
        UART1ComDriver_Cmd(UARTComDriver_Disable,0);
       break;
    
    case ErgoMode_Delivery2Mode :
        LED_LogEvent(LED_Event_RadioOff); 
        RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Initial_DeliveryMode2); // ok
        LongTimer_Stop(LongTimer_ErgoTimer);
       break;
    
    case ErgoMode_Delivery3Mode :
        LED_LogEvent(LED_Event_RadioOff); 
        RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Initial_DeliveryMode3); // ok
        LongTimer_Stop(LongTimer_ErgoTimer);
       break; 
    
    case ErgoMode_IndustrialMode :
        Motor_Protect_Enable=false;      
        LED_LogEvent(LED_Event_Ergo_IndustrialMode);      
        LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_15MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
       // me.ErgoModeAfterTimeOut = ErgoMode_Delivery1Mode;//kendy 
         me.ErgoModeAfterTimeOut = ErgoMode_FactoryMode;
      break;
       
    case ErgoMode_FactoryMode :
        LED_LogEvent(LED_Event_Ergo_FactoryMode);          
        RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Initial_SetNull);//ok
        //LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_15MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
        //me.ErgoModeAfterTimeOut = ErgoMode_Delivery1Mode;//KENDY
          LongTimer_Stop(LongTimer_ErgoTimer);
        UART1ComDriver_Cmd(UARTComDriver_Enable_RxTxToIndus,UART_BAUD_RATE_INDUS);
      break;
        
     case ErgoMode_FirstPairingMode : //p ok
        LED_LogEvent(LED_Event_Ergo_FirstPairingMode);          
        LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_15MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
        me.ErgoModeAfterTimeOut = ErgoMode_FactoryMode;
        UART1ComDriver_Cmd(UARTComDriver_Disable,0);
      break;
        
     case ErgoMode_SettingPart1Mode ://p ok
        Motor_Protect_Enable=false;    
        LED_LogEvent(LED_Event_Ergo_SettingPart1Mode);          
        LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_15MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
        me.ErgoModeAfterTimeOut = ErgoMode_FactoryMode;
        UART1ComDriver_Cmd(UARTComDriver_Disable,0);
      break;
             
     case ErgoMode_SettingPart1Modebis ://p ok
        Motor_Protect_Enable=false;  
        LED_LogEvent(LED_Event_Ergo_SettingPart1Modebis);      
        LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_15MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
        me.ErgoModeAfterTimeOut = ErgoMode_FactoryMode;
      break;
              
     case ErgoMode_SetAndNotPaired :  //kendy???
        Motor_Protect_Enable=false;
        LED_LogEvent(LED_Event_PowerOn_SetAndNotPaired);//kendy????
        SQC_LogEvent(SQC_Event_ErgoEnterInSetAndNotPaired);//kendy  ????
        RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Initial_SetAndNotPaired);//ok
            LongTimer_Stop(LongTimer_ErgoTimer);//kendy
       // LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_15MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
       // me.ErgoModeAfterTimeOut = ErgoMode_Delivery2Mode;
      break;
     case ErgoMode_SetNoTemporaryRemote :  //kendy???
        Motor_Protect_Enable=false;
       // LED_LogEvent(LED_Event_PowerOn_SetAndNotPaired);//kendy????
       // SQC_LogEvent(SQC_Event_ErgoEnterInSetAndNotPaired);//kendy  ????
        SQC_LogEvent(SQC_Event_ErgoEnterInSetNoTemporaryRemote);//kendy  
        RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Initial_SetAndNotPaired);//ok
            LongTimer_Stop(LongTimer_ErgoTimer);//kendy
       // LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_15MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
       // me.ErgoModeAfterTimeOut = ErgoMode_Delivery2Mode;
      break;
      
     case ErgoMode_MaintenanceResetingMode:
        Motor_Protect_Enable=false;  
        LED_LogEvent(LED_Event_Ergo_MaintenanceResetingMode);        
        RememberRemoteUsedToEnterInResetingMode();
        LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_2MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
        me.ErgoModeAfterTimeOut = ErgoMode_UserMode;
      break;
            
      case ErgoMode_SetAndTemporaryRemote :
        Motor_Protect_Enable=false;
        SQC_LogEvent(SQC_Event_ErgoEnterInSetAndTemporaryPaired);        
        RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Initial_SetAndTemporary);//p ok//////////???????kendy
        //LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_15MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
         LongTimer_Stop(LongTimer_ErgoTimer);//kendy 
       // me.ErgoModeAfterTimeOut = ErgoMode_Delivery2Mode;
       break;
  
     case ErgoMode_UserMode :  
        SQC_LogEvent(SQC_Event_ErgoEnterInUserMode);
        Motor_Protect_Enable=true;
               //MovingJustOnce=false;//kendy
        RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Initial_Finish);//ok
  
        if(Curr_SpeedMode == SPEED_28RPM)                    
          Current_Speed = STANDARD_28_SPEED;   
        else if(Curr_SpeedMode == SPEED_26RPM)
          Current_Speed = MIDDLE_26_SPEED; 
        else if(Curr_SpeedMode == SPEED_24RPM)
          Current_Speed = MIDDLE_24_SPEED; 
        else if(Curr_SpeedMode == SPEED_22RPM)
          Current_Speed = MIDDLE_22_SPEED; 
        else if(Curr_SpeedMode == SPEED_20RPM)
          Current_Speed = MIDDLE_20_SPEED; 
        else if(Curr_SpeedMode == SPEED_18RPM)
          Current_Speed = MIDDLE_18_SPEED; 
        else if(Curr_SpeedMode == SPEED_16RPM)
          Current_Speed = MIDDLE_16_SPEED; 
        else if(Curr_SpeedMode == SPEED_14RPM)
          Current_Speed = MIDDLE_14_SPEED; 
        else if(Curr_SpeedMode == SPEED_12RPM)
          Current_Speed = MIDDLE_12_SPEED; 
        else{
          Curr_SpeedMode = SPEED_10RPM;
          Current_Speed = MININUM_10_SPEED; 
        }  
       Roller_Speed = Current_Speed;
       if(Current_Speed/2 > MININUM_10_SPEED)
          Tilting_Speed = Current_Speed/2;
       else 
          Tilting_Speed = MININUM_10_SPEED;  
       LongTimer_Stop(LongTimer_ErgoTimer);            
      break;
      
     case ErgoMode_ConfigMode :
      Motor_Protect_Enable=false;
      LED_LogEvent(LED_Event_Ergo_ConfigMode);          
      LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_2MIN ,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);

      me.ErgoModeAfterTimeOut = ErgoMode_UserMode;      
      break;
        
     case ErgoMode_SleepMode :
       Motor_Protect_Enable=false;
       LED_LogEvent(LED_Event_Ergo_ToSleepMode);   
       SleepWakeTime = 0u;
       LongTimer_Stop(LongTimer_ErgoTimer);
       break;
      
     case ErgoMode_ReSettingDEL :
     case ErgoMode_ReSettingUEL :
      Motor_Protect_Enable=false; 
      LED_LogEvent(LED_Event_ErgoEnterInLimitResetting); 
      SQC_LogEvent(SQC_Event_ErgoEnterInLimitResetting);
      RememberRemoteUsedToEnterInResetingMode();
      LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_2MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
      me.ErgoModeAfterTimeOut = ErgoMode_UserMode;
      break;
           case ErgoMode_ReSettingSDP :
      Motor_Protect_Enable=false; 
    //  LED_LogEvent(LED_Event_ErgoEnterInLimitResetting); 
 
      RememberRemoteUsedToEnterInResetingMode();
      LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_2MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
      me.ErgoModeAfterTimeOut = ErgoMode_UserMode;
      break;
      
    case ErgoMode_SpeedAdjustmentMode :
      Motor_Protect_Enable=false;     
      SpeedChange=true;             
      Speed_MoveCycle=true;         
      LED_LogEvent(LED_Event_Ergo_SpeedAdjustmentMode);      //事件更新
      RememberRemoteUsedToEnterInResetingMode();             
      LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_2MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
      me.ErgoModeAfterTimeOut = ErgoMode_UserMode;//超时模式为用户模式
      break;  
    case ErgoMode_Tiltingspeedadjustment :   //kendy?????
     Motor_Protect_Enable=false;     
     SpeedChange=true;             
      Speed_MoveCycle=true;  
         
       Current_Speed = Tilting_Speed;//kendy2????

     // LED_LogEvent(LED_Event_Ergo_SpeedAdjustmentMode);      //事件更新
      RememberRemoteUsedToEnterInResetingMode();             
      LongTimer_Setup(LongTimer_ErgoTimer,ERGO_RESETTING_TIMEOUT_2MIN,FALSE,TRUE,ErgonomicManager_ErgoTimeOutHandler);
      me.ErgoModeAfterTimeOut = ErgoMode_UserMode;//超时模式为用户模式
      break;  
            
    case ErgoMode_EMS_Sensitivity_On :
      LongTimer_Stop(LongTimer_ErgoTimer);
      break;
      
    case ErgoMode_EMS_Sensitivity_Off:             
      if(InitialSetDone == Initial_SetAndNotPaired)//kendy??/
      {
        me.ErgoMode = ErgoMode_SetAndNotPaired;//kendy??/
      }
      else if(InitialSetDone == Initial_SetAndTemporary)
      {
        me.ErgoMode = ErgoMode_SetAndTemporaryRemote;//???????kendy
      }
      else if(InitialSetDone == Initial_Set_Finish)
      {
        me.ErgoMode = ErgoMode_UserMode;
      }
else if (InitialSetDone ==Initial_DeliveryMode1)
      {
        me.ErgoMode = ErgoMode_Delivery1Mode; 
      }else if (InitialSetDone ==Initial_DeliveryMode2)
      {
        me.ErgoMode = ErgoMode_Delivery2Mode; 
      }else if (InitialSetDone ==Initial_DeliveryMode3)
      {
        me.ErgoMode = ErgoMode_Delivery3Mode; 
      }else if (InitialSetDone ==Initial_Set_Null )
      {
        me.ErgoMode = ErgoMode_FactoryMode; 
      }
      else 
      {
        me.ErgoMode = ErgoMode_FactoryMode; 
      }
               
     Limits_SetMode(LimitMode_Normal);
      break;
      
    default :
      trap();
      break;
  }
  RTSErgonomicInterface_NotifyErgoChangeMode(me.ErgoMode);
}



//哪些事件允许时钟重置
static void RearmTimeout(ErgoEvent_t *Evt)
{
  switch(me.ErgoMode)
  {  
    case ErgoMode_IndustrialMode :
    case ErgoMode_FactoryMode :
    case ErgoMode_ConfigMode  :       
    case ErgoMode_FirstPairingMode : 
   // case ErgoMode_SetNoTemporaryRemote: //kendy ???????
    case ErgoMode_SetAndNotPaired  : //没有遥控器信息
     break;
     
    case ErgoMode_ReSettingDEL :         
    case ErgoMode_ReSettingUEL : 
    case ErgoMode_ReSettingSDP :
    case ErgoMode_MaintenanceResetingMode:
    case ErgoMode_SpeedAdjustmentMode:
    case ErgoMode_Tiltingspeedadjustment:   //kendy?     
         Ledtime_outsecond=LED_2MIN;
      if(IsCurrentRemoteTheResetingRemote() == TRUE)
        {
          LongTimer_Reload(LongTimer_ErgoTimer);   
        }
      break;
      
    case ErgoMode_SettingPart1Modebis :  
    case ErgoMode_SettingPart1Mode :     
    case ErgoMode_SetAndTemporaryRemote :
         Ledtime_outsecond=LED_15MIN;
      if(IsCurrentRemoteTheResetingRemote() == TRUE)
        {
          LongTimer_Reload(LongTimer_ErgoTimer);   
        }
      break;
      
    default :
      break;
  }
}


static void ErgonomicManager_HandleEvent(ErgoEvent_t *Evt)
{
  TBool AcceptedRemote      =  me.AcceptedRemoteArray[Evt->RemoteType];
  TBool AcceptedU56Frame    =  me.Accepted_U56_CMDArray[Evt->Type];
  TBool AcceptedU80Function = (AcceptedU56Frame == TRUE && me.Accepted_U80_CMDArray[Evt->U80Function] == TRUE) ? (TRUE) : (FALSE);
//  AcceptedU80Function_flag=AcceptedU80Function;//kendy
//  AcceptedRemote_flag=AcceptedRemote;//kendy
//  AcceptedU56Frame_value=Evt->Type;
//  AcceptedU56Frame_flag=AcceptedU56Frame;//kendy
  if(AcceptedRemote == TRUE && AcceptedU56Frame == TRUE && AcceptedU80Function == TRUE)
  {
    RearmTimeout(Evt);   
    DEBUG_PRINT("\n");
    switch(me.ErgoMode)
    {  
        case ErgoMode_Delivery1Mode :        
          DEBUG_PRINT("[DBG] Delivery1Mode\n");
          ErgonomicManager_Delivery1Mode_EventHandler(Evt);
          break;
          
        case ErgoMode_Delivery2Mode :        
          DEBUG_PRINT("[DBG] Delivery2Mode\n");
          ErgonomicManager_Delivery2Mode_EventHandler(Evt);
          break;
          
        case ErgoMode_Delivery3Mode :          
          DEBUG_PRINT("[DBG] Delivery3Mode\n");
          ErgonomicManager_Delivery3Mode_EventHandler(Evt);
          break;
          
        case ErgoMode_FactoryMode :         
          DEBUG_PRINT("[DBG] FactoryMode\n");
          ErgonomicManager_FactoryMode_EventHandler(Evt);
          break;
          
        case ErgoMode_IndustrialMode :         
          DEBUG_PRINT("[DBG] IndustrialMode\n");
          ErgonomicManager_IndustrialMode_EventHandler(Evt);
          break;
          
        case ErgoMode_FirstPairingMode :     
          DEBUG_PRINT("[DBG] FirstPairingMode\n");
          ErgonomicManager_FirstPairingMode_EventHandler(Evt);
          break;
          
        case ErgoMode_SettingPart1Mode :      
          DEBUG_PRINT("[DBG] SettingPart1Mode\n");
          ErgonomicManager_SettingPart1Mode_EventHandler(Evt);
          break;
        
        case ErgoMode_SettingPart1Modebis :      
          DEBUG_PRINT("[DBG] SettingPart1Modebis\n");
          ErgonomicManager_SettingPart1Modebis_EventHandler(Evt);
          break;
             
        case ErgoMode_SetAndNotPaired :           
          DEBUG_PRINT("[DBG] SetAndNotPaired\n");
          ErgonomicManager_SetAndNotPaired_EventHandler(Evt);
          break;
        case ErgoMode_SetNoTemporaryRemote :           
          DEBUG_PRINT("[DBG] SetNoTemporaryRemote\n");
          ErgonomicManager_SetNoTemporaryRemote_EventHandler(Evt);
          break;
          
        case ErgoMode_SleepMode :      
          DEBUG_PRINT("[DBG] ErgoMode_SleepMode\n");
          ErgonomicManager_SleepMode_EventHandler(Evt);
          break;
                
        case ErgoMode_SetAndTemporaryRemote :      
          DEBUG_PRINT("[DBG] SetAndTemporaryRemote\n");
          ErgonomicManager_SetAndTemporaryPairedMode_EventHandler(Evt);
          break;
          
        case ErgoMode_MaintenanceResetingMode :     
          DEBUG_PRINT("[DBG] MaintenanceResetingMode\n");
          ErgonomicManager_MaintenanceResetingMode_EventHandler(Evt);
          break;
        
        case ErgoMode_ConfigMode :        
          DEBUG_PRINT("[DBG] ConfigMode\n");
          ErgonomicManager_ConfigMode_EventHandler(Evt);
          break;  
          
        case ErgoMode_SpeedAdjustmentMode :       
          DEBUG_PRINT("[DBG] SpeedAdjustmentMode\n");
          ErgonomicManager_SpeedAdjustmentMode_EventHandler(Evt);
          break; 
        case ErgoMode_Tiltingspeedadjustment :       //kendy????????
          DEBUG_PRINT("[DBG] TiltingspeedadjustmentMode\n");
          ErgonomicManager_TiltingspeedadjustmentMode_EventHandler(Evt);
          break; 
          
        case ErgoMode_UserMode :
          if(User_Mode == User_ModeA_Us_Tilit)
          {
             ErgonomicManager_UserModeA_EventHandler(Evt);  
             DEBUG_PRINT("[DBG] UserModeA_Us_Tilit\n");
          }else
           {
             User_Mode = User_ModeB_Roller;
             ErgonomicManager_UserModeB_EventHandler(Evt);
             DEBUG_PRINT("[DBG] UserModeB_Roller\n");
           }
          break;
          
        case ErgoMode_ReSettingDEL :
          DEBUG_PRINT("[DBG] ReSet DEL\n");  
          ErgonomicManager_ErgoMode_ReSettingDEL_EventHandler(Evt);
          break;
        case ErgoMode_ReSettingSDP :
          DEBUG_PRINT("[DBG] ReSet SDP\n");  
          ErgonomicManager_ErgoMode_ReSettingSDP_EventHandler(Evt);
          break; 
        case ErgoMode_ReSettingUEL :
          DEBUG_PRINT("[DBG] ReSet UEL\n");
          ErgonomicManager_ErgoMode_ReSettingUEL_EventHandler(Evt);
          break;         
          
        case ErgoMode_EMS_Sensitivity_On:
          DEBUG_PRINT("[DBG] EMS_Sensitivity_On\n");
          if(Evt->BpState == ErgoRemote_BpPress && Evt->Type == ErgoIndusRemoteEventType_Autotest1)
          {
            RTSErgonomicInterface_SendSpecialRequest(RTSErgo_EMS_SensitivityFrameReceived);
          }
          break;
          
        case ErgoMode_EMS_Sensitivity_Off:
          DEBUG_PRINT("[DBG] EMS_Sensitivity_Off\n");
          break;
          
        default :
          break;
    }
  }
  else
  {
    //nop
    DEBUG_PRINT("  => Rejected CMD \n"); 
  }
}


ErgoMode_e ErgonomicManager_GetCurrentMode()
{
  return me.ErgoMode;
}



#define RelayBitMask    0x800000
#define KeyStateMask    0x400000
#define FrameNbMask     0x3C0000
#define FunctionNbMask (0xF000)
#define ParameterMask   0xFF0
#define CHKMask         0xF
#define DataMask        0x3F



static void UpdateU80Field(ErgoEvent_t *ptRemoteEvent,TU32 *U80_Field)
{
  if(*U80_Field != 0u)
  {
    ptRemoteEvent->U80Function = (U80_Function_e) ((*U80_Field & FunctionNbMask) >> 12u);
    
    if(ptRemoteEvent->U80Function == U80_Transmit_12bits_data)
    {
      ptRemoteEvent->U80_OptionValue = (*U80_Field & DataMask);
    }else
    {
      ptRemoteEvent->U80_OptionValue = ((*U80_Field & ParameterMask) >> 4u);
    }
  }else
  {
    ptRemoteEvent->U80Function = U80_U56;
    ptRemoteEvent->U80_OptionValue = 0u;
  }
}



static Press_Duration_e GetStimulusDuration(ErgoEvent_t *Evt,TStimuli *ptStimuli)
{
  TU8 PressDuration = 0u;
  PressDuration = ptStimuli->btStimuliDuration;
  //Parse duration Table to get correct Press_Duration
  TU8 i = 0U;
  TU8 TableValue;
  do
  {
    TableValue = Press_DurationValue[i++];
  }while(PressDuration > TableValue && i < 12u);
  
  return (Press_Duration_e) --i;
}




static void UpdateDuration(ErgoEvent_t *ptRemoteEvent, ErgoEvent_t *ptLastEvent,TStimuli *ptStimuli)
{
  if(ptRemoteEvent->BpState == ErgoRemote_BpPress)
  {
    ptRemoteEvent->PressDuration = GetStimulusDuration(ptRemoteEvent,ptStimuli);
  }
  else
  {
    ptRemoteEvent->PressDuration = ptLastEvent->PressDuration;
  }
}

#define U80_Mask_bit_1_4            (0x00F00000u)
#define Shift_Bit_1_4                (20u)
#define U80_Mask_bit_5_8            (0x000F0000u)
#define Shift_Bit_5_8                (16u)
#define U80_Mask_bit_9_12           (0x0000F000u)
#define Shift_Bit_9_12               (12u)
#define U80_Mask_bit_13_16          (0x00000F00u)
#define Shift_Bit_13_16              (8u)
#define U80_Mask_bit_17_20          (0x000000F0u)
#define Shift_Bit_17_20              (4u)
#define U80_Mask_bit_21_24          (0x0000000Fu)
#define Shift_Bit_21_24              (0u)

static TBool CheckU80Checksum(const TU32* U80_Field)
{
  TU8 cksum = 0u,bit_21_24 = 0u, bit_1_4 = 0u,bit_5_8=0u,bit_9_12 = 0u,bit_13_16 = 0u, bit_17_20 = 0u;

  //Collect nibble
  bit_1_4 =  (((*U80_Field) & U80_Mask_bit_1_4) >> Shift_Bit_1_4);
  bit_5_8 =  (((*U80_Field) & U80_Mask_bit_5_8) >> Shift_Bit_5_8);
  bit_9_12 = (((*U80_Field) & U80_Mask_bit_9_12) >> Shift_Bit_9_12);
  bit_13_16 =(((*U80_Field) & U80_Mask_bit_13_16) >> Shift_Bit_13_16);
  bit_17_20 =(((*U80_Field) & U80_Mask_bit_17_20) >> Shift_Bit_17_20);
  bit_21_24 =(*U80_Field) & U80_Mask_bit_21_24;// >> Shift_Bit_21_24) RIP logic
  
  //compute checksum
  cksum = bit_1_4 ^ bit_5_8 ^ bit_9_12 ^ bit_13_16 ^ bit_17_20;
  //compare computed checksum and received checksum
  return cksum == bit_21_24;
}

//遥控器 激励
void ErgonomicManager_TranslateStimulus(TStimuli *Stimuli)
{
  static ErgoEvent_t LastRemoteEvent;
  ErgoEvent_t RemoteEvent;
  Controller_RTSActivityNotify();
  // bit    1         2       3-6          7-12          13-20       21-24
  // 0x00 [relay][keyState][framenumber][functionnumber][Parameter][CKSUM]
  TU32 U80_Field =  ( ((TU32) Stimuli->btCommand2) << 16u) | (((TU32) Stimuli->btCommand3) << 8u) | ((TU32) Stimuli->btCommand4);
  if(U80_Field != 0u && CheckU80Checksum(&U80_Field) == FALSE)
  {
    //If the U80 checksum is bad, discard U80 frame part.
    U80_Field = 0u;
  }
  
  RemoteEvent.Type = ErgoRemoteEventUnkownCommand;
  RemoteEvent.U80Function = U80_U56;
  RemoteEvent.U80_OptionValue = 0u;
  RemoteEvent.RemoteAdress = GetCurrentRemoteAdress();
  
  TBool Interuptedflow = IsItFlowInteruptedStimulus(Stimuli);
  if(Interuptedflow == FALSE)
  {
    RemoteEvent.RemoteType = Stimuli->btTypeOfTransceiver;
    RemoteEvent.BpState = ((Stimuli->btStateOfCommand & MASK_FLOW_IN_PROGRESS )== 0u) ? (ErgoRemote_BpRelease) : (ErgoRemote_BpPress) ;
    me.RTSFlow = (RemoteEvent.BpState == ErgoRemote_BpPress);
    
    RemoteEvent.PairedState = Stimuli->btStateOfTransceiver;
    
    //Set LowPower Flag
    if(RemoteEvent.PairedState == RTSPROT_UNRECORDED)
    {
      RtsLowPowerManager_Set_UnpairedRTS_Flag();
      SQC_Count(&me.SQC.Counter.UnPairedRTSFrame,sizeof(me.SQC.Counter.UnPairedRTSFrame));
    }
    else if(RemoteEvent.PairedState == RTSPROT_RECORDED)
    {
      RtsLowPowerManager_Set_PairedRTS_Flag();
    }
    
    //According to remote type, get Event type (U56 + U80 if any)
    if(RemoteEvent.RemoteType == RTSPROT_RC_1BUTTON&& Stimuli->btCommand1 <= 0x0Fu)
    {  //1键 遥控器
      RemoteEvent.Type = SOMFY_Extended_FunctionArray[Stimuli->btCommand1];
      UpdateU80Field(&RemoteEvent,&U80_Field);     
    }
    else if(RemoteEvent.RemoteType  == RTSPROT_RC_3BUTTON&& Stimuli->btCommand1 <= 0x0Fu)
    {  //3键 遥控器
      RemoteEvent.Type = StandarRTS_FunctionArray[Stimuli->btCommand1];
      UpdateU80Field(&RemoteEvent,&U80_Field);     
    }
    else if((RemoteEvent.RemoteType  == RTSPROT_FACTORY_CRYPTED || RemoteEvent.RemoteType  == RTSPROT_FACTORY_UNCRYPTED)&& Stimuli->btCommand1 <= 0x0Fu)
    { //工厂遥控器 
      RemoteEvent.Type = IndusCmdArray[Stimuli->btCommand1];
      //Indus frame are all U56 => call to clear/set field properly
      UpdateU80Field(&RemoteEvent,&U80_Field);
      //Soft version frame is the only frame that contain "data"
      //It's quite dirty to store value in U80_option value, but ... 
      if(RemoteEvent.Type == ErgoIndusRemoteEventType_SoftVersion)
      {
        RemoteEvent.U80_OptionValue = Stimuli->btCommand3;
      }
    }
    else if( RemoteEvent.RemoteType  == RTSPROT_SENSOR && Stimuli->btCommand1 <= 0x0Fu)//kendyRTS
    {//RTS 传感器
      RemoteEvent.Type = SENSOR_U56_FunctionArray[Stimuli->btCommand1];
      UpdateU80Field(&RemoteEvent,&U80_Field);    
    }
//    else if(RemoteEvent.RemoteType  == RTSPROT_RC_RTDS && Stimuli->btCommand1 <= 0x0Fu)
//    {//RTD 遥控器
//      RemoteEvent.Type = RTDS_FunctionArray[Stimuli->btCommand1];
//      UpdateU80Field(&RemoteEvent,&U80_Field);
//    }
    else
    {
      RemoteEvent.RemoteType = RTSPROT_RC_UNKNOWN;
      RemoteEvent.Type = ErgoRemoteEventUnkownCommand;
      RemoteEvent.U80Function = U80_U56;
      RemoteEvent.U80_OptionValue = 0U;
    }
    
    UpdateDuration(&RemoteEvent, &LastRemoteEvent,Stimuli);
    LastRemoteEvent = RemoteEvent;
    
//    DEBUG_PRINT3("[DBG][Ergo] @ 0x%x%x%x ", RemoteEvent.RemoteAdress.btAddress[2],RemoteEvent.RemoteAdress.btAddress[1],RemoteEvent.RemoteAdress.btAddress[0]);
//    DEBUG_PRINT7("%s %s \t %s %s(%d) \t %s %s",
//                 RemoteType_TXT[RemoteEvent.RemoteType],
//                 RemotePairingState_TXT[RemoteEvent.PairedState],
//                 ErgoRemoteEventType_TXT[RemoteEvent.Type],
//                 U80_FunctionTXT[RemoteEvent.U80Function],
//                 RemoteEvent.U80_OptionValue,
//                 RemoteBP_State_TXT[RemoteEvent.BpState],
//                 Press_Duration_TXT[RemoteEvent.PressDuration]
//                   );
        
    ErgonomicManager_HandleEvent(&RemoteEvent);  //接收到无线信号  选择模式进入
  }
}

static TBool IsItFlowInteruptedStimulus(TStimuli * ptStimuli)
{
  return ( ((ptStimuli->btStateOfCommand & MASK_FLOW_IN_PROGRESS )== 0u)  && (ptStimuli->btStimuliDuration == 1u) ) ? (TRUE) : (FALSE);
}





static void ErgonomicManager_UserModeA_EventHandler(ErgoEvent_t *evt)
{
  MoveRequest_s Mvrq;  

  switch(me.ErgoModeState)
  {
  case ErgoModeState_WaitEvent :
    if(Tilting_Move == false)
    {
       Current_Speed = Roller_Speed;
    }
    

    if(evt->PairedState == RTSPROT_RECORDED)
    {      
        if((User_LedMode == PowerLevelLow)&&evt->PressDuration <BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)  
        {
           LED_LogEvent(LED_Event_UserLowPower);
        }else if((User_LedMode != PowerLevelLow)&& evt->PressDuration <BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
        {  
           LED_LogEvent(LED_Event_UserRemotecontral);
        }
   
      //U56 remote command
      //UP “up” command
      //else if((evt->Type == ErgoRemoteEventType_Up)&&(EncoderDriver_GetMoveDirection() != EncoderDriver_Roll))
      if(evt->Type == ErgoRemoteEventType_Up)
      {
          if(evt->PressDuration <= BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpRelease)
          {
              SQC_Count(&me.SQC.Counter.RTS_Up,sizeof(me.SQC.Counter.RTS_Up));
              
              Mvrq.MoveType = MoveRequestType_RollToPosition;
              Mvrq.MoveRequestParam.Position = PositionName_UEL;
              MoveRequest_Post(&Mvrq);
          }else if(evt->PressDuration > BUTTON_PUSH_500ms)
           {    
              if(evt->BpState == ErgoRemote_BpPress)
              {
                  Tilting_Move=true;
                  Current_Speed =Tilting_Speed;              
                  SQC_Count(&me.SQC.Counter.RTS_Up,sizeof(me.SQC.Counter.RTS_Up));
                  
                  Mvrq.MoveType = MoveRequestType_RollToPosition;
                  Mvrq.MoveRequestParam.Position = PositionName_UEL;
                  MoveRequest_Post(&Mvrq);  
              }else if(evt->BpState == ErgoRemote_BpRelease )
              {
                SQC_Count(&me.SQC.Counter.RTS_ModeStop,sizeof(me.SQC.Counter.RTS_ModeStop));
                Tilting_Move=false;
                
                Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                Mvrq.MoveRequestParam = NoMoveParam;
                MoveRequest_Post(&Mvrq);  
              }   
           }    
      }
      //DOWN “down” command
     // else if((evt->Type == ErgoRemoteEventType_Down)&&(EncoderDriver_GetMoveDirection() != EncoderDriver_UnRoll))
      else if((evt->Type == ErgoRemoteEventType_Down))
      {
          if(evt->PressDuration <= BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpRelease)
          {
              SQC_Count(&me.SQC.Counter.RTS_Down,sizeof(me.SQC.Counter.RTS_Down));
              
              Mvrq.MoveType = MoveRequestType_UnRollToPosition;
              Mvrq.MoveRequestParam.Position = PositionName_DEL;
              MoveRequest_Post(&Mvrq);
          }else if(evt->PressDuration > BUTTON_PUSH_500ms)
           {
              if(evt->BpState == ErgoRemote_BpPress)
              {   
                  Tilting_Move=true;
                  Current_Speed =Tilting_Speed;
                  SQC_Count(&me.SQC.Counter.RTS_Down,sizeof(me.SQC.Counter.RTS_Down));
                  
                  Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                  Mvrq.MoveRequestParam.Position = PositionName_DEL;
                  MoveRequest_Post(&Mvrq);        
              }else if(evt->BpState == ErgoRemote_BpRelease)
              {
                SQC_Count(&me.SQC.Counter.RTS_ModeStop,sizeof(me.SQC.Counter.RTS_ModeStop));
                Tilting_Move=false;
                
                Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                Mvrq.MoveRequestParam = NoMoveParam;
                MoveRequest_Post(&Mvrq);  
              }    
           }  
      }
      //“ModeUp” command
      else if(evt->Type == ErgoRemoteEventType_ModeUp && evt->BpState == ErgoRemote_BpPress)
      {
        if(EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll)
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeStop,sizeof(me.SQC.Counter.RTS_ModeStop));
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }else if(EncoderDriver_GetMoveDirection() == EncoderDriver_Roll)
        {
          //DEBUGPRINT("Currently Rolling => Do Nothing\n")
        }
        else
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeUp,sizeof(me.SQC.Counter.RTS_ModeUp));   
          
          Mvrq.MoveType = MoveRequestType_RollToPosition;
          Mvrq.MoveRequestParam.Position = PositionName_UEL;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }
      }
      //“ModeDown” command
      else if(evt->Type == ErgoRemoteEventType_ModeDown && evt->BpState == ErgoRemote_BpPress)
      {
        if(EncoderDriver_GetMoveDirection() == EncoderDriver_Roll)
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeStop,sizeof(me.SQC.Counter.RTS_ModeStop));
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }else if(EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll)
        {
          //DEBUGPRINT("Currently UnRolling => Do Nothing\n")
        }else
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeDown,sizeof(me.SQC.Counter.RTS_ModeDown));  
          Mvrq.MoveType = MoveRequestType_UnRollToPosition;
          Mvrq.MoveRequestParam.Position = PositionName_DEL;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }
      }
      //“STOP” command
      else if(evt->Type == ErgoRemoteEventType_Stop)
      {
        if(evt->BpState == ErgoRemote_BpPress && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
        {//the motor is moving,STOP it 
          SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }else if(evt->PressDuration <= BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpRelease && EncoderDriver_GetMoveDirection() == EncoderDriver_Stop)
        { 
          if(Position_IsPositionSet(PositionName_My) == TRUE)
          {
            TU32 CurrentPosition = EncoderDriver_GetPosition();  //获取当前位置
            TU32 MYPosition=Position_MyPlace();
            
            if(CurrentPosition <= (MYPosition+3u) && CurrentPosition >=(MYPosition-3u))
            {
              Mvrq.MoveType = MoveRequestType_None ;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }else
            {
              Mvrq.MoveType = MoveRequestType_TiltMyPosition ;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }
          }
        }else if(evt->PressDuration >= BUTTON_PUSH_5SEC && EncoderDriver_GetMoveDirection() == EncoderDriver_Stop)
        { 
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          SQC_Count(&me.SQC.Counter.My_Reseting,sizeof(me.SQC.Counter.My_Reseting));
          
          Limits_State_e Area = Limits_GetLimitsState();
          TU32 CurrentPos = EncoderDriver_GetPosition();

          if (Area == Limits_State_Between)
          {
            PositionCompare_e Pc = Position_Compare(PositionName_My,&CurrentPos,POSITION_TOLERANCE);
            if(Pc == CurrentPositionIsOnRef)
            {
              LED_LogEvent(LED_Event_ResetMY);
              Position_Disable(PositionName_My);
              
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }else
            {
              LED_LogEvent(LED_Event_ResetMY);
              Position_Set(PositionName_My,CurrentPos);
              
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }
          }
        }
      }
      //“UP&DOWN” command
      else if(evt->Type == ErgoRemoteEventType_Up_Down && evt->BpState == ErgoRemote_BpPress)
      { 
        if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }else if(evt->PressDuration >= BUTTON_PUSH_5SEC)
        {  
          TU32 CurrentPos = EncoderDriver_GetPosition();
          PositionCompare_e DEL = Position_Compare(PositionName_DEL,&CurrentPos,(POSITION_TOLERANCE+3));    //tolerance 5' in total 
          PositionCompare_e UEL = Position_Compare(PositionName_UEL,&CurrentPos,(POSITION_TOLERANCE+3));  
          
          if(DEL == CurrentPositionIsOnRef || DEL == CurrentPositionIsBelowRef)
          {          
            me.ErgoModeState = ErgoModeState_WaitRelease;
            me.ChangeStateOnBpRelease = TRUE;
            me.ErgoModeAfterRelease = ErgoMode_ReSettingDEL;
            
            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
          }else if (UEL == CurrentPositionIsOnRef || UEL == CurrentPositionIsAboveRef)
          {
            me.ErgoModeState = ErgoModeState_WaitRelease;
            me.ChangeStateOnBpRelease = TRUE;
            me.ErgoModeAfterRelease = ErgoMode_ReSettingUEL;
            
            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
          }else
          {        
            me.ErgoModeState = ErgoModeState_WaitRelease;
            me.ChangeStateOnBpRelease = TRUE;
            me.ErgoModeAfterRelease = ErgoMode_MaintenanceResetingMode;
            
            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
          }
        }
      }
            //“STOP&DOWN” command
      else if(evt->Type == ErgoRemoteEventType_Stop_Down && evt->BpState == ErgoRemote_BpPress)
      { 
        if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }else if(evt->PressDuration >= BUTTON_PUSH_2SEC)
        {  
            me.ErgoModeState = ErgoModeState_WaitRelease;
            me.ChangeStateOnBpRelease = TRUE;
            me.ErgoModeAfterRelease = ErgoMode_ReSettingSDP;
            
            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);

        }
      }
      //“PROG” command
      else if(evt->Type == ErgoRemoteEventType_Prog)
      {    
         if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }else if(evt->PressDuration >= BUTTON_PUSH_2SEC && evt->BpState == ErgoRemote_BpPress)
        {
          ConfigMode=true;
          
          RememberRemoteUsedToEnterInResetingMode();         //记录遥控器地址 有两种方式进入 ConfigMode 所以放在此处
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
          me.ChangeStateOnBpRelease = TRUE;
          me.ErgoModeAfterRelease = ErgoMode_ConfigMode;
            
          Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }
      }  
      //“Stop_Up” command
      else if(evt->Type == ErgoRemoteEventType_Stop_Up && evt->BpState == ErgoRemote_BpPress)
      {
        if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }
//        else if(evt->PressDuration >= BUTTON_PUSH_2SEC)
//        {
//          me.ErgoModeState = ErgoModeState_WaitRelease;
//          if(me.Pdata.Tilt_Size < MAX_TILT_SIZE)
//          {
//            LED_LogEvent(LED_Event_TiltSpeedINC);
//            
//            me.Pdata.Tilt_Size = ( (me.Pdata.Tilt_Size + TILT_SIZE_CHANGE_INCREMENT) < MAX_TILT_SIZE) ? (me.Pdata.Tilt_Size + TILT_SIZE_CHANGE_INCREMENT) :(MAX_TILT_SIZE);
//            
//            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
//            Mvrq.MoveRequestParam = NoMoveParam;
//            MoveRequest_Post(&Mvrq);
//          }else
//          {
//            LED_LogEvent(LED_Event_SetImpossibleUserMode);
//            
//            Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
//            Mvrq.MoveRequestParam = NoMoveParam;
//            MoveRequest_Post(&Mvrq); 
//          }
//        }
      }
      //“Stop_Down” command
      else if(evt->Type == ErgoRemoteEventType_Stop_Down && evt->BpState == ErgoRemote_BpPress)
      {
        if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }
//        else if(evt->PressDuration >= BUTTON_PUSH_2SEC)
//        {
//          me.ErgoModeState = ErgoModeState_WaitRelease;
//          if(me.Pdata.Tilt_Size > MIN_TILT_SIZE)
//          {
//            LED_LogEvent(LED_Event_TiltSpeedDEC);
//            
//            me.Pdata.Tilt_Size = (me.Pdata.Tilt_Size > (TILT_SIZE_CHANGE_INCREMENT + MIN_TILT_SIZE) ) ? (me.Pdata.Tilt_Size - TILT_SIZE_CHANGE_INCREMENT) :(MIN_TILT_SIZE);
//            
//            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
//            Mvrq.MoveRequestParam = NoMoveParam;
//            MoveRequest_Post(&Mvrq);
//          }else
//          {
//            LED_LogEvent(LED_Event_SetImpossibleUserMode);
//            
//            Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
//            Mvrq.MoveRequestParam = NoMoveParam;
//            MoveRequest_Post(&Mvrq); 
//          }
//        }
      }     
      //“UP_Stop_Down” command
      else if(evt->Type == ErgoRemoteEventType_Stop_Up_Down && evt->BpState == ErgoRemote_BpPress)
      {


        if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop )
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }

        else  if(evt->PressDuration >= BUTTON_PUSH_2SEC)
          {
                MovingJustOnce=true;
              me.ErgoModeState = ErgoModeState_WaitRelease;
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq); 

          }
  

      }
       //Sun_On  command
      else if(evt->Type == ErgoRemoteEventType_Sun_On && evt->BpState == ErgoRemote_BpPress)
      {



              me.ErgoModeState = ErgoModeState_WaitRelease;
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq); 

          
  

      }
                  //Sun_OFF  command
      else if(evt->Type == ErgoRemoteEventType_Sun_Off && evt->BpState == ErgoRemote_BpPress)
      {



              me.ErgoModeState = ErgoModeState_WaitRelease;
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq); 

          
  

      }
      //U80 remote command
      else if(evt->U80Function != U80_U56 && (evt->Type == ErgoRemoteEventType_Execute_U80 || evt->U80Function != U80_FrameCounter))
      {
        if(evt->U80Function == U80_Move_Of && evt->BpState == ErgoRemote_BpPress) //“move Of” command
        {
            me.ErgoModeState = ErgoModeState_WaitRelease;
            TS8 MoveOff =  (evt->U80_OptionValue > 128u) ? (-(evt->U80_OptionValue-128u)):(evt->U80_OptionValue );
            MoveOff *= me.Pdata.WheelDir;
            
            if(MoveOff < 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Stop || EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll))
            {
              Mvrq.MoveType = MoveRequestType_UnRolloff;
              Mvrq.MoveRequestParam.MoveOff = (abs(MoveOff)  * me.Pdata.Tilt_Size);
              MoveRequest_Post(&Mvrq);
            }else if(MoveOff < 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Roll))
            {
              Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }else if(MoveOff > 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll))
            {
              Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }else if(MoveOff > 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Stop || EncoderDriver_GetMoveDirection() == EncoderDriver_Roll))
            {
              Mvrq.MoveType = MoveRequestType_Rolloff;
              Mvrq.MoveRequestParam.MoveOff = (abs(MoveOff)  * me.Pdata.Tilt_Size);
              MoveRequest_Post(&Mvrq);
            }
        }
        else if(evt->U80Function == U80_Go_To)   //“go to” command
        {
            me.ErgoModeState = ErgoModeState_WaitRelease;
            Mvrq.MoveRequestParam.GOTO = evt->U80_OptionValue;
            if(Mvrq.MoveRequestParam.GOTO <= 200u)
            {
                if((Mvrq.MoveRequestParam.GOTO == 0u)&&(EncoderDriver_GetMoveDirection() != EncoderDriver_Roll))
                {
                    if(EncoderDriver_GetMoveDirection() != EncoderDriver_Roll)
                    {
                      SQC_Count(&me.SQC.Counter.RTS_Up,sizeof(me.SQC.Counter.RTS_Up));
                      Mvrq.MoveType = MoveRequestType_RollToPosition;
                      Mvrq.MoveRequestParam.Position = PositionName_UEL;
                      MoveRequest_Post(&Mvrq);
                    }  
                }else if((Mvrq.MoveRequestParam.GOTO == 200u)&&(EncoderDriver_GetMoveDirection() != EncoderDriver_UnRoll))
                {
                    if(EncoderDriver_GetMoveDirection() != EncoderDriver_UnRoll)
                    {
                      SQC_Count(&me.SQC.Counter.RTS_Up,sizeof(me.SQC.Counter.RTS_Down));
                      Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                      Mvrq.MoveRequestParam.Position = PositionName_DEL;
                      MoveRequest_Post(&Mvrq);
                    }
                }else
                {
                    SQC_Count(&me.SQC.Counter.RTS_Goto,sizeof(me.SQC.Counter.RTS_Goto));
                    Mvrq.MoveType = MoveRequestType_GoToPercent;
                    MoveRequest_Post(&Mvrq);
                }
            }else if(Mvrq.MoveRequestParam.GOTO == 201u && Position_IsPositionSet(PositionName_My) == TRUE)
            {
              SQC_Count(&me.SQC.Counter.RTS_My,sizeof(me.SQC.Counter.RTS_My));
              Mvrq.MoveType = MoveRequestType_GoToPosition;
              Mvrq.MoveRequestParam.Position = PositionName_My;
              MoveRequest_Post(&Mvrq);
            }else if(Mvrq.MoveRequestParam.GOTO == 255u)
            {
              SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));
              Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }
          } 
       }
    }
    break;
    
    
  case ErgoModeState_WaitRelease :
//    if(evt->BpState == ErgoRemote_BpRelease)
//    {
//      me.ErgoModeState = ErgoModeState_WaitEvent;
//      if(me.ChangeStateOnBpRelease == TRUE)
//      {
//        me.ChangeStateOnBpRelease = FALSE;
//        ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
//      }  
//    }
    
    ////////////  
    
    if(evt->BpState == ErgoRemote_BpRelease)
    {
      me.ErgoModeState = ErgoModeState_WaitEvent;
     

              
          if((evt->PressDuration < BUTTON_PUSH_5SEC)
             &&MovingJustOnce==true&&(evt->Type == ErgoRemoteEventType_Stop_Up_Down))
          {
                 MovingJustOnce=false;   
            me.ChangeStateOnBpRelease = TRUE;
            me.ErgoModeAfterRelease = ErgoMode_SpeedAdjustmentMode;         
            
            Pre_SpeedMode=Curr_SpeedMode; 
          }
            
   

           if(me.ChangeStateOnBpRelease == TRUE)
      {
        me.ChangeStateOnBpRelease = FALSE;
        ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
      } 
    }
    else
    {
      if((evt->PressDuration >= BUTTON_PUSH_5SEC)
         &&MovingJustOnce==true&&(evt->Type == ErgoRemoteEventType_Stop_Up_Down))
      {
        
        me.ErgoModeState = ErgoModeState_WaitRelease;
        MovingJustOnce=false;
        me.ChangeStateOnBpRelease = TRUE;
        me.ErgoModeAfterRelease = ErgoMode_Tiltingspeedadjustment; 
        Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
        Mvrq.MoveRequestParam = NoMoveParam;
        MoveRequest_Post(&Mvrq);
        
      }
      
    
    }
    break;
    
  default:
    break;
  }
}


static void ErgonomicManager_UserModeB_EventHandler(ErgoEvent_t *evt)
{
  MoveRequest_s Mvrq;  

  switch(me.ErgoModeState)
  {
  case ErgoModeState_WaitEvent :
//    if(evt->Type == ErgoIndusRemoteEventType_Stop && evt->BpState == ErgoRemote_BpPress && evt->PressDuration >= BUTTON_PUSH_2SEC)
//    { 
//          me.ErgoModeState = ErgoModeState_WaitRelease;
//      
//          UART1ComDriver_Cmd(UARTComDriver_Enable_TxSQC,UART_BAUD_RATE_SQC);
//          SQCmesage_Print();
//          UART1ComDriver_Cmd(UARTComDriver_Disable,0);  
//    }
    
    if(evt->PairedState == RTSPROT_RECORDED)
    {       

        if((User_LedMode == PowerLevelLow)&&evt->PressDuration <BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)  
        {
           LED_LogEvent(LED_Event_UserLowPower);
        }else if((User_LedMode != PowerLevelLow)&& evt->PressDuration <BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
        {  
           LED_LogEvent(LED_Event_UserRemotecontral);
        }
       
      //U56 command
      //"UP"command
       if(evt->Type == ErgoRemoteEventType_Up && evt->BpState == ErgoRemote_BpPress)
      {
        SQC_Count(&me.SQC.Counter.RTS_Up,sizeof(me.SQC.Counter.RTS_Up));
        me.ErgoModeState = ErgoModeState_WaitRelease;
        Mvrq.MoveType = MoveRequestType_RollToPosition;
        Mvrq.MoveRequestParam.Position = PositionName_UEL;
        MoveRequest_Post(&Mvrq);
      }
      //"DOWN"command
      else if(evt->Type == ErgoRemoteEventType_Down && evt->BpState == ErgoRemote_BpPress)
      {
        SQC_Count(&me.SQC.Counter.RTS_Down,sizeof(me.SQC.Counter.RTS_Down));
        me.ErgoModeState = ErgoModeState_WaitRelease;
        Mvrq.MoveType = MoveRequestType_UnRollToPosition;
        Mvrq.MoveRequestParam.Position = PositionName_DEL;
        MoveRequest_Post(&Mvrq);
      }
      //"MODE UP"command
      else if(evt->Type == ErgoRemoteEventType_ModeUp && evt->BpState == ErgoRemote_BpPress)
      {
        if(EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll)
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeStop,sizeof(me.SQC.Counter.RTS_ModeStop));
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }
        else if(EncoderDriver_GetMoveDirection() == EncoderDriver_Roll)
        {
          //DEBUGPRINT("Currently Rolling => Do Nothing\n")
        }
        else
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeUp,sizeof(me.SQC.Counter.RTS_ModeUp));
          
          Mvrq.MoveType = MoveRequestType_RollToPosition;
          Mvrq.MoveRequestParam.Position = PositionName_UEL;
          MoveRequest_Post(&Mvrq);
        }
        me.ErgoModeState = ErgoModeState_WaitRelease;
      }
      //"MODE DOWN"command
      else if(evt->Type == ErgoRemoteEventType_ModeDown && evt->BpState == ErgoRemote_BpPress)
      {
        if(EncoderDriver_GetMoveDirection() == EncoderDriver_Roll)
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeStop,sizeof(me.SQC.Counter.RTS_ModeStop));
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }
        else if(EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll)
        {
          //DEBUGPRINT("Currently UnRolling => Do Nothing\n")
        }
        else
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeDown,sizeof(me.SQC.Counter.RTS_ModeDown));
          
          Mvrq.MoveType = MoveRequestType_UnRollToPosition;
          Mvrq.MoveRequestParam.Position = PositionName_DEL;
          MoveRequest_Post(&Mvrq);
        }
        me.ErgoModeState = ErgoModeState_WaitRelease;
      }
      //"STOP"command
      else if(evt->Type == ErgoRemoteEventType_Stop)
      {
        if(evt->BpState == ErgoRemote_BpPress && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
        {
          SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }else if(evt->PressDuration <= BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpRelease && EncoderDriver_GetMoveDirection() == EncoderDriver_Stop)
        {  
          if(Position_IsPositionSet(PositionName_My) == TRUE)
          {
            TU32 CurrentPosition = EncoderDriver_GetPosition(); 
            TU32 MYPosition=Position_MyPlace();
            
            if(CurrentPosition <= (MYPosition+3u) && CurrentPosition >=(MYPosition-3u))
            {
              Mvrq.MoveType = MoveRequestType_None ;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }else
            {
              SQC_Count(&me.SQC.Counter.RTS_My,sizeof(me.SQC.Counter.RTS_My));
              
              Mvrq.MoveType = MoveRequestType_GoToPosition;
              Mvrq.MoveRequestParam.Position = PositionName_My;
              MoveRequest_Post(&Mvrq);
            }   
          }
        }else if(evt->PressDuration >= BUTTON_PUSH_5SEC && EncoderDriver_GetMoveDirection() == EncoderDriver_Stop)
        {  
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          SQC_Count(&me.SQC.Counter.My_Reseting,sizeof(me.SQC.Counter.My_Reseting));
          
          Limits_State_e Area = Limits_GetLimitsState();
          TU32 CurrentPos = EncoderDriver_GetPosition();

          if (Area == Limits_State_Between)
          {
            PositionCompare_e Pc = Position_Compare(PositionName_My,&CurrentPos,POSITION_TOLERANCE);
            if(Pc == CurrentPositionIsOnRef)
            {
              LED_LogEvent(LED_Event_ResetMY);
              Position_Disable(PositionName_My);
              
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }else
            {
              LED_LogEvent(LED_Event_ResetMY);
              Position_Set(PositionName_My,CurrentPos);
              
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }
          }
        }
      }
      //"UP&DOWN"command
      else if(evt->Type == ErgoRemoteEventType_Up_Down && evt->BpState == ErgoRemote_BpPress)
      { 
        if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }else if(evt->PressDuration >= BUTTON_PUSH_5SEC)
        {  
          TU32 CurrentPos = EncoderDriver_GetPosition();
          PositionCompare_e DEL = Position_Compare(PositionName_DEL,&CurrentPos,(POSITION_TOLERANCE+3)); //tolerance 5' in total     
          PositionCompare_e UEL = Position_Compare(PositionName_UEL,&CurrentPos,(POSITION_TOLERANCE+3));  
          
          if(DEL == CurrentPositionIsOnRef || DEL == CurrentPositionIsBelowRef)
          {
            me.ErgoModeState = ErgoModeState_WaitRelease;
            me.ChangeStateOnBpRelease = TRUE;
            me.ErgoModeAfterRelease = ErgoMode_ReSettingDEL;
            
            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
          }else if (UEL == CurrentPositionIsOnRef || UEL == CurrentPositionIsAboveRef)
          { 
            me.ErgoModeState = ErgoModeState_WaitRelease;
            me.ChangeStateOnBpRelease = TRUE;
            me.ErgoModeAfterRelease = ErgoMode_ReSettingUEL;
            
            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
          }else
          {
            me.ErgoModeState = ErgoModeState_WaitRelease;
            me.ChangeStateOnBpRelease = TRUE;
            me.ErgoModeAfterRelease = ErgoMode_MaintenanceResetingMode;
            
            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
          }
        }
      }
                  //“STOP&DOWN” command
      else if(evt->Type == ErgoRemoteEventType_Stop_Down && evt->BpState == ErgoRemote_BpPress)
      { 
        if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }else if(evt->PressDuration >= BUTTON_PUSH_2SEC)
        {  
            me.ErgoModeState = ErgoModeState_WaitRelease;
            me.ChangeStateOnBpRelease = TRUE;
            me.ErgoModeAfterRelease = ErgoMode_ReSettingSDP;
            
            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);

        }
      }
      //"PROG"command
      else if(evt->Type == ErgoRemoteEventType_Prog)
      {    
         if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }else if(evt->PressDuration >= BUTTON_PUSH_2SEC && evt->BpState == ErgoRemote_BpPress)
        {
          ConfigMode=true;
          RememberRemoteUsedToEnterInResetingMode();         
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
          me.ChangeStateOnBpRelease = TRUE;
          me.ErgoModeAfterRelease = ErgoMode_ConfigMode;
            
          Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }
      }  
      //"Stop_Up"command
      else if(evt->Type == ErgoRemoteEventType_Stop_Up && evt->BpState == ErgoRemote_BpPress)
      {
        if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }
//        else if(evt->PressDuration >= BUTTON_PUSH_2SEC)//kendy
//        {
//          me.ErgoModeState = ErgoModeState_WaitRelease;
//          if(me.Pdata.Tilt_Size < MAX_TILT_SIZE)
//          {
//            LED_LogEvent(LED_Event_TiltSpeedINC);
//            
//            me.Pdata.Tilt_Size = ( (me.Pdata.Tilt_Size + TILT_SIZE_CHANGE_INCREMENT) < MAX_TILT_SIZE) ? (me.Pdata.Tilt_Size + TILT_SIZE_CHANGE_INCREMENT) :(MAX_TILT_SIZE);
//            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
//            Mvrq.MoveRequestParam = NoMoveParam;
//            MoveRequest_Post(&Mvrq);
//          }else
//          {
//            LED_LogEvent(LED_Event_SetImpossibleUserMode);
//            
//            Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
//            Mvrq.MoveRequestParam = NoMoveParam;
//            MoveRequest_Post(&Mvrq); 
//          }
//        }
      }
      //"Stop_Down"command
      else if(evt->Type == ErgoRemoteEventType_Stop_Down && evt->BpState == ErgoRemote_BpPress)
      {
         if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }

      }     
      //"UP_Stop_Down"command
        

     else if(evt->Type == ErgoRemoteEventType_Stop_Up_Down&& evt->BpState == ErgoRemote_BpPress)
      { 
        if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop )
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }

        else  if(evt->PressDuration >= BUTTON_PUSH_2SEC)
          {
                MovingJustOnce=true;
              me.ErgoModeState = ErgoModeState_WaitRelease;
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq); 

          }
  


     }
     
                 //Sun_On  command
      else if(evt->Type == ErgoRemoteEventType_Sun_On && evt->BpState == ErgoRemote_BpPress)
      {



              me.ErgoModeState = ErgoModeState_WaitRelease;
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq); 

          
  

      }
                  //Sun_OFF  command
      else if(evt->Type == ErgoRemoteEventType_Sun_Off && evt->BpState == ErgoRemote_BpPress)
      {



              me.ErgoModeState = ErgoModeState_WaitRelease;
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq); 

          
  

      }
      //U80 command  
      else if(evt->U80Function != U80_U56 && (evt->Type == ErgoRemoteEventType_Execute_U80 || evt->U80Function != U80_FrameCounter))
      {
        if(evt->U80Function == U80_Move_Of && evt->BpState == ErgoRemote_BpPress) //“move Of” command
        {
            me.ErgoModeState = ErgoModeState_WaitRelease;
            TS8 MoveOff =  (evt->U80_OptionValue > 128u) ? (-(evt->U80_OptionValue-128u)):(evt->U80_OptionValue );
            MoveOff *= me.Pdata.WheelDir;
            
            if(MoveOff < 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Stop || EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll))
            {
              Mvrq.MoveType = MoveRequestType_UnRolloff;
              Mvrq.MoveRequestParam.MoveOff = (abs(MoveOff)  * me.Pdata.Tilt_Size);
              MoveRequest_Post(&Mvrq);
            }else if(MoveOff < 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Roll))
            {
              Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }else if(MoveOff > 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll))
            {
              Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }else if(MoveOff > 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Stop || EncoderDriver_GetMoveDirection() == EncoderDriver_Roll))
            {
              Mvrq.MoveType = MoveRequestType_Rolloff;
              Mvrq.MoveRequestParam.MoveOff = (abs(MoveOff)  * me.Pdata.Tilt_Size);
              MoveRequest_Post(&Mvrq);
            }
        }
        else if(evt->U80Function == U80_Go_To && evt->BpState == ErgoRemote_BpPress)
        {//"Go_To"command
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveRequestParam.GOTO = evt->U80_OptionValue;
          if(Mvrq.MoveRequestParam.GOTO <= 200u)
          {
              if(Mvrq.MoveRequestParam.GOTO == 0u)
              {
                SQC_Count(&me.SQC.Counter.RTS_Up,sizeof(me.SQC.Counter.RTS_Up));
                
                Mvrq.MoveType = MoveRequestType_RollToPosition;
                Mvrq.MoveRequestParam.Position = PositionName_UEL;
                MoveRequest_Post(&Mvrq);
              }else if(Mvrq.MoveRequestParam.GOTO == 200u)
              {
                SQC_Count(&me.SQC.Counter.RTS_Down,sizeof(me.SQC.Counter.RTS_Down));
                
                Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                Mvrq.MoveRequestParam.Position = PositionName_DEL;
                MoveRequest_Post(&Mvrq);
              }else{
                SQC_Count(&me.SQC.Counter.RTS_Goto,sizeof(me.SQC.Counter.RTS_Goto));
                
                Mvrq.MoveType = MoveRequestType_GoToPercent;
                MoveRequest_Post(&Mvrq);
              }
          }else if(Mvrq.MoveRequestParam.GOTO == 201u && Position_IsPositionSet(PositionName_My) == TRUE)
          {
            SQC_Count(&me.SQC.Counter.RTS_My,sizeof(me.SQC.Counter.RTS_My));
            Mvrq.MoveType = MoveRequestType_GoToPosition;
            Mvrq.MoveRequestParam.Position = PositionName_My;
            MoveRequest_Post(&Mvrq);
          }else if(Mvrq.MoveRequestParam.GOTO == 255u)
          {
            SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));
            Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
          }
        } 
      }
    }
    break;
      
  case ErgoModeState_WaitRelease :  
    
    
    
    if(evt->BpState == ErgoRemote_BpRelease)
    {
      me.ErgoModeState = ErgoModeState_WaitEvent;
     

              
          if((evt->PressDuration < BUTTON_PUSH_5SEC)
             &&MovingJustOnce==true&&(evt->Type == ErgoRemoteEventType_Stop_Up_Down))
          {
                 MovingJustOnce=false;   
            me.ChangeStateOnBpRelease = TRUE;
            me.ErgoModeAfterRelease = ErgoMode_SpeedAdjustmentMode;         
            
            Pre_SpeedMode=Curr_SpeedMode; 
          }
            
   

           if(me.ChangeStateOnBpRelease == TRUE)
      {
        me.ChangeStateOnBpRelease = FALSE;
        ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
      } 
    }
    else
    {
      if((evt->PressDuration >= BUTTON_PUSH_5SEC)
         &&MovingJustOnce==true&&(evt->Type == ErgoRemoteEventType_Stop_Up_Down))
      {
        
        me.ErgoModeState = ErgoModeState_WaitRelease;
        MovingJustOnce=false;
        me.ChangeStateOnBpRelease = TRUE;
        me.ErgoModeAfterRelease = ErgoMode_Tiltingspeedadjustment; 
        Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
        Mvrq.MoveRequestParam = NoMoveParam;
        MoveRequest_Post(&Mvrq);
        
      }
      
    
    }
//    else if((evt->Type == ErgoRemoteEventType_Stop_Up_Down)&&
//            (evt->BpState == ErgoRemote_BpPress)&&(evt->PressDuration < BUTTON_PUSH_5SEC))
//    {
//      me.ErgoModeState = ErgoModeState_WaitEvent;
//    }
    break;
    
  default:
    break;
  }
}





static void ErgonomicManager_SetAndTemporaryPairedMode_EventHandler(ErgoEvent_t *evt)
{
  MoveRequest_s Mvrq; 
  switch(me.ErgoModeState)
  {
  case ErgoModeState_WaitEvent :
    
    if(evt->Type == ErgoIndusRemoteEventType_Stop && evt->BpState == ErgoRemote_BpPress && evt->PressDuration >= BUTTON_PUSH_2SEC)
    { 
          me.ErgoModeState = ErgoModeState_WaitRelease;
      
          UART1ComDriver_Cmd(UARTComDriver_Enable_TxSQC,UART_BAUD_RATE_SQC);
          SQCmesage_Print();
          UART1ComDriver_Cmd(UARTComDriver_Disable,0);  
    }
    
    if(IsCurrentRemoteTheResetingRemote() == TRUE || evt->PairedState == RTSPROT_RECORDED)
     {  //go to
          if(evt->U80Function == U80_Go_To)
            {
                Mvrq.MoveRequestParam.GOTO = evt->U80_OptionValue;
               if(Mvrq.MoveRequestParam.GOTO <= 200u)
                  {
                   if(Mvrq.MoveRequestParam.GOTO == 0u && evt->BpState == ErgoRemote_BpPress) //go to 0%
                    {
		        SQC_Count(&me.SQC.Counter.RTS_Up,sizeof(me.SQC.Counter.RTS_Up));
		        me.ErgoModeState = ErgoModeState_WaitRelease;
                        
		        Mvrq.MoveType = MoveRequestType_RollToPosition;
		        Mvrq.MoveRequestParam.Position = PositionName_UEL;
		        MoveRequest_Post(&Mvrq);    
                    }else if(Mvrq.MoveRequestParam.GOTO == 200u && evt->BpState == ErgoRemote_BpPress)//go to 100%
                    {
		        SQC_Count(&me.SQC.Counter.RTS_Down,sizeof(me.SQC.Counter.RTS_Down));
		        me.ErgoModeState = ErgoModeState_WaitRelease;
                        
		        Mvrq.MoveType = MoveRequestType_UnRollToPosition;
		        Mvrq.MoveRequestParam.Position = PositionName_DEL;
		        MoveRequest_Post(&Mvrq);     
                    }else
                     {
                          SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));        
                          Mvrq.MoveType =MoveRequestType_SecurityStop;
                          Mvrq.MoveRequestParam = NoMoveParam;
                          MoveRequest_Post(&Mvrq);  
                          
                          me.ErgoModeState = ErgoModeState_WaitRelease;
                     } 
                }
               else//go to >100%
                    {
                      SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));
                      Mvrq.MoveType = MoveRequestType_SecurityStop;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq);
                      
                      me.ErgoModeState = ErgoModeState_WaitRelease;
                    }  
            }
            if(evt->U80Function == U80_Move_Of && evt->BpState == ErgoRemote_BpPress) //“move Of” command
            {
                me.ErgoModeState = ErgoModeState_WaitRelease;
                TS8 MoveOff =  (evt->U80_OptionValue > 128u) ? (-(evt->U80_OptionValue-128u)):(evt->U80_OptionValue );
                MoveOff *= me.Pdata.WheelDir;
                
                if(MoveOff < 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Stop || EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll))
                {
                  Mvrq.MoveType = MoveRequestType_UnRolloff;
                  Mvrq.MoveRequestParam.MoveOff = (abs(MoveOff)  * me.Pdata.Tilt_Size);
                  MoveRequest_Post(&Mvrq);
                }else if(MoveOff < 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Roll))
                {
                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq);
                }else if(MoveOff > 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll))
                {
                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq);
                }else if(MoveOff > 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Stop || EncoderDriver_GetMoveDirection() == EncoderDriver_Roll))
                {
                  Mvrq.MoveType = MoveRequestType_Rolloff;
                  Mvrq.MoveRequestParam.MoveOff = (abs(MoveOff)  * me.Pdata.Tilt_Size);
                  MoveRequest_Post(&Mvrq);
                }
            } 
          else
            { //U56 command
              //Prog 
              if(evt->Type == ErgoRemoteEventType_Prog) 
              {
                 if(evt->BpState == ErgoRemote_BpPress && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
                  {
                    me.ErgoModeState = ErgoModeState_WaitRelease;
                    
                    Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq);
                  }else
                  {
                      if(evt->BpState == ErgoRemote_BpRelease && evt->PressDuration <  BUTTON_PUSH_2SEC)
                      { //Prog < 2sec 
                          if(evt->PairedState == RTSPROT_UNRECORDED)//没有添加过的遥控器
                          { 
                            if(Banker_RecordTheCurrentRemote() == TRUE)
                            {  
                              RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_RemoteEepromBackup);
                            }
                          }
                            LED_LogEvent(LED_Event_ToUserMode);
                            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                            Mvrq.MoveRequestParam = NoMoveParam;
                            MoveRequest_Post(&Mvrq);
                            
                            me.ErgoModeState = ErgoModeState_WaitEvent;      //进入就是事件等待
                            ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);   //进入User Mode
                      }
                  }
              }
                //UP
                else if(evt->Type == ErgoRemoteEventType_Up && evt->BpState == ErgoRemote_BpPress)
                {
                  SQC_Count(&me.SQC.Counter.RTS_Up,sizeof(me.SQC.Counter.RTS_Up));
                  me.ErgoModeState = ErgoModeState_WaitRelease;
                  Mvrq.MoveType = MoveRequestType_RollToPosition;
                  Mvrq.MoveRequestParam.Position = PositionName_UEL;
                  MoveRequest_Post(&Mvrq);
                }
                //DOWN
                else if(evt->Type == ErgoRemoteEventType_Down && evt->BpState == ErgoRemote_BpPress)
                {
                  SQC_Count(&me.SQC.Counter.RTS_Down,sizeof(me.SQC.Counter.RTS_Down));
                  me.ErgoModeState = ErgoModeState_WaitRelease;
                  Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                  Mvrq.MoveRequestParam.Position = PositionName_DEL;
                  MoveRequest_Post(&Mvrq);
                } 
                    //STOP
                else if(evt->Type == ErgoRemoteEventType_Stop)
                {
                      if(evt->BpState == ErgoRemote_BpPress && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
                      {//STOP 按下就停止
                        SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);
                        
                        me.ErgoModeState = ErgoModeState_WaitRelease;
                      }
                }
              //UP_Stop_Down  //kendy
//              else if(evt->Type == ErgoRemoteEventType_Stop_Up_Down)
//              {
//                if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
//                {
//                  me.ErgoModeState = ErgoModeState_WaitRelease;
//                  
//                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
//                  Mvrq.MoveRequestParam = NoMoveParam;
//                  MoveRequest_Post(&Mvrq);
//                }else if(evt->PressDuration >= BUTTON_PUSH_2SEC && evt->BpState == ErgoRemote_BpPress)
//                {
//                  Banker_DeleteOnlyRemotes(); 
//                  ErgonomicManager_ChangeErgoMode(ErgoMode_Delivery2Mode);
//                  RADIO_OUTLINE=TRUE;  //Radio off
//                                 
//                  Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
//                  Mvrq.MoveRequestParam = NoMoveParam;
//                  MoveRequest_Post(&Mvrq); 
//                }
//              }    
       }
     }
    break;
    
  case ErgoModeState_WaitRelease :    
    if(evt->BpState == ErgoRemote_BpRelease)
    {
      me.ErgoModeState = ErgoModeState_WaitEvent;
      if(me.ChangeStateOnBpRelease == TRUE)
      {
        me.ChangeStateOnBpRelease = FALSE;
        ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
      }
    }
    break;
       
  default:
    break;
  }
}





//速度和用户模式 读取 
 void ErgonomicManager_ReadParameter()
{
    if(Curr_SpeedMode == SPEED_28RPM)                    
      Current_Speed = STANDARD_28_SPEED;   
    else if(Curr_SpeedMode == SPEED_26RPM)
      Current_Speed = MIDDLE_26_SPEED; 
    else if(Curr_SpeedMode == SPEED_24RPM)
      Current_Speed = MIDDLE_24_SPEED; 
    else if(Curr_SpeedMode == SPEED_22RPM)
      Current_Speed = MIDDLE_22_SPEED; 
    else if(Curr_SpeedMode == SPEED_20RPM)
      Current_Speed = MIDDLE_20_SPEED; 
    else if(Curr_SpeedMode == SPEED_18RPM)
      Current_Speed = MIDDLE_18_SPEED; 
    else if(Curr_SpeedMode == SPEED_16RPM)
      Current_Speed = MIDDLE_16_SPEED; 
    else if(Curr_SpeedMode == SPEED_14RPM)
      Current_Speed = MIDDLE_14_SPEED; 
    else if(Curr_SpeedMode == SPEED_12RPM)
      Current_Speed = MIDDLE_12_SPEED; 
    else
    {
      Curr_SpeedMode = SPEED_10RPM;
      Current_Speed  = MININUM_10_SPEED; 
    } 
    
    Roller_Speed =Current_Speed;
    if(Current_Speed/2 > MININUM_10_SPEED)
      Tilting_Speed = Current_Speed/2;
    else 
      Tilting_Speed = MININUM_10_SPEED; //the min tilting speed is 10RPM
}



static void ErgonomicManager_ErgoTimeOutHandler()
{
  MoveRequest_s Mvrq; 
  SQC_LogEvent(SQC_Event_ErgoTimeOut);
  LongTimer_Stop(LongTimer_ErgoTimer);
  if(me.ErgoModeAfterTimeOut == ErgoMode_FactoryMode)
  {    
    Mvrq.MoveType = MoveRequestType_Nolimit_ThreeShortFeedBack; 
    Mvrq.MoveRequestParam = NoMoveParam;
    MoveRequest_Post(&Mvrq); 
    
    Motor_Reset=TRUE;
  }else if(me.ErgoModeAfterTimeOut== ErgoMode_Delivery1Mode ||
           me.ErgoModeAfterTimeOut== ErgoMode_Delivery2Mode)  
  {
    RADIO_OUTLINE=TRUE;//kendy??/
    Banker_DeleteOnlyRemotes(); 
  }else
  {
    if(SpeedChange==true)
    {
       SpeedChange=false;
       Speed_MoveCycle=false;//kendy
       Curr_SpeedMode=Pre_SpeedMode; //kendy2??
       
    }//kendy//tilting speed 需要改变。？？？？？
    LED_LogEvent(LED_Event_BackUserMode);//用户模式关闭灯
    
    Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack; //超时电机抖动反馈
    Mvrq.MoveRequestParam = NoMoveParam;
    MoveRequest_Post(&Mvrq);
    
    if(me.ErgoModeAfterTimeOut == ErgoMode_UserMode)
    {            
        IWDG_ReloadCounter();
        SFY_OS_TaskDelay(OS_TIME_500_MS);
        LimitBack();  
    } 
  }

  me.ErgoModeState = ErgoModeState_WaitEvent;
  ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterTimeOut);
}


void ErgonomicManager_DeadManTimer_Timeout()
{
  MoveRequest_s Mvrq; 
  me.ErgoModeState = ErgoModeState_WaitEvent;
  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
  Mvrq.MoveRequestParam = NoMoveParam;
  MoveRequest_Post(&Mvrq);
}


/*!************************************************************************************************
* \fn         ErgonomicManager_FirstPairingMode_EventHandler(ErgoEvent_t *evt)
* \return     void
***************************************************************************************************/
//FirstPairingMode  
static void ErgonomicManager_FirstPairingMode_EventHandler(ErgoEvent_t *evt)
{
  MoveRequest_s Mvrq; 
  switch(me.ErgoModeState)
  {
  case ErgoModeState_WaitEvent :
    //Prog
    if(evt->Type == ErgoRemoteEventType_Prog && evt->BpState == ErgoRemote_BpRelease) 
    {
      if(evt->PressDuration < BUTTON_PUSH_2SEC)
      {   
           if(Banker_RecordTheCurrentRemote() == TRUE)
            {
              RememberRemoteUsedToEnterInResetingMode();   
              
              LED_LogEvent(LED_Event_TempPaired);
              
              RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_RemoteEepromBackup);
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
              
              ErgonomicManager_ChangeErgoMode(ErgoMode_SettingPart1Modebis);  
            }
      }
    }
    break;
    
  case ErgoModeState_WaitRelease :
    break;
        
  default:
    break;
  }
}


/*!************************************************************************************************
* \fn         ErgonomicManager_FactoryMode_EventHandler(ErgoEvent_t *evt)
* \return     void
***************************************************************************************************/
static void ErgonomicManager_FactoryMode_EventHandler(ErgoEvent_t *evt) 
{  
   MoveRequest_s Mvrq;  
      //其他模式 速度为20RPM
      //工厂模式 工厂遥控器 控制的转速为28RPM
   Current_Speed = STANDARD_28_SPEED; 
  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
      if(evt->Type == ErgoRemoteEventType_Up_Down)
      { //Up_Down any
        if(evt->BpState == ErgoRemote_BpPress)//modfiy as Xavier COLET required
        {
            LED_LogEvent(LED_Event_TempPaired);
            
            RememberRemoteUsedToEnterInResetingMode();         //记录遥控器地址              
            Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
            
            me.ErgoModeState = ErgoModeState_WaitRelease;
            me.ChangeStateOnBpRelease = TRUE;
            me.ErgoModeAfterRelease = ErgoMode_SettingPart1Mode;
            
            UART1ComDriver_Cmd(UARTComDriver_Enable_TxSQC,UART_BAUD_RATE_SQC);
            SQCmesage_Print();
            UART1ComDriver_Cmd(UARTComDriver_Disable,0);  
        }
      } //UP_Stop_Down  //KENDY turn off
//      else if((evt->Type == ErgoRemoteEventType_Stop_Up_Down ||evt->Type == ErgoIndusRemoteEventType_Up_Stop_Down)&& evt->BpState == ErgoRemote_BpPress)
//      {
//        if(evt->PressDuration >= BUTTON_PUSH_2SEC)
//        {
//          LowPowermodeTest=false;
//          ErgonomicManager_ChangeErgoMode(ErgoMode_Delivery1Mode);
//          RADIO_OUTLINE=TRUE;  //Radio off
//          
//          Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
//          Mvrq.MoveRequestParam = NoMoveParam;
//          MoveRequest_Post(&Mvrq); 
//        }
//      } //IndusRemote Up
      else if(evt->Type == ErgoIndusRemoteEventType_Up)
      {  
        Mvrq.MoveType = MoveRequestType_Manual_Roll;
        Mvrq.MoveRequestParam = NoMoveParam;
        MoveRequest_Post(&Mvrq);
        
        me.ErgoModeState = ErgoModeState_WaitRelease;
      }//IndusRemote Down
      else if(evt->Type == ErgoIndusRemoteEventType_Down)
      {   
        Mvrq.MoveType = MoveRequestType_Manual_UnRoll;
        Mvrq.MoveRequestParam = NoMoveParam;
        MoveRequest_Post(&Mvrq);
        
        me.ErgoModeState = ErgoModeState_WaitRelease;
      }//IndusRemote Stop    
      else if(evt->Type == ErgoIndusRemoteEventType_Stop)
      { 
        Mvrq.MoveType = MoveRequestType_Manual_Stop;  //人为停止
        Mvrq.MoveRequestParam = NoMoveParam;
        MoveRequest_Post(&Mvrq);
        
        me.ErgoModeState = ErgoModeState_WaitRelease;
      }//IndusRemote Stop+Up   
      else if(evt->Type == ErgoIndusRemoteEventType_Stop_Up)
      { 
        TU32 CurrentPosition = EncoderDriver_GetPosition();  //获取当前位置
        Mvrq.MoveType = MoveRequestType_GoToAbsPos;
        Mvrq.MoveRequestParam.AbsPos = (CurrentPosition-790u);//158*5
//        Mvrq.MoveRequestParam.AbsPos = (CurrentPosition-480u);  //RU24
        MoveRequest_Post(&Mvrq);
        
        me.ErgoModeState = ErgoModeState_WaitRelease;
      }//IndusRemote Stop+Down   
      else if(evt->Type == ErgoIndusRemoteEventType_Stop_Down)
      { 
        TU32 CurrentPosition = EncoderDriver_GetPosition();  //获取当前位置
        Mvrq.MoveType = MoveRequestType_GoToAbsPos;
        Mvrq.MoveRequestParam.AbsPos = (CurrentPosition+790u);
//         Mvrq.MoveRequestParam.AbsPos = (CurrentPosition+480u);  //RU24
        MoveRequest_Post(&Mvrq);
        
        me.ErgoModeState = ErgoModeState_WaitRelease;
      }//IndusRemote up+Down   
      else if(evt->Type == ErgoIndusRemoteEventType_Up_Down)
      { 
          LowPowermodeTest=true;
          Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        
        me.ErgoModeState = ErgoModeState_WaitRelease;
      }
      //软件版本核对
      else if(evt->U80Function == U80_Software_Version_Index) 
      {
        if(evt->U80_OptionValue == SOFTWARE_SerialNumber)    
        {
          Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }else{
          Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }
        me.ErgoModeState = ErgoModeState_WaitRelease;
      }   
    break;
     
    case ErgoModeState_WaitRelease :     
      if(evt->BpState == ErgoRemote_BpRelease)
      {
        me.ErgoModeState = ErgoModeState_WaitEvent;
        if(me.ChangeStateOnBpRelease == TRUE)
        {
          me.ChangeStateOnBpRelease = FALSE;
          LowPowermodeTest=false;
          ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
        }
      }   
      break;
       
  default:
    break;
  }
}

/*!************************************************************************************************
* \fn         ErgonomicManager_IndustrialMode_EventHandler(ErgoEvent_t *evt)
* \return     void
***************************************************************************************************/
static void ErgonomicManager_IndustrialMode_EventHandler(ErgoEvent_t *evt) 
{  
  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
  
    break;
     
    case ErgoModeState_WaitRelease :     
      if(evt->BpState == ErgoRemote_BpRelease)
      {
        me.ErgoModeState = ErgoModeState_WaitEvent;
        if(me.ChangeStateOnBpRelease == TRUE)
        {
          me.ChangeStateOnBpRelease = FALSE;
          ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
        }
      }   
      break;
       
  default:
    break;
  }
}


/*!************************************************************************************************
* \fn         ErgonomicManager_SettingPart1Mode_EventHandler(ErgoEvent_t *evt)
* \return     void
***************************************************************************************************/
static void ErgonomicManager_SettingPart1Mode_EventHandler(ErgoEvent_t *evt)
{  
  MoveRequest_s Mvrq;  
  Current_Speed = MIDDLE_20_SPEED; 

  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
    if(IsCurrentRemoteTheResetingRemote() == TRUE)
     {    
      if(evt->U80Function == U80_Go_To )
        {//"go to"command
            Mvrq.MoveRequestParam.GOTO = evt->U80_OptionValue;
           if(Mvrq.MoveRequestParam.GOTO <= 200u)
              {
                 if(Mvrq.MoveRequestParam.GOTO == 0u) //go to 0%
                    {
                      if(Position_IsPositionSet(PositionName_UEL) == false || Position_IsPositionSet(PositionName_DEL) == false)
                        {
                          if(evt->BpState == ErgoRemote_BpPress)
                            {
                              me.DeadmanModeStarted = TRUE; //反转补偿
                              Mvrq.MoveType = MoveRequestType_Nolimit_RollForTime;
                              Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS;  
                              MoveRequest_Post(&Mvrq);         
                            }else if(evt->BpState == ErgoRemote_BpRelease)
                            {
                              Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                              Mvrq.MoveRequestParam = NoMoveParam;
                              MoveRequest_Post(&Mvrq);  
                            }     
                        }else if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == true)
                        {
                              Limits_SetMode(LimitMode_Normal);
                              TU32 CurrentPosition = EncoderDriver_GetPosition();  
                              Limits_ComputeLimitsState(&CurrentPosition);         
                              Limits_State_e LimisState = Limits_GetLimitsState(); 
                              
                              if(LimisState == Limits_State_Between)  
                              {
                                  Up_moveonce=true;
                                 if(evt->BpState == ErgoRemote_BpPress)
                                  {
                                    me.DeadmanModeStarted = TRUE; //反转补偿
                                    Mvrq.MoveType = MoveRequestType_RollToPosition;
                                    Mvrq.MoveRequestParam.Position = PositionName_UEL;
                                    MoveRequest_Post(&Mvrq);       
                                  }else if(evt->BpState == ErgoRemote_BpRelease)
                                  {
                                    Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                                    Mvrq.MoveRequestParam = NoMoveParam;
                                    MoveRequest_Post(&Mvrq);  
                                  } 
                              }else if(LimisState == Limits_State_OnUp && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop &&Up_moveonce==true)        //限位之间 
                              {
                                Up_moveonce=false;
                                SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));        
                                Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                                Mvrq.MoveRequestParam = NoMoveParam;
                                MoveRequest_Post(&Mvrq);  

                                me.ErgoModeState = ErgoModeState_WaitRelease;
                              }                 
                              else
                              {
                                  if(evt->BpState == ErgoRemote_BpPress)
                                  {
                                    me.DeadmanModeStarted = TRUE; //反转补偿
                                    Mvrq.MoveType = MoveRequestType_Nolimit_RollForTime;
                                    Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                                    MoveRequest_Post(&Mvrq);         
                                  }else if(evt->BpState == ErgoRemote_BpRelease)
                                  {
                                    Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                                    Mvrq.MoveRequestParam = NoMoveParam;
                                    MoveRequest_Post(&Mvrq);  
                                  }                
                              
                              }
                        }          
                    }
                else if(Mvrq.MoveRequestParam.GOTO == 200u)//go to 100%
                   {
                      if(Position_IsPositionSet(PositionName_UEL) == false || Position_IsPositionSet(PositionName_DEL) == false)
                      {
                          if(evt->BpState == ErgoRemote_BpPress)
                          {
                            me.DeadmanModeStarted = TRUE; //反转补偿
                            Mvrq.MoveType = MoveRequestType_Nolimit_UnRollForTime;
                            Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                            MoveRequest_Post(&Mvrq);         
                          }else if(evt->BpState == ErgoRemote_BpRelease)
                          {
                            Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                            Mvrq.MoveRequestParam = NoMoveParam;
                            MoveRequest_Post(&Mvrq);  
                          }  
                      }else if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == true)
                      {
                            Limits_SetMode(LimitMode_Normal);
                            TU32 CurrentPosition = EncoderDriver_GetPosition();  //获取当前位置
                            Limits_ComputeLimitsState(&CurrentPosition);         
                            Limits_State_e LimisState = Limits_GetLimitsState(); 
                            
                            if(LimisState == Limits_State_Between)  
                            {
                               Down_moveonce=true;
                               if(evt->BpState == ErgoRemote_BpPress)
                                {
                                  me.DeadmanModeStarted = TRUE; //反转补偿
                                  Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                                  Mvrq.MoveRequestParam.Position = PositionName_DEL;
                                  MoveRequest_Post(&Mvrq);       
                                }else if(evt->BpState == ErgoRemote_BpRelease)
                                {
                                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                                  Mvrq.MoveRequestParam = NoMoveParam;
                                  MoveRequest_Post(&Mvrq);  
                                } 
                            }
                             else if(LimisState == Limits_State_OnDown && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && Down_moveonce==true)        //限位之间 
                            {
                              Down_moveonce=false;
                              SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));        
                              Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                              Mvrq.MoveRequestParam = NoMoveParam;
                              MoveRequest_Post(&Mvrq);  

                              me.ErgoModeState = ErgoModeState_WaitRelease;
                            }
                            else
                            {
                                if(evt->BpState == ErgoRemote_BpPress)
                                {
                                  me.DeadmanModeStarted = TRUE; //反转补偿
                                  Mvrq.MoveType = MoveRequestType_Nolimit_UnRollForTime;
                                  Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                                  MoveRequest_Post(&Mvrq);         
                                }else if(evt->BpState == ErgoRemote_BpRelease)
                                {
                                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                                  Mvrq.MoveRequestParam = NoMoveParam;
                                  MoveRequest_Post(&Mvrq);  
                                }                
                            }
                      }            
            }
           else//go to >100%
                    {
                      SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));
                      Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq);
                    }  
           }  
        }else if(evt->U80Function == U80_Move_Of && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          TS8 MoveOff =  (evt->U80_OptionValue > 128u) ? (-(evt->U80_OptionValue-128u)):(evt->U80_OptionValue );
          MoveOff *= me.Pdata.WheelDir;
          
          if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
          {
            Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
          }else if(MoveOff < 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Stop))
          {
            Mvrq.MoveType = MoveRequestType_Nolimit_UnRolloff;
            Mvrq.MoveRequestParam.MoveOff = (abs(MoveOff)  * me.Pdata.Tilt_Size);
            MoveRequest_Post(&Mvrq);
          }else if(MoveOff > 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Stop))
          {
            Mvrq.MoveType = MoveRequestType_Nolimit_Rolloff;
            Mvrq.MoveRequestParam.MoveOff = (abs(MoveOff)  * me.Pdata.Tilt_Size);
            MoveRequest_Post(&Mvrq);
          }
        }
      else{
          
      //UP
       if(evt->Type == ErgoRemoteEventType_Up)
        {
          if(Position_IsPositionSet(PositionName_UEL) == false || Position_IsPositionSet(PositionName_DEL) == false)
            {//限位没有设置完成 点动
              if(evt->BpState == ErgoRemote_BpPress)
                {
                  me.DeadmanModeStarted = TRUE; //反转补偿
                  Mvrq.MoveType = MoveRequestType_Nolimit_RollForTime;
                  Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS;
                  MoveRequest_Post(&Mvrq);         
                }else if(evt->BpState == ErgoRemote_BpRelease)
                {
                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq);  
                }     
            }else if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == true)
            {
                  Limits_SetMode(LimitMode_Normal);
                  TU32 CurrentPosition = EncoderDriver_GetPosition();  
                  Limits_ComputeLimitsState(&CurrentPosition);         
                  Limits_State_e LimisState = Limits_GetLimitsState(); 
                  
                  if(LimisState == Limits_State_Between)  
                  {
                      Up_moveonce=true;
                     if(evt->BpState == ErgoRemote_BpPress)
                      {
                        me.DeadmanModeStarted = TRUE; //反转补偿
                        Mvrq.MoveType = MoveRequestType_RollToPosition;
                        Mvrq.MoveRequestParam.Position = PositionName_UEL;
                        MoveRequest_Post(&Mvrq);       
                      }else if(evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                      } 
                  }else if(LimisState == Limits_State_OnUp && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop &&Up_moveonce==true)       
                  {
                    Up_moveonce=false;
                    SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));        
                    Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq);  

                    me.ErgoModeState = ErgoModeState_WaitRelease;
                  }                 
                  else
                  {
                      if(evt->BpState == ErgoRemote_BpPress)
                      {
                        me.DeadmanModeStarted = TRUE; //反转补偿
                        Mvrq.MoveType = MoveRequestType_Nolimit_RollForTime;
                        Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                        MoveRequest_Post(&Mvrq);         
                      }else if(evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                      }                
                  }    
            }                
        }
      //DOWN
      else if(evt->Type == ErgoRemoteEventType_Down)
      {
            if(Position_IsPositionSet(PositionName_UEL) == false || Position_IsPositionSet(PositionName_DEL) == false)
            {
                if(evt->BpState == ErgoRemote_BpPress)
                {
                  me.DeadmanModeStarted = TRUE; //反转补偿
                  Mvrq.MoveType = MoveRequestType_Nolimit_UnRollForTime;
                  Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS;
                  MoveRequest_Post(&Mvrq);         
                }
                else if(evt->BpState == ErgoRemote_BpRelease)
                {
                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq);  
                }  
            }else if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == true)
            {
                  Limits_SetMode(LimitMode_Normal);
                  TU32 CurrentPosition = EncoderDriver_GetPosition();  
                  Limits_ComputeLimitsState(&CurrentPosition);       
                  Limits_State_e LimisState = Limits_GetLimitsState(); 
                  
                  if(LimisState == Limits_State_Between)  
                  {
                     Down_moveonce=true;
                     if(evt->BpState == ErgoRemote_BpPress)
                      {
                        me.DeadmanModeStarted = TRUE; //反转补偿
                        Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                        Mvrq.MoveRequestParam.Position = PositionName_DEL;
                        MoveRequest_Post(&Mvrq);       
                      }else if(evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                      } 
                  }
                   else if(LimisState == Limits_State_OnDown && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && Down_moveonce==true)        //限位之间 
                  {
                    Down_moveonce=false;
                    SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));        
                    Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq);  

                    me.ErgoModeState = ErgoModeState_WaitRelease;
                  }
                  else
                  {
                      if(evt->BpState == ErgoRemote_BpPress)
                      {
                        me.DeadmanModeStarted = TRUE; //反转补偿
                        Mvrq.MoveType = MoveRequestType_Nolimit_UnRollForTime;
                        Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                        MoveRequest_Post(&Mvrq);         
                      }else if(evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                      }                
                  }
            }            
       }   
      //MODE UP
      else if(evt->Type == ErgoRemoteEventType_ModeUp && evt->BpState == ErgoRemote_BpPress)
      {
        if(EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll || EncoderDriver_GetMoveDirection() == EncoderDriver_Roll)
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeStop,sizeof(me.SQC.Counter.RTS_ModeStop));
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }
        else
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeUp,sizeof(me.SQC.Counter.RTS_ModeUp));         
          Mvrq.MoveType = MoveRequestType_RollToPosition;
          Mvrq.MoveRequestParam.Position = PositionName_UEL;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }
      }
      //MODE DOWN
      else if(evt->Type == ErgoRemoteEventType_ModeDown && evt->BpState == ErgoRemote_BpPress)
      {
         if(EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll || EncoderDriver_GetMoveDirection() == EncoderDriver_Roll)
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeStop,sizeof(me.SQC.Counter.RTS_ModeStop));
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }
        else
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeDown,sizeof(me.SQC.Counter.RTS_ModeDown));     
          Mvrq.MoveType = MoveRequestType_UnRollToPosition;
          Mvrq.MoveRequestParam.Position = PositionName_DEL;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }
      }
      //stop
     else if(evt->Type ==ErgoRemoteEventType_Stop )
      { 
        if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }else if(evt->PressDuration >= BUTTON_PUSH_2SEC && evt->BpState == ErgoRemote_BpPress)
        {  //stop >2S 
           if(Position_IsPositionSet(PositionName_UEL) == false && Position_IsPositionSet(PositionName_DEL) == false)
           { 
              LED_LogEvent(LED_Event_ResetRotationDir);
              SQC_Count(&me.SQC.Counter.Motor_RotationResetting,sizeof(me.SQC.Counter.Motor_RotationResetting));
              
              me.ErgoModeState = ErgoModeState_WaitRelease;
              
              Direction_change=true;
              me.Pdata.WheelDir = (me.Pdata.WheelDir == WheelDir_Normal) ? (WheelDir_Inverted) : (WheelDir_Normal);
              
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);     
           }else if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == true)
           {   
                  Limits_SetMode(LimitMode_Normal);
                  TU32 CurrentPosition = EncoderDriver_GetPosition();  
                  Limits_ComputeLimitsState(&CurrentPosition);       
                  Limits_State_e LimisState = Limits_GetLimitsState(); 
                  
                  if(LimisState == Limits_State_Above || LimisState == Limits_State_Below)  
                  {
                      me.ErgoModeState = ErgoModeState_WaitRelease;
                      
                      LED_LogEvent(LED_Event_SetImpossibleSetMode);
                      
                      Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBackMoveTo;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq); 
                  }else
                  {
                      if(Banker_RecordTheCurrentRemote() == TRUE)
                      {
                          LED_LogEvent(LED_Event_Set_ToSetNotTemMode);             
                          RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_RemoteEepromBackup);

                          me.ErgoModeState = ErgoModeState_WaitRelease;
                          me.ChangeStateOnBpRelease = TRUE;
                          me.ErgoModeAfterRelease = ErgoMode_SetAndTemporaryRemote;

                          Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                          Mvrq.MoveRequestParam = NoMoveParam;
                          MoveRequest_Post(&Mvrq);
                      }
                      else 
                      {
                           me.ErgoModeState = ErgoModeState_WaitRelease;
                      } 
                  }
             }
         }
      }
      else if(evt->Type == ErgoRemoteEventType_Stop_Up)
      {//Stop_Up
        if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
              me.ErgoModeState = ErgoModeState_WaitRelease;
              
              Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
        }else if(evt->PressDuration < BUTTON_PUSH_2SEC && EncoderDriver_GetMoveDirection() == EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
          {
              me.ErgoModeState = ErgoModeState_WaitRelease;
              
              TU32 CurrentPos = EncoderDriver_GetPosition();
              if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == false)
              {
                 TU32 Distance  =(TU32) labs(CurrentPos -  *Position_GetValue(PositionName_UEL));
                 TS32 Distance1 =(TS32)(CurrentPos -  *Position_GetValue(PositionName_UEL));
                if((Distance < ONE_PRODUCT_TURN) || Distance1<0)
                {                           
                  LED_LogEvent(LED_Event_SetImpossibleSetMode);
                  
                  Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq); 
                }else
                { 
                  Limits_SetMode(LimitMode_settingDEL); 
                  DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[2]);
                  LED_LogEvent(LED_Event_ResetDEL);
                  SQC_Count(&me.SQC.Counter.DEL_Reseting,sizeof(me.SQC.Counter.DEL_Reseting));
                  Position_Set(PositionName_DEL,CurrentPos);
                                  
                  Mvrq.MoveType = MoveRequestType_RollToPosition;
                  Mvrq.MoveRequestParam.Position = PositionName_UEL;
                  MoveRequest_Post(&Mvrq);                
                } 
              }else if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == true)
              {  
                 if(CurrentPos <= (*Position_GetValue(PositionName_UEL)+ONE_PRODUCT_TURN))
                 {
                    LED_LogEvent(LED_Event_SetImpossibleSetMode);
                   
                    Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq); 
                 }else
                  { 
                    Limits_SetMode(LimitMode_settingDEL); 
                    DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[2]);
                    LED_LogEvent(LED_Event_ResetDEL);
                    SQC_Count(&me.SQC.Counter.DEL_Reseting,sizeof(me.SQC.Counter.DEL_Reseting));
                    Position_Set(PositionName_DEL,CurrentPos);
                                  
                    Mvrq.MoveType = MoveRequestType_RollToPosition;
                    Mvrq.MoveRequestParam.Position = PositionName_UEL;
                    MoveRequest_Post(&Mvrq);                
                  }  
             }else
              { 
                  Limits_SetMode(LimitMode_settingDEL);  
                  DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[2]);
                  LED_LogEvent(LED_Event_ResetDEL);
                  SQC_Count(&me.SQC.Counter.DEL_Reseting,sizeof(me.SQC.Counter.DEL_Reseting));
                  Position_Set(PositionName_DEL,CurrentPos);
                  
                  Mvrq.MoveType = MoveRequestType_Nolimit_RollForTime;
                  Mvrq.MoveRequestParam.Time_mS =MAXMOVETIME_MS;
                  MoveRequest_Post(&Mvrq);
              }
          }
      }
      else if(evt->Type == ErgoRemoteEventType_Stop_Down )
      {//Stop_Down
         if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
              me.ErgoModeState = ErgoModeState_WaitRelease;
              
              Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
        }else if(evt->PressDuration < BUTTON_PUSH_2SEC && EncoderDriver_GetMoveDirection() == EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
          {            
              me.ErgoModeState = ErgoModeState_WaitRelease;
              
              TU32 CurrentPos = EncoderDriver_GetPosition();
              if(Position_IsPositionSet(PositionName_DEL) == true && Position_IsPositionSet(PositionName_UEL) == false)
              { 
                TU32 Distance  =(TU32) labs(CurrentPos -  *Position_GetValue(PositionName_DEL));
                TS32 Distance2 =(TS32)(CurrentPos -  *Position_GetValue(PositionName_DEL));
                if((Distance < ONE_PRODUCT_TURN) || Distance2>0)
                { 
                  LED_LogEvent(LED_Event_SetImpossibleSetMode);
                  
                  Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq); 
                }else
                { 
                  Limits_SetMode(LimitMode_settingUEL); 
                  DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[1]);
                  LED_LogEvent(LED_Event_ResetUEL);
                  SQC_Count(&me.SQC.Counter.UEL_Reseting,sizeof(me.SQC.Counter.UEL_Reseting));
                  Position_Set(PositionName_UEL,CurrentPos);
                  
                  Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                  Mvrq.MoveRequestParam.Position = PositionName_DEL;
                  MoveRequest_Post(&Mvrq);                
                }  
             }else if(Position_IsPositionSet(PositionName_DEL) == true && Position_IsPositionSet(PositionName_UEL) == true)
             {
                if(CurrentPos >= (*Position_GetValue(PositionName_DEL)-ONE_PRODUCT_TURN))
                 {
                    LED_LogEvent(LED_Event_SetImpossibleSetMode);
                   
                    Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq); 
                 }else
                  {
                    Limits_SetMode(LimitMode_settingUEL); 
                    DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[1]);
                    LED_LogEvent(LED_Event_ResetUEL);
                    SQC_LogEvent(SQC_Event_ResetUEL);
                    SQC_Count(&me.SQC.Counter.UEL_Reseting,sizeof(me.SQC.Counter.UEL_Reseting));
                    Position_Set(PositionName_UEL,CurrentPos);
                    
                    Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                    Mvrq.MoveRequestParam.Position = PositionName_DEL;
                    MoveRequest_Post(&Mvrq);                
                  } 
             }else
              { 
                  Limits_SetMode(LimitMode_settingUEL); 
                  DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[1]);
                  LED_LogEvent(LED_Event_ResetUEL);
                  SQC_LogEvent(SQC_Event_ResetUEL);
                  SQC_Count(&me.SQC.Counter.UEL_Reseting,sizeof(me.SQC.Counter.UEL_Reseting));
                  Position_Set(PositionName_UEL,CurrentPos);
                  
                  me.ErgoModeState = ErgoModeState_WaitRelease;

                  Mvrq.MoveType = MoveRequestType_Nolimit_UnRollForTime;
                  Mvrq.MoveRequestParam.Time_mS =MAXMOVETIME_MS;
                  MoveRequest_Post(&Mvrq);
              }
         }
     }
   }
 } 
    break;
     
    case ErgoModeState_WaitRelease :     
      if(evt->BpState == ErgoRemote_BpRelease)
      {
        me.ErgoModeState = ErgoModeState_WaitEvent;
        if(me.ChangeStateOnBpRelease == TRUE)
        {
          me.ChangeStateOnBpRelease = FALSE;
          ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
        }
      }     
      break;
       
  default:
    break;
  }
}




/*!************************************************************************************************
* \fn         ErgonomicManager_SettingPart1Modebis_EventHandler(ErgoEvent_t *evt)
* \return     void
***************************************************************************************************/
static void ErgonomicManager_SettingPart1Modebis_EventHandler(ErgoEvent_t *evt)
{  
  MoveRequest_s Mvrq;  
  Current_Speed = MIDDLE_20_SPEED; 
  
  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
    if(IsCurrentRemoteTheResetingRemote() == TRUE)
     {    //go to
      if(evt->U80Function == U80_Go_To )
        {
            Mvrq.MoveRequestParam.GOTO = evt->U80_OptionValue;
           if(Mvrq.MoveRequestParam.GOTO <= 200u)
              {
                 if(Mvrq.MoveRequestParam.GOTO == 0u) //go to 0%
                    {
                      if(Position_IsPositionSet(PositionName_UEL) == false || Position_IsPositionSet(PositionName_DEL) == false)
                        {
                          if(evt->BpState == ErgoRemote_BpPress)
                            {
                              me.DeadmanModeStarted = TRUE; 
                              Mvrq.MoveType = MoveRequestType_Nolimit_RollForTime;
                              Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                              MoveRequest_Post(&Mvrq);         
                            }else if(evt->BpState == ErgoRemote_BpRelease)
                            {
                              Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                              Mvrq.MoveRequestParam = NoMoveParam;
                              MoveRequest_Post(&Mvrq);  
                            }     
                        }else if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == true)
                        {
                              Limits_SetMode(LimitMode_Normal);
                              TU32 CurrentPosition = EncoderDriver_GetPosition(); 
                              Limits_ComputeLimitsState(&CurrentPosition);        
                              Limits_State_e LimisState = Limits_GetLimitsState(); 
                              
                              if(LimisState == Limits_State_Between)  
                              {
                                  Up_moveonce=true;
                                 if(evt->BpState == ErgoRemote_BpPress)
                                  {
                                    me.DeadmanModeStarted = TRUE; 
                                    Mvrq.MoveType = MoveRequestType_RollToPosition;
                                    Mvrq.MoveRequestParam.Position = PositionName_UEL;
                                    MoveRequest_Post(&Mvrq);       
                                  }else if(evt->BpState == ErgoRemote_BpRelease)
                                  {
                                    Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                                    Mvrq.MoveRequestParam = NoMoveParam;
                                    MoveRequest_Post(&Mvrq);  
                                  } 
                              }else if(LimisState == Limits_State_OnUp && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop &&Up_moveonce==true)        //限位之间 
                              {
                                Up_moveonce=false;
                                SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));        
                                Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                                Mvrq.MoveRequestParam = NoMoveParam;
                                MoveRequest_Post(&Mvrq);  

                                me.ErgoModeState = ErgoModeState_WaitRelease;
                              }                 
                              else
                              {
                                  if(evt->BpState == ErgoRemote_BpPress)
                                  {
                                    me.DeadmanModeStarted = TRUE;
                                    Mvrq.MoveType = MoveRequestType_Nolimit_RollForTime;
                                    Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                                    MoveRequest_Post(&Mvrq);         
                                  }else if(evt->BpState == ErgoRemote_BpRelease)
                                  {
                                    Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                                    Mvrq.MoveRequestParam = NoMoveParam;
                                    MoveRequest_Post(&Mvrq);  
                                  }                
                              
                              }
                        }          
                    }
                else if(Mvrq.MoveRequestParam.GOTO == 200u)//go to 100%
                   {
                      if(Position_IsPositionSet(PositionName_UEL) == false || Position_IsPositionSet(PositionName_DEL) == false)
                      {
                          if(evt->BpState == ErgoRemote_BpPress)
                          {
                            me.DeadmanModeStarted = TRUE; //反转补偿
                            Mvrq.MoveType = MoveRequestType_Nolimit_UnRollForTime;
                            Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                            MoveRequest_Post(&Mvrq);         
                          }else if(evt->BpState == ErgoRemote_BpRelease)
                          {
                            Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                            Mvrq.MoveRequestParam = NoMoveParam;
                            MoveRequest_Post(&Mvrq);  
                          }  
                      }else if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == true)
                      {
                            Limits_SetMode(LimitMode_Normal);
                            TU32 CurrentPosition = EncoderDriver_GetPosition();  
                            Limits_ComputeLimitsState(&CurrentPosition);         
                            Limits_State_e LimisState = Limits_GetLimitsState(); 
                            
                            if(LimisState == Limits_State_Between)  
                            {
                               Down_moveonce=true;
                               if(evt->BpState == ErgoRemote_BpPress)
                                {
                                  me.DeadmanModeStarted = TRUE; //反转补偿
                                  Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                                  Mvrq.MoveRequestParam.Position = PositionName_DEL;
                                  MoveRequest_Post(&Mvrq);       
                                }else if(evt->BpState == ErgoRemote_BpRelease)
                                {
                                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                                  Mvrq.MoveRequestParam = NoMoveParam;
                                  MoveRequest_Post(&Mvrq);  
                                } 
                            }
                             else if(LimisState == Limits_State_OnDown && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && Down_moveonce==true)        //限位之间 
                            {
                              Down_moveonce=false;
                              SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));        
                              Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                              Mvrq.MoveRequestParam = NoMoveParam;
                              MoveRequest_Post(&Mvrq);  

                              me.ErgoModeState = ErgoModeState_WaitRelease;
                            }
                            else
                            {
                                if(evt->BpState == ErgoRemote_BpPress)
                                {
                                  me.DeadmanModeStarted = TRUE; //反转补偿
                                  Mvrq.MoveType = MoveRequestType_Nolimit_UnRollForTime;
                                  Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                                  MoveRequest_Post(&Mvrq);         
                                }else if(evt->BpState == ErgoRemote_BpRelease)
                                {
                                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                                  Mvrq.MoveRequestParam = NoMoveParam;
                                  MoveRequest_Post(&Mvrq);  
                                }                
                            }
                    
                      }            
            }
           else//go to >100%
                    {
                      SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));
                      Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq);
                    }  
          }  
        }else if(evt->U80Function == U80_Move_Of && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          TS8 MoveOff =  (evt->U80_OptionValue > 128u) ? (-(evt->U80_OptionValue-128u)):(evt->U80_OptionValue );
          MoveOff *= me.Pdata.WheelDir;
          
          if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
          {
            Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
          }else if(MoveOff < 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Stop))
          {
            Mvrq.MoveType = MoveRequestType_Nolimit_UnRolloff;
            Mvrq.MoveRequestParam.MoveOff = (abs(MoveOff)  * me.Pdata.Tilt_Size);
            MoveRequest_Post(&Mvrq);
          }else if(MoveOff > 0 && (EncoderDriver_GetMoveDirection() == EncoderDriver_Stop))
          {
            Mvrq.MoveType = MoveRequestType_Nolimit_Rolloff;
            Mvrq.MoveRequestParam.MoveOff = (abs(MoveOff)  * me.Pdata.Tilt_Size);
            MoveRequest_Post(&Mvrq);
          }
        }
      else{
          
       //UP
       if(evt->Type == ErgoRemoteEventType_Up)
        {
        if(Position_IsPositionSet(PositionName_UEL) == false || Position_IsPositionSet(PositionName_DEL) == false)
            {
              if(evt->BpState == ErgoRemote_BpPress)
                {
                  me.DeadmanModeStarted = TRUE; //反转补偿
                  Mvrq.MoveType = MoveRequestType_Nolimit_RollForTime;
                  Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                  MoveRequest_Post(&Mvrq);         
                }else if(evt->BpState == ErgoRemote_BpRelease)
                {
                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq);  
                }     
            }else if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == true)
            {
                  Limits_SetMode(LimitMode_Normal);
                  TU32 CurrentPosition = EncoderDriver_GetPosition();  
                  Limits_ComputeLimitsState(&CurrentPosition);         
                  Limits_State_e LimisState = Limits_GetLimitsState(); 
                  
                  if(LimisState == Limits_State_Between)  
                  {
                      Up_moveonce=true;
                     if(evt->BpState == ErgoRemote_BpPress)
                      {
                        me.DeadmanModeStarted = TRUE; //反转补偿
                        Mvrq.MoveType = MoveRequestType_RollToPosition;
                        Mvrq.MoveRequestParam.Position = PositionName_UEL;
                        MoveRequest_Post(&Mvrq);       
                      }else if(evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                      } 
                  }else if(LimisState == Limits_State_OnUp && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop &&Up_moveonce==true)        //限位之间 
                  {
                    Up_moveonce=false;
                    SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));        
                    Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq);  

                    me.ErgoModeState = ErgoModeState_WaitRelease;
                  }                 
                  else
                  {
                      if(evt->BpState == ErgoRemote_BpPress)
                      {
                        me.DeadmanModeStarted = TRUE; //反转补偿
                        Mvrq.MoveType = MoveRequestType_Nolimit_RollForTime;
                        Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                        MoveRequest_Post(&Mvrq);         
                      }else if(evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                      }                
                  }
            }                   
        }
      //DOWN
      else if(evt->Type == ErgoRemoteEventType_Down)
      {
            if(Position_IsPositionSet(PositionName_UEL) == false || Position_IsPositionSet(PositionName_DEL) == false)
            {
                if(evt->BpState == ErgoRemote_BpPress)
                {
                  me.DeadmanModeStarted = TRUE; //反转补偿
                  Mvrq.MoveType = MoveRequestType_Nolimit_UnRollForTime;
                  Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                  MoveRequest_Post(&Mvrq);         
                }else if(evt->BpState == ErgoRemote_BpRelease)
                {
                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq);  
                }  
            }else if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == true)
            {
                  Limits_SetMode(LimitMode_Normal);
                  TU32 CurrentPosition = EncoderDriver_GetPosition();  
                  Limits_ComputeLimitsState(&CurrentPosition);       
                  Limits_State_e LimisState = Limits_GetLimitsState(); 
                  
                  if(LimisState == Limits_State_Between)  
                  {
                     Down_moveonce=true;
                     if(evt->BpState == ErgoRemote_BpPress)
                      {
                        me.DeadmanModeStarted = TRUE; //反转补偿
                        Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                        Mvrq.MoveRequestParam.Position = PositionName_DEL;
                        MoveRequest_Post(&Mvrq);       
                      }else if(evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                      } 
                  }
                   else if(LimisState == Limits_State_OnDown && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && Down_moveonce==true)        //限位之间 
                  {
                    Down_moveonce=false;
                    SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));        
                    Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq);  

                    me.ErgoModeState = ErgoModeState_WaitRelease;
                  }
                  else
                  {
                      if(evt->BpState == ErgoRemote_BpPress)
                      {
                        me.DeadmanModeStarted = TRUE; //反转补偿
                        Mvrq.MoveType = MoveRequestType_Nolimit_UnRollForTime;
                        Mvrq.MoveRequestParam.Time_mS = MAXMOVETIME_MS; 
                        MoveRequest_Post(&Mvrq);         
                      }else if(evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                      }                
                  }    
            }            
       }   
      //MODE UP
      else if(evt->Type == ErgoRemoteEventType_ModeUp && evt->BpState == ErgoRemote_BpPress)
      {
        if(EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll || EncoderDriver_GetMoveDirection() == EncoderDriver_Roll)
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeStop,sizeof(me.SQC.Counter.RTS_ModeStop));
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }else
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeUp,sizeof(me.SQC.Counter.RTS_ModeUp));         
          Mvrq.MoveType = MoveRequestType_RollToPosition;
          Mvrq.MoveRequestParam.Position = PositionName_UEL;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }
      }
      //MODE DOWN
      else if(evt->Type == ErgoRemoteEventType_ModeDown && evt->BpState == ErgoRemote_BpPress)
      {
         if(EncoderDriver_GetMoveDirection() == EncoderDriver_UnRoll || EncoderDriver_GetMoveDirection() == EncoderDriver_Roll)
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeStop,sizeof(me.SQC.Counter.RTS_ModeStop));
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }else
        {
          SQC_Count(&me.SQC.Counter.RTS_ModeDown,sizeof(me.SQC.Counter.RTS_ModeDown));     
          Mvrq.MoveType = MoveRequestType_UnRollToPosition;
          Mvrq.MoveRequestParam.Position = PositionName_DEL;
          MoveRequest_Post(&Mvrq);
          
          me.ErgoModeState = ErgoModeState_WaitRelease;
        }
      }
      //stop
     else if(evt->Type ==ErgoRemoteEventType_Stop )
      {
         if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }else if(evt->PressDuration >= BUTTON_PUSH_2SEC && evt->BpState == ErgoRemote_BpPress)
        {  //stop >2S 
           if(Position_IsPositionSet(PositionName_UEL) == false && Position_IsPositionSet(PositionName_DEL) == false)
           {
              LED_LogEvent(LED_Event_ResetRotationDir);
              SQC_Count(&me.SQC.Counter.Motor_RotationResetting,sizeof(me.SQC.Counter.Motor_RotationResetting));
              
              me.ErgoModeState = ErgoModeState_WaitRelease;
              
              //RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_InvertRotationDirection);
              //RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_RemoteEepromBackup);
              Direction_change=true;
              me.Pdata.WheelDir = (me.Pdata.WheelDir == WheelDir_Normal) ? (WheelDir_Inverted) : (WheelDir_Normal);
              
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);     
           }else if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == true)
           {
             
                  Limits_SetMode(LimitMode_Normal);
                  TU32 CurrentPosition = EncoderDriver_GetPosition();  
                  Limits_ComputeLimitsState(&CurrentPosition);       
                  Limits_State_e LimisState = Limits_GetLimitsState(); 
                  
                  if(LimisState == Limits_State_Above || LimisState == Limits_State_Below)  
                  {
                      me.ErgoModeState = ErgoModeState_WaitRelease;
                      
                      LED_LogEvent(LED_Event_SetImpossibleSetMode);
                      
                      Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBackMoveTo;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq); 
                  }else
                  {
                      LED_LogEvent(LED_Event_ToUserMode);
                      
                      me.ErgoModeState = ErgoModeState_WaitRelease;
                      me.ChangeStateOnBpRelease = TRUE;
                      me.ErgoModeAfterRelease = ErgoMode_UserMode;

                      Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq);
                  }
           }else 
           {
              me.ErgoModeState = ErgoModeState_WaitRelease;
           }  
        }
      }
      else if(evt->Type == ErgoRemoteEventType_Stop_Up)
      {//Stop_Up
          if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }else if(evt->PressDuration < BUTTON_PUSH_2SEC && EncoderDriver_GetMoveDirection() == EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
          {   
              me.ErgoModeState = ErgoModeState_WaitRelease;
              
              TU32 CurrentPos = EncoderDriver_GetPosition();
              if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == false)
              {
                 TU32 Distance  =(TU32) labs(CurrentPos -  *Position_GetValue(PositionName_UEL));
                 TS32 Distance1 =(TS32)(CurrentPos -  *Position_GetValue(PositionName_UEL));
                if((Distance < ONE_PRODUCT_TURN) || Distance1<0)
                {  
                  LED_LogEvent(LED_Event_SetImpossibleSetMode);
                  
                  Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq); 
                }else
                {  
                  Limits_SetMode(LimitMode_settingDEL); 
                  DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[2]);
                  LED_LogEvent(LED_Event_ResetDEL);
                  SQC_LogEvent(SQC_Event_ResetDEL);
                  SQC_Count(&me.SQC.Counter.DEL_Reseting,sizeof(me.SQC.Counter.DEL_Reseting));
                  Position_Set(PositionName_DEL,CurrentPos);
                  
                  Mvrq.MoveType = MoveRequestType_RollToPosition;
                  Mvrq.MoveRequestParam.Position = PositionName_UEL;
                  MoveRequest_Post(&Mvrq);                
                } 
              }else if(Position_IsPositionSet(PositionName_UEL) == true && Position_IsPositionSet(PositionName_DEL) == true)
              {  
                 if(CurrentPos <= (*Position_GetValue(PositionName_UEL)+ONE_PRODUCT_TURN))
                 {
                    LED_LogEvent(LED_Event_SetImpossibleSetMode);

                    Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq); 
                 }else
                  { 
                    Limits_SetMode(LimitMode_settingDEL); 
                    DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[2]);
                    LED_LogEvent(LED_Event_ResetDEL);
                    SQC_LogEvent(SQC_Event_ResetDEL);
                    SQC_Count(&me.SQC.Counter.DEL_Reseting,sizeof(me.SQC.Counter.DEL_Reseting));
                    Position_Set(PositionName_DEL,CurrentPos);
                    
                    Mvrq.MoveType = MoveRequestType_RollToPosition;
                    Mvrq.MoveRequestParam.Position = PositionName_UEL;
                    MoveRequest_Post(&Mvrq); 
                    
                    
                    Limits_SetMode(LimitMode_Normal);
                  }  
             }else
              {
                  Limits_SetMode(LimitMode_settingDEL); 
                  DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[2]);
                  LED_LogEvent(LED_Event_ResetDEL);
                  SQC_LogEvent(SQC_Event_ResetDEL);
                  SQC_Count(&me.SQC.Counter.DEL_Reseting,sizeof(me.SQC.Counter.DEL_Reseting));
                  Position_Set(PositionName_DEL,CurrentPos);

                  Mvrq.MoveType = MoveRequestType_Nolimit_RollForTime;
                  Mvrq.MoveRequestParam.Time_mS =MAXMOVETIME_MS;
                  MoveRequest_Post(&Mvrq);
              }
          }
      }
      else if(evt->Type == ErgoRemoteEventType_Stop_Down )
      {//Stop_Down
         if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq);
        }else if(evt->PressDuration < BUTTON_PUSH_2SEC && EncoderDriver_GetMoveDirection() == EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
          {         
              me.ErgoModeState = ErgoModeState_WaitRelease;

              TU32 CurrentPos = EncoderDriver_GetPosition();
              if(Position_IsPositionSet(PositionName_DEL) == true && Position_IsPositionSet(PositionName_UEL) == false)
              {
                TU32 Distance  =(TU32) labs(CurrentPos -  *Position_GetValue(PositionName_DEL));
                TS32 Distance2 =(TS32)(CurrentPos -  *Position_GetValue(PositionName_DEL));
                if((Distance < ONE_PRODUCT_TURN) || Distance2>0)
                { 
                  LED_LogEvent(LED_Event_SetImpossibleSetMode);

                  Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq); 
                }else
                {  
                  Limits_SetMode(LimitMode_settingUEL); 
                  DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[1]);
                  LED_LogEvent(LED_Event_ResetUEL);
                  SQC_Count(&me.SQC.Counter.UEL_Reseting,sizeof(me.SQC.Counter.UEL_Reseting));
                  Position_Set(PositionName_UEL,CurrentPos);
                  
                  Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                  Mvrq.MoveRequestParam.Position = PositionName_DEL;
                  MoveRequest_Post(&Mvrq);                
                }  
             }else if(Position_IsPositionSet(PositionName_DEL) == true && Position_IsPositionSet(PositionName_UEL) == true)
             {
                if(CurrentPos >= (*Position_GetValue(PositionName_DEL)-ONE_PRODUCT_TURN))
                 {
                    LED_LogEvent(LED_Event_SetImpossibleSetMode);
                   
                    Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq); 
                 }else
                  { 
                    Limits_SetMode(LimitMode_settingUEL); 
                    DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[1]);
                    LED_LogEvent(LED_Event_ResetUEL);
                    SQC_Count(&me.SQC.Counter.UEL_Reseting,sizeof(me.SQC.Counter.UEL_Reseting));
                    Position_Set(PositionName_UEL,CurrentPos);
                    
                    Mvrq.MoveType = MoveRequestType_UnRollToPosition;
                    Mvrq.MoveRequestParam.Position = PositionName_DEL;
                    MoveRequest_Post(&Mvrq);   
                    
                    Limits_SetMode(LimitMode_Normal);
                  } 
             }else
              { 
                  Limits_SetMode(LimitMode_settingUEL); 
                  DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[1]);
                  LED_LogEvent(LED_Event_ResetUEL);
                  SQC_Count(&me.SQC.Counter.UEL_Reseting,sizeof(me.SQC.Counter.UEL_Reseting));
                  Position_Set(PositionName_UEL,CurrentPos);
                                      
                  Mvrq.MoveType = MoveRequestType_Nolimit_UnRollForTime;
                  Mvrq.MoveRequestParam.Time_mS =MAXMOVETIME_MS;
                  MoveRequest_Post(&Mvrq);
              }
         }
        }   
     }
   }
    
    break;
     
    case ErgoModeState_WaitRelease :     
      if(evt->BpState == ErgoRemote_BpRelease)
      {
        me.ErgoModeState = ErgoModeState_WaitEvent;
        if(me.ChangeStateOnBpRelease == TRUE)
        {
          me.ChangeStateOnBpRelease = FALSE;
          ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
        }
      }     
      break;
       
  default:
    break;
  }
}
static void ErgonomicManager_SetNoTemporaryRemote_EventHandler(ErgoEvent_t *evt) //kendy??????
{  
  MoveRequest_s Mvrq;  

  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
             if(evt->Type == ErgoRemoteEventType_Up_Down)
      { //Up_Down any
        if(evt->BpState == ErgoRemote_BpPress)//modfiy as Xavier COLET required
        {

           if(Banker_RecordTheCurrentRemote() == TRUE)
            {
             LED_LogEvent(LED_Event_TempPaired);
              RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_RemoteEepromBackup);
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
              
              me.ErgoModeState = ErgoModeState_WaitEvent;         
              ErgonomicManager_ChangeErgoMode(ErgoMode_SetAndTemporaryRemote);  
            }

        }
      } 
      

     
    break;
     
    case ErgoModeState_WaitRelease :
      
      if(evt->BpState == ErgoRemote_BpRelease)
      {
        me.ErgoModeState = ErgoModeState_WaitEvent;
        if(me.ChangeStateOnBpRelease == TRUE)
        {
          me.ChangeStateOnBpRelease = FALSE;
          ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
        }
      }
      
  
      
      break;
    
    
  default:
    break;
  }
}

static void ErgonomicManager_SetAndNotPaired_EventHandler(ErgoEvent_t *evt)  //kendy?????
{  
  MoveRequest_s Mvrq;  

  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
      //kendy 
//        if(evt->Type == ErgoIndusRemoteEventType_Stop && evt->BpState == ErgoRemote_BpPress && evt->PressDuration >= BUTTON_PUSH_2SEC)
//        { 
//              me.ErgoModeState = ErgoModeState_WaitRelease;
//          
//              UART1ComDriver_Cmd(UARTComDriver_Enable_TxSQC,UART_BAUD_RATE_SQC);
//              SQCmesage_Print();
//              UART1ComDriver_Cmd(UARTComDriver_Disable,0);  
//        }
      
          //Prog 
        if(evt->Type == ErgoRemoteEventType_Prog) 
        {
          if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
          {
            me.ErgoModeState = ErgoModeState_WaitRelease;
            
            Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
            Mvrq.MoveRequestParam = NoMoveParam;
            MoveRequest_Post(&Mvrq);
          }else if(evt->BpState == ErgoRemote_BpRelease && evt->PressDuration <  BUTTON_PUSH_2SEC)
          { //Prog < 2sec 
            if(Banker_RecordTheCurrentRemote() == TRUE)
            {
              LED_LogEvent(LED_Event_ToUserMode);
              RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_RemoteEepromBackup);
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
              
              me.ErgoModeState = ErgoModeState_WaitEvent;         
              ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);  
            }
          }
        }
              else if(evt->Type == ErgoRemoteEventType_Up_Down)
      { //Up_Down any
        if(evt->BpState == ErgoRemote_BpPress)//modfiy as Xavier COLET required
        {

           if(Banker_RecordTheCurrentRemote() == TRUE)
            {
             LED_LogEvent(LED_Event_TempPaired);
              RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_RemoteEepromBackup);
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
              
              me.ErgoModeState = ErgoModeState_WaitEvent;         
              ErgonomicManager_ChangeErgoMode(ErgoMode_SetAndTemporaryRemote);  
            }

        }
      } 
    break;
     
    case ErgoModeState_WaitRelease :
      
      if(evt->BpState == ErgoRemote_BpRelease)
      {
        me.ErgoModeState = ErgoModeState_WaitEvent;
        if(me.ChangeStateOnBpRelease == TRUE)
        {
          me.ChangeStateOnBpRelease = FALSE;
          ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
        }
      }
      
      break;
    
    
  default:
    break;
  }
}


//nothing to do with radio 
static void ErgonomicManager_SleepMode_EventHandler(ErgoEvent_t *evt)
{  
  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
      if(evt->PairedState == RTSPROT_RECORDED)
      {
        if(SleepModeChange == true)
        {
         // SleepModeChange =false;
          ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);
        }
      } 
    break;
     
    case ErgoModeState_WaitRelease :     
      if(evt->BpState == ErgoRemote_BpRelease)
      {
        me.ErgoModeState = ErgoModeState_WaitEvent;
        if(me.ChangeStateOnBpRelease == TRUE)
        {
          me.ChangeStateOnBpRelease = FALSE;
         // ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
        }
      }
      break; 
  default:
    break;
  }
}


static void ErgonomicManager_Delivery1Mode_EventHandler(ErgoEvent_t *evt)
{  
  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
    break;
     
    case ErgoModeState_WaitRelease :     
    break; 
    
    default:
    break;
  }
}


static void ErgonomicManager_Delivery2Mode_EventHandler(ErgoEvent_t *evt)
{  
  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
    break;
     
    case ErgoModeState_WaitRelease :     
    break; 
    
    default:
    break;
  }
}

static void ErgonomicManager_Delivery3Mode_EventHandler(ErgoEvent_t *evt)
{  
  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
    break;
     
    case ErgoModeState_WaitRelease :     
    break; 
    
    default:
    break;
  }
}


static void  ErgonomicManager_MaintenanceResetingMode_EventHandler(ErgoEvent_t *evt) //ok
{  
  MoveRequest_s Mvrq;  

  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
         if(IsCurrentRemoteTheResetingRemote() == TRUE)
         {
               if((evt->Type == ErgoRemoteEventType_Up       ||
                  evt->Type == ErgoRemoteEventType_Down     ||
                  evt->Type == ErgoRemoteEventType_ModeDown ||
                  evt->Type == ErgoRemoteEventType_ModeUp   ||
                  evt->Type == ErgoRemoteEventType_Execute_U80)
                  && evt->BpState == ErgoRemote_BpPress )//Wheel Up/Down ext...
              {
                  LED_LogEvent(LED_Event_NoBlinkUserMode); //new
                  me.ErgoModeState = ErgoModeState_WaitRelease;
                  me.ChangeStateOnBpRelease = TRUE;
                  me.ErgoModeAfterRelease = ErgoMode_UserMode;
              }else if(evt->Type ==ErgoRemoteEventType_Stop && evt->BpState == ErgoRemote_BpPress)
              { 
                if(evt->PressDuration >= BUTTON_PUSH_2SEC )
                {  //stop >2S  
                  LED_LogEvent(LED_Event_ToUserMode);
                  SQC_LogEvent(SQC_Event_ResetRotationDir);
                  SQC_Count(&me.SQC.Counter.Motor_RotationResetting,sizeof(me.SQC.Counter.Motor_RotationResetting));
                  
                  me.ErgoModeState = ErgoModeState_WaitRelease;
                  me.ChangeStateOnBpRelease = TRUE;
                  me.ErgoModeAfterRelease = ErgoMode_UserMode;
                  
                  Direction_change=true;
                  me.Pdata.WheelDir = (me.Pdata.WheelDir == WheelDir_Normal) ? (WheelDir_Inverted) : (WheelDir_Normal);
                  
                  Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq); 
                }
              }else if(evt->Type == ErgoRemoteEventType_Stop_Up_Down && evt->BpState == ErgoRemote_BpPress)
              {//Stop_Up_Down >2S
                if(evt->PressDuration >= BUTTON_PUSH_2SEC)
                {
                  LED_LogEvent(LED_Event_ToUserMode);
                  
                  SQC_LogEvent(SQC_Event_ErgoWheelDirchange);
                  SQC_Count(&me.SQC.Counter.Wheel_RotationResetting,sizeof(me.SQC.Counter.Wheel_RotationResetting));
                  
                  me.ErgoModeState = ErgoModeState_WaitRelease;
                  me.ChangeStateOnBpRelease = TRUE;
                  me.ErgoModeAfterRelease = ErgoMode_UserMode;
                  
                  me.Pdata.WheelDir = (me.Pdata.WheelDir == WheelDir_Normal) ? (WheelDir_Inverted) : (WheelDir_Normal);
                  
                  Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq);
                }
              }
              else if(evt->Type == ErgoRemoteEventType_Stop_Down && evt->BpState == ErgoRemote_BpPress )
              {//Stop_Down >2S
                if(evt->PressDuration >= BUTTON_PUSH_2SEC)
                {
                    if(User_Mode == User_ModeA_Us_Tilit)
                    {
                      LED_LogEvent(LED_Event_ChangeToUserModeB);
                      User_Mode = User_ModeB_Roller;
                    }else 
                    {
                      LED_LogEvent(LED_Event_ChangeToUserModeA);
                      User_Mode = User_ModeA_Us_Tilit;
                    }
                      
                    RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Speed_UserModeChange);
                    
                    me.ErgoModeState = ErgoModeState_WaitRelease;
                    me.ChangeStateOnBpRelease = TRUE;
                    me.ErgoModeAfterRelease = ErgoMode_UserMode;
                    
                    Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq);
                }
              }
         }
    break;
     
    case ErgoModeState_WaitRelease :     
      if(evt->BpState == ErgoRemote_BpRelease)
      {
        me.ErgoModeState = ErgoModeState_WaitEvent;
        if(me.ChangeStateOnBpRelease == TRUE)
        {
          me.ChangeStateOnBpRelease = FALSE;
          ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
        }
      }  
      break;
    
  default:
    break;
  }
}
    


static void  ErgonomicManager_ConfigMode_EventHandler(ErgoEvent_t *evt)
{
  MoveRequest_s Mvrq; 
  
  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
      if(evt->Type == ErgoRemoteEventType_Prog) 
      {
        if(evt->BpState == ErgoRemote_BpRelease && evt->PressDuration < BUTTON_PUSH_2SEC)
        {
          if(evt->PairedState == RTSPROT_RECORDED)
          {
            if(ConfigMode==true && IsCurrentRemoteTheResetingRemote() == TRUE)
            {
               ConfigMode=false;
               LED_LogEvent(LED_Event_NoBlinkUserMode);
               ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);            
            }
            else
            {          
                if(Banker_DeleteTheCurrentRemote())
                { 
                    LED_LogEvent(LED_Event_CantAddOrDeletTeleco);
                    
                    Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq);
                }else
                {
                    LED_LogEvent(LED_Event_ToUserMode);
                    
                    Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                    Mvrq.MoveRequestParam = NoMoveParam;
                    MoveRequest_Post(&Mvrq);
                }              
                    ConfigMode=false;
                ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);
                RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_RemoteEepromBackup);
            }
          }    
          else if(evt->PairedState == RTSPROT_UNRECORDED)
          {
            if(Banker_RecordTheCurrentRemote() == TRUE)
            {
              LED_LogEvent(LED_Event_ToUserMode); //add
              SQC_LogEvent(SQC_Event_AddTeleco);
              
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }
            else
            {
              LED_LogEvent(LED_Event_CantAddOrDeletTeleco);
              SQC_LogEvent(SQC_Event_CantAddTeleco);
              
              Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;//add
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);    
            }
            ConfigMode=false;
            ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);
            RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_RemoteEepromBackup);
          }          
        }
      }else if(evt->Type == ErgoRemoteEventType_Stop_Up_Down && evt->PairedState == RTSPROT_RECORDED)
          {
            if(evt->PressDuration >= BUTTON_PUSH_2SEC)
            {     
              me.ErgoModeState = ErgoModeState_WaitRelease;
              me.ChangeStateOnBpRelease = TRUE;
              me.ErgoModeAfterRelease = ErgoMode_Delivery3Mode;
              
              RADIO_OUTLINE=TRUE;
 
              Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
              Mvrq.MoveRequestParam = NoMoveParam;
              MoveRequest_Post(&Mvrq);
            }
          } 
      else if((evt->Type == ErgoRemoteEventType_Up_Down ||
               evt->Type == ErgoRemoteEventType_Stop_Up ||
               evt->Type == ErgoRemoteEventType_Stop_Down) && evt->PairedState == RTSPROT_RECORDED)
          {
            if(evt->PressDuration >= BUTTON_PUSH_2SEC)
            {    
              LED_LogEvent(LED_Event_NoBlinkUserMode);
              me.ErgoModeState = ErgoModeState_WaitRelease;
              
              me.ChangeStateOnBpRelease = TRUE;
              me.ErgoModeAfterRelease = ErgoMode_UserMode;  
            }
          }
     else if(evt->Type == ErgoRemoteEventType_Stop && evt->PairedState == RTSPROT_RECORDED)
          {
            if(evt->PressDuration >= BUTTON_PUSH_5SEC)
            { 
              LED_LogEvent(LED_Event_NoBlinkUserMode);
              me.ErgoModeState = ErgoModeState_WaitRelease;
              
              me.ChangeStateOnBpRelease = TRUE;
              me.ErgoModeAfterRelease = ErgoMode_UserMode;  
            }
          }
     else if(evt->Type ==NormalMode_Prog)//kendy
     {
       //////////////////////////
       if(evt->BpState == ErgoRemote_BpRelease && evt->PressDuration < BUTTON_PUSH_2SEC)
       {
         
         if(evt->PairedState == RTSPROT_RECORDED)
         {
           if(Banker_DeleteTheCurrentSensor())
           { 
           ;
           }            
               LED_LogEvent(LED_Event_ToUserMode);
             
             Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
             Mvrq.MoveRequestParam = NoMoveParam;
             MoveRequest_Post(&Mvrq);
               ConfigMode=false;
           ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);
           RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_RemoteEepromBackup);
           
         }  else if(evt->PairedState == RTSPROT_UNRECORDED)
         {
           if(Banker_RecordTheCurrentSensor() == TRUE)
           {
            // LED_LogEvent(LED_Event_ToUserMode); //add
             SQC_LogEvent(SQC_Event_AddTeleco);
             
             Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
             Mvrq.MoveRequestParam = NoMoveParam;
             MoveRequest_Post(&Mvrq);
           }
           else
           {
           //  LED_LogEvent(LED_Event_CantAddOrDeletTeleco);
             SQC_LogEvent(SQC_Event_CantAddTeleco);
             
             Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;//add
             Mvrq.MoveRequestParam = NoMoveParam;
             MoveRequest_Post(&Mvrq);    
           }
           ConfigMode=false;
           ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);
           RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_RemoteEepromBackup);
         }  
       }         
        else if(evt->BpState == ErgoRemote_BpPress && evt->PressDuration > BUTTON_PUSH_7SEC)
       {

             Banker_DeleteOnlySensors();           
               LED_LogEvent(LED_Event_ToUserMode);
             
             Mvrq.MoveType = MoveRequestType_Nolimit_DoubleShortFeedBack;
             Mvrq.MoveRequestParam = NoMoveParam;
             MoveRequest_Post(&Mvrq);
               ConfigMode=false;
           ErgonomicManager_ChangeErgoMode(ErgoMode_UserMode);
           RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_RemoteEepromBackup);
           

 
       } 
      ///////////////////////
      }
      break;
      
    case ErgoModeState_WaitRelease :
      if(evt->BpState == ErgoRemote_BpRelease)
      {
        me.ErgoModeState = ErgoModeState_WaitEvent;
        if(me.ChangeStateOnBpRelease == TRUE)
        {
          me.ChangeStateOnBpRelease = FALSE;
          ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
        }
      }
      
      break;
         
    default:
      break;
  }
}
static void  ErgonomicManager_TiltingspeedadjustmentMode_EventHandler(ErgoEvent_t *evt) //kendy????????
{  
  MoveRequest_s Mvrq;  

  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
      
         if(IsCurrentRemoteTheResetingRemote() == TRUE)
         {
//            //UP 
//            if(evt->Type == ErgoRemoteEventType_Up)
//            {
//               if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
//                {
//                   Speed_MoveCycle=TRUE;
//                }
//               else if(evt->PressDuration >=  BUTTON_PUSH_2SEC && evt->BpState == ErgoRemote_BpPress)
//               {
//                    me.ErgoModeState = ErgoModeState_WaitRelease;
//                    if(Curr_SpeedMode != SPEED_28RPM)  //RU24
//                    {
//                      LED_LogEvent(LED_Event_SpeedINC);
//                      
//                      Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
//                      Mvrq.MoveRequestParam = NoMoveParam;
//                      MoveRequest_Post(&Mvrq); 
//                      
//                      SpeedCycle_last=SpeedADJCycle_timems-100u;
//                    }else
//                    {
//                      LED_LogEvent(LED_Event_SetImpossibleSetMode);
//                      
//                      Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
//                      Mvrq.MoveRequestParam = NoMoveParam;
//                      MoveRequest_Post(&Mvrq); 
//                    }
//
//                      Curr_SpeedMode--;
//                    if(Curr_SpeedMode == SPEED_10RPM)
//                      Current_Speed = MININUM_10_SPEED; 
//                    else if(Curr_SpeedMode == SPEED_12RPM)
//                      Current_Speed = MIDDLE_12_SPEED;
//                    else if(Curr_SpeedMode == SPEED_14RPM)
//                      Current_Speed = MIDDLE_14_SPEED; 
//                    else if(Curr_SpeedMode == SPEED_16RPM)
//                      Current_Speed = MIDDLE_16_SPEED; 
//                    else if(Curr_SpeedMode == SPEED_18RPM)
//                      Current_Speed = MIDDLE_18_SPEED;
//                    else if(Curr_SpeedMode == SPEED_20RPM)
//                      Current_Speed = MIDDLE_20_SPEED; 
//                    else if(Curr_SpeedMode == SPEED_22RPM)
//                      Current_Speed = MIDDLE_22_SPEED;
//                    else if(Curr_SpeedMode == SPEED_24RPM)
//                      Current_Speed = MIDDLE_24_SPEED;
//                    else if(Curr_SpeedMode == SPEED_26RPM)
//                      Current_Speed = MIDDLE_26_SPEED; 
//                    else {
//                      Curr_SpeedMode = SPEED_28RPM;
//                      Current_Speed  = STANDARD_28_SPEED;
//                    }
//                    LOG_PRINT1("[LOG][Ergo] Curr_SpeedMode = %d\n", Curr_SpeedMode);
//               }
//            }
//            //DOWN
//            else if(evt->Type == ErgoRemoteEventType_Down)
//            {
//               if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
//                {
//                     Speed_MoveCycle=TRUE;
//                }
//               else if(evt->PressDuration >=  BUTTON_PUSH_2SEC && evt->BpState == ErgoRemote_BpPress)
//               {
//                    me.ErgoModeState = ErgoModeState_WaitRelease;
//                    if(Curr_SpeedMode != SPEED_10RPM)
//                    { 
//                      LED_LogEvent(LED_Event_SpeedDEC);
//                      
//                      Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
//                      Mvrq.MoveRequestParam = NoMoveParam;
//                      MoveRequest_Post(&Mvrq); 
//                      SpeedCycle_last=SpeedADJCycle_timems-100u;
//                    }else
//                    {
//                      LED_LogEvent(LED_Event_SetImpossibleSetMode);
//                       
//                      Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
//                      Mvrq.MoveRequestParam = NoMoveParam;
//                      MoveRequest_Post(&Mvrq); 
//                    } 
//                    
//                      Curr_SpeedMode++;
//                      if(Curr_SpeedMode == SPEED_28RPM)                    
//                        Current_Speed = STANDARD_28_SPEED;   
//                      else if(Curr_SpeedMode == SPEED_26RPM)
//                        Current_Speed = MIDDLE_26_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_24RPM)
//                        Current_Speed = MIDDLE_24_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_22RPM)
//                        Current_Speed = MIDDLE_22_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_20RPM)
//                        Current_Speed = MIDDLE_20_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_18RPM)
//                        Current_Speed = MIDDLE_18_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_16RPM)
//                        Current_Speed = MIDDLE_16_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_14RPM)
//                        Current_Speed = MIDDLE_14_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_12RPM)
//                        Current_Speed = MIDDLE_12_SPEED; 
//                      else{Curr_SpeedMode = SPEED_10RPM;
//                        Current_Speed = MININUM_10_SPEED; 
//                      }  
//                     LOG_PRINT1("[LOG][Ergo] Curr_SpeedMode = %d\n", Curr_SpeedMode);
//               }
//             }
                 
            // Stop
            if(evt->Type == ErgoRemoteEventType_Stop)
            {
              if(evt->PressDuration < BUTTON_PUSH_500ms && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
              {
                Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                Mvrq.MoveRequestParam = NoMoveParam;
                MoveRequest_Post(&Mvrq);
                
                SpeedCycle_last=SpeedADJCycle_timems-1000u;
              }
              else if(evt->PressDuration >= BUTTON_PUSH_2SEC && EncoderDriver_GetMoveDirection() == EncoderDriver_Stop)
              { // stop>2s 退出
//              if(evt->PressDuration >= BUTTON_PUSH_2SEC && EncoderDriver_GetMoveDirection() == EncoderDriver_Stop)
//              { // stop>2s 退出
               // LED_LogEvent(LED_Event_ToUserMode);
                
                Speed_MoveCycle=false;
		SpeedChange=false;   
                RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Speed_UserModeChange);
                
                me.ErgoModeState = ErgoModeState_WaitRelease;               
                me.ChangeStateOnBpRelease = TRUE;
                me.ErgoModeAfterRelease = ErgoMode_UserMode;
                
                Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                Mvrq.MoveRequestParam = NoMoveParam;
                MoveRequest_Post(&Mvrq);
              }
            }
         }
   
    break;
     
    case ErgoModeState_WaitRelease :
      
      if(evt->BpState == ErgoRemote_BpRelease)
      {
        me.ErgoModeState = ErgoModeState_WaitEvent;
        if(me.ChangeStateOnBpRelease == TRUE)
        {
          me.ChangeStateOnBpRelease = FALSE;
          ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
        }
      }
      
      break;
      
  default:
    break;
  }
}

       
static void  ErgonomicManager_SpeedAdjustmentMode_EventHandler(ErgoEvent_t *evt) //ok
{  
  MoveRequest_s Mvrq;  

  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
         if(IsCurrentRemoteTheResetingRemote() == TRUE)
         {
            //UP 
            if(evt->Type == ErgoRemoteEventType_Up)
            {

              ////////////////////////////kendy/////////////
               if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
                {
                   Speed_MoveCycle=TRUE;
                }
               else if(evt->PressDuration >=  BUTTON_PUSH_2SEC && evt->BpState == ErgoRemote_BpPress)
               {
                    me.ErgoModeState = ErgoModeState_WaitRelease;
                    if(Curr_SpeedMode != SPEED_28RPM)  //RU24
                    {
                      LED_LogEvent(LED_Event_SpeedINC);
                      
                      Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq); 
                      
                      SpeedCycle_last=SpeedADJCycle_timems-100u;
                    }else
                    {
                      LED_LogEvent(LED_Event_SetImpossibleSetMode);
                      
                      Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq); 
                    }

                      Curr_SpeedMode--;
                    if(Curr_SpeedMode == SPEED_10RPM)
                      Current_Speed = MININUM_10_SPEED; 
                    else if(Curr_SpeedMode == SPEED_12RPM)
                      Current_Speed = MIDDLE_12_SPEED;
                    else if(Curr_SpeedMode == SPEED_14RPM)
                      Current_Speed = MIDDLE_14_SPEED; 
                    else if(Curr_SpeedMode == SPEED_16RPM)
                      Current_Speed = MIDDLE_16_SPEED; 
                    else if(Curr_SpeedMode == SPEED_18RPM)
                      Current_Speed = MIDDLE_18_SPEED;
                    else if(Curr_SpeedMode == SPEED_20RPM)
                      Current_Speed = MIDDLE_20_SPEED; 
                    else if(Curr_SpeedMode == SPEED_22RPM)
                      Current_Speed = MIDDLE_22_SPEED;
                    else if(Curr_SpeedMode == SPEED_24RPM)
                      Current_Speed = MIDDLE_24_SPEED;
                    else if(Curr_SpeedMode == SPEED_26RPM)
                      Current_Speed = MIDDLE_26_SPEED; 
                    else {
                      Curr_SpeedMode = SPEED_28RPM;
                      Current_Speed  = STANDARD_28_SPEED;
                    }
                    LOG_PRINT1("[LOG][Ergo] Curr_SpeedMode = %d\n", Curr_SpeedMode);
               }
            }
            //DOWN
            else if(evt->Type == ErgoRemoteEventType_Down)
            {
               if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
                {
                     Speed_MoveCycle=TRUE;
                }
               else if(evt->PressDuration >=  BUTTON_PUSH_2SEC && evt->BpState == ErgoRemote_BpPress)
               {
                    me.ErgoModeState = ErgoModeState_WaitRelease;
                    if(Curr_SpeedMode != SPEED_10RPM)
                    { 
                      LED_LogEvent(LED_Event_SpeedDEC);
                      
                      Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq); 
                      SpeedCycle_last=SpeedADJCycle_timems-100u;
                    }else
                    {
                      LED_LogEvent(LED_Event_SetImpossibleSetMode);
                       
                      Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq); 
                    } 
                    
                      Curr_SpeedMode++;
                      if(Curr_SpeedMode == SPEED_28RPM)                    
                        Current_Speed = STANDARD_28_SPEED;   
                      else if(Curr_SpeedMode == SPEED_26RPM)
                        Current_Speed = MIDDLE_26_SPEED; 
                      else if(Curr_SpeedMode == SPEED_24RPM)
                        Current_Speed = MIDDLE_24_SPEED; 
                      else if(Curr_SpeedMode == SPEED_22RPM)
                        Current_Speed = MIDDLE_22_SPEED; 
                      else if(Curr_SpeedMode == SPEED_20RPM)
                        Current_Speed = MIDDLE_20_SPEED; 
                      else if(Curr_SpeedMode == SPEED_18RPM)
                        Current_Speed = MIDDLE_18_SPEED; 
                      else if(Curr_SpeedMode == SPEED_16RPM)
                        Current_Speed = MIDDLE_16_SPEED; 
                      else if(Curr_SpeedMode == SPEED_14RPM)
                        Current_Speed = MIDDLE_14_SPEED; 
                      else if(Curr_SpeedMode == SPEED_12RPM)
                        Current_Speed = MIDDLE_12_SPEED; 
                      else{Curr_SpeedMode = SPEED_10RPM;
                        Current_Speed = MININUM_10_SPEED; 
                      }  
                     LOG_PRINT1("[LOG][Ergo] Curr_SpeedMode = %d\n", Curr_SpeedMode);
               }
             }
                 
            // Stop
            else if(evt->Type == ErgoRemoteEventType_Stop)
            {
              if(evt->PressDuration < BUTTON_PUSH_500ms && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
              {
                Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                Mvrq.MoveRequestParam = NoMoveParam;
                MoveRequest_Post(&Mvrq);
                
                SpeedCycle_last=SpeedADJCycle_timems-1000u;
              }
              else if(evt->PressDuration >= BUTTON_PUSH_2SEC && EncoderDriver_GetMoveDirection() == EncoderDriver_Stop)
              { // stop>2s 退出
                LED_LogEvent(LED_Event_ToUserMode);
                
                Speed_MoveCycle=false;
		SpeedChange=false;   
                RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Speed_UserModeChange);
                
                me.ErgoModeState = ErgoModeState_WaitRelease;               
                me.ChangeStateOnBpRelease = TRUE;
                me.ErgoModeAfterRelease = ErgoMode_UserMode;
                
                Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                Mvrq.MoveRequestParam = NoMoveParam;
                MoveRequest_Post(&Mvrq);
              }
            }
                  //"UP_Stop_Down"command
      else if(evt->Type == ErgoRemoteEventType_Stop_Up_Down && evt->BpState == ErgoRemote_BpPress)
      {

          if((evt->PressDuration >= BUTTON_PUSH_2SEC)&& evt->BpState == ErgoRemote_BpPress)//kendy???????
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          
          me.ChangeStateOnBpRelease = TRUE;
          me.ErgoModeAfterRelease = ErgoMode_Tiltingspeedadjustment;         
             SpeedCycle_last   = 0U;//kendy   
                Speed_MoveCycle=false;
		SpeedChange=false;  
          Pre_SpeedMode=Curr_SpeedMode; //kendy2??????
          
          Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
          Mvrq.MoveRequestParam = NoMoveParam;
          MoveRequest_Post(&Mvrq); 
          RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Speed_UserModeChange);
                

                

        }
      }
         }
   
    break;
     
    case ErgoModeState_WaitRelease :
      
      if(evt->BpState == ErgoRemote_BpRelease)
      {
        me.ErgoModeState = ErgoModeState_WaitEvent;
        if(me.ChangeStateOnBpRelease == TRUE)
        {
          me.ChangeStateOnBpRelease = FALSE;
          ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
        }
      }
      
      break;
      
  default:
    break;
  }
}




static void ErgonomicManager_ErgoMode_ReSettingUEL_EventHandler(ErgoEvent_t *evt)
{
  MoveRequest_s Mvrq; 

  switch(me.ErgoModeState)
  {
  case ErgoModeState_WaitEvent :
 
    if(IsCurrentRemoteTheResetingRemote() == TRUE)
    {    
          //go to
      if(evt->U80Function == U80_Go_To)
        {
            //me.ErgoModeState = ErgoModeState_WaitRelease;
            Mvrq.MoveRequestParam.GOTO = evt->U80_OptionValue;
           if(Mvrq.MoveRequestParam.GOTO <= 200u)
              {
               if(Mvrq.MoveRequestParam.GOTO == 0u) //go to 0%
                {                                 
                      TU32 CurrentPosition = EncoderDriver_GetPosition(); 
                      Limits_ComputeLimitsState(&CurrentPosition);         
                      Limits_State_e LimisState = Limits_GetLimitsState(); 
                      //DEBUG_PRINT1("[DBG] LimisState = %d\n",LimisState);
                      
                      if(LimisState == Limits_State_Between)      
                      {
                          if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpRelease)
                          {
                            Mvrq.MoveType = MoveRequestType_ShortRoll;
                            Mvrq.MoveRequestParam = NoMoveParam;
                            MoveRequest_Post(&Mvrq);
                          }
                          else if(evt->PressDuration >= BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
                          {
                            me.DeadmanModeStarted = TRUE;
                            Mvrq.MoveType = MoveRequestType_RollToLimits;
                            Mvrq.MoveRequestParam.Limit = ModeLimitName_UpLimit;
                            MoveRequest_Post(&Mvrq);         
                          }
                          else if(evt->BpState == ErgoRemote_BpRelease)
                          {
                            Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                            Mvrq.MoveRequestParam = NoMoveParam;
                            MoveRequest_Post(&Mvrq);  
                          }                  
                      }
                      else if(LimisState == Limits_State_Below||LimisState == Limits_State_OnDown) 
                      {

                        if(EncoderDriver_GetMoveDirection() == EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
                        {
                            //me.ErgoModeState = ErgoModeState_WaitRelease;
                            Mvrq.MoveType = MoveRequestType_RollToLimits;
                            Mvrq.MoveRequestParam.Limit = ModeLimitName_UpLimit;
                            MoveRequest_Post(&Mvrq);
                        }else if(evt->BpState == ErgoRemote_BpRelease)
                          {
                            Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                            Mvrq.MoveRequestParam = NoMoveParam;
                            MoveRequest_Post(&Mvrq);  
                          } 
                      }               
                }
                else if(Mvrq.MoveRequestParam.GOTO == 200u)//go to 100%
                {
                     TU32 CurrentPosition = EncoderDriver_GetPosition(); 
                    Limits_ComputeLimitsState(&CurrentPosition);         
                    Limits_State_e LimisState = Limits_GetLimitsState(); 
                    //DEBUG_PRINT1("[DBG] LimisState = %d\n",LimisState);
                    
                    if(LimisState == Limits_State_Between)        
                    {
                        if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpRelease)
                        {
                          Mvrq.MoveType = MoveRequestType_ShortUnRoll;
                          Mvrq.MoveRequestParam = NoMoveParam;
                          MoveRequest_Post(&Mvrq);
                        }
                        else if(evt->PressDuration >= BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
                        {
                          me.DeadmanModeStarted = TRUE;
                          Mvrq.MoveType = MoveRequestType_UnRollToLimits;
                          Mvrq.MoveRequestParam.Limit = ModeLimitName_DownLimit;
                          MoveRequest_Post(&Mvrq);         
                        }
                        else if(evt->BpState == ErgoRemote_BpRelease)
                        {
                          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                          Mvrq.MoveRequestParam = NoMoveParam;
                          MoveRequest_Post(&Mvrq);  
                        }                  
                    }   
                  }
                  else
                      {
                          SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));        
                          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                          Mvrq.MoveRequestParam = NoMoveParam;
                          MoveRequest_Post(&Mvrq);  
                          
                          me.ErgoModeState = ErgoModeState_WaitRelease;
                      } 

            }
           else//go to >100%
                {
                  SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));
                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq);
                  me.ErgoModeState = ErgoModeState_WaitRelease;
                }  
        }      //U80  move of + -
      else if(evt->Type == ErgoRemoteEventType_Execute_U80) 
      {
        if(evt->U80Function == U80_Move_Of && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          TS8 MoveOff =  (evt->U80_OptionValue > 128u) ? (-(evt->U80_OptionValue-128u)):(evt->U80_OptionValue );
          MoveOff *= me.Pdata.WheelDir;
          if(MoveOff < 0)
          {
            Mvrq.MoveType = MoveRequestType_UnRolloff;
            Mvrq.MoveRequestParam.MoveOff = (TU8) (abs((int)MoveOff));
            MoveRequest_Post(&Mvrq);
          }
          else
          {
            Mvrq.MoveType = MoveRequestType_Rolloff;
            Mvrq.MoveRequestParam.MoveOff = (TU8) (abs((int)MoveOff));
            MoveRequest_Post(&Mvrq);
          }
        }
      }  
      else{
               //UP
            if(evt->Type == ErgoRemoteEventType_Up  || evt->Type == ErgoRemoteEventType_ModeUp)  
            {
                TU32 CurrentPosition = EncoderDriver_GetPosition();  
                Limits_ComputeLimitsState(&CurrentPosition);         
                Limits_State_e LimisState = Limits_GetLimitsState(); 
                //DEBUG_PRINT1("[DBG] LimisState = %d\n",LimisState);
                
                if(LimisState == Limits_State_Between)      
                {
                    if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpRelease)
                    {
                      Mvrq.MoveType = MoveRequestType_ShortRoll;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq);
                    }
                    else if(evt->PressDuration >= BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
                    {
                      me.DeadmanModeStarted = TRUE;
                      Mvrq.MoveType = MoveRequestType_RollToLimits;
                      Mvrq.MoveRequestParam.Limit = ModeLimitName_UpLimit;
                      MoveRequest_Post(&Mvrq);         
                    }
                    else if(evt->BpState == ErgoRemote_BpRelease)
                    {
                      Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq);  
                    }                  
                }
		else if(LimisState == Limits_State_Below||LimisState == Limits_State_OnDown) 
	        {

                  if(EncoderDriver_GetMoveDirection() == EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
                  {
                      //me.ErgoModeState = ErgoModeState_WaitRelease;
                      Mvrq.MoveType = MoveRequestType_RollToLimits;
                      Mvrq.MoveRequestParam.Limit = ModeLimitName_UpLimit;
                      MoveRequest_Post(&Mvrq);
                  }else if(evt->BpState == ErgoRemote_BpRelease)
                    {
                      Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq);  
                    } 
                }            
            }
            //Down
            else if(evt->Type == ErgoRemoteEventType_Down || evt->Type == ErgoRemoteEventType_ModeDown)
            {
                TU32 CurrentPosition = EncoderDriver_GetPosition();
                Limits_ComputeLimitsState(&CurrentPosition);        
                Limits_State_e LimisState = Limits_GetLimitsState(); 
                //DEBUG_PRINT1("[DBG] LimisState = %d\n",LimisState);
                
                if(LimisState == Limits_State_Between)        
                {
                    if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpRelease)
                    {
                      Mvrq.MoveType = MoveRequestType_ShortUnRoll;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq);
                    }
                    else if(evt->PressDuration >= BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
                    {
                      me.DeadmanModeStarted = TRUE;
                      Mvrq.MoveType = MoveRequestType_UnRollToLimits;
                      Mvrq.MoveRequestParam.Limit = ModeLimitName_DownLimit;
                      MoveRequest_Post(&Mvrq);         
                    }
                    else if(evt->BpState == ErgoRemote_BpRelease)
                    {
                      Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq);  
                    }                  
                }
            }
            // Stop
            else if(evt->Type == ErgoRemoteEventType_Stop)
            {
              if(evt->PressDuration <= BUTTON_PUSH_500ms && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
              {
                Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                Mvrq.MoveRequestParam = NoMoveParam;
                MoveRequest_Post(&Mvrq);
              }
              else if(evt->PressDuration >= BUTTON_PUSH_2SEC)
              {
                me.ErgoModeState = ErgoModeState_WaitRelease;
                
                TU32 CurrentPos = EncoderDriver_GetPosition();
                
                if(me.ErgoMode == ErgoMode_ReSettingUEL)
                {
                  LED_LogEvent(LED_Event_ToUserMode);
                  SQC_Count(&me.SQC.Counter.UEL_Reseting,sizeof(me.SQC.Counter.UEL_Reseting));
                  Position_Set(PositionName_UEL,CurrentPos);
                }
                else
                {
                  trap();
                }
                me.ErgoModeState = ErgoModeState_WaitRelease;
                me.ChangeStateOnBpRelease = TRUE;
                me.ErgoModeAfterRelease = ErgoMode_UserMode;
                
                Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                Mvrq.MoveRequestParam = NoMoveParam;
                MoveRequest_Post(&Mvrq);
              }
            }
     }

    }
    break;
    
    
  case ErgoModeState_WaitRelease :
    if(evt->BpState == ErgoRemote_BpRelease)
    {
      me.ErgoModeState = ErgoModeState_WaitEvent;
      if(me.ChangeStateOnBpRelease == TRUE)
      {
        me.ChangeStateOnBpRelease = FALSE;
        ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
      }
      
      if(me.DeadmanModeStarted == TRUE)
      {
        me.DeadmanModeStarted = FALSE;
        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
        Mvrq.MoveRequestParam = NoMoveParam;
        MoveRequest_Post(&Mvrq);
      }  
    }
    break;
        
  default:
    break;
  }
}



static void ErgonomicManager_ErgoMode_ReSettingDEL_EventHandler(ErgoEvent_t *evt)
{
  MoveRequest_s Mvrq; 

  switch(me.ErgoModeState)
  {
  case ErgoModeState_WaitEvent :
 
    if(IsCurrentRemoteTheResetingRemote() == TRUE)
    {    
          //go to
      if(evt->U80Function == U80_Go_To)
        {
            //me.ErgoModeState = ErgoModeState_WaitRelease;
            Mvrq.MoveRequestParam.GOTO = evt->U80_OptionValue;
           if(Mvrq.MoveRequestParam.GOTO <= 200u)
              {
               if(Mvrq.MoveRequestParam.GOTO == 0u) //go to 0%
                {                                 
                    TU32 CurrentPosition = EncoderDriver_GetPosition(); 
                    Limits_ComputeLimitsState(&CurrentPosition);         
                    Limits_State_e LimisState = Limits_GetLimitsState(); 
                    //DEBUG_PRINT1("[DBG] LimisState = %d\n",LimisState);
                    
                    if(LimisState == Limits_State_Between)        
                    {
                        if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpRelease)
                        {
                          Mvrq.MoveType = MoveRequestType_ShortRoll;
                          Mvrq.MoveRequestParam = NoMoveParam;
                          MoveRequest_Post(&Mvrq);
                        }
                        else if(evt->PressDuration >= BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
                        {
                          me.DeadmanModeStarted = TRUE;
                          Mvrq.MoveType = MoveRequestType_RollToLimits;
                          Mvrq.MoveRequestParam.Limit = ModeLimitName_UpLimit;
                          MoveRequest_Post(&Mvrq);         
                        }
                        else if(evt->BpState == ErgoRemote_BpRelease)
                        {
                          Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                          Mvrq.MoveRequestParam = NoMoveParam;
                          MoveRequest_Post(&Mvrq);  
                        }                  
                    }   
                }
                else if(Mvrq.MoveRequestParam.GOTO == 200u)//go to 100%
                {
                  TU32 CurrentPosition = EncoderDriver_GetPosition();  
                  Limits_ComputeLimitsState(&CurrentPosition);        
                  Limits_State_e LimisState = Limits_GetLimitsState(); 
                  //DEBUG_PRINT1("[DBG] LimisState = %d\n",LimisState);
                  
                  if(LimisState == Limits_State_Between)       
                  {
                      if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_ShortUnRoll;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);
                      }
                      else if(evt->PressDuration >= BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
                      {
                        me.DeadmanModeStarted = TRUE;
                        Mvrq.MoveType = MoveRequestType_UnRollToLimits;
                        Mvrq.MoveRequestParam.Limit = ModeLimitName_DownLimit;
                        MoveRequest_Post(&Mvrq);         
                      }
                      else if(evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                      }                  
                  }
		 else if(LimisState == Limits_State_Above||LimisState == Limits_State_OnUp) 
	         {
                      if(EncoderDriver_GetMoveDirection() == EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
                      { 
                          me.DeadmanModeStarted = TRUE;
                          Mvrq.MoveType = MoveRequestType_UnRollToLimits;
                          Mvrq.MoveRequestParam.Limit = ModeLimitName_DownLimit;
                          MoveRequest_Post(&Mvrq);
                      }else if(evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                      }   
                 }       
                }
                else
                    {
                        SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));        
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                        
                        me.ErgoModeState = ErgoModeState_WaitRelease;
                    } 

            }
           else//go to >100%
                {
                  SQC_Count(&me.SQC.Counter.RTS_Stop,sizeof(me.SQC.Counter.RTS_Stop));
                  Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                  Mvrq.MoveRequestParam = NoMoveParam;
                  MoveRequest_Post(&Mvrq);
                  me.ErgoModeState = ErgoModeState_WaitRelease;
                }  
        }      //U80  move of + -
      else if(evt->Type == ErgoRemoteEventType_Execute_U80) 
      {
        if(evt->U80Function == U80_Move_Of && evt->BpState == ErgoRemote_BpPress)
        {
          me.ErgoModeState = ErgoModeState_WaitRelease;
          TS8 MoveOff =  (evt->U80_OptionValue > 128u) ? (-(evt->U80_OptionValue-128u)):(evt->U80_OptionValue );
          MoveOff *= me.Pdata.WheelDir;
          if(MoveOff < 0)
          {
            Mvrq.MoveType = MoveRequestType_UnRolloff;
            Mvrq.MoveRequestParam.MoveOff = (TU8) (abs((int)MoveOff));
            MoveRequest_Post(&Mvrq);
          }
          else
          {
            Mvrq.MoveType = MoveRequestType_Rolloff;
            Mvrq.MoveRequestParam.MoveOff = (TU8) (abs((int)MoveOff));
            MoveRequest_Post(&Mvrq);
          }
        }
      }  
      else{
               //UP
            if(evt->Type == ErgoRemoteEventType_Up  || evt->Type == ErgoRemoteEventType_ModeUp)  
            {
                TU32 CurrentPosition = EncoderDriver_GetPosition();  
                Limits_ComputeLimitsState(&CurrentPosition);         
                Limits_State_e LimisState = Limits_GetLimitsState(); 
                //DEBUG_PRINT1("[DBG] LimisState = %d\n",LimisState);
                
                if(LimisState == Limits_State_Between)        
                {
                    if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpRelease)
                    {
                      Mvrq.MoveType = MoveRequestType_ShortRoll;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq);
                    }
                    else if(evt->PressDuration >= BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
                    {
                      me.DeadmanModeStarted = TRUE;
                      Mvrq.MoveType = MoveRequestType_RollToLimits;
                      Mvrq.MoveRequestParam.Limit = ModeLimitName_UpLimit;
                      MoveRequest_Post(&Mvrq);         
                    }
                    else if(evt->BpState == ErgoRemote_BpRelease)
                    {
                      Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                      Mvrq.MoveRequestParam = NoMoveParam;
                      MoveRequest_Post(&Mvrq);  
                    }                  
                }
          
            }
            //Down
            else if(evt->Type == ErgoRemoteEventType_Down || evt->Type == ErgoRemoteEventType_ModeDown)
            {
                  TU32 CurrentPosition = EncoderDriver_GetPosition();  
                  Limits_ComputeLimitsState(&CurrentPosition);         
                  Limits_State_e LimisState = Limits_GetLimitsState(); 
                  //DEBUG_PRINT1("[DBG] LimisState = %d\n",LimisState);
                  
                  if(LimisState == Limits_State_Between)        
                  {
                      if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_ShortUnRoll;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);
                      }
                      else if(evt->PressDuration >= BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
                      {
                        me.DeadmanModeStarted = TRUE;
                        Mvrq.MoveType = MoveRequestType_UnRollToLimits;
                        Mvrq.MoveRequestParam.Limit = ModeLimitName_DownLimit;
                        MoveRequest_Post(&Mvrq);         
                      }
                      else if(evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                      }                  
                  }//else if(LimisState == Limits_State_Below||LimisState == Limits_State_OnDown)  //高于限位 或 在上限位   联动模式
		 else if(LimisState == Limits_State_Above||LimisState == Limits_State_OnUp) 
	         {

                      if(EncoderDriver_GetMoveDirection() == EncoderDriver_Stop && evt->BpState == ErgoRemote_BpPress)
                      { 
                          me.DeadmanModeStarted = TRUE;
                          Mvrq.MoveType = MoveRequestType_UnRollToLimits;
                          Mvrq.MoveRequestParam.Limit = ModeLimitName_DownLimit;
                          MoveRequest_Post(&Mvrq);
                      }else if(evt->BpState == ErgoRemote_BpRelease)
                      {
                        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                        Mvrq.MoveRequestParam = NoMoveParam;
                        MoveRequest_Post(&Mvrq);  
                      }   
                 }   
            }
            // Stop
            else if(evt->Type == ErgoRemoteEventType_Stop)
            {
              if(evt->PressDuration <= BUTTON_PUSH_500ms && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
              {
                Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                Mvrq.MoveRequestParam = NoMoveParam;
                MoveRequest_Post(&Mvrq);
              }
              else if(evt->PressDuration >= BUTTON_PUSH_2SEC)
              {
                me.ErgoModeState = ErgoModeState_WaitRelease;
                
                TU32 CurrentPos = EncoderDriver_GetPosition();
                
                if(me.ErgoMode == ErgoMode_ReSettingDEL)
                {
                  LED_LogEvent(LED_Event_ToUserMode);
                  SQC_Count(&me.SQC.Counter.DEL_Reseting,sizeof(me.SQC.Counter.DEL_Reseting));
                  Position_Set(PositionName_DEL,CurrentPos);
                }
                else
                {
                  trap();
                }
                me.ErgoModeState = ErgoModeState_WaitRelease;
                me.ChangeStateOnBpRelease = TRUE;
                me.ErgoModeAfterRelease = ErgoMode_UserMode;
                
                Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                Mvrq.MoveRequestParam = NoMoveParam;
                MoveRequest_Post(&Mvrq);
              }
            }
     }

    }
    break;
       
  case ErgoModeState_WaitRelease :
    if(evt->BpState == ErgoRemote_BpRelease)
    {
      me.ErgoModeState = ErgoModeState_WaitEvent;
      if(me.ChangeStateOnBpRelease == TRUE)
      {
        me.ChangeStateOnBpRelease = FALSE;
        ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
      }
      
      if(me.DeadmanModeStarted == TRUE)
      {
        me.DeadmanModeStarted = FALSE;
        Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
        Mvrq.MoveRequestParam = NoMoveParam;
        MoveRequest_Post(&Mvrq);
      }  
    }
    break;
      
  default:
    break;
  }
}

static void  ErgonomicManager_ErgoMode_ReSettingSDP_EventHandler(ErgoEvent_t *evt) //ok
{  
  MoveRequest_s Mvrq;  

  switch(me.ErgoModeState)
  {
    case ErgoModeState_WaitEvent :
         if(IsCurrentRemoteTheResetingRemote() == TRUE)
         {
//            //UP 
//            if(evt->Type == ErgoRemoteEventType_Up)
//            {
//
//              ////////////////////////////kendy/////////////
//               if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
//                {
//                   Speed_MoveCycle=TRUE;
//                }
//               else if(evt->PressDuration >=  BUTTON_PUSH_2SEC && evt->BpState == ErgoRemote_BpPress)
//               {
//                    me.ErgoModeState = ErgoModeState_WaitRelease;
//                    if(Curr_SpeedMode != SPEED_28RPM)  //RU24
//                    {
//                      LED_LogEvent(LED_Event_SpeedINC);
//                      
//                      Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
//                      Mvrq.MoveRequestParam = NoMoveParam;
//                      MoveRequest_Post(&Mvrq); 
//                      
//                      SpeedCycle_last=SpeedADJCycle_timems-100u;
//                    }else
//                    {
//                      LED_LogEvent(LED_Event_SetImpossibleSetMode);
//                      
//                      Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
//                      Mvrq.MoveRequestParam = NoMoveParam;
//                      MoveRequest_Post(&Mvrq); 
//                    }
//
//                      Curr_SpeedMode--;
//                    if(Curr_SpeedMode == SPEED_10RPM)
//                      Current_Speed = MININUM_10_SPEED; 
//                    else if(Curr_SpeedMode == SPEED_12RPM)
//                      Current_Speed = MIDDLE_12_SPEED;
//                    else if(Curr_SpeedMode == SPEED_14RPM)
//                      Current_Speed = MIDDLE_14_SPEED; 
//                    else if(Curr_SpeedMode == SPEED_16RPM)
//                      Current_Speed = MIDDLE_16_SPEED; 
//                    else if(Curr_SpeedMode == SPEED_18RPM)
//                      Current_Speed = MIDDLE_18_SPEED;
//                    else if(Curr_SpeedMode == SPEED_20RPM)
//                      Current_Speed = MIDDLE_20_SPEED; 
//                    else if(Curr_SpeedMode == SPEED_22RPM)
//                      Current_Speed = MIDDLE_22_SPEED;
//                    else if(Curr_SpeedMode == SPEED_24RPM)
//                      Current_Speed = MIDDLE_24_SPEED;
//                    else if(Curr_SpeedMode == SPEED_26RPM)
//                      Current_Speed = MIDDLE_26_SPEED; 
//                    else {
//                      Curr_SpeedMode = SPEED_28RPM;
//                      Current_Speed  = STANDARD_28_SPEED;
//                    }
//                    LOG_PRINT1("[LOG][Ergo] Curr_SpeedMode = %d\n", Curr_SpeedMode);
//               }
//            }
//            //DOWN
//            else if(evt->Type == ErgoRemoteEventType_Down)
//            {
//               if(evt->PressDuration < BUTTON_PUSH_500ms && evt->BpState == ErgoRemote_BpPress)
//                {
//                     Speed_MoveCycle=TRUE;
//                }
//               else if(evt->PressDuration >=  BUTTON_PUSH_2SEC && evt->BpState == ErgoRemote_BpPress)
//               {
//                    me.ErgoModeState = ErgoModeState_WaitRelease;
//                    if(Curr_SpeedMode != SPEED_10RPM)
//                    { 
//                      LED_LogEvent(LED_Event_SpeedDEC);
//                      
//                      Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
//                      Mvrq.MoveRequestParam = NoMoveParam;
//                      MoveRequest_Post(&Mvrq); 
//                      SpeedCycle_last=SpeedADJCycle_timems-100u;
//                    }else
//                    {
//                      LED_LogEvent(LED_Event_SetImpossibleSetMode);
//                       
//                      Mvrq.MoveType = MoveRequestType_Nolimit_ErrFeedBack;
//                      Mvrq.MoveRequestParam = NoMoveParam;
//                      MoveRequest_Post(&Mvrq); 
//                    } 
//                    
//                      Curr_SpeedMode++;
//                      if(Curr_SpeedMode == SPEED_28RPM)                    
//                        Current_Speed = STANDARD_28_SPEED;   
//                      else if(Curr_SpeedMode == SPEED_26RPM)
//                        Current_Speed = MIDDLE_26_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_24RPM)
//                        Current_Speed = MIDDLE_24_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_22RPM)
//                        Current_Speed = MIDDLE_22_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_20RPM)
//                        Current_Speed = MIDDLE_20_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_18RPM)
//                        Current_Speed = MIDDLE_18_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_16RPM)
//                        Current_Speed = MIDDLE_16_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_14RPM)
//                        Current_Speed = MIDDLE_14_SPEED; 
//                      else if(Curr_SpeedMode == SPEED_12RPM)
//                        Current_Speed = MIDDLE_12_SPEED; 
//                      else{Curr_SpeedMode = SPEED_10RPM;
//                        Current_Speed = MININUM_10_SPEED; 
//                      }  
//                     LOG_PRINT1("[LOG][Ergo] Curr_SpeedMode = %d\n", Curr_SpeedMode);
//               }
//             }
//                 
            // Stop
            if(evt->Type == ErgoRemoteEventType_Stop)
            {
              if(evt->PressDuration <= BUTTON_PUSH_500ms && EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
              {
                Mvrq.MoveType = MoveRequestType_Nolimit_Stop;
                Mvrq.MoveRequestParam = NoMoveParam;
                MoveRequest_Post(&Mvrq);
              }
              else if(evt->PressDuration >= BUTTON_PUSH_2SEC)
              {
                me.ErgoModeState = ErgoModeState_WaitRelease;
                
//                TU32 CurrentPos = EncoderDriver_GetPosition();
//                
//                if(me.ErgoMode == ErgoMode_ReSettingDEL)
//                {
//                  LED_LogEvent(LED_Event_ToUserMode);
//                  SQC_Count(&me.SQC.Counter.DEL_Reseting,sizeof(me.SQC.Counter.DEL_Reseting));
//                  Position_Set(PositionName_DEL,CurrentPos);
//                }
//                else
//                {
//                  trap();
//                }
                me.ErgoModeState = ErgoModeState_WaitRelease;
                me.ChangeStateOnBpRelease = TRUE;
                me.ErgoModeAfterRelease = ErgoMode_UserMode;
                
                Mvrq.MoveType = MoveRequestType_Nolimit_ShortFeedBack;
                Mvrq.MoveRequestParam = NoMoveParam;
                MoveRequest_Post(&Mvrq);
              }
            }
         }
   
    break;
     
    case ErgoModeState_WaitRelease :
      
      if(evt->BpState == ErgoRemote_BpRelease)
      {
        me.ErgoModeState = ErgoModeState_WaitEvent;
        if(me.ChangeStateOnBpRelease == TRUE)
        {
          me.ChangeStateOnBpRelease = FALSE;
          ErgonomicManager_ChangeErgoMode(me.ErgoModeAfterRelease);
        }
      }
      
      break;
      
  default:
    break;
  }
}
