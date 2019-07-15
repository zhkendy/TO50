// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

/**
* @file 
* @brief The moveplanner is in charg e of building a move list (MovementManager_Move_s) from a moverequest (MoveRequest_s)
* Then to build a segment list from MovementManager_Move_s
* @details
* @author Gabriel Mula
* @date 2016
*/
//
#include "MovementManager.h"
#include "Stack.h"
#include "Limits.h"
#include "Position.h"
#include "Encoder_Driver.h"
#include "config.h"
#include "MovementPlanner.h"
#include "SQC.h"

#ifdef MOVEMENTPLANNER_PRINT_DEBUG	
#include "Movement_DebugTXT.h"
#include "DebugPrint.h"
#endif

#ifdef MOVEMENTPLANNER_PRINT_INFO
#include "Movement_DebugTXT.h"
#include "DebugPrint_info.h"
#endif

#ifdef MOVEMENTPLANNER_PRINT_LOG	
#include "Movement_DebugTXT.h"
#include "DebugPrint_Log.h"
#endif

extern TU16 Current_Speed;  
extern bool Tilting_Move;
extern bool Tilt_MyPosition;
extern bool Get_MotorDriver_Rotation();
TU16    SpeedADJCycle_timems=20200u;



//MoveRequest_s => MovementManager_Move_s List
TBool MovementPlanner_BuildMoveList(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = FALSE;
  if(MvRq->MoveType < MoveRequestType_LAST_ENUM)
  {
    Stack_Purge(TmpMvStack);
    DEBUG_PRINT1("[DBG][MV_Planner] Build MvReq(%s) Move list :\n",MoveRequestType_TXT[MvRq->MoveType]);
    switch(MvRq->MoveType)
    {
      case MoveRequestType_SecurityStop :              
        Rtn = BuildMoveList_SecurityStop(MvRq,TmpMvStack);
        break;
      case MoveRequestType_None  :
        Rtn = BuildMoveList_None(MvRq,TmpMvStack);                     
        break;
      case MoveRequestType_Manual_Stop :  
        SQC_LogEvent(SQC_Event_StopReq); 
        Rtn = BuildMoveList_Manual_Stop(MvRq,TmpMvStack);      
        break;  
      case MoveRequestType_Manual_Roll :  
        LED_LogEvent(LED_Event_RollReq); 
        SQC_LogEvent(SQC_Event_RollReq); 
        Rtn = BuildMoveList_Manual_Roll(MvRq,TmpMvStack);        
        break;
      case MoveRequestType_Manual_UnRoll :  
        LED_LogEvent(LED_Event_UnRollReq); 
        SQC_LogEvent(SQC_Event_UnRollReq); 
        Rtn = BuildMoveList_Manual_UnRoll(MvRq,TmpMvStack);         
        break;
      case MoveRequestType_Manual_RollForTime : 
        LED_LogEvent(LED_Event_RollReq); 
        SQC_LogEvent(SQC_Event_RollReq);
        Rtn = BuildMoveList_Manual_RollForTime(MvRq,TmpMvStack);
        break;
      case MoveRequestType_Manual_UnRollForTime :  
        LED_LogEvent(LED_Event_UnRollReq); 
        SQC_LogEvent(SQC_Event_UnRollReq); 
        Rtn = BuildMoveList_Manual_UnRollForTime(MvRq,TmpMvStack);       
        break;
      case MoveRequestType_Manual_PWM :             
        Rtn = BuildMoveList_Manual_PWM(MvRq,TmpMvStack);        
        break;
      case MoveRequestType_Nolimit_RollForTime :  
        LED_LogEvent(LED_Event_RollReq); 
        SQC_LogEvent(SQC_Event_RollReq); 
        Rtn = BuildMoveList_Nolimit_RollForTime(MvRq,TmpMvStack);     
        break;
      case MoveRequestType_Nolimit_UnRollForTime : 
        LED_LogEvent(LED_Event_UnRollReq); 
        SQC_LogEvent(SQC_Event_UnRollReq); 
        Rtn = BuildMoveList_Nolimit_UnRollForTime(MvRq,TmpMvStack);   
        break;
      case MoveRequestType_Nolimit_StopForTime : 
        SQC_LogEvent(SQC_Event_StopReq);
        Rtn = BuildMoveList_Nolimit_StopForTime(MvRq,TmpMvStack);   
        break;
      case MoveRequestType_Nolimit_RollTo :     
        LED_LogEvent(LED_Event_RollReq); 
        SQC_LogEvent(SQC_Event_RollReq); 
        Rtn = BuildMoveList_Nolimit_RollTo(MvRq,TmpMvStack);   
        break;
      case MoveRequestType_Nolimit_UnRollTo :      
        LED_LogEvent(LED_Event_UnRollReq);
        SQC_LogEvent(SQC_Event_UnRollReq); 
        Rtn = BuildMoveList_Nolimit_UnRollTo(MvRq,TmpMvStack);    
        break;
      case MoveRequestType_Nolimit_ShortUnRoll :  
        SQC_LogEvent(SQC_Event_UnRollReq);
        Rtn = BuildMoveList_Nolimit_ShortUnRoll(MvRq,TmpMvStack);  
        break;
      case MoveRequestType_Nolimit_ShortRoll : 
        LED_LogEvent(LED_Event_RollReq);       
        SQC_LogEvent(SQC_Event_RollReq); 
        Rtn = BuildMoveList_Nolimit_ShortRoll(MvRq,TmpMvStack);    
        break;
      case MoveRequestType_Nolimit_Rolloff :     
        LED_LogEvent(LED_Event_RollReq);
        SQC_LogEvent(SQC_Event_RollReq); 
        Rtn = BuildMoveList_Nolimit_Rolloff(MvRq,TmpMvStack);    
        break;
      case MoveRequestType_Nolimit_UnRolloff :   
        LED_LogEvent(LED_Event_UnRollReq); 
        SQC_LogEvent(SQC_Event_UnRollReq);
        Rtn = BuildMoveList_Nolimit_UnRolloff(MvRq,TmpMvStack);  
        break;
      case MoveRequestType_RollForTime :     
        LED_LogEvent(LED_Event_RollReq); 
        SQC_LogEvent(SQC_Event_RollReq);
        Rtn = BuildMoveList_RollForTime(MvRq,TmpMvStack);  
        break;
      case MoveRequestType_UnRollForTime :  
        LED_LogEvent(LED_Event_UnRollReq);
        SQC_LogEvent(SQC_Event_UnRollReq);
        Rtn = BuildMoveList_UnRollForTime(MvRq,TmpMvStack);  
        break;
      case MoveRequestType_RollTo :        
        LED_LogEvent(LED_Event_RollReq); 
        SQC_LogEvent(SQC_Event_RollReq); 
        Rtn = BuildMoveList_RollToAbsPos(MvRq,TmpMvStack);    
        break;
      case MoveRequestType_UnRollTo :      
        LED_LogEvent(LED_Event_UnRollReq); 
        SQC_LogEvent(SQC_Event_UnRollReq);
        Rtn = BuildMoveList_UnRollToAbsPos(MvRq,TmpMvStack);  
        break;
      case MoveRequestType_Nolimit_Stop : 
        SQC_LogEvent(SQC_Event_StopReq);
        Rtn = BuildMoveList_Nolimit_Stop(MvRq,TmpMvStack);     
        break;
      case MoveRequestType_GoToPercent :                 
        Rtn = BuildMoveList_GoToPercent(MvRq,TmpMvStack);    
        break;
      case MoveRequestType_GoToAbsPos :            
        Rtn = BuildMoveList_GoToAbsPos(MvRq,TmpMvStack);   
        break;
      case MoveRequestType_Nolimit_ShortFeedBack : 
        SQC_LogEvent(SQC_Event_FeedbackReq);         
        Rtn = BuildMoveList_Nolimit_ShortFeedBack(MvRq,TmpMvStack);   
        break;
      case MoveRequestType_Nolimit_ErrFeedBack :   
        LED_LogEvent(LED_Event_ErrorFeedbackReq);
        SQC_LogEvent(SQC_Event_ErrorFeedbackReq);
        Rtn = BuildMoveList_Nolimit_ErrFeedBack(MvRq,TmpMvStack);    
        break;
      case MoveRequestType_Nolimit_ErrFeedBackMoveTo :  
        LED_LogEvent(LED_Event_ErrorFeedbackReq);
        SQC_LogEvent(SQC_Event_ErrorFeedbackReq);
        Rtn = BuildMoveList_Nolimit_ErrFdBack_MoveTo(MvRq,TmpMvStack);    
        break;
      case MoveRequestType_Nolimit_DoubleShortFeedBack :
        SQC_LogEvent(SQC_Event_DoubleFeedbackReq);
        Rtn = BuildMoveList_Nolimit_DoubleShortFeedBack(MvRq,TmpMvStack);  
        break;
      case MoveRequestType_Nolimit_ThreeShortFeedBack :       
        Rtn = BuildMoveList_Nolimit_ThreeShortFeedBack(MvRq,TmpMvStack);  
        break;
      case MoveRequestType_Nolimit_FourShortFeedBack :       
        Rtn = BuildMoveList_Nolimit_FourShortFeedBack(MvRq,TmpMvStack);  
        break;
      case MoveRequestType_Nolimit_10sCycleMove :     
        //LED_LogEvent(LED_Event_DoubleFeedbackReq);   
        Rtn = BuildMoveList_10sCycle_MoveMent(MvRq,TmpMvStack);  
        break;
      case MoveRequestType_Nolimit_TiltingCycleMove :     
        //LED_LogEvent(LED_Event_DoubleFeedbackReq);   
        Rtn = BuildMoveList_TiltingCycle_MoveMent(MvRq,TmpMvStack);  
        break;
      case MoveRequestType_ShortUnRoll :
        LED_LogEvent(LED_Event_UnRollReq); ;
        SQC_LogEvent(SQC_Event_UnRollReq);
        Rtn = BuildMoveList_ShortUnRoll(MvRq,TmpMvStack);     
        break;
      case MoveRequestType_ShortRoll :     
        LED_LogEvent(LED_Event_RollReq); 
        SQC_LogEvent(SQC_Event_RollReq);
        Rtn = BuildMoveList_ShortRoll(MvRq,TmpMvStack);  
        break;
      case MoveRequestType_Rolloff :      
        LED_LogEvent(LED_Event_RollReq); 
        SQC_LogEvent(SQC_Event_RollReq); 
        Rtn = BuildMoveList_Rolloff(MvRq,TmpMvStack);    
        break;
      case MoveRequestType_UnRolloff :     
        LED_LogEvent(LED_Event_UnRollReq); 
        SQC_LogEvent(SQC_Event_UnRollReq);
        Rtn = BuildMoveList_UnRolloff(MvRq,TmpMvStack);    
        break;
      case MoveRequestType_RollToLimits :      
        LED_LogEvent(LED_Event_RollReq); 
        SQC_LogEvent(SQC_Event_RollReq); 
        Rtn = BuildMoveList_RollToLimits(MvRq,TmpMvStack); 
        break;
      case MoveRequestType_UnRollToLimits :
        LED_LogEvent(LED_Event_UnRollReq); 
        SQC_LogEvent(SQC_Event_UnRollReq);
        Rtn = BuildMoveList_UnRollToLimits(MvRq,TmpMvStack);   
        break;
      case MoveRequestType_GoToLimits :               
        Rtn = BuildMoveList_GoToLimits(MvRq,TmpMvStack);    
        break;
      case MoveRequestType_RollToPosition :     
        LED_LogEvent(LED_Event_RollReq); 
        SQC_LogEvent(SQC_Event_RollReq); 
        Rtn = BuildMoveList_RollToPosition(MvRq,TmpMvStack); 
        break;
      case MoveRequestType_UnRollToPosition :
        LED_LogEvent(LED_Event_UnRollReq); 
        SQC_LogEvent(SQC_Event_UnRollReq);
        Rtn = BuildMoveList_UnRollToPosition(MvRq,TmpMvStack);
        break;
      case MoveRequestType_GoToPosition :              
        Rtn = BuildMoveList_GoToPosition(MvRq,TmpMvStack); 
        break;
      case MoveRequestType_CompensationMove :
        Rtn = BuildMoveList_Nolimit_UnRolloff(MvRq,TmpMvStack);  
        break;
      case MoveRequestType_TiltMyPosition :
        Rtn = BuildMoveList_TiltMyPosition(MvRq,TmpMvStack);  
        break;
        
      default :
        break;
    }
  }
  
  DEBUG_PRINT1("[DBG][MV_Planner] Build MvReq success (%d);  \n",Rtn);
  return Rtn;
}

TBool MovementPlanner_PlanMove(MovementManager_Move_s *Mv,StackObjRef Segment)
{
  TBool Rtn = FALSE;
  Stack_Purge(Segment);
  if(Mv->MoveType < MovementManager_MoveType_LASTENUM)
  {
    switch(Mv->MoveType)
    {
      case MovementManager_MoveType_SecurityStop :     
        Rtn =PlanMove_SecurityStop(Mv,Segment);
        break;
        
      case MovementManager_MoveType_None:
        Rtn =PlanMove_None(Mv,Segment);
        break;
        
      case MovementManager_MoveType_Manual_PWM:        
        Rtn =PlanMove_Manual_PWM(Mv,Segment);
        break;
        
      case MovementManager_MoveType_Move:
        Rtn =PlanMove_Move(Mv,Segment);
        break;
        
      case MovementManager_MoveType_MoveForTime:       
        Rtn =PlanMove_MoveForTime(Mv,Segment);
        break;
        
      case MovementManager_MoveType_Stop:           
        Rtn =PlanMove_Stop(Mv,Segment);
        break;
        
      case MovementManager_MoveType_StopForTime:      
        Rtn =PlanMove_StopForTime(Mv,Segment);
        break;
        
      case MovementManager_MoveType_MoveTo:           
        Rtn =PlanMove_MoveTo(Mv,Segment);
        break;
        
      default :
        break;
    } 
  }
  return  Rtn;
}


static TU32 MovementPlanner_EstimateMoveDistance(TU16 RPMSpeed,TU32 MoveTime_ms)
{
  TU32 tmp = (RPMSpeed * MoveTime_ms);
  TU32 Rtn = tmp/(60000);
  return Rtn;
}


static TU32 MovementPlanner_EstimateMoveTime(TU16 RPMSpeed,TU32 Distance)
{
  TU32 tmp = Distance * 60000;
  TU32 Rtn = tmp / RPMSpeed;
  return Rtn;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Build Move List///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static TBool BuildMoveList_SecurityStop(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  Stack_SecurityStop(TmpMvStack);
  return  TRUE;
}

static TBool BuildMoveList_None(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  Stack_None(TmpMvStack);
  return  TRUE;
}

//Manual Move are perform without counting position
//��Ϊ�ƶ� ������λ��
//The limits monitor is off
//��λ��عر�
//The MotorControl secu monitor is on, but after a blocking, the direction is not lock
static TBool BuildMoveList_Manual_Stop(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{//��Ϊֹͣ
  TBool Rtn = FALSE;
  //Stop 
  if(EncoderDriver_GetMoveDirection() != EncoderDriver_Stop)
  {
    Rtn = TRUE;
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);//NormalUserStop
  }
  return  Rtn;
}

//��Ϊ�˶��б� չ��
static TBool BuildMoveList_Manual_Roll(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = TRUE;
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  EncoderDriverMovingDirection_e CurrentDirection = EncoderDriver_GetMoveDirection();
  if(CurrentDirection == EncoderDriver_UnRoll)
  {
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);//NormalUserStop
  }
 // Stack_Move(TmpMvStack,MotorDirection_Roll,MoveOption_UseCustom,ACC_FAST,ACC_FAST,MININUM_10_SPEED);  
  Stack_Move(TmpMvStack,MotorDirection_Roll,MoveOption_NoOption,0,0,0);   //MoveOption_NoOption =Default ACC/DCC/Speed
  return  Rtn;
}

//��Ϊ�˶��б� ����
static TBool BuildMoveList_Manual_UnRoll(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = TRUE;
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  EncoderDriverMovingDirection_e CurrentDirection = EncoderDriver_GetMoveDirection();
  if(CurrentDirection == EncoderDriver_Roll)
  {
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);//NormalUserStop
  }
  //Stack_Move(TmpMvStack,MotorDirection_UnRoll,MoveOption_UseCustom,ACC_FAST,ACC_FAST,MININUM_10_SPEED);  
  Stack_Move(TmpMvStack,MotorDirection_UnRoll,MoveOption_NoOption,0,0,0);  //MoveOption_NoOption =Default ACC/DCC/Speed
  return  Rtn;
}

//��Ϊ�˶� ��ʱ�� չ��
static TBool BuildMoveList_Manual_RollForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = TRUE;
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  EncoderDriverMovingDirection_e CurrentDirection = EncoderDriver_GetMoveDirection();
  if(CurrentDirection == EncoderDriver_UnRoll)
  {
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);//NormalUserStop
  }
  
  Stack_MoveForTime(TmpMvStack,MotorDirection_Roll,MoveOption_NoOption,MvRq->MoveRequestParam.Time_mS);
  return  Rtn;
}

//��Ϊ�˶� ��ʱ�� ����
static TBool BuildMoveList_Manual_UnRollForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = TRUE;
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  EncoderDriverMovingDirection_e CurrentDirection = EncoderDriver_GetMoveDirection();
  //Stop before reverse
  if(CurrentDirection == EncoderDriver_Roll)
  {
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);//NormalUserStop
  }
  
  Stack_MoveForTime(TmpMvStack,MotorDirection_UnRoll,MoveOption_NoOption,MvRq->MoveRequestParam.Time_mS);
  return  Rtn;
}

//��Ϊ�˶� PWM����
static TBool BuildMoveList_Manual_PWM(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = TRUE;
  MotorDirection_e Dir = (MvRq->MoveRequestParam.PWM >= 0) ? (MotorDirection_UnRoll) : (MotorDirection_Roll);
  Stack_Manual_PWM(TmpMvStack,Dir,abs(MvRq->MoveRequestParam.PWM)); 
  return  Rtn;
}

//չ�� ����ʱ�����
static TBool BuildMoveList_RollForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool DirIsLocked = MovementManager_IsDirectionLock(MotorDirection_Roll);//�жϷ����Ƿ�����
  TU32  CurrentPosition = EncoderDriver_GetPosition();                     //��ȡ��ǰλ��
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);            //�����෴����
  //Estimate final position
  //���������ƶ�ֹͣλ��
  TU32 EstimatedMoveDistance = MovementPlanner_EstimateMoveDistance(Current_Speed,MvRq->MoveRequestParam.Time_mS);
  TU32 EstimatedFinalPosition = CurrentPosition - EstimatedMoveDistance;
  //Is final position inside current limit ?//�ж��Ƿ�����λ��
  TBool TargetIsInsideLimit = Limits_IsTargetPositionInsideLimits(&EstimatedFinalPosition,POSITION_TOLERANCE);
  
  return (TargetIsInsideLimit == TRUE && DirIsLocked == FALSE) ? (BuildMoveList_Nolimit_RollForTime(MvRq,TmpMvStack) ) : (FALSE);
  //����λ�� ����û�б�lock ִ�в���
}


//���� ����ʱ�����
static TBool BuildMoveList_UnRollForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool DirIsLocked = MovementManager_IsDirectionLock(MotorDirection_UnRoll);//�жϷ����Ƿ�����
  TU32 CurrentPosition = EncoderDriver_GetPosition();                        //��ȡ��ǰλ��
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);            //�����෴����
  //Estimate final position
  TU32 EstimatedMoveDistance = MovementPlanner_EstimateMoveDistance(Current_Speed,MvRq->MoveRequestParam.Time_mS);
  TU32 EstimatedFinalPosition = CurrentPosition + EstimatedMoveDistance;
  //Is final position inside current limit ?//�ж��Ƿ�����λ��
  TBool TargetIsInsideLimit = Limits_IsTargetPositionInsideLimits(&EstimatedFinalPosition,POSITION_TOLERANCE);
  
  return (TargetIsInsideLimit == TRUE && DirIsLocked == FALSE) ? (BuildMoveList_Nolimit_UnRollForTime(MvRq,TmpMvStack)) : (FALSE);
   //����λ�� ����û�б�lock ִ�в���
}

static TBool BuildMoveList_Nolimit_StopForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  Stack_StopForTime(TmpMvStack,MoveOption_NormalUserStop,MvRq->MoveRequestParam.Time_mS);
  return  TRUE;
}



//չ�������Ե�ַ
static TBool BuildMoveList_RollToAbsPos(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = FALSE;
  //Is target position inside limit ?
  TBool TargetIsInsideLimit = Limits_IsTargetPositionInsideLimits(&MvRq->MoveRequestParam.AbsPos,POSITION_TOLERANCE);
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  if(TargetIsInsideLimit == TRUE)
  { //Ŀ��λ������λ֮��
    Rtn = BuildMoveList_Nolimit_RollTo(MvRq,TmpMvStack);
  }
  else
  { //Ŀ�겻����λ֮��       ������λ
    //Go as close as posible to the requested target
    MvRq->MoveRequestParam.Limit = ModeLimitName_UpLimit;
    Rtn = BuildMoveList_RollToLimits(MvRq,TmpMvStack);
  }
  return  Rtn;
}



//���𵽾��Ե�ַ
static TBool BuildMoveList_UnRollToAbsPos(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = FALSE;
  //Is target position inside limit ?
  TBool TargetIsInsideLimit = Limits_IsTargetPositionInsideLimits(&MvRq->MoveRequestParam.AbsPos,POSITION_TOLERANCE);
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  if(TargetIsInsideLimit == TRUE)
  {//Ŀ��λ������λ֮��
    Rtn = BuildMoveList_Nolimit_UnRollTo(MvRq,TmpMvStack);
  }
  else
  {//Ŀ��λ�ò�����λ֮��    ������λ
   //Go as close as posible to the requested target
    MvRq->MoveRequestParam.Limit = ModeLimitName_DownLimit;
    Rtn = BuildMoveList_UnRollToLimits(MvRq,TmpMvStack);
  }
  return  Rtn;
}

//����λ����  չ����
static TBool BuildMoveList_Nolimit_RollTo(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = FALSE;
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  TBool DirIsLocked = MovementManager_IsDirectionLock(MotorDirection_Roll); //�жϷ����Ƿ�����
  
  //Test if target can be reach by rolling move, and not already on target
  TU32 CurrentPosition = EncoderDriver_GetPosition();   //��ȡ��ǰλ��
  PositionCompare_e PosCmp =  Position_CompareTwoAbsolutePosition(&MvRq->MoveRequestParam.AbsPos ,&CurrentPosition,POSITION_TOLERANCE);  
  //�Ƚ�Ŀ��λ���ܷ�ﵽ�򳬹�
  EncoderDriverMovingDirection_e MoveDir = EncoderDriver_GetMoveDirection();
  
  if(PosCmp == CurrentPositionIsBelowRef && DirIsLocked == FALSE)
  { //λ�ñȽ� ��Ŀ��λ�õ����� ���� �÷���û�б�����
    Rtn = TRUE;
    //Stop before reverse  ���з����෴ Ӧ��ֹͣ
    if(MoveDir == EncoderDriver_UnRoll)
    {
      Stack_Stop(TmpMvStack,MoveOption_NormalUserStop); //NormalUserStop
    }
    //Adjust target to perform gearbox compensation move later, and reach target by unrolling move
    //����Ŀ��ȥʵ�ֳ����䲹����֮����˶���  ����
    MvRq->MoveRequestParam.AbsPos = MvRq->MoveRequestParam.AbsPos - RETURNMOVELENGHT;
    if(Tilting_Move==true)
      Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MoveOption_UseCustom,6000u,ACC_FAST,Current_Speed,MvRq->MoveRequestParam.AbsPos); 
    else 
      Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MoveOption_NoOption,0,0,0,MvRq->MoveRequestParam.AbsPos);  //չ����Ŀ�ĵ�ַ   �˶�ģʽ NoOption
  }  
  return Rtn;
}


//����λ����  ����
static TBool BuildMoveList_Nolimit_UnRollTo(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = FALSE;
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  TBool DirIsLocked = MovementManager_IsDirectionLock(MotorDirection_UnRoll);//�жϷ����Ƿ�����
  
  //Test if target can be reach by unrolling move
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  PositionCompare_e PosCmp =  Position_CompareTwoAbsolutePosition(&MvRq->MoveRequestParam.AbsPos ,&CurrentPosition,2);  
  
  EncoderDriverMovingDirection_e MoveDir = EncoderDriver_GetMoveDirection();
  
  if(PosCmp == CurrentPositionIsAboveRef && DirIsLocked == FALSE)
  {
    Rtn = TRUE;
    //Stop before reverse  ����֮ǰ��ֹͣ
    if(MoveDir == EncoderDriver_Roll)
    {
      Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);
    }
    if(Tilting_Move==true)
      Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MoveOption_UseCustom,6000u,ACC_FAST,Current_Speed,MvRq->MoveRequestParam.AbsPos); 
    else 
      Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MoveOption_NoOption,0,0,0,MvRq->MoveRequestParam.AbsPos);//�˶�ģʽ NoOption
  }
  
  return Rtn;
}



static TBool BuildMoveList_GoToPercent(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  //Get absolute position
  MvRq->MoveRequestParam.AbsPos = Position_ConvertPercentageCloseToAbsolutePosition(MvRq->MoveRequestParam.GOTO);
  return  BuildMoveList_GoToAbsPos(MvRq,TmpMvStack);  
}

static TBool BuildMoveList_GoToAbsPos(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = FALSE;
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  PositionCompare_e PosCmp =  Position_CompareTwoAbsolutePosition(&MvRq->MoveRequestParam.AbsPos ,&CurrentPosition,POSITION_TOLERANCE);
  
  if(PosCmp == CurrentPositionIsAboveRef)
  {
    Rtn = BuildMoveList_UnRollToAbsPos(MvRq,TmpMvStack);
  }
  else if(PosCmp == CurrentPositionIsBelowRef)
  {
    Rtn = BuildMoveList_RollToAbsPos(MvRq,TmpMvStack);
  }
  return  Rtn;
}

//չ������λ��
static TBool BuildMoveList_RollToLimits(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  MvRq->MoveRequestParam.AbsPos = Limits_GetLimitValue(MvRq->MoveRequestParam.Limit);
  return  BuildMoveList_RollToAbsPos(MvRq,TmpMvStack);  
}

//������λ��
static TBool BuildMoveList_UnRollToLimits(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  MvRq->MoveRequestParam.AbsPos = Limits_GetLimitValue(MvRq->MoveRequestParam.Limit);
  return  BuildMoveList_UnRollToAbsPos(MvRq,TmpMvStack);  
}

static TBool BuildMoveList_GoToLimits(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  MvRq->MoveRequestParam.AbsPos = Limits_GetLimitValue(MvRq->MoveRequestParam.Limit);
  return  BuildMoveList_GoToAbsPos(MvRq,TmpMvStack);  
}

static TBool BuildMoveList_RollToPosition(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  //Get position value ,if set
  const TU32 *Postarget = Position_GetValue(MvRq->MoveRequestParam.Position);
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  TBool Rtn = FALSE;
  if(Postarget != NULL )
  {
    MvRq->MoveRequestParam.AbsPos = *Postarget;
    Rtn = BuildMoveList_RollToAbsPos(MvRq,TmpMvStack);
  }
  return Rtn;
}


static TBool BuildMoveList_UnRollToPosition(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  //Get position value ,if set
  const TU32 *Postarget = Position_GetValue(MvRq->MoveRequestParam.Position);
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  TBool Rtn = FALSE;
  
  if(Postarget != NULL )
  {
    MvRq->MoveRequestParam.AbsPos = *Postarget;
    Rtn =  BuildMoveList_UnRollToAbsPos(MvRq,TmpMvStack);
  }
  return Rtn;
}


static TBool BuildMoveList_TiltMyPosition(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  //Get position value ,if set

    MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
    MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
    TBool Rtn = FALSE;  
    TU32 CurrentPosition = EncoderDriver_GetPosition();
    TU32 DEL=Position_DEL();         //��ȡ����λ����
    TU32 UEL=Position_UEL();         //��ȡ����λ����
    TU32 MyPlace=Position_MyPlace(); //��ȡMYλ������
    
    EncoderDriverMovingDirection_e MoveDir = EncoderDriver_GetMoveDirection();
  
    if(MoveDir != EncoderDriver_Stop)
    {
      Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);
    }
         
    if(CurrentPosition <(DEL-3))
    {
      Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MoveOption_NoOption,0u,0u,Current_Speed,DEL);
      Stack_StopForTime(TmpMvStack,MoveOption_HardStop,100);
      Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MoveOption_NoOption,0u,0u,Current_Speed,MyPlace);
    }else if(CurrentPosition >= (DEL-3) && CurrentPosition <= (DEL+3))
      Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MoveOption_NoOption,0u,0u,Current_Speed,MyPlace);
      
    Rtn =true;
   
  return Rtn;
}






static TBool BuildMoveList_GoToPosition(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = FALSE;
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  const TU32 *Postarget = Position_GetValue(MvRq->MoveRequestParam.Position);
  
  if(Postarget != NULL )
  {
    PositionCompare_e PosCmp =  Position_CompareTwoAbsolutePosition(Postarget,&CurrentPosition,POSITION_TOLERANCE);
    if(PosCmp == CurrentPositionIsAboveRef)
    {
      Rtn = BuildMoveList_UnRollToPosition(MvRq,TmpMvStack);
    }
    else if(PosCmp == CurrentPositionIsBelowRef)
    {
      Rtn = BuildMoveList_RollToPosition(MvRq,TmpMvStack);
    }
  }
  
  return Rtn;
  
}

static TBool BuildMoveList_Nolimit_Stop(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);
  return  TRUE;
}

//��ջ�̷��� ����
static void Stack_ShortFeedBack_UpFirst(StackObjRef TmpMvStack,TBool CompensateFirstMove)
{
    MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
    TU8 DeadZoneComp = (LastDir == MotorDirection_Roll) ? (0u):(GEARBOX_DEADZONE_COMPENSATION);
    TU32 EstimatedMoveDistance = FEEDBACK_LENGHT_MOTORTURN + DeadZoneComp;
    MoveOption_e MvOpt =  MoveOption_ShortMove ;
    TU32 CurrentPosition = EncoderDriver_GetPosition();
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition - EstimatedMoveDistance));
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition)); 
}

//��ջ�̷��� ����
static void Stack_ShortFeedBack_DownFirst(StackObjRef TmpMvStack,TBool CompensateFirstMove)
{ 
    MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
    TU8 DeadZoneComp = (LastDir == MotorDirection_UnRoll) ? (0u):(GEARBOX_DEADZONE_COMPENSATION);
    TU32 EstimatedMoveDistance = FEEDBACK_LENGHT_MOTORTURN + DeadZoneComp;
    MoveOption_e MvOpt   =  MoveOption_ShortMove ;
    TU32 CurrentPosition = EncoderDriver_GetPosition();
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition + EstimatedMoveDistance));
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition));   
}



//��ջ�̷��� ����
static void Stack_ShortMove_UpFirst(StackObjRef TmpMvStack,TBool CompensateFirstMove,TU16 time)
{
  TU32 MoveTimeReturn =  (MovementPlanner_EstimateMoveTime(Current_Speed,(TU32) (GEARBOX_DEADZONE_COMPENSATION))+time);
  
  if(CompensateFirstMove == TRUE)
  {
    MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
    TU32 MoveTimeGo = (LastDir == MotorDirection_Roll) ? (time):(MovementPlanner_EstimateMoveTime(Current_Speed,((TU32) GEARBOX_DEADZONE_COMPENSATION))+10000);
    Stack_MoveForTime(TmpMvStack,MotorDirection_Roll,MoveOption_NoOption,MoveTimeGo);  
  }
  else
  {
    Stack_MoveForTime(TmpMvStack,MotorDirection_Roll,MoveOption_NoOption,MoveTimeReturn);
  } 
    Stack_StopForTime(TmpMvStack,MoveOption_NoOption,50);
    Stack_MoveForTime(TmpMvStack,MotorDirection_UnRoll,MoveOption_NoOption,MoveTimeReturn);
    Stack_StopForTime(TmpMvStack,MoveOption_NoOption,50);
}

//��ջ�̷��� ����
static void Stack_ShortMove_DownFirst(StackObjRef TmpMvStack,TBool CompensateFirstMove,TU16 time)
{
  TU32 MoveTimeReturn =  (MovementPlanner_EstimateMoveTime(Current_Speed,(TU32) (GEARBOX_DEADZONE_COMPENSATION))+time);
  
  if(CompensateFirstMove == TRUE)
  {
    MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
    TU32 MoveTimeGo = (LastDir == MotorDirection_UnRoll) ? (time):(MovementPlanner_EstimateMoveTime(Current_Speed,((TU32) GEARBOX_DEADZONE_COMPENSATION))+10000);
    Stack_MoveForTime(TmpMvStack,MotorDirection_UnRoll,MoveOption_NoOption,MoveTimeGo);  
  }
  else
  {
    Stack_MoveForTime(TmpMvStack,MotorDirection_UnRoll,MoveOption_NoOption,MoveTimeReturn);
  } 
    Stack_StopForTime(TmpMvStack,MoveOption_NoOption,50);
    Stack_MoveForTime(TmpMvStack,MotorDirection_Roll,MoveOption_NoOption,MoveTimeReturn);
    Stack_StopForTime(TmpMvStack,MoveOption_NoOption,50); 
}

//��ջ�̷��� ����kendy
static void Stack_TiltingShortMove_UpFirst(StackObjRef TmpMvStack,TBool CompensateFirstMove,TU16 time)
{
  
  MoveOption_e MvOpt =  MoveOption_ShortMove ;
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,MIDDLE_26_SPEED,(CurrentPosition-80));
  Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
  Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,MIDDLE_26_SPEED,CurrentPosition);
  Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);  
  
}

//��ջ�̷��� ����kendy
static void Stack_TiltingShortMove_DownFirst(StackObjRef TmpMvStack,TBool CompensateFirstMove,TU16 time)
{
  MoveOption_e MvOpt =  MoveOption_ShortMove ;
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,MIDDLE_26_SPEED,(CurrentPosition-80));
  Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
  Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,MIDDLE_26_SPEED,CurrentPosition);
  Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);  
}


//��ջ������ ����
static void Stack_ErrFeedBack_UpFirst(StackObjRef TmpMvStack)
{
    MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
    TU8 DeadZoneComp = (LastDir == MotorDirection_Roll) ? (0u):(GEARBOX_DEADZONE_COMPENSATION);
    TU32 EstimatedMoveDistance = FEEDBACK_LENGHT_MOTORTURN + DeadZoneComp;
    MoveOption_e MvOpt =  MoveOption_ShortMove ;
    TU32 CurrentPosition = EncoderDriver_GetPosition();
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition - EstimatedMoveDistance));
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition - EstimatedMoveDistance- EstimatedMoveDistance));
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition - EstimatedMoveDistance- EstimatedMoveDistance- EstimatedMoveDistance));   
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,600); 
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition- EstimatedMoveDistance- EstimatedMoveDistance)); 
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition - EstimatedMoveDistance)); 
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition));  
}



//��ջ������ ����
static void Stack_ErrFeedBack_DownFirst(StackObjRef TmpMvStack)
{
    MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
    TU8 DeadZoneComp = (LastDir == MotorDirection_Roll) ? (0u):(GEARBOX_DEADZONE_COMPENSATION);
    TU32 EstimatedMoveDistance = FEEDBACK_LENGHT_MOTORTURN + DeadZoneComp;
    MoveOption_e MvOpt =  MoveOption_ShortMove ;
    TU32 CurrentPosition = EncoderDriver_GetPosition();
    
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition + EstimatedMoveDistance)); 
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition + EstimatedMoveDistance + EstimatedMoveDistance)); 
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition + EstimatedMoveDistance + EstimatedMoveDistance + EstimatedMoveDistance));    
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,600);   
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition + EstimatedMoveDistance + EstimatedMoveDistance));
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition + EstimatedMoveDistance));
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition)); 
}




//��ջ������ ����
static void Stack_ErrFeedBack_UpFirstDEL(StackObjRef TmpMvStack)
{
    TU32 DEL=Position_DEL();         //��ȡ����λ����
    MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
    TU8 DeadZoneComp = (LastDir == MotorDirection_Roll) ? (0u):(GEARBOX_DEADZONE_COMPENSATION);
    TU32 EstimatedMoveDistance = FEEDBACK_LENGHT_MOTORTURN + DeadZoneComp;
    MoveOption_e MvOpt =  MoveOption_ShortMove ;
    TU32 CurrentPosition = EncoderDriver_GetPosition();
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition - EstimatedMoveDistance));
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition - EstimatedMoveDistance- EstimatedMoveDistance));
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition - EstimatedMoveDistance- EstimatedMoveDistance- EstimatedMoveDistance));
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,600);
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition- EstimatedMoveDistance- EstimatedMoveDistance)); 
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition - EstimatedMoveDistance)); 
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition)); 
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MoveOption_NoOption,0u,0u,Current_Speed,DEL);
}

//��ջ������ ����
static void Stack_ErrFeedBack_DownFirstUEL(StackObjRef TmpMvStack)
{
    TU32 UEL=Position_UEL();         //��ȡ����λ����
    MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
    TU8 DeadZoneComp = (LastDir == MotorDirection_Roll) ? (0u):(GEARBOX_DEADZONE_COMPENSATION);
    TU32 EstimatedMoveDistance = FEEDBACK_LENGHT_MOTORTURN + DeadZoneComp;
    MoveOption_e MvOpt =  MoveOption_ShortMove ;
    TU32 CurrentPosition = EncoderDriver_GetPosition();
    
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition + EstimatedMoveDistance)); 
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition + EstimatedMoveDistance + EstimatedMoveDistance)); 
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition + EstimatedMoveDistance + EstimatedMoveDistance + EstimatedMoveDistance)); 
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,600);   
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition + EstimatedMoveDistance + EstimatedMoveDistance));
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition + EstimatedMoveDistance));
    Stack_StopForTime(TmpMvStack,MoveOption_HardStop,400);
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,STANDARD_28_SPEED,(CurrentPosition)); 
    Stack_StopForTime(TmpMvStack,MoveOption_NoOption,400);
  
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MoveOption_NoOption,0u,0u,Current_Speed,UEL); 
}




//The first movement of the feedback should be done opposite to the last motor move. Otherwise, 
//the first movement of the feedback should be done to the middle of both end limits.
static TBool BuildMoveList_Nolimit_ShortFeedBack(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = TRUE;
  TU16 CloseToLimit    = FEEDBACK_LENGHT_MOTORTURN + GEARBOX_DEADZONE_COMPENSATION ;
  Limits_State_e Ls    = Limits_GetLimitsState(); //��ȡ��λλ������״̬
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  
  EncoderDriverMovingDirection_e MoveDir = EncoderDriver_GetMoveDirection();
  
  
  if(MoveDir != EncoderDriver_Stop)
  {
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);
  }
  
    //Depending on last direction and position, chose first move direction
  if(Ls == Limits_State_Above || Ls == Limits_State_OnUp ||
    (Ls == Limits_State_Between && Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_UpLimit) < CloseToLimit) )
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary
    //��������λ ������λ ���� ����λ֮�俿������λ
    Stack_ShortFeedBack_DownFirst(TmpMvStack,TRUE);
  }
  else if(Ls == Limits_State_Below || Ls == Limits_State_OnDown ||
         (Ls == Limits_State_Between && Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_DownLimit) < CloseToLimit) )
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary
    //��������λ ������λ ���� ����λ֮�俿������λ
    Stack_ShortFeedBack_UpFirst(TmpMvStack,TRUE);
  }
  else //Limits_State_Between && far from limit
  {
    if(MotorControl_GetLastMoveDirection() == MotorDirection_Roll)
    {
      //Estimate First move distance, compensate gearbox deadzone if necessary
      Stack_ShortFeedBack_DownFirst(TmpMvStack,TRUE);
    }
    else
    {
      //Estimate First move distance, compensate gearbox deadzone if necessary
      Stack_ShortFeedBack_UpFirst(TmpMvStack,TRUE);
    }
  }
  return  Rtn;
}

//������
static TBool BuildMoveList_Nolimit_ErrFeedBack(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = TRUE;
  Limits_State_e Ls = Limits_GetLimitsState();//��ȡ��λλ������״̬
  TU16 CloseToLimit = (FEEDBACK_LENGHT_MOTORTURN * 3) + GEARBOX_DEADZONE_COMPENSATION;
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
  
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  
  EncoderDriverMovingDirection_e MoveDir = EncoderDriver_GetMoveDirection();
  
  if(MoveDir != EncoderDriver_Stop)
  {
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);
  }
  
  //Depending on last direction and position, chose first move direction
  if(Ls == Limits_State_Above || Ls == Limits_State_OnUp ||
    (Ls == Limits_State_Between && Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_UpLimit) < CloseToLimit) )
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary
    Stack_ErrFeedBack_DownFirst(TmpMvStack);
  }else if(Ls == Limits_State_Below || Ls == Limits_State_OnDown ||
         (Ls == Limits_State_Between && Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_DownLimit) < CloseToLimit) )
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary
    Stack_ErrFeedBack_UpFirst(TmpMvStack);
  }
  else //Limits_State_Between && far from limit
  {
      if(MotorControl_GetLastMoveDirection() == MotorDirection_Roll)
      {
        //Estimate First move distance, compensate gearbox deadzone if necessary
        Stack_ErrFeedBack_DownFirst(TmpMvStack);
      }
      else
      {
        //Estimate First move distance, compensate gearbox deadzone if necessary
        Stack_ErrFeedBack_UpFirst(TmpMvStack);
      }
  }
  return  Rtn;
}

//������ + ���е��ͽ���λ
static TBool BuildMoveList_Nolimit_ErrFdBack_MoveTo(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = TRUE;
  Limits_State_e Ls = Limits_GetLimitsState();//��ȡ��λλ������״̬
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
  
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  
  EncoderDriverMovingDirection_e MoveDir = EncoderDriver_GetMoveDirection();
  
  if(MoveDir != EncoderDriver_Stop)
  {
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);
  }
  
  //Depending on last direction and position, chose first move direction
  if(Ls == Limits_State_Above)
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary
    Stack_ErrFeedBack_DownFirstUEL(TmpMvStack);
  }
  else if(Ls == Limits_State_Below)
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary
    Stack_ErrFeedBack_UpFirstDEL(TmpMvStack);
  }
  else //Limits_State_Between && far from limit
  {
    if(MotorControl_GetLastMoveDirection() == MotorDirection_Roll)
    {
      //Estimate First move distance, compensate gearbox deadzone if necessary
      Stack_ErrFeedBack_DownFirst(TmpMvStack);
    }
    else
    {
      //Estimate First move distance, compensate gearbox deadzone if necessary
      Stack_ErrFeedBack_UpFirst(TmpMvStack);
    }
  }
  return  Rtn;
}

//�б��� ���ζ���
static TBool BuildMoveList_Nolimit_DoubleShortFeedBack(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = TRUE;
  Limits_State_e Ls = Limits_GetLimitsState();//��ȡ��λλ������״̬
  TU16 CloseToLimit = FEEDBACK_LENGHT_MOTORTURN + GEARBOX_DEADZONE_COMPENSATION ;
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
  
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  
  EncoderDriverMovingDirection_e MoveDir = EncoderDriver_GetMoveDirection();
  
  if(MoveDir != EncoderDriver_Stop)
  {
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);
  }

  //Depending on last direction and position, chose first move direction
  if(Ls == Limits_State_Above || Ls == Limits_State_OnUp ||
     (Ls == Limits_State_Between && Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_UpLimit) < CloseToLimit) )
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary  
    Stack_ShortFeedBack_DownFirst(TmpMvStack,TRUE);
    Stack_ShortFeedBack_DownFirst(TmpMvStack,FALSE);
  }
  else if(Ls == Limits_State_Below || Ls == Limits_State_OnDown ||
          (Ls == Limits_State_Between && Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_DownLimit) < CloseToLimit) )
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary
    Stack_ShortFeedBack_UpFirst(TmpMvStack,TRUE);
    Stack_ShortFeedBack_UpFirst(TmpMvStack,FALSE);
  }
  else //Limits_State_Between && far from limit
  {
    if(MotorControl_GetLastMoveDirection() == MotorDirection_Roll)
    {
      //Estimate First move distance, compensate gearbox deadzone if necessary
      Stack_ShortFeedBack_DownFirst(TmpMvStack,TRUE);    
      Stack_ShortFeedBack_DownFirst(TmpMvStack,FALSE);
    }
    else
    {
      //Estimate First move distance, compensate gearbox deadzone if necessary
      Stack_ShortFeedBack_UpFirst(TmpMvStack,TRUE);
      Stack_ShortFeedBack_UpFirst(TmpMvStack,FALSE);
    }
  }
  return  Rtn;
}

//�б��� ���ζ���
static TBool BuildMoveList_Nolimit_ThreeShortFeedBack(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = TRUE;
  Limits_State_e Ls = Limits_GetLimitsState();//��ȡ��λλ������״̬
  TU16 CloseToLimit = FEEDBACK_LENGHT_MOTORTURN + GEARBOX_DEADZONE_COMPENSATION ;
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
  
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  
  EncoderDriverMovingDirection_e MoveDir = EncoderDriver_GetMoveDirection();
  
  if(MoveDir != EncoderDriver_Stop)
  {
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);
  }

  //Depending on last direction and position, chose first move direction
  if(Ls == Limits_State_Above || Ls == Limits_State_OnUp ||
     (Ls == Limits_State_Between && Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_UpLimit) < CloseToLimit) )
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary  
    Stack_ShortFeedBack_DownFirst(TmpMvStack,TRUE);
    Stack_ShortFeedBack_DownFirst(TmpMvStack,FALSE);
    Stack_ShortFeedBack_DownFirst(TmpMvStack,FALSE);
  }
  else if(Ls == Limits_State_Below || Ls == Limits_State_OnDown ||
          (Ls == Limits_State_Between && Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_DownLimit) < CloseToLimit) )
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary
    Stack_ShortFeedBack_UpFirst(TmpMvStack,TRUE);
    Stack_ShortFeedBack_UpFirst(TmpMvStack,FALSE);
    Stack_ShortFeedBack_UpFirst(TmpMvStack,FALSE);
  }
  else //Limits_State_Between && far from limit
  {
    if(MotorControl_GetLastMoveDirection() == MotorDirection_Roll)
    {
      //Estimate First move distance, compensate gearbox deadzone if necessary
      Stack_ShortFeedBack_DownFirst(TmpMvStack,TRUE);    
      Stack_ShortFeedBack_DownFirst(TmpMvStack,FALSE);
      Stack_ShortFeedBack_DownFirst(TmpMvStack,FALSE);
    }
    else
    {
      //Estimate First move distance, compensate gearbox deadzone if necessary
      Stack_ShortFeedBack_UpFirst(TmpMvStack,TRUE);
      Stack_ShortFeedBack_UpFirst(TmpMvStack,FALSE);
      Stack_ShortFeedBack_UpFirst(TmpMvStack,FALSE);
    }
  }
  return  Rtn;
}



//�б��� �Ĵζ���
static TBool BuildMoveList_Nolimit_FourShortFeedBack(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = TRUE;

  MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
  
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  
  EncoderDriverMovingDirection_e MoveDir = EncoderDriver_GetMoveDirection();
  
  if(MoveDir != EncoderDriver_Stop)
  {
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);
  }


  Stack_ShortFeedBack_UpFirst(TmpMvStack,TRUE);
  Stack_ShortFeedBack_UpFirst(TmpMvStack,FALSE);
  Stack_ShortFeedBack_UpFirst(TmpMvStack,FALSE);
  Stack_ShortFeedBack_UpFirst(TmpMvStack,FALSE);

  return  Rtn;
}




//The first movement of the feedback should be done opposite to the last motor move. Otherwise, 
//the first movement of the feedback should be done to the middle of both end limits.
static TBool BuildMoveList_10sCycle_MoveMent(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{  
  
  TBool Rtn = TRUE;
 // TU32 EstimatedMoveDistance = MovementPlanner_EstimateMoveDistance(Current_Speed,10000);
  TU16 CloseToLimit = 500;
  TU16 Move_time = 0;
  Limits_State_e Ls = Limits_GetLimitsState(); //��ȡ��λλ������״̬
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  
  EncoderDriverMovingDirection_e MoveDir = EncoderDriver_GetMoveDirection();
  if(MoveDir != EncoderDriver_Stop)
  {
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);
  }
    //Depending on last direction and position, chose first move direction
 if( Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_DownLimit) >= CloseToLimit )
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary
    //��������λ ������λ ���� ����λ֮�俿������λ
      Stack_ShortMove_DownFirst(TmpMvStack,TRUE,10000);
      SpeedADJCycle_timems=20200;
  }
 else if( Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_UpLimit) >= CloseToLimit )
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary
    //��������λ ������λ ���� ����λ֮�俿������λ
      Stack_ShortMove_UpFirst(TmpMvStack,TRUE,10000);
      SpeedADJCycle_timems=20200;
  }
  else //Limits_State_Between && far from limit
  {
    //if(MotorControl_GetLastMoveDirection() == MotorDirection_Roll)
      if(Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_DownLimit) >= Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_UpLimit))
    {
      //Estimate First move distance, compensate gearbox deadzone if necessary
      Move_time= (Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_DownLimit)*60/Current_Speed)*1000;
      if(Move_time>10000)
        Move_time=10000;
      Stack_ShortMove_DownFirst(TmpMvStack,TRUE,Move_time);
      SpeedADJCycle_timems=Move_time*2+200;
    }else
    {
      //Estimate First move distance, compensate gearbox deadzone if necessary
      Move_time= (Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_UpLimit)*60/Current_Speed)*1000;
      if(Move_time>10000)
        Move_time=10000;
      Stack_ShortMove_UpFirst(TmpMvStack,TRUE,Move_time);
      SpeedADJCycle_timems=Move_time*2+200;
    }
  }
  return  Rtn;
  
}
//The first movement of the feedback should be done opposite to the last motor move. Otherwise, 
//the first movement of the feedback should be done to the middle of both end limits.
static TBool BuildMoveList_TiltingCycle_MoveMent(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{  

  
  TBool Rtn = TRUE;
 // TU32 EstimatedMoveDistance = MovementPlanner_EstimateMoveDistance(Current_Speed,10000);
  TU16 CloseToLimit = ONE_PRODUCT_TURN/2+3;
 // TU16 Move_time = 0;
  Limits_State_e Ls = Limits_GetLimitsState(); //��ȡ��λλ������״̬
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  
  EncoderDriverMovingDirection_e MoveDir = EncoderDriver_GetMoveDirection();
 // MoveOption_e MvOpt =  MoveOption_ShortMove ;
  if(MoveDir != EncoderDriver_Stop)
  {
    Stack_Stop(TmpMvStack,MoveOption_NormalUserStop);
  }

  
    //Depending on last direction and position, chose first move direction
 if( Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_DownLimit) >= CloseToLimit )
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary
    //��������λ ������λ ���� ����λ֮�俿������λ
   

     // Stack_TiltingShortMove_DpFirst(TmpMvStack,TRUE,10000);
      MoveOption_e MvOpt =  MoveOption_UseCustom ;
  TU32 CurrentPosition = EncoderDriver_GetPosition();
  //Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0,0,Current_Speed,(CurrentPosition+CloseToLimit));
   Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,6000u,ACC_FAST,Current_Speed,(CurrentPosition+CloseToLimit)); 
  Stack_StopForTime(TmpMvStack,MoveOption_HardStop,100);
 // Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0,0,Current_Speed,CurrentPosition);
   Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,6000u,ACC_FAST,Current_Speed,CurrentPosition); 
  Stack_StopForTime(TmpMvStack,MoveOption_HardStop,100); 
      SpeedADJCycle_timems=5200;
  }
 else if( Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_UpLimit) >= CloseToLimit )
  {
    //Estimate First move distance, compensate gearbox deadzone if necessary
    //��������λ ������λ ���� ����λ֮�俿������λ
  //Stack_TiltingShortMove_UpFirst(TmpMvStack,TRUE,10000);
  
    MoveOption_e MvOpt =  MoveOption_UseCustom ;
  TU32 CurrentPosition = EncoderDriver_GetPosition();
 // Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0,0,Current_Speed,(CurrentPosition-CloseToLimit));
 Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,6000u,ACC_FAST,Current_Speed,(CurrentPosition-CloseToLimit)); 
  Stack_StopForTime(TmpMvStack,MoveOption_HardStop,100);
 // Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0,0,Current_Speed,CurrentPosition);
   Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,6000u,ACC_FAST,Current_Speed,CurrentPosition); 
  Stack_StopForTime(TmpMvStack,MoveOption_HardStop,100); 
  
      SpeedADJCycle_timems=5200;
  }
 
  return  Rtn;
  
}

//�б��� ��չ��
static TBool BuildMoveList_ShortUnRoll(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  MvRq->MoveRequestParam.MoveOff = 1;
  return  BuildMoveList_UnRolloff(MvRq,TmpMvStack);
}

//�б��� �̾���
static TBool BuildMoveList_ShortRoll(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  MvRq->MoveRequestParam.MoveOff = 1;
  return  BuildMoveList_Rolloff(MvRq,TmpMvStack);
}

//�б��� ���� ����
static TBool BuildMoveList_Rolloff(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = FALSE;
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  Limits_State_e CurrentLimitsState = Limits_GetLimitsState();//��ȡ��λλ������״̬
  if(CurrentLimitsState > Limits_State_OnUp)
  {
    MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
    TU8 DeadZoneComp = (LastDir == MotorDirection_Roll) ? (0):(GEARBOX_DEADZONE_COMPENSATION);
    TU32 EstimatedMoveDistance = (SHORTMOVE_LENGHT_MOTORTURN * MvRq->MoveRequestParam.MoveOff) + DeadZoneComp;
    TU32 CurrentPosition = EncoderDriver_GetPosition();
    TU32 LimitDistance = Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_UpLimit);
    if(LimitDistance > EstimatedMoveDistance)
    {
      Rtn = BuildMoveList_Nolimit_Rolloff(MvRq,TmpMvStack);
    }
    else
    {
      MvRq->MoveRequestParam.Limit = ModeLimitName_UpLimit;
      Rtn = BuildMoveList_RollToLimits(MvRq,TmpMvStack);  //չ������λ��
    }
  }
  return Rtn;
}


//�б��� չ�� ����
static TBool BuildMoveList_UnRolloff(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  TBool Rtn = FALSE;
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  Limits_State_e CurrentLimitsState = Limits_GetLimitsState();//��ȡ��λλ������״̬
  if(CurrentLimitsState < Limits_State_OnDown)
  {
    MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
    TU8 DeadZoneComp = (LastDir == MotorDirection_UnRoll) ? (0):(GEARBOX_DEADZONE_COMPENSATION);
    TU32 EstimatedMoveDistance = (SHORTMOVE_LENGHT_MOTORTURN * MvRq->MoveRequestParam.MoveOff) + DeadZoneComp;
    TU32 CurrentPosition = EncoderDriver_GetPosition();
    TU32 LimitDistance = Limits_GetLimitDistance(&CurrentPosition,ModeLimitName_DownLimit);
    if(LimitDistance > EstimatedMoveDistance)//���ڵ���λ�ľ������Ŀ���ƶ����룬���������ܣ�
    {
      Rtn =  BuildMoveList_Nolimit_UnRolloff(MvRq,TmpMvStack);
    }
    else//���ڵ���λ�ľ���С��Ŀ���ƶ����룬���������ܣ�λ��������λ�����Ե�����λ����
    {
      MvRq->MoveRequestParam.Limit = ModeLimitName_DownLimit;
      Rtn = BuildMoveList_UnRollToLimits(MvRq,TmpMvStack);
    }
  }
  return Rtn;
}

//����λ ��ʱ�����
static TBool BuildMoveList_Nolimit_RollForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  if(MvRq->MoveRequestParam.Time_mS < 1000u)
  {//1S����
    Stack_MoveForTime(TmpMvStack,MotorDirection_Roll,MoveOption_ShortMove,MvRq->MoveRequestParam.Time_mS);
  }
  else
  {//1S����
    Stack_MoveForTime(TmpMvStack,MotorDirection_Roll,MoveOption_NoOption,MvRq->MoveRequestParam.Time_mS);
  }
  return TRUE;
}

static TBool BuildMoveList_Nolimit_UnRollForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  if(MvRq->MoveRequestParam.Time_mS < 1000u)
  {
    Stack_MoveForTime(TmpMvStack,MotorDirection_UnRoll,MoveOption_ShortMove,MvRq->MoveRequestParam.Time_mS);
  }
  else
  {
    Stack_MoveForTime(TmpMvStack,MotorDirection_UnRoll,MoveOption_NoOption,MvRq->MoveRequestParam.Time_mS);
  }
  return TRUE;
}

static TBool BuildMoveList_Nolimit_ShortUnRoll(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  MvRq->MoveRequestParam.MoveOff = 1u;
  return  BuildMoveList_Nolimit_UnRolloff(MvRq,TmpMvStack);
}

static TBool BuildMoveList_Nolimit_ShortRoll(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  MvRq->MoveRequestParam.MoveOff = 1u;
  return  BuildMoveList_Nolimit_Rolloff(MvRq,TmpMvStack);
}

//����λ����  չ����   Rolloff
static TBool BuildMoveList_Nolimit_Rolloff(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  MovementManager_UnLockOppositeDirection(MotorDirection_Roll);
  TBool DirIsLocked = MovementManager_IsDirectionLock(MotorDirection_Roll); //�жϷ����Ƿ�����
  TBool Rtn = FALSE;
  if(DirIsLocked == FALSE)
  {
    MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
    TU8 DeadZoneComp = (LastDir == MotorDirection_Roll) ? (0u):(GEARBOX_DEADZONE_COMPENSATION);
    TU32 EstimatedMoveDistance = (SHORTMOVE_LENGHT_MOTORTURN * MvRq->MoveRequestParam.MoveOff) + DeadZoneComp;
    MoveOption_e MvOpt =  (MvRq->MoveRequestParam.MoveOff >= 3u) ? (MoveOption_ShortMove) : (MoveOption_ShortMove) ;//MoveOption_e MvOpt
    TU32 CurrentPosition = EncoderDriver_GetPosition();
    MvRq->MoveRequestParam.AbsPos = CurrentPosition - EstimatedMoveDistance;
    Stack_MoveTo(TmpMvStack,MotorDirection_Roll,MvOpt,0u,0u,(Current_Speed/2),MvRq->MoveRequestParam.AbsPos);
    Rtn = TRUE;
  }
  return Rtn;
}

//����λ����  ����  UnRolloff
static TBool BuildMoveList_Nolimit_UnRolloff(MoveRequest_s *MvRq,StackObjRef TmpMvStack)
{
  MovementManager_UnLockOppositeDirection(MotorDirection_UnRoll);
  TBool DirIsLocked = MovementManager_IsDirectionLock(MotorDirection_UnRoll); //�жϷ����Ƿ�����
  TBool Rtn = FALSE;
  if(DirIsLocked == FALSE)
  {
    MotorDirection_e LastDir = MotorControl_GetLastMoveDirection();
    TU8 DeadZoneComp = (LastDir == MotorDirection_UnRoll) ? (0u):(GEARBOX_DEADZONE_COMPENSATION);
    TU32 EstimatedMoveDistance = (SHORTMOVE_LENGHT_MOTORTURN * MvRq->MoveRequestParam.MoveOff) + DeadZoneComp;
    MoveOption_e MvOpt =  (MvRq->MoveRequestParam.MoveOff >= 3u) ? (MoveOption_ShortMove) : (MoveOption_ShortMove) ;
    TU32 CurrentPosition = EncoderDriver_GetPosition();
    MvRq->MoveRequestParam.AbsPos = CurrentPosition + EstimatedMoveDistance;
    Stack_MoveTo(TmpMvStack,MotorDirection_UnRoll,MvOpt,0u,0u,(Current_Speed/2),MvRq->MoveRequestParam.AbsPos);
    Rtn =  TRUE;
  } 
  return Rtn ; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  STACK MOVE /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void Stack_SecurityStop(StackObjRef TmpMvStack)
{
  MovementManager_Move_s SecurityStop;              //ֹͣ
  SecurityStop.MoveDirection = MotorDirection_Stop; //ֹͣ����
  SecurityStop.MoveType = MovementManager_MoveType_SecurityStop;//��ȫֹͣ
  Stack_PushTail(TmpMvStack,&SecurityStop);
}

static void Stack_None(StackObjRef TmpMvStack)
{
  MovementManager_Move_s None;                     //��
  None.MoveDirection = MotorDirection_Stop;        //ֹͣ����
  None.MoveType = MovementManager_MoveType_None;   //��
  Stack_PushTail(TmpMvStack,&None);
}

//��Ϊ�˶� PWM����
static void Stack_Manual_PWM(StackObjRef TmpMvStack,MotorDirection_e MoveDirection,TU16 PWM)
{
  MovementManager_Move_s ManualPWM;
  ManualPWM.MoveType = MovementManager_MoveType_Manual_PWM;
  ManualPWM.PWM = (TU16) PWM;
  ManualPWM.MoveDirection = MoveDirection;
  Stack_PushTail(TmpMvStack,&ManualPWM);
}

//�˶�����ѹջ
static void Stack_Move(StackObjRef TmpMvStack,MotorDirection_e MoveDirection,MoveOption_e Option,TU16 CustomAcc,TU16 CustomDcc,TU16 CustomSpeed)
{
  MovementManager_Move_s Move;
  Move.MoveType = MovementManager_MoveType_Move;
  Move.MoveOption = (Option == MoveOption_UseCustom || Option == MoveOption_NoOption) ? (Option) : (MoveOption_NoOption);
  
  Move.MoveDirection = MoveDirection;
  
  Move.CustomACC   = CustomAcc;
  Move.CustomDCC   = CustomDcc;
  Move.CustomSpeed = CustomSpeed;
  
  Stack_PushTail(TmpMvStack,&Move);
}

//��ʱ�˶�����ѹջ
void Stack_MoveForTime(StackObjRef TmpMvStack,MotorDirection_e MoveDirection,MoveOption_e Option,TU32 Timems)
{
  MovementManager_Move_s Move;
  Move.MoveType   = MovementManager_MoveType_MoveForTime;
  Move.MoveOption = (Option == MoveOption_NoOption ||Option == MoveOption_ShortMove) ? (Option) : (MoveOption_NoOption);
  Move.MoveDirection = MoveDirection;
  Move.MoveTime_ms   = Timems;
  Stack_PushTail(TmpMvStack,&Move);
}

//ջ stop  
static void Stack_Stop(StackObjRef TmpMvStack,MoveOption_e Option)
{
  MovementManager_Move_s Move;
  Move.MoveType = MovementManager_MoveType_Stop;
  Move.MoveOption = (Option == MoveOption_HardStop) ? (MoveOption_HardStop) : (MoveOption_NormalUserStop);
  
  Stack_PushTail(TmpMvStack,&Move);
}


//ѹջ stop  ֹͣʱ��
 void Stack_StopForTime(StackObjRef TmpMvStack,MoveOption_e Option,TU32 TimesMs)
{
  MovementManager_Move_s Move;
  Move.MoveType    = MovementManager_MoveType_StopForTime;
  Move.MoveOption  = (Option == MoveOption_ForceStop) ? (MoveOption_ForceStop) : (MoveOption_NoOption);
  //�ж��Ƿ�ΪMoveOption_ForceStop  ����  ��
  Move.MoveTime_ms = TimesMs;
  Stack_PushTail(TmpMvStack,&Move);
}


//����ѹջ ���е� Ŀ�ĵ�ַ
 void Stack_MoveTo(StackObjRef TmpMvStack,MotorDirection_e MoveDirection,MoveOption_e Option,TU16 CustomAcc,TU16 CustomDcc,TU16 CustomSpeed,TU32 AbsPos)
{
  MovementManager_Move_s Move;
  Move.MoveType = MovementManager_MoveType_MoveTo; //�˶�ģʽ Ŀ���ƶ�
  Move.MoveOption    = Option;
  Move.MoveDirection = MoveDirection;
  
  Move.CustomACC   = CustomAcc;
  Move.CustomDCC   = CustomDcc;
  Move.CustomSpeed = CustomSpeed;
  
  Move.AbsFinalPlanedPosition = AbsPos;
  Stack_PushTail(TmpMvStack,&Move);  
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////   PLAN MOVE  ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static TBool PlanMove_SecurityStop(MovementManager_Move_s *Mv,StackObjRef Segment)  // ��ȫֹͣ ɲ��
{
  TBool Rtn = TRUE;
  Stack_Purge(Segment);
  MotorControl_Segment_s SecurityStopSegment;
  SecurityStopSegment.UnInteruptible = TRUE;  //�����Ա���� ��ֹ
  
  SecurityStopSegment.SpeedTrigger.SpeedTriggerValue = 0U;     //�����ٶ�ΪĿ��
  SecurityStopSegment.SpeedTrigger.TriggerEnable = TRUE;
  SecurityStopSegment.PositionTrigger.PosTriggerValue= 0U;
  SecurityStopSegment.PositionTrigger.TriggerEnable=FALSE;
  SecurityStopSegment.TimeTrigger.TimeTriggerValue= 0U;
  SecurityStopSegment.TimeTrigger.TriggerEnable=FALSE;
  
  SecurityStopSegment.SetPoint.AbsTarget  = ABS_MAX_PWM;       //ɲ��
  SecurityStopSegment.SetPoint.AbsStartUp = ABS_MAX_PWM;
  SecurityStopSegment.SetPoint.AccMax = 0U;
  SecurityStopSegment.SetPoint.DccMax = 0U;
  SecurityStopSegment.SetPoint.Direction = MotorDirection_Stop;//ֹͣ
  SecurityStopSegment.SetPoint.LoopMode  = LoopMode_Brake;     //ɲ��
  Stack_PushTail(Segment,&SecurityStopSegment);
  return  Rtn;
}

//���˶��ƻ�
static TBool PlanMove_None(MovementManager_Move_s *Mv,StackObjRef Segment)
{
  TBool Rtn = TRUE;
  Stack_Purge(Segment);
  MotorControl_Segment_s NoneSegment;
  NoneSegment.UnInteruptible = FALSE; //���Ա���� ��ֹ
  
  NoneSegment.SpeedTrigger.SpeedTriggerValue = 0U;
  NoneSegment.SpeedTrigger.TriggerEnable = FALSE;
  NoneSegment.PositionTrigger.PosTriggerValue= 0U;
  NoneSegment.PositionTrigger.TriggerEnable=FALSE;
  NoneSegment.TimeTrigger.TimeTriggerValue= 0U;
  NoneSegment.TimeTrigger.TriggerEnable=FALSE;
  
  NoneSegment.SetPoint.AbsTarget  = ABS_MAX_PWM;  //0U;
  NoneSegment.SetPoint.AbsStartUp = ABS_MAX_PWM;  //0U;
  NoneSegment.SetPoint.AccMax = 0U;
  NoneSegment.SetPoint.DccMax = 0U;
  NoneSegment.SetPoint.Direction = MotorDirection_Stop;//ֹͣ
  NoneSegment.SetPoint.LoopMode  = LoopMode_Brake;     //ɲ��
  Stack_PushTail(Segment,&NoneSegment);
  return  Rtn;
}

//�˹�ģʽ��PWM����   ���ù� sage
static TBool PlanMove_Manual_PWM(MovementManager_Move_s *Mv,StackObjRef Segment)
{
  TBool Rtn = TRUE;
  Stack_Purge(Segment);
  MotorControl_Segment_s PWMSegment;
  PWMSegment.UnInteruptible = FALSE;//���Ա���� ��ֹ
  
  PWMSegment.SpeedTrigger.SpeedTriggerValue = 0U;
  PWMSegment.SpeedTrigger.TriggerEnable = FALSE;
  PWMSegment.PositionTrigger.PosTriggerValue= 0U;
  PWMSegment.PositionTrigger.TriggerEnable=FALSE;
  PWMSegment.TimeTrigger.TimeTriggerValue= 0U;
  PWMSegment.TimeTrigger.TriggerEnable=FALSE;
  
  PWMSegment.SetPoint.AbsTarget  = Mv->PWM;
  PWMSegment.SetPoint.AbsStartUp = 0U;  //��0��ʼsage
  PWMSegment.SetPoint.AccMax = 0U;
  PWMSegment.SetPoint.DccMax = 0U;
  PWMSegment.SetPoint.Direction = MotorDirection_Stop; //ֹͣ
  PWMSegment.SetPoint.LoopMode  = LoopMode_PWM;        //ѭ����ʽ  LoopMode_PWM
  Stack_PushTail(Segment,&PWMSegment);
  return  Rtn;
}

//��ʱģʽ��PWM����  
static TBool PlanMove_MoveForTime(MovementManager_Move_s *Mv,StackObjRef Segment)
{
  TBool Rtn = TRUE;
  MotorControl_Segment_s MoveForTime;
  MoveForTime.UnInteruptible = FALSE;//���Ա���� ��ֹ
  MoveForTime.SpeedTrigger.SpeedTriggerValue = 0U;
  MoveForTime.SpeedTrigger.TriggerEnable = FALSE;
  MoveForTime.PositionTrigger.PosTriggerValue = 0U;
  MoveForTime.PositionTrigger.TriggerEnable=FALSE;
  MoveForTime.TimeTrigger.TimeTriggerValue=Mv->MoveTime_ms;
  MoveForTime.TimeTrigger.TriggerEnable=TRUE;
  //  
  MoveForTime.SetPoint.LoopMode  = LoopMode_Speed;   //ѭ����ʽ  LoopMode_Speed
  MoveForTime.SetPoint.Direction = Mv->MoveDirection;
  TU16 absCurrentSpeed = (TU16) abs((int)EncoderDriver_GetSpeed());
  
  if(Mv->MoveOption == MoveOption_ShortMove)         //MoveOption_ShortMove
  {
    MoveForTime.SetPoint.AbsTarget  = STANDARD_28_SPEED;//��׼�ٶ�
    MoveForTime.SetPoint.AbsStartUp = STARTUP_SPEED;  //��ʼ�ٶ�
    MoveForTime.SetPoint.AccMax     = ACC_FAST;       //(8000u)
    MoveForTime.SetPoint.DccMax     = ACC_FAST;
  }else                                               //�������˶�ģʽ
  {
    MoveForTime.SetPoint.AbsTarget  = Current_Speed; //��ǰĬ���ٶ�
    MoveForTime.SetPoint.AbsStartUp = STARTUP_SPEED; //��׼�ٶ�
    MoveForTime.SetPoint.AccMax     = ACC_FAST;
    MoveForTime.SetPoint.DccMax     = ACC_BRUTAL;//16000;
  }
  Stack_PushTail(Segment,&MoveForTime);
  
  MotorControl_Segment_s Stop;
  Stop.SpeedTrigger.SpeedTriggerValue = 0U;
  Stop.SpeedTrigger.TriggerEnable = TRUE;
  Stop.PositionTrigger.PosTriggerValue = 0U;
  Stop.PositionTrigger.TriggerEnable=FALSE;
  Stop.TimeTrigger.TimeTriggerValue= 0U;
  Stop.TimeTrigger.TriggerEnable=FALSE;
  Stop.UnInteruptible = TRUE;                    //�����Ա���� ��ֹ
  Stop.SetPoint.Direction  = MotorDirection_Stop;//ֹͣ
  Stop.SetPoint.AbsTarget  = ABS_MAX_PWM;
  Stop.SetPoint.AbsStartUp = ABS_MAX_PWM;
  Stop.SetPoint.AccMax = 0U;
  Stop.SetPoint.DccMax = 0U;
  Stop.SetPoint.LoopMode = LoopMode_Brake;       //ѭ����ʽ  LoopMode_Brake
  Stack_PushTail(Segment,&Stop);
  
  return  Rtn;
}

//PlanMove_StopForTime
static TBool PlanMove_StopForTime(MovementManager_Move_s *Mv,StackObjRef Segment)
{
  TBool Rtn = TRUE;
  MotorControl_Segment_s StopForTime;
  
  StopForTime.UnInteruptible = (Mv->MoveOption == MoveOption_ForceStop) ? (TRUE) : (FALSE);
  //ǿ��ֹͣ������ ֹͣ�ж�  ������Խ���ֹͣ
  StopForTime.SpeedTrigger.SpeedTriggerValue  = 0U;
  StopForTime.SpeedTrigger.TriggerEnable      = FALSE;
  StopForTime.PositionTrigger.PosTriggerValue = 0U;
  StopForTime.PositionTrigger.TriggerEnable   =FALSE;
  
  StopForTime.TimeTrigger.TimeTriggerValue=Mv->MoveTime_ms;
  StopForTime.TimeTrigger.TriggerEnable   =TRUE;
  
  StopForTime.SetPoint.AbsTarget  = ABS_MAX_PWM;//0U;
  StopForTime.SetPoint.AbsStartUp = ABS_MAX_PWM;//0U;
  StopForTime.SetPoint.AccMax = 0U;
  StopForTime.SetPoint.DccMax = 0U;
  StopForTime.SetPoint.Direction = MotorDirection_Stop;
  StopForTime.SetPoint.LoopMode  = LoopMode_Brake;//sss
  
  Stack_PushTail(Segment,&StopForTime);
  return  Rtn;
}


//��ʱģʽ��PWM����
static TBool PlanMove_MoveTo(MovementManager_Move_s *Mv,StackObjRef Segment)
{
  TBool Rtn = TRUE;
  MotorControl_Segment_s MoveToStop;
  MotorControl_Segment_s MoveToFar,MoveToClose;
  
  if(Mv->MoveOption == MoveOption_UseCustom)//�û�ģʽ �˶�����
  {
    MotorControl_Segment_s MoveToCustom;
    
    MoveToCustom.UnInteruptible = FALSE;//���Ա���� ��ֹ
    MoveToCustom.SpeedTrigger.SpeedTriggerValue = 0U;
    MoveToCustom.SpeedTrigger.TriggerEnable = FALSE;
    MoveToCustom.TimeTrigger.TimeTriggerValue= 0U;
    MoveToCustom.TimeTrigger.TriggerEnable=FALSE;
    
    MoveToCustom.PositionTrigger.PosTriggerValue = Mv->AbsFinalPlanedPosition;
    MoveToCustom.PositionTrigger.TriggerEnable=TRUE;
    
    MoveToCustom.SetPoint.AbsTarget  = Mv->CustomSpeed;
    MoveToCustom.SetPoint.AbsStartUp = STARTUP_SPEED;
    MoveToCustom.SetPoint.AccMax = Mv->CustomACC;
    MoveToCustom.SetPoint.DccMax = Mv->CustomDCC;
    MoveToCustom.SetPoint.Direction = Mv->MoveDirection;
    MoveToCustom.SetPoint.LoopMode = LoopMode_Speed;//ѭ����ʽ LoopMode_Speed
    Stack_PushTail(Segment,&MoveToCustom);
  }else if(Mv->MoveOption == MoveOption_ShortMove)
  {
    TS16 Speed = EncoderDriver_GetSpeed();
    MoveToClose.UnInteruptible = TRUE;    //�����Ա���� ��ֹ
    MoveToClose.SpeedTrigger.SpeedTriggerValue = 0U;
    MoveToClose.SpeedTrigger.TriggerEnable = FALSE;
    MoveToClose.TimeTrigger.TimeTriggerValue= 0U;
    MoveToClose.TimeTrigger.TriggerEnable=FALSE;
    
    MoveToClose.PositionTrigger.PosTriggerValue = Mv->AbsFinalPlanedPosition - Mv->MoveDirection;
    MoveToClose.PositionTrigger.TriggerEnable=TRUE;
    
    MoveToClose.SetPoint.AbsTarget  = MIDDLE_24_SPEED;
    MoveToClose.SetPoint.AbsStartUp = (Speed == 0) ? (MIDDLE_22_SPEED) : (abs(Speed));//(STARTUP_SPEED) : (abs(Speed));
    MoveToClose.SetPoint.AccMax = ACC_FAST;
    MoveToClose.SetPoint.DccMax = ACC_FAST;//ACC_FAST;
    MoveToClose.SetPoint.Direction = Mv->MoveDirection;
    MoveToClose.SetPoint.LoopMode  = LoopMode_Speed;//ѭ����ʽ LoopMode_Speed
    Stack_PushTail(Segment,&MoveToClose);
  }
  else
  {//MoveOption_NoOption*******************************************************
    
//    TS16 lsp = Mv->MoveDirection * (ONE_PRODUCT_TURN/2);
//    TS16 Speed = EncoderDriver_GetSpeed();
//    if( MoveDistance > (ONE_PRODUCT_TURN/2))
//    {
//      MoveToFar.UnInteruptible = FALSE;
//      MoveToFar.SpeedTrigger.SpeedTriggerValue = 0U;
//      MoveToFar.SpeedTrigger.TriggerEnable = FALSE;
//      MoveToFar.TimeTrigger.TimeTriggerValue= 0U;
//      MoveToFar.TimeTrigger.TriggerEnable=FALSE;
//      
//      MoveToFar.PositionTrigger.PosTriggerValue = Mv->AbsFinalPlanedPosition - lsp;
//      MoveToFar.PositionTrigger.TriggerEnable=TRUE;
//      
//      MoveToFar.SetPoint.AbsTarget = Current_Speed;
//      MoveToFar.SetPoint.AbsStartUp = (Speed == 0) ? (STARTUP_SPEED) : (abs(Speed));
//      MoveToFar.SetPoint.AccMax = ACC_SOFT;
//      MoveToFar.SetPoint.DccMax = ACC_FAST;
//      MoveToFar.SetPoint.Direction = Mv->MoveDirection;
//      MoveToFar.SetPoint.LoopMode = LoopMode_Speed;
//      Stack_PushTail(Segment,&MoveToFar);
//    }
//    
//    MoveToClose.UnInteruptible = FALSE;
//    MoveToClose.SpeedTrigger.SpeedTriggerValue = 0U;
//    MoveToClose.SpeedTrigger.TriggerEnable = FALSE;
//    MoveToClose.TimeTrigger.TimeTriggerValue= 0U;
//    MoveToClose.TimeTrigger.TriggerEnable=FALSE;
//    
//    MoveToClose.PositionTrigger.PosTriggerValue = Mv->AbsFinalPlanedPosition - Mv->MoveDirection;
//    MoveToClose.PositionTrigger.TriggerEnable=TRUE;
//    
//    MoveToClose.SetPoint.AbsTarget = STARTUP_SPEED;
//    MoveToClose.SetPoint.AbsStartUp = (Speed == 0) ? (STARTUP_SPEED) : (abs(Speed));
//    MoveToClose.SetPoint.AccMax = ACC_SOFT;
//    MoveToClose.SetPoint.DccMax = ACC_FAST;
//    MoveToClose.SetPoint.Direction = Mv->MoveDirection;
//    MoveToClose.SetPoint.LoopMode = LoopMode_Speed;
//    Stack_PushTail(Segment,&MoveToClose);
    ////////////////////////kendy///////////////
    TU32 CurrentPosition = EncoderDriver_GetPosition();
    TU32 MoveDistance =(TU32) labs(CurrentPosition - Mv->AbsFinalPlanedPosition);
    
    
    if( MoveDistance > (ONE_PRODUCT_TURN/2))//������ڰ�Ȧ�����
    {
      TS16 lsp = Mv->MoveDirection * (ONE_PRODUCT_TURN/2);
      TS16 Speed = EncoderDriver_GetSpeed();
      if( MoveDistance > (ONE_PRODUCT_TURN/2))//������ڰ�Ȧ�����
      { //�ƶ���Զ��  ����Զ
        MoveToFar.UnInteruptible = FALSE;//���Ա���� ��ֹ
        MoveToFar.SpeedTrigger.SpeedTriggerValue = 0U;
        MoveToFar.SpeedTrigger.TriggerEnable = FALSE;
        MoveToFar.TimeTrigger.TimeTriggerValue= 0U;
        MoveToFar.TimeTrigger.TriggerEnable=FALSE;
        
        MoveToFar.PositionTrigger.PosTriggerValue = Mv->AbsFinalPlanedPosition - lsp;
        MoveToFar.PositionTrigger.TriggerEnable=TRUE;
        
        MoveToFar.SetPoint.AbsTarget =Current_Speed;
        MoveToFar.SetPoint.AbsStartUp = (Speed == 0) ? (STARTUP_SPEED) : (abs(Speed));
        MoveToFar.SetPoint.AccMax = 6000u;//ACC_SOFT;
        MoveToFar.SetPoint.DccMax = ACC_FAST;//ACC_FAST;
        MoveToFar.SetPoint.Direction = Mv->MoveDirection;
        MoveToFar.SetPoint.LoopMode  = LoopMode_Speed;//ѭ����ʽ LoopMode_Speed
        Stack_PushTail(Segment,&MoveToFar);
      }

        //��Ҫ����Ŀ�� �ƶ�������
      //if(Current_Speed > STARTUP_SPEED)
      //{
        MoveToClose.UnInteruptible = FALSE;//���Ա���� ��ֹ
        MoveToClose.SpeedTrigger.SpeedTriggerValue = 0U;
        MoveToClose.SpeedTrigger.TriggerEnable = FALSE;
        MoveToClose.TimeTrigger.TimeTriggerValue= 0U;
        MoveToClose.TimeTrigger.TriggerEnable=FALSE;
        
        MoveToClose.PositionTrigger.PosTriggerValue = Mv->AbsFinalPlanedPosition - Mv->MoveDirection;
        MoveToClose.PositionTrigger.TriggerEnable=TRUE;
        
        MoveToClose.SetPoint.AbsTarget = (1185u);//12RPM*96 ==158*7.5
        MoveToClose.SetPoint.AbsStartUp = (Speed == 0) ? (STARTUP_SPEED) : (abs(Speed));
        MoveToClose.SetPoint.AccMax = 2000U;//kendy //6000u 
        MoveToClose.SetPoint.DccMax = ACC_FAST;//kendy //ACC_SOFT 
        MoveToClose.SetPoint.Direction = Mv->MoveDirection;
        MoveToClose.SetPoint.LoopMode = LoopMode_Speed;//ѭ����ʽ LoopMode_Speed
        Stack_PushTail(Segment,&MoveToClose);
      //}
    }
    else
    {
        TS16 Speed = EncoderDriver_GetSpeed();
        MoveToClose.UnInteruptible = TRUE;    //�����Ա���� ��ֹ
        MoveToClose.SpeedTrigger.SpeedTriggerValue = 0U;
        MoveToClose.SpeedTrigger.TriggerEnable = FALSE;
        MoveToClose.TimeTrigger.TimeTriggerValue= 0U;
        MoveToClose.TimeTrigger.TriggerEnable=FALSE;

        MoveToClose.PositionTrigger.PosTriggerValue = Mv->AbsFinalPlanedPosition - Mv->MoveDirection;
        MoveToClose.PositionTrigger.TriggerEnable=TRUE;
//
//        MoveToClose.SetPoint.AbsTarget  = STANDARD_28_SPEED;
//        MoveToClose.SetPoint.AbsStartUp = (Speed == 0) ? (STARTUP_SPEED) : (abs(Speed));
//        
//        MoveToClose.SetPoint.AccMax = ACC_BRUTAL;
//        MoveToClose.SetPoint.DccMax = ACC_BRUTAL;//ACC_FAST;
         MoveToClose.SetPoint.AbsTarget = (1185u);//12RPM*96 ==158*7.5//kendy 
        MoveToClose.SetPoint.AbsStartUp = (Speed == 0) ? (STANDARD_28_SPEED) : (abs(Speed));
        MoveToClose.SetPoint.AccMax = 2000U;//kendy //6000u 
        MoveToClose.SetPoint.DccMax = ACC_FAST;//kendy //ACC_SOFT 
        MoveToClose.SetPoint.Direction = Mv->MoveDirection;
        MoveToClose.SetPoint.LoopMode  = LoopMode_Speed;//ѭ����ʽ LoopMode_Speed
        Stack_PushTail(Segment,&MoveToClose); 
    }
//     
  }
   
  MoveToStop.UnInteruptible = TRUE;                //�����Ա���� ��ֹ
  MoveToStop.SpeedTrigger.SpeedTriggerValue = 0U;
  MoveToStop.SpeedTrigger.TriggerEnable = TRUE;
  MoveToStop.TimeTrigger.TimeTriggerValue= 0U;
  MoveToStop.TimeTrigger.TriggerEnable=FALSE;
  
  MoveToStop.PositionTrigger.PosTriggerValue = 0U;
  MoveToStop.PositionTrigger.TriggerEnable=FALSE;
  
  MoveToStop.SetPoint.AbsTarget  = ABS_MAX_PWM;
  MoveToStop.SetPoint.AbsStartUp = ABS_MAX_PWM;
  MoveToStop.SetPoint.AccMax = 0U;
  MoveToStop.SetPoint.DccMax = 0U;
  MoveToStop.SetPoint.Direction = MotorDirection_Stop;//ֹͣ
  MoveToStop.SetPoint.LoopMode  = LoopMode_Brake; //ѭ����ʽ LoopMode_Brake
  Stack_PushTail(Segment,&MoveToStop);
  
  return  Rtn;
}


//�ƻ� ֹͣ
static TBool PlanMove_Stop(MovementManager_Move_s *Mv,StackObjRef Segment)
{
  TBool Rtn = TRUE;
  MotorControl_Segment_s Stop;
  
  Stop.SpeedTrigger.SpeedTriggerValue = 0U;
  Stop.SpeedTrigger.TriggerEnable = TRUE;
  
  Stop.PositionTrigger.PosTriggerValue = 0U;
  Stop.PositionTrigger.TriggerEnable=FALSE;
  Stop.TimeTrigger.TimeTriggerValue= 0U;
  Stop.TimeTrigger.TriggerEnable=FALSE;
  
  Stop.UnInteruptible = TRUE;//�����Ա���� ��ֹ
  Stop.SetPoint.Direction = MotorDirection_Stop;//ֹͣ
  if(Mv->MoveOption == MoveOption_HardStop)//Ӳֹͣ
  {
    Stop.SetPoint.AbsTarget  = ABS_MAX_PWM;
    Stop.SetPoint.AbsStartUp = ABS_MAX_PWM;
    Stop.SetPoint.AccMax = 0U;
    Stop.SetPoint.DccMax = 0U;
    Stop.SetPoint.LoopMode = LoopMode_Brake; //ɲ��
  }
  else
  {                                          //��ֹͣ  ���ټ���
    Stop.SetPoint.AbsTarget  = ABS_MAX_PWM;
    Stop.SetPoint.AbsStartUp = ABS_MAX_PWM;
    Stop.SetPoint.AccMax = 0U;
    Stop.SetPoint.DccMax = 0U;
    Stop.SetPoint.LoopMode = LoopMode_Brake;
  }
  
  DEBUG_PRINT("[DBG][MV_Planner] Stack Stop segment\n");
  Stack_PushTail(Segment,&Stop);
  return  Rtn;
}

//�ƻ� �ƶ�
static TBool PlanMove_Move(MovementManager_Move_s *Mv,StackObjRef Segment)
{
  MotorControl_Segment_s Move;
  
  Move.UnInteruptible = FALSE;//���Ա���� ��ֹ
  Move.SpeedTrigger.SpeedTriggerValue = 0U;
  Move.SpeedTrigger.TriggerEnable = FALSE;    //�ٶȴ����ر�
  Move.TimeTrigger.TimeTriggerValue= 0U;
  Move.TimeTrigger.TriggerEnable=FALSE;       //�ƶ�ʱ�䴥���ر�
  Move.PositionTrigger.PosTriggerValue = 0U;
  Move.PositionTrigger.TriggerEnable   =FALSE;//λ�ô����ر�
  
  Move.SetPoint.AbsTarget  = Current_Speed;
  Move.SetPoint.AbsStartUp = STARTUP_SPEED;
  Move.SetPoint.AccMax = ACC_SOFT;
  Move.SetPoint.DccMax = ACC_FAST;
  Move.SetPoint.Direction = Mv->MoveDirection;
  Move.SetPoint.LoopMode = LoopMode_Speed;
  Stack_PushTail(Segment,&Move);
  return TRUE;
}



