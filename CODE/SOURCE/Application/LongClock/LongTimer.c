// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************




/**
* @file LongTimer.c
* @addtogroup LongTimer
* @ingroup Application
* @{
* @author Gabriel Mula.
* @date 2016.
* @brief Longtimer Object provide a 32bit timers.
* @details This object manage a collection of staticaly allocated timers.\n
* To add/Delete chronometer, edit LongTimerName_e enum.\n
* Object need to be init (initialise all timer and create an os timer).\n
* Those timer can count up to 2^32 ms.\n
* This object need a regular 16 bits OS_Timer and an Os_event (LongTimerTick) to work. Each time the regular timer timeout, it's add time to the long timer.\n
* LongTimer resolution depand on LongTimerTimebase define value. Caution, CPU cost is not negligeable on STM8. 100ms is a good compromise.\n
* To reduce the CPU cost, handling timer overflow (by calling callback function) and computing timer state are two separate process.\n
* It's under user responsibility to call void LongTimer_TimeAdd(TU32 Time) to count time and void LongTimer_HandleTimeout() to run timeout function where it's revelant.\n
* It could be usefull to add Listener interface to this object, but i have choose to call LongTimer_HandleTimeout() under the idle task.
 \n
@startuml
TimerTask -> LongTimer : LongTimerTimeAdd(TU32 Time)
LongTimer -> LongTimer : When overflow, set G_TimerTimeOut = TRUE
IdleTask  -> IdleTask : Call LongTimer_HandleTimeout() 
@enduml

* For lowpower application, it's interesting to be wakeup by LongTimer, to trigger eeprom backup every 8h for exemple.\n
@startuml
LowPower  -> LongTimer : LongTimerTimeAdd(TU32 Time)
LongTimer -> LongTimer : When overflow, set G_TimerTimeOut = TRUE
LowPower  -> LowPower : If LongTimer_DidTimerTimeout() == TRUE wakeup
@enduml
*/


#include "Config.h"
#include "LongTimer.h"

/**
* @var G_TimerTimeOut
* @brief True when at least a longTimerOverflow
*/
static TBool G_TimerTimeOut = FALSE;

/**
* @struct LongTimer_s
* @brief This struct represent a longtimer instance. You can add/remove longtimer to LongTimerArray by editing LongTimerName_e enum.
*/
typedef struct LongTimer_s
{
  LongTimerState_e State;       /**< State of the timer LongTimerState_e*/
  TBool Loop;                   /**< True if the timer must loop after timeout*/
  TU32 TimeLeft_ms;             /**< Time left, in millisecond, before timeout*/
  TU32 InitialTime_ms;          /**< Timer start with this value*/
  void (*Fptr_Callback)();      /**< This callback is called after timer timeout*/
}LongTimer_s;


/**
* @var LongTimerArray
* @brief Array of all declared LongTimer_s
*/
static LongTimer_s LongTimerArray[LongTimerName_LASTENUM];
static void CallBackTrap();

static void CallBackTrap()
{
  trap();
}

/**
* @brief Init function, Must be call first
* @details Initialise G_TimerTimeOut and LongTimerArray. Create an OS timer.
*/
//初始化 三个longtimer参数  
/*  
  LongTimer_ErgoTimer,
  LongTimer_AutoSave,
  LongTimer_3MinMaxMoveTimer,
*/
void LongTimer_Init()
{
  G_TimerTimeOut = FALSE;
  for(TU8 i = 0U; i<LongTimerName_LASTENUM;++i)
  {
    LongTimerArray[i].State = LongTimerState_Stop;
    LongTimerArray[i].Loop  = FALSE;
    LongTimerArray[i].TimeLeft_ms = 0U;
    LongTimerArray[i].InitialTime_ms = 0U;
    LongTimerArray[i].Fptr_Callback = CallBackTrap;
  }
  
  if(SFY_OS_TimerCreate(LONGTIMER_TIMER_ID,OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Controller,LongTimerTick) != OS_SUCCESS)
  {
    trap();
  }
}

/**
* @brief Start Longtimer
*/
void LongTimer_StartTick()
{
  SFY_OS_TimerStart(LONGTIMER_TIMER_ID,LongTimerTimebase_OS,LongTimerTimebase_OS);
}

/**
* @brief return G_TimerTimeOut
*/
TBool LongTimer_DidTimerTimeout()
{
  return G_TimerTimeOut;
}


/**
* @brief Count Time to all running timer
* @param TU32 Time Time in ms to add
* @details Decrement all running tumer by Time. A timer reaching 0 timeout.
*/
void LongTimer_TimeAdd(TU32 Time)
{
  for(TU8 i = 0U; i<LongTimerName_LASTENUM;++i)
  {
    if(LongTimerArray[i].State == LongTimerState_Running)
    {
      if(LongTimerArray[i].TimeLeft_ms > Time)
      {
        LongTimerArray[i].TimeLeft_ms -= Time;
      }
      else
      {
        LongTimerArray[i].TimeLeft_ms = 0U;
        LongTimerArray[i].State = LongTimerState_Timeout;
        G_TimerTimeOut  = TRUE;
      }
    }
  }
}


/**
* @brief Handle longtimer timeout
*longtimer 超时处理
* @details This function Check if G_TimerTimeOut == TRUE. It then loop throw LongTimerArray and call all TimeoutFunction of longtimer in LongTimerState_Timeout state.
*/
void LongTimer_HandleTimeout()
{
  if(G_TimerTimeOut == TRUE)
  {
    G_TimerTimeOut = FALSE;
    for(TU8 i = 0U; i<LongTimerName_LASTENUM;++i)
    {
      if(LongTimerArray[i].State == LongTimerState_Timeout)
      {
        LongTimerArray[i].State = LongTimerState_Stop;
        LongTimerArray[i].Fptr_Callback();      
        if(LongTimerArray[i].Loop == TRUE)
        {
          LongTimerArray[i].TimeLeft_ms = LongTimerArray[i].InitialTime_ms;
          LongTimerArray[i].State = LongTimerState_Running;
        }
      }
    }
  }
}



/**
* @brief Setup a longtimer.
* @param Timer : Must be LongTimerName_e value
* @param TimerValueMS : Initial Timerr value. Timeout will happen in TimerValueMS ms.
* @param Loop : If true, the timer restart after timeout, forever.
* @param PlayImmediately : If true the timer is started immediatly. Else, you must start the timer later with LongTimer_Play()
*/
void LongTimer_Setup(LongTimerName_e Timer, TU32 TimerValueMS, TBool Loop,TBool PlayImmediately, void (*Fptr_Callback)())
{
  LongTimerArray[Timer].State = ( PlayImmediately == TRUE ? (LongTimerState_Running) : (LongTimerState_Stop) );
  LongTimerArray[Timer].Loop = Loop;
  LongTimerArray[Timer].TimeLeft_ms = TimerValueMS;
  LongTimerArray[Timer].InitialTime_ms = TimerValueMS;
  LongTimerArray[Timer].Fptr_Callback = Fptr_Callback;
}

/**
* @brief Timer state getter
* @param Timer : Must be LongTimerName_e value.
* @return Copy of timer state.
*/
LongTimerState_e LongTimer_GetState(LongTimerName_e Timer)
{
  return LongTimerArray[Timer].State;
}

/**
* @brief Start a timer.
* @param Timer : Must be LongTimerName_e value.
* @return Return True if no error. Return False if timeout state is already LongTimerState_Running or LongTimerState_Timeout
* @details Timer in state LongTimerState_Stop are reload and set in LongTimerState_Running\n
* Timer in state LongTimerState_Pause are set in LongTimerState_Running\n
*/
TBool LongTimer_Play(LongTimerName_e Timer)
{
  TBool Rtn = FALSE;
  if(LongTimerArray[Timer].State == LongTimerState_Stop)
  {
    LongTimerArray[Timer].TimeLeft_ms = LongTimerArray[Timer].InitialTime_ms;
    LongTimerArray[Timer].State = LongTimerState_Running;
    Rtn = TRUE;
  }
  else if(LongTimerArray[Timer].State == LongTimerState_Pause)
  {
    LongTimerArray[Timer].State = LongTimerState_Running;
    Rtn = TRUE;
  }
  //else => already running or timeout
  return Rtn;
}


/**
* @brief Reload a timer.
* @param Timer : Must be LongTimerName_e value.
* @return Return True if no error. Return False if timeout state is LongTimerState_Timeout
* @details Timer is reload and set in LongTimerState_Running\n
*/
TBool LongTimer_Reload(LongTimerName_e Timer)
{
  TBool Rtn = FALSE;
  if(LongTimerArray[Timer].State != LongTimerState_Timeout)
  {
    Rtn = TRUE;
    LongTimerArray[Timer].TimeLeft_ms = LongTimerArray[Timer].InitialTime_ms;
    LongTimerArray[Timer].State = LongTimerState_Running;
  }
  return Rtn;
}


/**
* @brief Stop a timer.
* @param Timer : Must be LongTimerName_e value.
* @return Return True if no error. Return False if timeout state is LongTimerState_Timeout
*/
TBool LongTimer_Stop(LongTimerName_e Timer)
{
  TBool Rtn = FALSE;
  //Only the state change. This will allow, if needed, to implement a get_Timer value function...
  //The timer is reload in Play function.
  if(LongTimerArray[Timer].State != LongTimerState_Timeout)
  {
    Rtn = TRUE;
    LongTimerArray[Timer].State = LongTimerState_Stop;
  }
  return Rtn;
}


/**
* @brief Pause a timer.
* @param Timer : Must be LongTimerName_e value.
* @return Return True if no error. Return False if timeout state is LongTimerState_Timeout
*/
//LongTimer 暂停
TBool LongTimer_Pause(LongTimerName_e Timer)
{
  TBool Rtn = FALSE;
  if(LongTimerArray[Timer].State != LongTimerState_Timeout)
  {
    Rtn = TRUE;
    LongTimerArray[Timer].State = LongTimerState_Pause;
  }
  return Rtn;
}

/** @}*/