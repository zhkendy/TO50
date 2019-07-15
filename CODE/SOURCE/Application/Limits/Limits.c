// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#include "Position.h"
#include "PositionChangeObserver.h"
#include "RTS_Ergonomic_ManagerObserver.h"
#include "MovementRequest_Interface.h"
#include "MotorControl.h"
#include "Limits.h"

#ifdef LIMITS_PRINT_DEBUG
#include "Limits_DebugTXT.h"
#include "DebugPrint.h"
#endif

#ifdef LIMITS_PRINT_INFO
#include "Limits_DebugTXT.h"
#include "DebugPrint_info.h"
#endif

#ifdef LIMITS_PRINT_LOG	
#include "Limits_DebugTXT.h"
#include "DebugPrint_Log.h"
#endif


typedef struct Limits_s
{
  TBool LimitsMonitor;
  Limits_State_e LimitsState;

  ModeLimit_s ModeLimitArray[LimitMode_LastEnum];
  LimitMode_e CurrentMode;
  ModeLimit_s CurrentLimits;
}Limits_s;


Limits_s LimitsSingleton;
#define me LimitsSingleton


static void ComputeModeLimitsValue();
static void RTSErgoChangeModeHandler(ErgoMode_e NewMode);
static void RTSErgoSpecialRequestHandler(RTSErgoSpecialRequest_e Sreq);
static TU32 Limits_GetDownLimitDistance(const TU32 *CurrentPosition);
static TU32 Limits_GetUpLimitDistance(const TU32 *CurrentPosition);


//限位初始化
void limits_Init()
{
  me.CurrentMode   = LimitMode_LastEnum;
  me.LimitsMonitor = TRUE;
  
  //Register to position
  PositionObserver_s LimitsPositionObserver;
  LimitsPositionObserver.Fptr_PositionChangeEvent = ComputeModeLimitsValue;       //计算各种模式的限位值
  Position_RegisterObserver(PositionChangeObserver_Limits,LimitsPositionObserver);//记录各个模式的限位值
  
  //Register to RTS Observer
  RTSErgonomicObserver_s ErgoObserver;
  ErgoObserver.Fptr_ErgoChageMode = RTSErgoChangeModeHandler; ////判断是否使用正常限位  还是更改限位
  ErgoObserver.Fptr_RTSErgoSpecialRequest = RTSErgoSpecialRequestHandler;
  RTSErgonomicObserver_RegisterObserver(RTSErgonomicObserver_Limits,ErgoObserver); 
}


Limits_State_e Limits_GetLimitsState()
{
  return me.LimitsState;
}

//限位创建
void limits_Create(TU32 *CurrentPosition)
{
  me.CurrentMode = LimitMode_Normal;//正常限位模式
  ComputeModeLimitsValue();          //计算各种模式下限位值
  Limits_SetMode(me.CurrentMode);    //限位模式选择
  Limits_ComputeLimitsState(CurrentPosition);//限位位置比较
}

//计算各种模式下限位值
static void ComputeModeLimitsValue()
{
  TU32 PosUEL,PosDEL,PosOrigin,PosEndOfWorld;
  PosUEL = *Position_GetValue(PositionName_UEL);
  PosDEL = *Position_GetValue(PositionName_DEL);
  PosOrigin = *Position_GetValue(PositionName_Origin);
  PosEndOfWorld = *Position_GetValue(PositionName_EndOfWorld);

  me.ModeLimitArray[LimitMode_Normal].UpLimit   = PosUEL;                          //限位正常模式 有初始值 最终
  me.ModeLimitArray[LimitMode_Normal].DownLimit = PosDEL;
  
  me.ModeLimitArray[LimitMode_settingUEL].UpLimit     = PosOrigin;                  //初始化 设置上限位模式
  me.ModeLimitArray[LimitMode_settingUEL].DownLimit   = PosDEL;                 
  
  me.ModeLimitArray[LimitMode_settingDEL].UpLimit     = PosUEL;                     //初始化 设置下限位模式
  me.ModeLimitArray[LimitMode_settingDEL].DownLimit   = PosEndOfWorld;;              

  me.ModeLimitArray[LimitMode_ResettingUEL].UpLimit   = PosOrigin;                 //重设上限位模式
  me.ModeLimitArray[LimitMode_ResettingUEL].DownLimit = PosDEL - ONE_PRODUCT_TURN;
  
  me.ModeLimitArray[LimitMode_ResettingDEL].UpLimit   = PosUEL + ONE_PRODUCT_TURN; //重设下限位模式
  me.ModeLimitArray[LimitMode_ResettingDEL].DownLimit = PosEndOfWorld;;                
}


void Limits_SetMode(LimitMode_e Mode)
{
  me.CurrentMode   = Mode;
  me.CurrentLimits = me.ModeLimitArray[Mode];
}


//获取限位值 上下限位值
TU32 Limits_GetLimitValue(ModeLimitName_e LimitName)
{
  TU32 Rtn = 0u;
  if(LimitName == ModeLimitName_UpLimit)
  {
    Rtn = me.CurrentLimits.UpLimit;
  }
  else
  {
    Rtn = me.CurrentLimits.DownLimit;
  }
  return Rtn;
}


//当前位值距离上限位距离
static TU32 Limits_GetUpLimitDistance(const TU32 *CurrentPosition)
{
  TU32 Rtn = 0u;
  if(*CurrentPosition > me.CurrentLimits.UpLimit)
  {
    Rtn = *CurrentPosition - me.CurrentLimits.UpLimit;
  }
  return Rtn;
}

//当前位值距离下限位距离
static TU32 Limits_GetDownLimitDistance(const TU32 *CurrentPosition)
{
  TU32 Rtn = 0u;
  if(*CurrentPosition < me.CurrentLimits.DownLimit)
  {
    Rtn = me.CurrentLimits.DownLimit - *CurrentPosition;
  }
  return Rtn;
}

//当前位值距离限位距离计算
TU32 Limits_GetLimitDistance(const TU32 *CurrentPosition,ModeLimitName_e LimitName)
{
  TU32 Rtn = 0;
  if(LimitName == ModeLimitName_UpLimit)
  {
    Rtn = Limits_GetUpLimitDistance(CurrentPosition);   //距离上限位距离
  }
  else
  {
    Rtn = Limits_GetDownLimitDistance(CurrentPosition); //距离下限位位置
  }
  return Rtn;
}
    





//当前位置与限位位置比较
void Limits_ComputeLimitsState(const TU32 *CurrentPosition)
{
    PositionCompare_e CompareToUpLimit = Position_CompareTwoAbsolutePosition(&me.CurrentLimits.UpLimit,CurrentPosition,(POSITION_TOLERANCE));
    PositionCompare_e CompareToDownLimits = Position_CompareTwoAbsolutePosition(&me.CurrentLimits.DownLimit,CurrentPosition,(POSITION_TOLERANCE));
    
    if(CompareToUpLimit == CurrentPositionIsBelowRef && CompareToDownLimits == CurrentPositionIsAboveRef)
    {
      me.LimitsState = Limits_State_Between;
    }else if(CompareToUpLimit == CurrentPositionIsOnRef && CompareToDownLimits == CurrentPositionIsAboveRef)
    {
      me.LimitsState = Limits_State_OnUp;
    }else if(CompareToUpLimit == CurrentPositionIsBelowRef && CompareToDownLimits == CurrentPositionIsOnRef)
    {
      me.LimitsState = Limits_State_OnDown;
    }else if(CompareToUpLimit == CurrentPositionIsAboveRef && CompareToDownLimits == CurrentPositionIsAboveRef)
    { //当前位置在上限位以上
      //Allow Up limit "violation" to perform compensation move
      TU32 CompensationMove = me.CurrentLimits.UpLimit - RETURNMOVELENGHT;
      PositionCompare_e CompareToCompensationMove = Position_CompareTwoAbsolutePosition(&CompensationMove,CurrentPosition,POSITION_TOLERANCE);
      
      if(CompareToCompensationMove == CurrentPositionIsAboveRef)
      {
        me.LimitsState = Limits_State_Above;
      }else
      {
        me.LimitsState = Limits_State_OnUp;
      }
    }else if(CompareToUpLimit == CurrentPositionIsBelowRef && CompareToDownLimits == CurrentPositionIsBelowRef)//当前位置在下限位以下
    {
      me.LimitsState = Limits_State_Below;
    }else
    {
      //Others combinaison are impossible, trap and reset
      trap();
    }
}


//限位位置状态检测 超过就停止运行
void Limits_Monitor()
{
  if(me.LimitsMonitor == TRUE)
  {
    if( (me.LimitsState == Limits_State_Above && MotorControl_GetCurrentSetMoveDirection() == MotorDirection_Roll) ||
        (me.LimitsState == Limits_State_Below && MotorControl_GetCurrentSetMoveDirection() == MotorDirection_UnRoll)
      )
    {//超过限位 安全停止
      MoveRequest_s Mvrq; 
      Mvrq.MoveType = MoveRequestType_SecurityStop; //超过限位 安全停止
      Mvrq.MoveRequestParam = NoMoveParam;
      MoveRequest_Post(&Mvrq);
    }
  }
}

void Limits_MonitorCMD(TBool Cmd)
{
  me.LimitsMonitor = Cmd;
}

//判断目标位置是否在限位内
TBool Limits_IsTargetPositionInsideLimits(const TU32 *TargetPosition,TU8 Tolerance)
{
  TBool Rtn = FALSE;
  PositionCompare_e CompareToUpLimit = Position_CompareTwoAbsolutePosition(&me.CurrentLimits.UpLimit,TargetPosition,Tolerance);
  PositionCompare_e CompareToDownLimits = Position_CompareTwoAbsolutePosition(&me.CurrentLimits.DownLimit,TargetPosition,Tolerance);
  
  if( (CompareToUpLimit == CurrentPositionIsBelowRef || CompareToUpLimit == CurrentPositionIsOnRef) &&
     (CompareToDownLimits == CurrentPositionIsAboveRef || CompareToDownLimits == CurrentPositionIsOnRef) )
  {
    Rtn = TRUE;
  }
  return Rtn;
}


//判断是否使用正常限位  还是更改限位
static void RTSErgoChangeModeHandler(ErgoMode_e NewMode)
{
//  DEBUG_PRINT1("[DBG] Limits Mode = %s\n",
//               LimitMode_TXT[NewMode]);
    
  switch(NewMode)
  {
      case ErgoMode_ReSettingDEL :
        Limits_SetMode(LimitMode_ResettingDEL);
        DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[4]);
        break;
      case ErgoMode_ReSettingUEL :
        Limits_SetMode(LimitMode_ResettingUEL);
        DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[3]);
        break;   
        
      case ErgoMode_SettingPart1Mode : 
      case ErgoMode_SettingPart1Modebis :
        //DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[0]);
        Limits_SetMode(LimitMode_Normal);  
        break;   
      
      case ErgoMode_IndustrialMode :
      case ErgoMode_FactoryMode : 
      case ErgoMode_Delivery1Mode :
      case ErgoMode_Delivery2Mode :
      case ErgoMode_Delivery3Mode :
      case ErgoMode_FirstPairingMode : 
      case ErgoMode_SetAndTemporaryRemote :
      case ErgoMode_SleepMode :
      case ErgoMode_MaintenanceResetingMode : //限位正常模式   
      case ErgoMode_SpeedAdjustmentMode :
      case ErgoMode_Tiltingspeedadjustment :  //kendy????
      case ErgoMode_ReSettingSDP://kendy
      case ErgoMode_ConfigMode :
      case ErgoMode_SetAndNotPaired :
      case ErgoMode_SetNoTemporaryRemote ://kendy 
      case ErgoMode_UserMode :
      case ErgoMode_EMS_Sensitivity_On :
      case ErgoMode_EMS_Sensitivity_Off :
         DEBUG_PRINT1("[DBG] Limits Mode = %s\n",LimitMode_TXT[0]);
         Limits_SetMode(LimitMode_Normal);
        //以上模式限位正常
        break;
        
      default :
        trap();//unknow request
        break;
  }
  DEBUG_PRINT2("[DBG] Limits Up/Down [%ld]/[%ld]\n",
               me.CurrentLimits.UpLimit,
               me.CurrentLimits.DownLimit);
}

static void RTSErgoSpecialRequestHandler(RTSErgoSpecialRequest_e Sreq)
{

}
