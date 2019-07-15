// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

/**
* @file 
* @brief 
* @details
* @author Gabriel Mula
* @date 2016
*/

#include "Config.h"
#include "Banker.h"
#include "RTS_Ergonomic_ManagerObservable.h"
#include "RTS_Ergonomic_Manager.h"
#include "Stack.h"
#include "SFY_OS.h"
#include "Encoder_Driver.h"
#include "MotorControl.h"
#include "MovementPlanner.h"
#include "Chrono.h"
#include "SQC.h"
#include "LongTimer.h"
#include "PowerMonitor.h"
#include "MovementManager.h"
#include "EncoderDriver_DriftCorrector.h"

#ifdef MOVEMENTMANAGER_PRINT_DEBUG	
#include "Movement_DebugTXT.h"
#include "DebugPrint.h"
#endif

#ifdef MOVEMENTMANAGER_PRINT_INFO
#include "Movement_DebugTXT.h"
#include "DebugPrint_info.h"
#endif

#ifdef MOVEMENTMANAGER_PRINT_LOG	
#include "Movement_DebugTXT.h"
#include "DebugPrint_Log.h"
#endif

extern bool Direction_change;
extern bool Motor_Reset;
bool MotorBlocked=false;

//运行请求 初始化
MoveRequest_s MoveRequestInit = 
{
  .MoveType = MoveRequestType_None,
  .MoveRequestParam = 0,
};

MotorControl_Segment_s MotorControl_Segment_init =
{
  .UnInteruptible  =  FALSE,                                   
  .SpeedTrigger    = {FALSE,0},
  .PositionTrigger = {FALSE,0u},
  .TimeTrigger     = {FALSE,0u},
  .SetPoint = {MotorDirection_Stop,LoopMode_Brake,0u,0u,0u,0u},
};



typedef struct MovementManager_t
{  
  MotorDirection_e BlockedDirection;
  TBool RunDeadZoneGearBoxCompensationMove;
  TBool RecordBlockedDirection;
  TBool EncoderShutOffTimerStarted;
  TBool LockByMotorControl;
  
  MoveRequest_s CurrentHandledMoveRequest;
  
  StackObjRef Stack_MoveRequest;   
  TU8 Stack_MoveRequestBuffer[ sizeof(MoveRequest_s)]; 
  
  MovementManager_Move_s CurrentMove;
  StackObjRef Stack_Move;  
  TU8 MoveStackBuffer[ sizeof(MovementManager_Move_s) * MOVE_STACK_DEPTH ]; 
  
  StackObjRef Stack_Segment;  
  TU8 SegmentStackBuffer[ sizeof(MotorControl_Segment_s) * SEGMENT_STACK_DEPTH]; 
  
  MoveRequest_s TmpMoveRequest;
  
  StackObjRef Stack_MoveTmp;  
  TU8 MoveTmpStackBuffer[ sizeof(MovementManager_Move_s) * MOVE_STACK_DEPTH ]; 
  
}MovementManager_t;



#define me MovementManager

typedef struct MoveRequestPrio_s
{
  MoveRequestType_e RequestType;
  TU8   MovePriority;
  TBool IsInterruptibleBySamePriority;
}MoveRequestPrio_s;

static const MoveRequestPrio_s MoveRequestPrioArrays[MoveRequestType_LAST_ENUM] =
{
  {MoveRequestType_SecurityStop,0,FALSE},    //运动请求  优先级  是否可以被同级打断
  {MoveRequestType_None,7,TRUE},    
  {MoveRequestType_Manual_Stop,2,FALSE},   
  {MoveRequestType_Manual_Roll,3,TRUE},    
  {MoveRequestType_Manual_UnRoll,3,TRUE},   
  {MoveRequestType_Manual_RollForTime,3,FALSE},   
  {MoveRequestType_Manual_UnRollForTime,3,FALSE},   
  {MoveRequestType_Manual_PWM,3,TRUE},    
  {MoveRequestType_Nolimit_RollForTime,4,TRUE},   
  {MoveRequestType_Nolimit_UnRollForTime,4,TRUE},   
  {MoveRequestType_Nolimit_StopForTime,1,FALSE}, 
  {MoveRequestType_Nolimit_Stop,2,FALSE},   
  {MoveRequestType_Nolimit_RollTo,4,TRUE},    
  {MoveRequestType_Nolimit_UnRollTo,4,TRUE},    
  {MoveRequestType_Nolimit_ShortUnRoll,4,FALSE},    
  {MoveRequestType_Nolimit_ShortRoll,4,FALSE},    
  {MoveRequestType_Nolimit_Rolloff,3,TRUE},    
  {MoveRequestType_Nolimit_UnRolloff,3,TRUE},   
  {MoveRequestType_Nolimit_ShortFeedBack,1,FALSE},  
  {MoveRequestType_Nolimit_ErrFeedBack,6,TRUE},  
  {MoveRequestType_Nolimit_ErrFeedBackMoveTo,6,TRUE}, 
  {MoveRequestType_Nolimit_DoubleShortFeedBack,1,TRUE},
  {MoveRequestType_Nolimit_ThreeShortFeedBack,5,TRUE},
  {MoveRequestType_Nolimit_FourShortFeedBack,6,TRUE},
  {MoveRequestType_Nolimit_10sCycleMove,6,TRUE},
  {MoveRequestType_Nolimit_TiltingCycleMove,6,TRUE},
  {MoveRequestType_RollForTime,4,TRUE},   
  {MoveRequestType_UnRollForTime,4,TRUE},   
  {MoveRequestType_RollTo,4,TRUE},   
  {MoveRequestType_UnRollTo,4,TRUE}, 
  {MoveRequestType_GoToPercent,4,TRUE},  
  {MoveRequestType_GoToAbsPos,4,TRUE},  
  {MoveRequestType_ShortUnRoll,4,FALSE},    
  {MoveRequestType_ShortRoll,4,FALSE},   
  {MoveRequestType_Rolloff,4,TRUE},   
  {MoveRequestType_UnRolloff,4,TRUE},   
  {MoveRequestType_RollToLimits,4,TRUE},    
  {MoveRequestType_UnRollToLimits,4,TRUE},   
  {MoveRequestType_GoToLimits,4,TRUE},   
  {MoveRequestType_RollToPosition,4,TRUE},   
  {MoveRequestType_UnRollToPosition,4,TRUE},    
  {MoveRequestType_GoToPosition,4,TRUE}, 
  {MoveRequestType_CompensationMove,2,FALSE},
  {MoveRequestType_TiltMyPosition,4,TRUE},
};

 
static void  MovementManager_SpecialMoveConfig();
static void  MovementManager_PreMoveAction();
static MovementManager_t MovementManager;
static void  StartEncoderAutoShutOffTimer();
static void  StopEncoderAutoShutOffTimer();
static void  MovementManager_3MinMoveTimeout();
void  MotorControlIsLockBySecuManager(void);
void  MotorControlIsUnLock(void);
static const MoveRequestPrio_s* GetMoveRequestPrio(MoveRequestType_e MvType);



//电机控制 被安全机制锁住
 void MotorControlIsLockBySecuManager(void)
{
  SQC_LogEvent(SQC_Event_MotorBlocked);
  MotorState_e MtrState = MotorControl_GetSecuLockState();//获取电机控制安全锁状态
  if(me.RecordBlockedDirection == TRUE && MtrState == MotorState_BlockedRoll || MtrState == MotorState_Rolling_LowSpeed)            //需要修改 sage
  {//卷起方向堵转    堵转 或者 低速 当前都会被锁起
    me.BlockedDirection = MotorDirection_Roll;
  }else if(me.RecordBlockedDirection == TRUE && MtrState == MotorState_BlockedUnRoll || MtrState == MotorState_UnRolling_LowSpeed)  //需要修改
  {//展开方向堵转    堵转 或者 低速 当前都会被锁起
    me.BlockedDirection = MotorDirection_UnRoll;
  } 
  Stack_Purge(me.Stack_MoveRequest);//清电机运行请求
  Stack_Purge(me.Stack_Move);       //清电机运行
  Stack_Purge(me.Stack_Segment);    //清运行参数 
  me.LockByMotorControl = TRUE;     //电机锁住
  MotorBlocked=me.LockByMotorControl;
}

 void MotorControlIsUnLock(void)
{
  me.RunDeadZoneGearBoxCompensationMove = FALSE;
  me.LockByMotorControl = FALSE;
  MotorBlocked=me.LockByMotorControl;
  MovementManager_SetNextAction();  //Set Null without Compensation move
}

void MotorControlReset(void)
{
  me.RunDeadZoneGearBoxCompensationMove = FALSE;                          //电机反向补偿
  me.RecordBlockedDirection     = TRUE;                                   //记录阻塞方向
  me.EncoderShutOffTimerStarted = FALSE;                                  //编码器关闭倒计时
  me.LockByMotorControl         = FALSE;                                  //电机控制锁住电机
  MotorBlocked=me.LockByMotorControl;
  Stack_Purge(me.Stack_MoveRequest);
  Stack_Purge(me.Stack_Move);
  Stack_Purge(me.Stack_Segment);       //堆栈净化 清除参数
  
  MoveRequest_s Mvrq = MoveRequestInit; 
  Mvrq.MoveType = MoveRequestType_None;
  Mvrq.MoveRequestParam = NoMoveParam;
  MoveRequest_Post(&Mvrq);             //电机停止
  
  MovementManager_SetNextAction();     //NULL 执行下一个动作
}

//运行管理初始化    绝对初始化
void MovementManager_Init()
{
  me.RunDeadZoneGearBoxCompensationMove = FALSE;                          //电机反向补偿
  me.RecordBlockedDirection     = TRUE;                                   //记录阻塞方向
  me.EncoderShutOffTimerStarted = FALSE;                                  //编码器关闭倒计时
  me.LockByMotorControl         = FALSE;                                  //电机控制锁住电机
  MotorBlocked=me.LockByMotorControl;
  MotorControl_Init(MotorControlIsLockBySecuManager,MotorControlIsUnLock);//电机控制初始化
  Position_Init();                                                        //位置信息初始化  绝对初始化
  limits_Init();                                                          //限位初始化
  
  //堆栈创建
  //运动“请求 运行 参数 运行监测” 
  me.Stack_MoveRequest = Stack_Create_StaticAllocation(sizeof(MoveRequest_s),1,me.Stack_MoveRequestBuffer,MoveReqStack); 
  me.Stack_Move        = Stack_Create_StaticAllocation(sizeof(MovementManager_Move_s),MOVE_STACK_DEPTH,me.MoveStackBuffer,MoveStack);
  me.Stack_Segment     = Stack_Create_StaticAllocation(sizeof(MotorControl_Segment_s),SEGMENT_STACK_DEPTH ,me.SegmentStackBuffer,SegmentStack);
  me.Stack_MoveTmp     = Stack_Create_StaticAllocation(sizeof(MovementManager_Move_s),MOVE_STACK_DEPTH,me.MoveTmpStackBuffer,MoveStackTmp);
  
  if(SFY_OS_TimerCreate(MOVEMENTMANAGER_LOOP_TIMER_ID,OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_LOOP) != OS_SUCCESS)
  {//开启一个时钟队列 TASK_Motor TASK_Motor_MOVEMENTMANAGER_LOOP  用于电机运行监测
    trap();
  }
  
  if(SFY_OS_TimerCreate(MOVEMENTMANAGER_STOP_TIMER_ID,OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Controller,TASK_Controller_STOP_TO) != OS_SUCCESS)
  {//开启一个时钟队列 TASK_Controller TASK_Controller_STOP_TO     用于关闭编码器电源 数据保存
    trap();
  } 
  MoveRequest_s Mvrq = MoveRequestInit;
  Mvrq.MoveType      = MoveRequestType_None;//运行请求为空
  me.CurrentHandledMoveRequest = Mvrq;
  
  LongTimer_Setup(LongTimer_6MinMaxMoveTimer,MAXMOVETIME_MS,FALSE,FALSE,MovementManager_3MinMoveTimeout);
  //设置一个定时器用于 防止运行时间超过6min
}

void MovementManager_Create(TBool WatchdogReset)
{
  MotorControl_Create(WatchdogReset);
  Position_Create();                                 //导入数组位置参数
  TU32 CurrentPosition = EncoderDriver_GetPosition();//获取当前位置
  limits_Create(&CurrentPosition);                   //限位创建
  
  SFY_OS_TimerStart(MOVEMENTMANAGER_LOOP_TIMER_ID,MOVECONTROLLOOP_TIME,MOVECONTROLLOOP_TIME);
}


static void StartEncoderAutoShutOffTimer()
{
  me.EncoderShutOffTimerStarted = TRUE;
  SFY_OS_TimerStart(MOVEMENTMANAGER_STOP_TIMER_ID,
                    ENCODER_SWTCH_OFF_TIME_AFTER_MOVE,
                    OS_ONE_SHOT_TIMER);
}


static void StopEncoderAutoShutOffTimer()
{
  me.EncoderShutOffTimerStarted = FALSE;
  SFY_OS_TimerStop(MOVEMENTMANAGER_STOP_TIMER_ID);
  SFY_OS_EventReset(OS_TASK_Controller,TASK_Controller_STOP_TO);
}

//运动控制 为空
TBool MovementManager_NoneSet()
{
  TU8 MoveStacked = Stack_GetObjectCount(me.Stack_Move);                        //运动堆栈数量读取
  TU8 SegStacked  = Stack_GetObjectCount(me.Stack_Segment);                     //参数堆栈数量读取
  TBool NoneIsSet = FALSE;
  if(MoveStacked == 0u &&
     SegStacked  == 0u &&
     me.CurrentHandledMoveRequest.MoveType == MoveRequestType_None)             //堆栈都为空，没有运动请求
  {
    NoneIsSet = TRUE;
  } 
  return NoneIsSet;
}

static const MoveRequestPrio_s* GetMoveRequestPrio(MoveRequestType_e MvType)
{
  //Ugly code, caused by LDRA moron constraints
  TU8 index = 0;
  TBool StopLoop = FALSE;
  
  for(TU8 i = 0u ; (i< MoveRequestType_LAST_ENUM && StopLoop == FALSE); ++i)
  {
    if(MoveRequestPrioArrays[i].RequestType == MvType)
    {
      StopLoop = TRUE;
      index = i;
    }
  }
  return (StopLoop == TRUE) ? (&(MoveRequestPrioArrays[index])) : ((MoveRequestPrio_s*)NULL);
}


//电机运动请求
void MoveRequest_Execute(const MoveRequest_s *Mvrq)
{
  const MoveRequestPrio_s* NewMoveRequestPrio     = GetMoveRequestPrio(Mvrq->MoveType);                       //新请求的优先级
  const MoveRequestPrio_s* CurrentMoveRequestPrio = GetMoveRequestPrio(me.CurrentHandledMoveRequest.MoveType);//目前请求的优先级
  TBool IsCurrentSegmentUnInteruptible             = MotorControl_GetCurrentSegment()->UnInteruptible;         //是否可以被打断 终止
  
  if(me.LockByMotorControl == FALSE)/////错误状态之后电机被锁住
  {
    //Test to start action immediatly
    if(IsCurrentSegmentUnInteruptible == FALSE && ( (NewMoveRequestPrio->MovePriority <  CurrentMoveRequestPrio->MovePriority)  ||
                                                    (NewMoveRequestPrio->MovePriority == CurrentMoveRequestPrio->MovePriority && 
                                                     CurrentMoveRequestPrio->IsInterruptibleBySamePriority == TRUE) ||
                                                    (NewMoveRequestPrio->RequestType == MoveRequestType_None) ) 
       )
    {
      DEBUG_PRINT("[DBG] Start Immediatly\n");
      //优先级运行马上执行
      Stack_Purge(me.Stack_MoveRequest);
      Stack_PushHead(me.Stack_MoveRequest,Mvrq);
      MovementManager_SetNextAction();  //执行下一个动作
    }
    else
    {
      MoveRequest_s StackedReq = MoveRequestInit;
      if(Stack_ReadObject(me.Stack_MoveRequest,&StackedReq,0) == TRUE)
      {
        const MoveRequestPrio_s* StackedMoveRequestPrio = NULL;
        StackedMoveRequestPrio = GetMoveRequestPrio(StackedReq.MoveType);//获取电机运动优先级
        DEBUG_PRINT2("[DBG][MV_Manager] NewMoveRequestPrio->MovePriority = %d StackedMovePrio=%d\n",
                     NewMoveRequestPrio->MovePriority,
                     CurrentMoveRequestPrio->MovePriority);
        if(NewMoveRequestPrio->MovePriority <= StackedMoveRequestPrio->MovePriority)
        {
          Stack_Purge(me.Stack_MoveRequest);
          Stack_PushHead(me.Stack_MoveRequest,Mvrq);
          DEBUG_PRINT("[DBG][MV_Manager] Request stacked\n");
        }
        else
        {
          //请求丢失
          DEBUG_PRINT("[DBG][MV_Manager] Stack Full => Request Lost !!! \n");
        }
      }
      else
      {
          //请求被压入堆栈
        DEBUG_PRINT("[DBG][MV_Manager] Request stacked\n");
        Stack_PushHead(me.Stack_MoveRequest,Mvrq);
      }
    }
  }
  else
  {
    //LDRA
     DEBUG_PRINT("LockByMotorControl \n");
  }
}


//3min钟超时
static void MovementManager_3MinMoveTimeout()
{
  LED_LogEvent(LED_Event_3MinMoveTimeout);
  SQC_LogEvent(SQC_Event_3MinMoveTimeout);
  Stack_Purge(me.Stack_MoveRequest);
  Stack_Purge(me.Stack_Move);
  Stack_Purge(me.Stack_Segment);       
  
  MoveRequest_s Mvrq = MoveRequestInit; 
  Mvrq.MoveType = MoveRequestType_None;
  Mvrq.MoveRequestParam = NoMoveParam;
  MoveRequest_Post(&Mvrq);             
  
  MovementManager_SetNextAction();     
}



TBool MovementManager_IsDirectionLock(MotorDirection_e DirToTest)
{
  return (me.BlockedDirection == DirToTest) ? (TRUE) : (FALSE);
}

//解锁相反方向
void MovementManager_UnLockOppositeDirection(MotorDirection_e MoveDir)
{
  if((MoveDir == MotorDirection_UnRoll && me.BlockedDirection == MotorDirection_Roll) ||
     (MoveDir == MotorDirection_Roll   && me.BlockedDirection == MotorDirection_UnRoll))
  {
    me.BlockedDirection = MotorDirection_Stop;
    LED_LogEvent(LED_Event_BackUserMode);
  }
}



void MovementManagerLoop()
{
  PowerMonitor_MeasureVbat(TRUE);                       //电池检测
  TU32  CurrentPosition = EncoderDriver_GetPosition();  //获取当前位置信息
  TS16  CurrentSpeed    = EncoderDriver_GetSpeed();     //获取当前速度信息
  TBool EncoderOn       = EncoderDriver_IsPowerOn();    //编码器电源是否开启
  
  if(FORBID_ENCODER_PWROFF == FALSE && EncoderOn == TRUE && CurrentSpeed == 0 && me.EncoderShutOffTimerStarted == FALSE)
  {
    StartEncoderAutoShutOffTimer();                     //没有速度就开始编码器电源关闭计时 执行一次     3sec
  }
  else if(CurrentSpeed != 0 && me.EncoderShutOffTimerStarted == TRUE)
  {
    StopEncoderAutoShutOffTimer();                      //有速度且开始计时   停止编码器电源关闭计时
  }
  
  Limits_ComputeLimitsState(&CurrentPosition);             //限位位置比较
  Limits_Monitor();                                        //限位位置状态检测
  MotorControl_ControlLoop(&CurrentSpeed,&CurrentPosition);//电机控制循环
    
  if(CurrentSpeed == 0)
  {
    Chrono_Stop(Chrono_MoveTime);    //速度是零  运动时间停止
  }else
  {
    Chrono_Start(Chrono_MoveTime);   //速度不是零  运动时间计时
  } 
}



static void MovementManager_SpecialMoveConfig()
{
  StopEncoderAutoShutOffTimer();    //停止编码器关闭计时
  
  switch(me.CurrentHandledMoveRequest.MoveType)
  {
      case MoveRequestType_SecurityStop:     //超过限位 安全停止
      case MoveRequestType_None:
        me.RunDeadZoneGearBoxCompensationMove = FALSE;
        me.RecordBlockedDirection = TRUE;
        EncoderDriver_PosCounterCMD(TRUE);
        MotorControl_SecurityMonitorCMD(TRUE);
        Limits_MonitorCMD(TRUE);
        break;
        
      case MoveRequestType_Manual_Stop:                //人为停止
      case MoveRequestType_Manual_Roll:
      case MoveRequestType_Manual_UnRoll:
      case MoveRequestType_Manual_RollForTime:
      case MoveRequestType_Manual_UnRollForTime:      
        me.RunDeadZoneGearBoxCompensationMove = FALSE;  //没有齿轮补偿
        me.RecordBlockedDirection = FALSE;              //不记录阻塞方向
        EncoderDriver_PosCounterCMD(FALSE);             //不记录位置
        MotorControl_SecurityMonitorCMD(TRUE);          //电机控制 安全检测
        Limits_MonitorCMD(FALSE);                       //不检测限位 
        break;
        
      case MoveRequestType_Manual_PWM:
        me.RunDeadZoneGearBoxCompensationMove = FALSE;
        me.RecordBlockedDirection = FALSE;
        EncoderDriver_PosCounterCMD(FALSE);
        MotorControl_SecurityMonitorCMD(FALSE);
        Limits_MonitorCMD(FALSE);
        break;
        
      case MoveRequestType_Nolimit_RollForTime:
      case MoveRequestType_Nolimit_UnRollForTime:
      case MoveRequestType_Nolimit_StopForTime:
      case MoveRequestType_Nolimit_Stop:
      case MoveRequestType_Nolimit_RollTo:
      case MoveRequestType_Nolimit_UnRollTo:
      case MoveRequestType_Nolimit_ShortUnRoll:
      case MoveRequestType_Nolimit_ShortRoll:
      case MoveRequestType_Nolimit_Rolloff:
      case MoveRequestType_Nolimit_UnRolloff:
      case MoveRequestType_Nolimit_ShortFeedBack:
      case MoveRequestType_Nolimit_ErrFeedBackMoveTo:
      case MoveRequestType_Nolimit_ErrFeedBack:
      case MoveRequestType_Nolimit_DoubleShortFeedBack:
      case MoveRequestType_Nolimit_ThreeShortFeedBack:
      case MoveRequestType_Nolimit_FourShortFeedBack:
        me.RunDeadZoneGearBoxCompensationMove = TRUE;
        me.RecordBlockedDirection = TRUE;
        EncoderDriver_PosCounterCMD(TRUE);               //记录位置
        MotorControl_SecurityMonitorCMD(TRUE);           //电机控制 安全检测
        Limits_MonitorCMD(FALSE);                        //不检测限位
        break; 
        
      case MoveRequestType_CompensationMove:
        me.RunDeadZoneGearBoxCompensationMove = FALSE;
        me.RecordBlockedDirection = TRUE;
        EncoderDriver_PosCounterCMD(TRUE);             //记录位置
        MotorControl_SecurityMonitorCMD(TRUE);         //电机控制 安全检测
        Limits_MonitorCMD(TRUE);                       //检测限位
        break;
        
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
      case MoveRequestType_Nolimit_10sCycleMove: //new sage
      case MoveRequestType_Nolimit_TiltingCycleMove://nwe kendy
      case MoveRequestType_TiltMyPosition:       //new sage
        me.RunDeadZoneGearBoxCompensationMove = TRUE; //正常模式 反向齿轮补偿
        me.RecordBlockedDirection = TRUE;             //记录阻塞方向
        EncoderDriver_PosCounterCMD(TRUE);            //计算速度
        MotorControl_SecurityMonitorCMD(TRUE);        //安全检测
        Limits_MonitorCMD(TRUE);                      //检测限位          
        break; 
        
      default :
        break;
  } 
}

//电机运动管理 设置下一个动作 step 1
void MovementManager_SetNextAction()
{        
  if(Stack_PopHead(me.Stack_MoveRequest,&me.TmpMoveRequest) == TRUE)//运动请求 导出堆栈成功
  {
     DEBUG_PRINT("[DBG][MV_Manager] Pop Action, build move list...\n");
    if(MovementPlanner_BuildMoveList(&me.TmpMoveRequest,me.Stack_MoveTmp) == TRUE)  
    {//根据运动指令创建运动列表  成功
      LongTimer_Reload(LongTimer_6MinMaxMoveTimer);//重载3min时间
      MotorControl_HoldCurrentSetSegment();        //停止当前状态
      //Load new move list into move stack
      Stack_Purge(me.Stack_Move);                  //清堆栈
      MovementManager_Move_s MoveTmp;
      while(Stack_PopHead(me.Stack_MoveTmp,&MoveTmp) == TRUE)//导入新参数
      {
        Stack_PushTail(me.Stack_Move,&MoveTmp);
      }     
      me.CurrentHandledMoveRequest = me.TmpMoveRequest;//运行请求
      MovementManager_SpecialMoveConfig();//特殊运动 初始化
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_Motor, TASK_Motor_MOVEMENTMANAGER_NEWMOVE);
      //置MOVEMENTMANAGER_NEWMOVE 事件标志
    }
    else
    {
      SQC_LogEvent(SQC_Event_MoveActionRequestRejected);
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_Motor, TASK_Motor_MOVEMENTMANAGER_NEWACTION);
    }
  }
  else
   { //运动请求 导出堆栈失败
      DEBUG_PRINT("[DBG][MV_Manager] No more action, set NULL\n");
      MoveRequest_s Mvrq = MoveRequestInit; 
      
      if(Motor_Reset == TRUE)//如果电机有复位请求，需要在所有运动请求完成之后开始复位，即抖动结束
      {
          Motor_Reset = FALSE;
          Banker_ApplyFactorySettings();
          RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_MotorReset);
      } 
      
      if(Direction_change==true)
      {
          Direction_change= false;
          RTSErgonomicInterface_SendSpecialRequest(RTSErgo_Request_InvertRotationDirection);   
      }
      
      if(MotorControl_GetLastMoveDirection() == MotorDirection_Roll && me.RunDeadZoneGearBoxCompensationMove == TRUE && USE_RETURNMOVE == 1)
      {
        //运动请求执行完成之后 如果是展开方向 调整百叶角度
        Mvrq.MoveType = MoveRequestType_CompensationMove;
        Mvrq.MoveRequestParam.MoveOff = 0U;
        MoveRequest_Post(&Mvrq); 
      }
      else if(me.CurrentHandledMoveRequest.MoveType != MoveRequestType_None)
      { //其他事件处理
        Mvrq.MoveType = MoveRequestType_None; //没有运动请求
        Mvrq.MoveRequestParam = NoMoveParam;
        MoveRequest_Post(&Mvrq);
      }
      else
      {
      //else do nothing
      }
  }
}





//
void MovementManager_PlanAndSetNextMove()
{
  if(Stack_PopHead(me.Stack_Move,&me.CurrentMove) == TRUE)
  { //读取运动堆栈成功
    //DEBUG_PRINT1("[DBG][MV_Manager] Pop %s => plan...\n",MovementManager_MoveType_TXT[me.CurrentMove.MoveType])
    if( MovementPlanner_PlanMove(&me.CurrentMove,me.Stack_Segment) == TRUE) //根据模式参数设置
    {
      MovementManager_PreMoveAction();
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_Motor, TASK_Motor_MOVEMENTMANAGER_NEWSEG);//事件标志 新的参数
    }
    else
    {
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_Motor, TASK_Motor_MOVEMENTMANAGER_NEWMOVE);
    }
  }else
  {
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_Motor, TASK_Motor_MOVEMENTMANAGER_NEWACTION);
  }
}

//读取编码轮信息
static void MovementManager_PreMoveAction()
{
  EncoderDriver_SensorCmd(TRUE);
}


//设置下一个参数  step 3
void MovementManager_SetNextSegment()
{
  MotorControl_Segment_s NewSeg = MotorControl_Segment_init;  //复位参数
  if(Stack_PopHead(me.Stack_Segment,&NewSeg) == TRUE)
  {//读取控制参数成功
   // DEBUG_PRINT("[DBG][MV_Manager] Pop && Set segment\n")
    MotorControl_SetSegment(&NewSeg);//设置新的参数   循环模式  设置参数
  }else
  {//参数读取不成功 回到 step 2
    SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_Motor, TASK_Motor_MOVEMENTMANAGER_NEWMOVE);
  } 
}



