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

//�������� ��ʼ��
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
  {MoveRequestType_SecurityStop,0,FALSE},    //�˶�����  ���ȼ�  �Ƿ���Ա�ͬ�����
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



//������� ����ȫ������ס
 void MotorControlIsLockBySecuManager(void)
{
  SQC_LogEvent(SQC_Event_MotorBlocked);
  MotorState_e MtrState = MotorControl_GetSecuLockState();//��ȡ������ư�ȫ��״̬
  if(me.RecordBlockedDirection == TRUE && MtrState == MotorState_BlockedRoll || MtrState == MotorState_Rolling_LowSpeed)            //��Ҫ�޸� sage
  {//�������ת    ��ת ���� ���� ��ǰ���ᱻ����
    me.BlockedDirection = MotorDirection_Roll;
  }else if(me.RecordBlockedDirection == TRUE && MtrState == MotorState_BlockedUnRoll || MtrState == MotorState_UnRolling_LowSpeed)  //��Ҫ�޸�
  {//չ�������ת    ��ת ���� ���� ��ǰ���ᱻ����
    me.BlockedDirection = MotorDirection_UnRoll;
  } 
  Stack_Purge(me.Stack_MoveRequest);//������������
  Stack_Purge(me.Stack_Move);       //��������
  Stack_Purge(me.Stack_Segment);    //�����в��� 
  me.LockByMotorControl = TRUE;     //�����ס
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
  me.RunDeadZoneGearBoxCompensationMove = FALSE;                          //������򲹳�
  me.RecordBlockedDirection     = TRUE;                                   //��¼��������
  me.EncoderShutOffTimerStarted = FALSE;                                  //�������رյ���ʱ
  me.LockByMotorControl         = FALSE;                                  //���������ס���
  MotorBlocked=me.LockByMotorControl;
  Stack_Purge(me.Stack_MoveRequest);
  Stack_Purge(me.Stack_Move);
  Stack_Purge(me.Stack_Segment);       //��ջ���� �������
  
  MoveRequest_s Mvrq = MoveRequestInit; 
  Mvrq.MoveType = MoveRequestType_None;
  Mvrq.MoveRequestParam = NoMoveParam;
  MoveRequest_Post(&Mvrq);             //���ֹͣ
  
  MovementManager_SetNextAction();     //NULL ִ����һ������
}

//���й����ʼ��    ���Գ�ʼ��
void MovementManager_Init()
{
  me.RunDeadZoneGearBoxCompensationMove = FALSE;                          //������򲹳�
  me.RecordBlockedDirection     = TRUE;                                   //��¼��������
  me.EncoderShutOffTimerStarted = FALSE;                                  //�������رյ���ʱ
  me.LockByMotorControl         = FALSE;                                  //���������ס���
  MotorBlocked=me.LockByMotorControl;
  MotorControl_Init(MotorControlIsLockBySecuManager,MotorControlIsUnLock);//������Ƴ�ʼ��
  Position_Init();                                                        //λ����Ϣ��ʼ��  ���Գ�ʼ��
  limits_Init();                                                          //��λ��ʼ��
  
  //��ջ����
  //�˶������� ���� ���� ���м�⡱ 
  me.Stack_MoveRequest = Stack_Create_StaticAllocation(sizeof(MoveRequest_s),1,me.Stack_MoveRequestBuffer,MoveReqStack); 
  me.Stack_Move        = Stack_Create_StaticAllocation(sizeof(MovementManager_Move_s),MOVE_STACK_DEPTH,me.MoveStackBuffer,MoveStack);
  me.Stack_Segment     = Stack_Create_StaticAllocation(sizeof(MotorControl_Segment_s),SEGMENT_STACK_DEPTH ,me.SegmentStackBuffer,SegmentStack);
  me.Stack_MoveTmp     = Stack_Create_StaticAllocation(sizeof(MovementManager_Move_s),MOVE_STACK_DEPTH,me.MoveTmpStackBuffer,MoveStackTmp);
  
  if(SFY_OS_TimerCreate(MOVEMENTMANAGER_LOOP_TIMER_ID,OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_LOOP) != OS_SUCCESS)
  {//����һ��ʱ�Ӷ��� TASK_Motor TASK_Motor_MOVEMENTMANAGER_LOOP  ���ڵ�����м��
    trap();
  }
  
  if(SFY_OS_TimerCreate(MOVEMENTMANAGER_STOP_TIMER_ID,OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Controller,TASK_Controller_STOP_TO) != OS_SUCCESS)
  {//����һ��ʱ�Ӷ��� TASK_Controller TASK_Controller_STOP_TO     ���ڹرձ�������Դ ���ݱ���
    trap();
  } 
  MoveRequest_s Mvrq = MoveRequestInit;
  Mvrq.MoveType      = MoveRequestType_None;//��������Ϊ��
  me.CurrentHandledMoveRequest = Mvrq;
  
  LongTimer_Setup(LongTimer_6MinMaxMoveTimer,MAXMOVETIME_MS,FALSE,FALSE,MovementManager_3MinMoveTimeout);
  //����һ����ʱ������ ��ֹ����ʱ�䳬��6min
}

void MovementManager_Create(TBool WatchdogReset)
{
  MotorControl_Create(WatchdogReset);
  Position_Create();                                 //��������λ�ò���
  TU32 CurrentPosition = EncoderDriver_GetPosition();//��ȡ��ǰλ��
  limits_Create(&CurrentPosition);                   //��λ����
  
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

//�˶����� Ϊ��
TBool MovementManager_NoneSet()
{
  TU8 MoveStacked = Stack_GetObjectCount(me.Stack_Move);                        //�˶���ջ������ȡ
  TU8 SegStacked  = Stack_GetObjectCount(me.Stack_Segment);                     //������ջ������ȡ
  TBool NoneIsSet = FALSE;
  if(MoveStacked == 0u &&
     SegStacked  == 0u &&
     me.CurrentHandledMoveRequest.MoveType == MoveRequestType_None)             //��ջ��Ϊ�գ�û���˶�����
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


//����˶�����
void MoveRequest_Execute(const MoveRequest_s *Mvrq)
{
  const MoveRequestPrio_s* NewMoveRequestPrio     = GetMoveRequestPrio(Mvrq->MoveType);                       //����������ȼ�
  const MoveRequestPrio_s* CurrentMoveRequestPrio = GetMoveRequestPrio(me.CurrentHandledMoveRequest.MoveType);//Ŀǰ��������ȼ�
  TBool IsCurrentSegmentUnInteruptible             = MotorControl_GetCurrentSegment()->UnInteruptible;         //�Ƿ���Ա���� ��ֹ
  
  if(me.LockByMotorControl == FALSE)/////����״̬֮��������ס
  {
    //Test to start action immediatly
    if(IsCurrentSegmentUnInteruptible == FALSE && ( (NewMoveRequestPrio->MovePriority <  CurrentMoveRequestPrio->MovePriority)  ||
                                                    (NewMoveRequestPrio->MovePriority == CurrentMoveRequestPrio->MovePriority && 
                                                     CurrentMoveRequestPrio->IsInterruptibleBySamePriority == TRUE) ||
                                                    (NewMoveRequestPrio->RequestType == MoveRequestType_None) ) 
       )
    {
      DEBUG_PRINT("[DBG] Start Immediatly\n");
      //���ȼ���������ִ��
      Stack_Purge(me.Stack_MoveRequest);
      Stack_PushHead(me.Stack_MoveRequest,Mvrq);
      MovementManager_SetNextAction();  //ִ����һ������
    }
    else
    {
      MoveRequest_s StackedReq = MoveRequestInit;
      if(Stack_ReadObject(me.Stack_MoveRequest,&StackedReq,0) == TRUE)
      {
        const MoveRequestPrio_s* StackedMoveRequestPrio = NULL;
        StackedMoveRequestPrio = GetMoveRequestPrio(StackedReq.MoveType);//��ȡ����˶����ȼ�
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
          //����ʧ
          DEBUG_PRINT("[DBG][MV_Manager] Stack Full => Request Lost !!! \n");
        }
      }
      else
      {
          //����ѹ���ջ
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


//3min�ӳ�ʱ
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

//�����෴����
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
  PowerMonitor_MeasureVbat(TRUE);                       //��ؼ��
  TU32  CurrentPosition = EncoderDriver_GetPosition();  //��ȡ��ǰλ����Ϣ
  TS16  CurrentSpeed    = EncoderDriver_GetSpeed();     //��ȡ��ǰ�ٶ���Ϣ
  TBool EncoderOn       = EncoderDriver_IsPowerOn();    //��������Դ�Ƿ���
  
  if(FORBID_ENCODER_PWROFF == FALSE && EncoderOn == TRUE && CurrentSpeed == 0 && me.EncoderShutOffTimerStarted == FALSE)
  {
    StartEncoderAutoShutOffTimer();                     //û���ٶȾͿ�ʼ��������Դ�رռ�ʱ ִ��һ��     3sec
  }
  else if(CurrentSpeed != 0 && me.EncoderShutOffTimerStarted == TRUE)
  {
    StopEncoderAutoShutOffTimer();                      //���ٶ��ҿ�ʼ��ʱ   ֹͣ��������Դ�رռ�ʱ
  }
  
  Limits_ComputeLimitsState(&CurrentPosition);             //��λλ�ñȽ�
  Limits_Monitor();                                        //��λλ��״̬���
  MotorControl_ControlLoop(&CurrentSpeed,&CurrentPosition);//�������ѭ��
    
  if(CurrentSpeed == 0)
  {
    Chrono_Stop(Chrono_MoveTime);    //�ٶ�����  �˶�ʱ��ֹͣ
  }else
  {
    Chrono_Start(Chrono_MoveTime);   //�ٶȲ�����  �˶�ʱ���ʱ
  } 
}



static void MovementManager_SpecialMoveConfig()
{
  StopEncoderAutoShutOffTimer();    //ֹͣ�������رռ�ʱ
  
  switch(me.CurrentHandledMoveRequest.MoveType)
  {
      case MoveRequestType_SecurityStop:     //������λ ��ȫֹͣ
      case MoveRequestType_None:
        me.RunDeadZoneGearBoxCompensationMove = FALSE;
        me.RecordBlockedDirection = TRUE;
        EncoderDriver_PosCounterCMD(TRUE);
        MotorControl_SecurityMonitorCMD(TRUE);
        Limits_MonitorCMD(TRUE);
        break;
        
      case MoveRequestType_Manual_Stop:                //��Ϊֹͣ
      case MoveRequestType_Manual_Roll:
      case MoveRequestType_Manual_UnRoll:
      case MoveRequestType_Manual_RollForTime:
      case MoveRequestType_Manual_UnRollForTime:      
        me.RunDeadZoneGearBoxCompensationMove = FALSE;  //û�г��ֲ���
        me.RecordBlockedDirection = FALSE;              //����¼��������
        EncoderDriver_PosCounterCMD(FALSE);             //����¼λ��
        MotorControl_SecurityMonitorCMD(TRUE);          //������� ��ȫ���
        Limits_MonitorCMD(FALSE);                       //�������λ 
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
        EncoderDriver_PosCounterCMD(TRUE);               //��¼λ��
        MotorControl_SecurityMonitorCMD(TRUE);           //������� ��ȫ���
        Limits_MonitorCMD(FALSE);                        //�������λ
        break; 
        
      case MoveRequestType_CompensationMove:
        me.RunDeadZoneGearBoxCompensationMove = FALSE;
        me.RecordBlockedDirection = TRUE;
        EncoderDriver_PosCounterCMD(TRUE);             //��¼λ��
        MotorControl_SecurityMonitorCMD(TRUE);         //������� ��ȫ���
        Limits_MonitorCMD(TRUE);                       //�����λ
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
        me.RunDeadZoneGearBoxCompensationMove = TRUE; //����ģʽ ������ֲ���
        me.RecordBlockedDirection = TRUE;             //��¼��������
        EncoderDriver_PosCounterCMD(TRUE);            //�����ٶ�
        MotorControl_SecurityMonitorCMD(TRUE);        //��ȫ���
        Limits_MonitorCMD(TRUE);                      //�����λ          
        break; 
        
      default :
        break;
  } 
}

//����˶����� ������һ������ step 1
void MovementManager_SetNextAction()
{        
  if(Stack_PopHead(me.Stack_MoveRequest,&me.TmpMoveRequest) == TRUE)//�˶����� ������ջ�ɹ�
  {
     DEBUG_PRINT("[DBG][MV_Manager] Pop Action, build move list...\n");
    if(MovementPlanner_BuildMoveList(&me.TmpMoveRequest,me.Stack_MoveTmp) == TRUE)  
    {//�����˶�ָ����˶��б�  �ɹ�
      LongTimer_Reload(LongTimer_6MinMaxMoveTimer);//����3minʱ��
      MotorControl_HoldCurrentSetSegment();        //ֹͣ��ǰ״̬
      //Load new move list into move stack
      Stack_Purge(me.Stack_Move);                  //���ջ
      MovementManager_Move_s MoveTmp;
      while(Stack_PopHead(me.Stack_MoveTmp,&MoveTmp) == TRUE)//�����²���
      {
        Stack_PushTail(me.Stack_Move,&MoveTmp);
      }     
      me.CurrentHandledMoveRequest = me.TmpMoveRequest;//��������
      MovementManager_SpecialMoveConfig();//�����˶� ��ʼ��
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_Motor, TASK_Motor_MOVEMENTMANAGER_NEWMOVE);
      //��MOVEMENTMANAGER_NEWMOVE �¼���־
    }
    else
    {
      SQC_LogEvent(SQC_Event_MoveActionRequestRejected);
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_Motor, TASK_Motor_MOVEMENTMANAGER_NEWACTION);
    }
  }
  else
   { //�˶����� ������ջʧ��
      DEBUG_PRINT("[DBG][MV_Manager] No more action, set NULL\n");
      MoveRequest_s Mvrq = MoveRequestInit; 
      
      if(Motor_Reset == TRUE)//�������и�λ������Ҫ�������˶��������֮��ʼ��λ������������
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
        //�˶�����ִ�����֮�� �����չ������ ������Ҷ�Ƕ�
        Mvrq.MoveType = MoveRequestType_CompensationMove;
        Mvrq.MoveRequestParam.MoveOff = 0U;
        MoveRequest_Post(&Mvrq); 
      }
      else if(me.CurrentHandledMoveRequest.MoveType != MoveRequestType_None)
      { //�����¼�����
        Mvrq.MoveType = MoveRequestType_None; //û���˶�����
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
  { //��ȡ�˶���ջ�ɹ�
    //DEBUG_PRINT1("[DBG][MV_Manager] Pop %s => plan...\n",MovementManager_MoveType_TXT[me.CurrentMove.MoveType])
    if( MovementPlanner_PlanMove(&me.CurrentMove,me.Stack_Segment) == TRUE) //����ģʽ��������
    {
      MovementManager_PreMoveAction();
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_Motor, TASK_Motor_MOVEMENTMANAGER_NEWSEG);//�¼���־ �µĲ���
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

//��ȡ��������Ϣ
static void MovementManager_PreMoveAction()
{
  EncoderDriver_SensorCmd(TRUE);
}


//������һ������  step 3
void MovementManager_SetNextSegment()
{
  MotorControl_Segment_s NewSeg = MotorControl_Segment_init;  //��λ����
  if(Stack_PopHead(me.Stack_Segment,&NewSeg) == TRUE)
  {//��ȡ���Ʋ����ɹ�
   // DEBUG_PRINT("[DBG][MV_Manager] Pop && Set segment\n")
    MotorControl_SetSegment(&NewSeg);//�����µĲ���   ѭ��ģʽ  ���ò���
  }else
  {//������ȡ���ɹ� �ص� step 2
    SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_Motor, TASK_Motor_MOVEMENTMANAGER_NEWMOVE);
  } 
}



