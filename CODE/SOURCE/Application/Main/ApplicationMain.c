// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

// External inclusions
//--------------------
#include "Config.h"
#include "SFY_OS.h"
#include "Task_Motor.h"
#include "Task_RTS.h"
#include "Task_Controller.h"
#include "Task_Idle.h"
#include "STM8_Archiver.h"
#include "UARTComDriver.h"
#include "SQC.h"
#include "Stm8_Adc.h"
#include "PowerMonitor.h"
#include "LongTimer.h"
#include "OSTick.h"
#include "RTSProtocol.h"
#include "RTS_InputCapture.h"
#include "Controller.h"
#include "RTS_Ergonomic_Manager.h"
#include "MovementManager.h"
#include "RtsLowPowerManager.h"
#include "Chrono.h"
#include "Platform_Config.h"

/*!************************************************************************************************
* \fn         void Application_OS_Init(void)
* \return     void
***************************************************************************************************/

void Application_OS_Init(void)
{
    //--------------------------------------------
    // Create tasks ¥¥Ω®»ŒŒÒ
    //--------------------------------------------
  if( SFY_OS_TaskCreate( OS_TASK_Motor, &Task_Motor, 0 ) != OS_SUCCESS)
  {
    trap();
  }
  if( SFY_OS_TaskCreate( OS_TASK_RTS,&Task_RTS, 0 ) != OS_SUCCESS)
  {
    trap();
  }
  if( SFY_OS_TaskCreate( OS_TASK_Controller,&Task_Controller, 0 ) != OS_SUCCESS)
  {
    trap();
  }
  if( SFY_OS_TaskCreate( OS_TASK_IDLE,&Task_Idle, 0 ) != OS_SUCCESS)
  {
    trap();
  }
    //--------------------------------------------
    // Create Timers  Ω®¡¢RTS ±÷”∂”¡–
    //--------------------------------------------
    // Timer for specific uses  
  
  if( SFY_OS_TimerCreate(RTSPROTOCOL_TIMER_ID,OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_RTS,TASK_RTS_TIMER_EVENT) != OS_SUCCESS)
  {//Task ¥¥Ω®÷Æ∫Û ¬Ì…œ…Ë÷√RTS ±÷” ¬º˛±Í÷æ
    trap();
  }
}


/*!************************************************************************************************
* \fn         void Application_InitClasses(void)
* \return     void
***************************************************************************************************/

void Application_InitClasses(void)
{
    OSTick_Init(); 
    Controller_Init();   
    SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_Controller, TASK_Controller_Boot_EVENT);                       
}


/*!************************************************************************************************
* \fn         void Application_LowPowerProcess(void)
* \return     void
***************************************************************************************************/

  
TU8 Application_Check_Heap_SpaceLeft()
{
  TU8 *pt_init_Heap = (TU8*) __section_begin("HEAP");
  TU8 *pt_end_Heap  = (TU8*) __section_end("HEAP") - 1;
  TU16 HeapSize = __section_size("HEAP");
  TU16 FreeSpaceLeft = 0U;
  
  while(pt_init_Heap < pt_end_Heap && *pt_end_Heap == LUCIFER)
  {
    ++FreeSpaceLeft;
    --pt_end_Heap;
  }
  TU16 FreeSpaceInPercent = ( (TU32) FreeSpaceLeft * (TU32) 100) / (TU32) HeapSize;
  return (TU8) (FreeSpaceInPercent);
}


TU8 Application_Check_Stack_SpaceLeft()
{
  TU8 *pt_init_Stack = (TU8*)  __section_begin("CSTACK");
  TU8 *pt_end_Stack  = (TU8*)  __section_end("CSTACK");
  TU16 StackSize = __section_size("CSTACK");
  TU16 FreeSpaceLeft = 0U;
  
  while(pt_init_Stack < pt_end_Stack && *pt_init_Stack == LUCIFER)
  {
    ++FreeSpaceLeft;
    ++pt_init_Stack;
  }
  TU16 FreeSpaceInPercent = ((TU32) FreeSpaceLeft * (TU32) 100) / (TU32) StackSize;
  return (TU8) (FreeSpaceInPercent);
}


/**********************************************************************
Copyright © (2008), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/

