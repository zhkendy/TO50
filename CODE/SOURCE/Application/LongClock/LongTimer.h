// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************




/**
*  @file LongTimer.h
*  @addtogroup LongTimer
*  @{

*/



#include "Config.h"
#include "SFY_OS.h"

/**
* @brief LongTimer Resolution.
*/
#define LongTimerTimebase_OS (OS_TIME_100_MS)  //LongTimerTimebase ʱ��
#define LongTimerTimebase_ms (100)
/**
* @brief Event called when Timer Time base timeout
*/
#define LongTimerTick     (TASK_Controller_LongTimerTick_EVENT)

/**
* @brief Add/Remove enum to Add/Remove LongTimer
*/
typedef enum LongTimerName_e
{
  LongTimer_ErgoTimer,
  LongTimer_AutoSave,         //4H�Զ�����
  LongTimer_6MinMaxMoveTimer, //6min����ʱ��
  LongTimer_30MinCharger,     //30min
  LongTimerName_LASTENUM,
}LongTimerName_e;

/**
* @brief State of a LongTimer
*/
typedef enum LongTimerState_e
{
  LongTimerState_Running,   //����
  LongTimerState_Stop,      //ֹͣ
  LongTimerState_Pause,     //��ͣ
  LongTimerState_Timeout,   //��ʱ
}LongTimerState_e;

void  LongTimer_Init();
void  LongTimer_StartTick();
void  LongTimer_TimeAdd(TU32 Time);
void  LongTimer_HandleTimeout();
TBool LongTimer_DidTimerTimeout();
void  LongTimer_Setup(LongTimerName_e Timer, TU32 TimerValueMS, TBool Loop,TBool PlayImmediately, void (*Fptr_Callback)());
TBool LongTimer_Reload(LongTimerName_e Timer);
TBool LongTimer_Play(LongTimerName_e Timer);
TBool LongTimer_Stop(LongTimerName_e Timer);
TBool LongTimer_Pause(LongTimerName_e Timer);
LongTimerState_e LongTimer_GetState(LongTimerName_e Timer);

/** @}*/

