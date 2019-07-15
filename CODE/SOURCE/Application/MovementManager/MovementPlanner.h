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
*
*/

#pragma once

#include "MovementManager.h"
#include "MovementRequest_Interface.h"
//
TBool MovementPlanner_BuildMoveList(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
TBool MovementPlanner_PlanMove(MovementManager_Move_s *Mv,StackObjRef Segment);

static TU32 MovementPlanner_EstimateMoveDistance(TU16 RPMSpeed,TU32 MoveTime_ms);
static TU32 MovementPlanner_EstimateMoveTime(TU16 RPMSpeed,TU32 Distance);


static TBool BuildMoveList_SecurityStop(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_None(MoveRequest_s *MvRq,StackObjRef TmpMvStack);

static TBool BuildMoveList_Manual_Stop(MoveRequest_s *MvRq,StackObjRef TmpMvStack); 
static TBool BuildMoveList_Manual_Roll(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Manual_UnRoll(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Manual_RollForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Manual_UnRollForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Manual_PWM(MoveRequest_s *MvRq,StackObjRef TmpMvStack);

static TBool BuildMoveList_Nolimit_RollForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_UnRollForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_StopForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_RollTo(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_UnRollTo(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_ShortUnRoll(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_ShortRoll(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_Rolloff(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_UnRolloff(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_ShortFeedBack(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_ErrFeedBack(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_DoubleShortFeedBack(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_ThreeShortFeedBack(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_FourShortFeedBack(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_10sCycle_MoveMent(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_TiltingCycle_MoveMent(MoveRequest_s *MvRq,StackObjRef TmpMvStack);//kendy
static TBool BuildMoveList_Nolimit_Stop(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Nolimit_ErrFdBack_MoveTo(MoveRequest_s *MvRq,StackObjRef TmpMvStack);

static TBool BuildMoveList_RollForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_UnRollForTime(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_RollToAbsPos(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_UnRollToAbsPos(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_GoToPercent(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_GoToAbsPos(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_ShortUnRoll(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_ShortRoll(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_Rolloff(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_UnRolloff(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_RollToLimits(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_UnRollToLimits(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_GoToLimits(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_RollToPosition(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_UnRollToPosition(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_GoToPosition(MoveRequest_s *MvRq,StackObjRef TmpMvStack);
static TBool BuildMoveList_TiltMyPosition(MoveRequest_s *MvRq,StackObjRef TmpMvStack);


static TBool PlanMove_SecurityStop(MovementManager_Move_s *Mv,StackObjRef Segment);
static TBool PlanMove_None(MovementManager_Move_s *Mv,StackObjRef Segment);
static TBool PlanMove_Manual_PWM(MovementManager_Move_s *Mv,StackObjRef Segment);
static TBool PlanMove_Move(MovementManager_Move_s *Mv,StackObjRef Segment);
static TBool PlanMove_MoveForTime(MovementManager_Move_s *Mv,StackObjRef Segment);
static TBool PlanMove_StopForTime(MovementManager_Move_s *Mv,StackObjRef Segment);
static TBool PlanMove_MoveTo(MovementManager_Move_s *Mv,StackObjRef Segment);

static TBool PlanMove_Stop(MovementManager_Move_s *Mv,StackObjRef Segment);



static void Stack_SecurityStop(StackObjRef TmpMvStack);
static void Stack_None(StackObjRef TmpMvStack);
static void Stack_Manual_PWM(StackObjRef TmpMvStack,MotorDirection_e MoveDirection,TU16 PWM);
static void Stack_Move(StackObjRef TmpMvStack,MotorDirection_e MoveDirection,MoveOption_e Option,TU16 CustomAcc,TU16 CustomDcc,TU16 CustomSpeed);
 void Stack_MoveForTime(StackObjRef TmpMvStack,MotorDirection_e MoveDirection,MoveOption_e Option,TU32 Timems);
static void Stack_Stop(StackObjRef TmpMvStack,MoveOption_e Option);
 void Stack_StopForTime(StackObjRef TmpMvStack,MoveOption_e Option,TU32 TimesMs);
 void Stack_MoveTo(StackObjRef TmpMvStack,MotorDirection_e MoveDirection,MoveOption_e Option,TU16 CustomAcc,TU16 CustomDcc,TU16 CustomSpeed,TU32 AbsPos);
static void Stack_ShortFeedBack_UpFirst(StackObjRef TmpMvStack,TBool CompensateFirstMove);
static void Stack_ShortFeedBack_DownFirst(StackObjRef TmpMvStack,TBool CompensateFirstMove);
static void Stack_ErrFeedBack_UpFirst(StackObjRef TmpMvStack);
static void Stack_ErrFeedBack_DownFirst(StackObjRef TmpMvStack);
static void Stack_ShortMove_DownFirst(StackObjRef TmpMvStack,TBool CompensateFirstMove,TU16 time);
static void Stack_ShortMove_UpFirst(StackObjRef TmpMvStack,TBool CompensateFirstMove,TU16 time);
static void Stack_TiltingShortMove_UpFirst(StackObjRef TmpMvStack,TBool CompensateFirstMove,TU16 time);
static void Stack_TiltingShortMove_DownFirst(StackObjRef TmpMvStack,TBool CompensateFirstMove,TU16 time);

