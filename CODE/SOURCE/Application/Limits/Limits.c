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


//��λ��ʼ��
void limits_Init()
{
  me.CurrentMode   = LimitMode_LastEnum;
  me.LimitsMonitor = TRUE;
  
  //Register to position
  PositionObserver_s LimitsPositionObserver;
  LimitsPositionObserver.Fptr_PositionChangeEvent = ComputeModeLimitsValue;       //�������ģʽ����λֵ
  Position_RegisterObserver(PositionChangeObserver_Limits,LimitsPositionObserver);//��¼����ģʽ����λֵ
  
  //Register to RTS Observer
  RTSErgonomicObserver_s ErgoObserver;
  ErgoObserver.Fptr_ErgoChageMode = RTSErgoChangeModeHandler; ////�ж��Ƿ�ʹ��������λ  ���Ǹ�����λ
  ErgoObserver.Fptr_RTSErgoSpecialRequest = RTSErgoSpecialRequestHandler;
  RTSErgonomicObserver_RegisterObserver(RTSErgonomicObserver_Limits,ErgoObserver); 
}


Limits_State_e Limits_GetLimitsState()
{
  return me.LimitsState;
}

//��λ����
void limits_Create(TU32 *CurrentPosition)
{
  me.CurrentMode = LimitMode_Normal;//������λģʽ
  ComputeModeLimitsValue();          //�������ģʽ����λֵ
  Limits_SetMode(me.CurrentMode);    //��λģʽѡ��
  Limits_ComputeLimitsState(CurrentPosition);//��λλ�ñȽ�
}

//�������ģʽ����λֵ
static void ComputeModeLimitsValue()
{
  TU32 PosUEL,PosDEL,PosOrigin,PosEndOfWorld;
  PosUEL = *Position_GetValue(PositionName_UEL);
  PosDEL = *Position_GetValue(PositionName_DEL);
  PosOrigin = *Position_GetValue(PositionName_Origin);
  PosEndOfWorld = *Position_GetValue(PositionName_EndOfWorld);

  me.ModeLimitArray[LimitMode_Normal].UpLimit   = PosUEL;                          //��λ����ģʽ �г�ʼֵ ����
  me.ModeLimitArray[LimitMode_Normal].DownLimit = PosDEL;
  
  me.ModeLimitArray[LimitMode_settingUEL].UpLimit     = PosOrigin;                  //��ʼ�� ��������λģʽ
  me.ModeLimitArray[LimitMode_settingUEL].DownLimit   = PosDEL;                 
  
  me.ModeLimitArray[LimitMode_settingDEL].UpLimit     = PosUEL;                     //��ʼ�� ��������λģʽ
  me.ModeLimitArray[LimitMode_settingDEL].DownLimit   = PosEndOfWorld;;              

  me.ModeLimitArray[LimitMode_ResettingUEL].UpLimit   = PosOrigin;                 //��������λģʽ
  me.ModeLimitArray[LimitMode_ResettingUEL].DownLimit = PosDEL - ONE_PRODUCT_TURN;
  
  me.ModeLimitArray[LimitMode_ResettingDEL].UpLimit   = PosUEL + ONE_PRODUCT_TURN; //��������λģʽ
  me.ModeLimitArray[LimitMode_ResettingDEL].DownLimit = PosEndOfWorld;;                
}


void Limits_SetMode(LimitMode_e Mode)
{
  me.CurrentMode   = Mode;
  me.CurrentLimits = me.ModeLimitArray[Mode];
}


//��ȡ��λֵ ������λֵ
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


//��ǰλֵ��������λ����
static TU32 Limits_GetUpLimitDistance(const TU32 *CurrentPosition)
{
  TU32 Rtn = 0u;
  if(*CurrentPosition > me.CurrentLimits.UpLimit)
  {
    Rtn = *CurrentPosition - me.CurrentLimits.UpLimit;
  }
  return Rtn;
}

//��ǰλֵ��������λ����
static TU32 Limits_GetDownLimitDistance(const TU32 *CurrentPosition)
{
  TU32 Rtn = 0u;
  if(*CurrentPosition < me.CurrentLimits.DownLimit)
  {
    Rtn = me.CurrentLimits.DownLimit - *CurrentPosition;
  }
  return Rtn;
}

//��ǰλֵ������λ�������
TU32 Limits_GetLimitDistance(const TU32 *CurrentPosition,ModeLimitName_e LimitName)
{
  TU32 Rtn = 0;
  if(LimitName == ModeLimitName_UpLimit)
  {
    Rtn = Limits_GetUpLimitDistance(CurrentPosition);   //��������λ����
  }
  else
  {
    Rtn = Limits_GetDownLimitDistance(CurrentPosition); //��������λλ��
  }
  return Rtn;
}
    





//��ǰλ������λλ�ñȽ�
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
    { //��ǰλ��������λ����
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
    }else if(CompareToUpLimit == CurrentPositionIsBelowRef && CompareToDownLimits == CurrentPositionIsBelowRef)//��ǰλ��������λ����
    {
      me.LimitsState = Limits_State_Below;
    }else
    {
      //Others combinaison are impossible, trap and reset
      trap();
    }
}


//��λλ��״̬��� ������ֹͣ����
void Limits_Monitor()
{
  if(me.LimitsMonitor == TRUE)
  {
    if( (me.LimitsState == Limits_State_Above && MotorControl_GetCurrentSetMoveDirection() == MotorDirection_Roll) ||
        (me.LimitsState == Limits_State_Below && MotorControl_GetCurrentSetMoveDirection() == MotorDirection_UnRoll)
      )
    {//������λ ��ȫֹͣ
      MoveRequest_s Mvrq; 
      Mvrq.MoveType = MoveRequestType_SecurityStop; //������λ ��ȫֹͣ
      Mvrq.MoveRequestParam = NoMoveParam;
      MoveRequest_Post(&Mvrq);
    }
  }
}

void Limits_MonitorCMD(TBool Cmd)
{
  me.LimitsMonitor = Cmd;
}

//�ж�Ŀ��λ���Ƿ�����λ��
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


//�ж��Ƿ�ʹ��������λ  ���Ǹ�����λ
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
      case ErgoMode_MaintenanceResetingMode : //��λ����ģʽ   
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
        //����ģʽ��λ����
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
