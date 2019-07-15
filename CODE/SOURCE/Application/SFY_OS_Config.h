// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

//--------------------------------------------------------------------------
// SOMFY Operating System Abstraction Layer - Version 0.5 - 13/05/2008
// Configuration file
//--------------------------------------------------------------------------


//---------------------------------------------------------------------
// File "SFY_OS_Config.h"   Generated with SOMFY code generator v[3.5.0.0]
//---------------------------------------------------------------------
#include "SFY_Types.h"
#include "stm8s.h"
// Header safety
//---------------------
#ifndef SFY_OS_CONFIG_H
#define SFY_OS_CONFIG_H


// Public types
//--------------

// 1st step : TASKS CONFIGURATION
// To do : declare your tasks ids classified by decreasing priority
// -------------------------------------------------------------------------
typedef enum
{
  TIMER_TASK,
  OS_TASK_RTS,
  OS_TASK_Motor,
  OS_TASK_Controller,
  OS_TASK_IDLE,
  OS_TASKS_NUMBER, // Warning : must be the last one !
} TeTask;



// 2nd step : TASKS STACKS CONFIGURATION
// To do : declare the size of all task stacks (in Words)
// Tips : declare a #define for each tasks, then sum them 
// Example following :
// #define OS_TASK_1_STACK_SIZE 100
// #define OS_TASK_2_STACK_SIZE 100 
// #define OS_STACK_SIZE_IN_WORDS (OS_TASK_1_STACK_SIZE + OS_TASK_2_STACK_SIZE)
// -------------------------------------------------------------------------
///xiugai sage
//#define TASK_Motor_Stack_Size           256  //  1024  |  512
//#define TASK_RTS_Stack_Size             256  //  2048  | 1024
//#define TASK_Controller_Stack_Size      256  //  2048  | 1024
////
//#define OS_STACK_SIZE_IN_WORDS (TASK_Motor_Stack_Size + TASK_RTS_Stack_Size + TASK_Controller_Stack_Size)



// 3rd step : INTERRUPT STACK CONFIGURATION
// To do : 1 - Declare the size of the interrupt stack (in Words)
//         2 - Declare the maximum number of OS calls in one interrupt
// ------------------------------------------------------------------------- 

//// Declare the size of the interrupt stack (in Words)
//#define OS_INTERRUPT_STACK_SIZE_IN_WORDS 200
//
//// Declare the maximum number of OS calls in one interrupt
//#define OS_MAXIMUM_OS_CALLS_DURING_INTERRUPT 5



// 4th step : TASK EVENTS CONFIGURATION
// To do : for each tasks, declare all your events   
// WARNING : some OS are limited to 8 events. If using more, be sure that 
// your OS will accept it and that you won't swap to another OS one day !
// Example following :       
// #define OS_TASK1_EXAMPLE_EVT_1 (1<<0)
// #define OS_TASK1_EXAMPLE_EVT_2 (1<<1)
// #define OS_TASK1_EXAMPLE_EVT_3 (1<<2) 
// -------------------------------------------------------------------------

// TASK_Motor events
#define TASK_Motor_MOVEMENTMANAGER_NEWMOVE                      (1u<<1u)
#define TASK_Motor_MOVEMENTMANAGER_NEWSEG                       (1u<<2u)
#define TASK_Motor_MOVEMENTMANAGER_NEWACTION                    (1u<<3u)
#define TASK_Motor_MOVEMENTMANAGER_LOOP                         (1u<<4u)
#define TASK_Motor_SECU_TIMER                                   (1u<<5u)


// TASK_RTS events
#define TASK_RTS_FRAME_RX_EVENT                                 (1u<<1u)
#define TASK_RTS_TIMER_EVENT                                    (1u<<2u)
#define TASK_RTS_DATA_CHANGE                                    (1u<<3u)
#define TASK_RTS_ERGO_DEAD_MAN_TIMER                            (1u<<4u)


#define TASK_IDLE_EVENT                                         (1u<<0u)

//TASK_Controller
#define TASK_Controller_LOWPOWER_TIMEOUT_EVENT                  (1u<<1u)
#define TASK_Controller_Boot_EVENT                              (1u<<2u)
#define TASK_Controller_IndusUARTRX_EVENT                       (1u<<3u)
#define TASK_Controller_EMS_TIMEOUT_EVENT                       (1u<<4u)
#define TASK_Controller_LongTimerTick_EVENT                     (1u<<5u)
#define TASK_Controller_EMS_LowPowerTest_EVENT                  (1u<<6u)
#define TASK_Controller_STOP_TO                                 (1u<<7u)
// 5th step : MESSAGE BOXES CONFIGURATION
// To do : 1 - Uncomment the #define if you are using message boxes   
//         2 - Declare your message boxes in the enum
//         3 - Indicate the maximum number of messages for your application
// ------------------------------------------------------------------------- 

// Uncomment if using message boxes
// #define YES_I_AM_USING_MESSAGE_BOXES

// Enumerate all your message boxes
typedef enum
{
  OS_MESSAGE_BOXES_NUMBER, // Warning : must be the last one !
} TeMessageBox;

// Indicate the maximum number of messages
#define OS_MAXIMUM_NUMBER_OF_MESSAGES 10



// 6th step : RESOURCES CONFIGURATION
// To do : 1 - Uncomment the #define if you are using resources   
//         2 - Declare your resources in the enum
// ------------------------------------------------------------------------- 

// Uncomment if using resources
// #define YES_I_AM_USING_RESOURCES

// Enumerate your resources
typedef enum
{
  OS_RESOURCES_NUMBER, // Warning : must be the last one !
} TeResource;



// 7th step : TIMERS CONFIGURATION
// To do : 1 - Uncomment the #define if you are using timers   
//         2 - Declare your timers in the enum
// ------------------------------------------------------------------------- 

// Uncomment if using timers
#define YES_I_AM_USING_TIMERS

// Enumerate your timers
typedef enum
{
  RTSPROTOCOL_TIMER_ID,
  CONTROLLER_EMS_START_FRAME_TIMER_ID,
  CONTROLLER_LOWPOWER_TIMER_ID,
  LONGTIMER_TIMER_ID,
  MOVEMENTMANAGER_LOOP_TIMER_ID,      //ÔË¶¯Ñ­»·Ê±¼ä
  MOVEMENTMANAGER_STOP_TIMER_ID,      //Í£Ö¹Ê±¼ä¼ÆÊý
  MOTORCONTROL_SECU_TIMER_ID,         //°²È«Ê±¼ä¼ÆÊý
  OS_TIMERS_NUMBER,                   // Warning : must be the last one !
} TeTimer;



// 8th step : OS TICKS CONFIGURATION
// To do : indicate the number of µs per OS tick   
// WARNING : On Windows, will be automatically set to 1000 µs
// -------------------------------------------------------------------------
#define OS_NUMBER_OF_US_PER_TICK (1000)

// 9th step : OS CRITICAL SECTION CONFIGURATION
// To do : indicate ENTRY EXIT Functions to be called   
// -------------------------------------------------------------------------



#define SFY_OS_CRITICAL_SECTION_ENTRY() \
{\
__istate_t IT_State;\
IT_State =  __get_interrupt_state();\
  __disable_interrupt();\

#define SFY_OS_CRITICAL_SECTION_EXIT() \
__set_interrupt_state(IT_State);\
}

// Header end
//--------------
#endif // SFY_OS_CONFIG_H

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
