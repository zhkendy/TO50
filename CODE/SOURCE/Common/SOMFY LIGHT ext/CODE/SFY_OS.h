// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


//---------------------------------------------------------------------
// File "SFY_OS.h"   Generated with SOMFY code generator v[4.6.11.0]
// SOMFY OS LIGHT 
//   * Version 1.0 - 28/10/2013
//     - Handles multiple µC because no specific µC instruction
//     - Handles multiple Compilers because no specific Compilers instruction
//     - Optimized for 8-bit µC
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef __SFY_OS_H__
#define __SFY_OS_H__


// External inclusions
//--------------------
#include "SFY_OS_Config.h"


// Public types
//--------------

#define OS_INFINITE_TIME (0xFFFF)


//! -------------------------------------------------------------------------
// SFY OS Internal types
// -------------------------------------------------------------------------
// 
// Return values

typedef enum
{
  OS_SUCCESS = 0,
  OS_ERROR,
  OS_TIMEOUT,
  OS_RESOURCE_OWNED,
  OS_RESOURCE_NOT_OWNED
} OS_RETURN;

//! Function pointer
typedef void (*Function_Pointer)(void);

//! Two possibilities for event flags clearance
typedef enum
{
  OS_DO_NOT_CLEAR_EVENT_FLAGS, //!< Don't clear event flags
  OS_CLEAR_EVENT_FLAGS         //!< Automatically clear event flags
} EventFlagsClearingMethod;

typedef enum
{
  OS_SIGNAL_TO_SPECIFIC_TASK,    //!< Signal for only one task
  OS_SIGNAL_TO_ALL_TASKS,        //!< Signal for all task waiting or not for that event
  OS_SIGNAL_TO_ALL_TASKS_WAITING //!< Signal for all task waiting for that event
} EventSignalMethod;

// Time constants
//---------------
#define OS_TIME_1_MS                (TU16)(    1000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_2_MS                (TU16)(    2000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_3_MS                (TU16)(    3000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_5_MS                (TU16)(    5000 / OS_NUMBER_OF_US_PER_TICK) 
#define OS_TIME_6_MS                (TU16)(    6000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_7_MS                (TU16)(    7000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_8_MS                (TU16)(    8000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_9_MS                (TU16)(    9000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_10_MS               (TU16)(   10000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_20_MS               (TU16)(   20000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_25_MS               (TU16)(   25000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_30_MS               (TU16)(   30000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_40_MS               (TU16)(   40000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_50_MS               (TU16)(   50000 / OS_NUMBER_OF_US_PER_TICK) 
#define OS_TIME_60_MS               (TU16)(   60000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_70_MS               (TU16)(   70000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_80_MS               (TU16)(   80000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_90_MS               (TU16)(   90000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_100_MS              (TU16)(  100000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_120_MS              (TU16)(  120000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_150_MS              (TU16)(  150000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_200_MS              (TU16)(  200000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_250_MS              (TU16)(  250000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_300_MS              (TU16)(  300000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_400_MS              (TU16)(  400000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_500_MS              (TU16)(  500000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_600_MS              (TU16)(  600000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_700_MS              (TU16)(  700000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_800_MS              (TU16)(  800000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_900_MS              (TU16)(  900000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_1_S                 (TU16)( 1000000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_1200_MS             (TU16)( 1200000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_1500_MS             (TU16)( 1500000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_2500_MS             (TU16)( 2500000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_2_S                 (TU16)( 2000000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_3_S                 (TU16)( 3000000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_4_S                 (TU16)( 4000000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_5_S                 (TU16)( 5000000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_6_S                 (TU16)( 6000000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_7_S                 (TU16)( 7000000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_8_S                 (TU16)( 8000000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_9_S                 (TU16)( 9000000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_10_S                (TU16)(10000000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_TIME_1_MIN               (TU16)(60000000 / OS_NUMBER_OF_US_PER_TICK)
#define OS_ONE_SHOT_TIMER 0


// Public functions declaration
//-----------------------------------

// Internal initialization
void SFY_OS_Init(void);

// Start OS.
// WARNING : Will never returned !
void SFY_OS_Start(void);

// Entry in an interrupt
void SFY_OS_InterruptEntry(void);

// Exit from an interrupt
void SFY_OS_InterruptExit(void);

/* LDRA Rule Deactivation : 210 S Macro name is not upper case. */
/*LDRA_INSPECTED 210 S*/
#define SFY_OS_Enable_Interrupts()   \
  enableInterrupts()

/* LDRA Rule Deactivation : 210 S Macro name is not upper case. */
/*LDRA_INSPECTED 210 S*/
#define SFY_OS_Disable_Interrupts()   \
  disableInterrupts()

// Interrupt routine that must be called to update the OS Tick
void SFY_OS_TickUpdate(void);

// Create a task
OS_RETURN SFY_OS_TaskCreate(TeTask eTaskIdArg, Function_Pointer pfFunctionArg, TU16 u16StackSizeInWordsArg);

// Start a task
/* LDRA Rule Deactivation : 210 S Macro name is not upper case. */
/*LDRA_INSPECTED 210 S*/
#define SFY_OS_TaskStart(eTaskIdArg)   \
  OS_SUCCESS

// Ask a task to wait a time in milliseconds
OS_RETURN SFY_OS_TaskDelay(TU16 u16WaitingTimeArg);

TU8 SFY_OS_EventWait(TU8 u8EventsArg, TU16 u16WaitingTimeArg, EventFlagsClearingMethod FlagClearanceMethodArg);

OS_RETURN SFY_OS_EventSignal(EventSignalMethod SignalMethodArg, TeTask eTaskIdArg, TU8 u8EventsArg);

OS_RETURN SFY_OS_EventSignalFromInterruption(EventSignalMethod SignalMethodArg, TeTask eTaskIdArg, TU8 u8EventsArg);

OS_RETURN SFY_OS_EventReset(TeTask eTaskIdArg, TU8 u8EventsArg);

OS_RETURN SFY_OS_MessageAssociateToEvent(TeMessageBox eMsgBoxIdArg, TeTask eTaskIdArg, TU8 u8EventArg);

OS_RETURN SFY_OS_MessageSend(TeMessageBox eMsgBoxIdArg, void * pMessageArg);

void * SFY_OS_MessageGet(TeMessageBox eMsgBoxIdArg);

/* LDRA Rule Deactivation : 210 S Macro name is not upper case. */
/*LDRA_INSPECTED 210 S*/
#define SFY_OS_MessageWait(eMsgBoxIdArg, u16WaitingTimeArg)   \
  SFY_OS_MessageGet(eMsgBoxIdArg)

/* LDRA Rule Deactivation : 210 S Macro name is not upper case. */
/*LDRA_INSPECTED 210 S*/
#define SFY_OS_ResourceGet(eResourceIdArg)   \
  OS_SUCCESS

/* LDRA Rule Deactivation : 210 S Macro name is not upper case. */
/*LDRA_INSPECTED 210 S*/
#define SFY_OS_ResourceWait(eResourceId, u16WaitingTimeArg)   \
  OS_SUCCESS

/* LDRA Rule Deactivation : 210 S Macro name is not upper case. */
/*LDRA_INSPECTED 210 S*/
#define SFY_OS_ResourceRelease(eResourceIdArg)   \
  OS_SUCCESS

OS_RETURN SFY_OS_TimerCreate(TeTimer eTimerIdArg, EventSignalMethod SignalMethodArg, TeTask eTaskIdArg, TU8 u8EventArg);

OS_RETURN SFY_OS_TimerStart(TeTimer eTimerIdArg, TU16 u16InitialValueArg, TU16 u16CyclicValueArg);

OS_RETURN SFY_OS_TimerRestart(TeTimer eTimerIdArg);

OS_RETURN SFY_OS_TimerStop(TeTimer eTimerIdArg);

OS_RETURN SFY_OS_TimerNewCyclicValue(TeTimer eTimerIdArg, TU16 u16CyclicValueArg);
// Header end
//--------------
#endif // __SFY_OS_H__

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
