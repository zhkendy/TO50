// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


//---------------------------------------------------------------------
// File "SFY_OS.c"   Generated with SOMFY code generator v[4.6.11.0]
// SOMFY OS LIGHT 
//   * Version 1.0 - 28/10/2013
//     - Handles multiple µC because no specific µC instruction
//     - Handles multiple Compilers because no specific Compilers instruction
//     - Optimized for 8-bit µC
//---------------------------------------------------------------------


// External inclusions
//--------------------
#include "SFY_Types.h"


// Internal inclusion
//------------------
#include "SFY_OS.h"


// Private types
//-------------

typedef struct
{
  TeMessageBox eAssociatedBox;
  void * pJoinedPiece;
} TsMessage;

typedef struct
{
  TeTask eAssociatedTask;
  TU8 u8AssociatedEvent;
} TsMessageBox;

typedef struct
{
  TBool bRunning;
  TeTask eAssociatedTask;
  TU8 u8EventToSet;
  TU16 u16Value;
  TU16 u16CyclicValue;
} TsTimer;


#if (OS_MAX_STACK_SIZE_IN_WORDS < PRODUCT_HEAP_SIZE)
  #error "Heap size is not big enough"
#endif

typedef struct
{
  volatile TU8 u8Event;
  Function_Pointer pfFunction;
} TsTask;

/* me is a macro equivalent to this in C++ */
/* LDRA Rule Deactivation : 210 S Macro name is not upper case. */
/*LDRA_INSPECTED 210 S*/
#define me (&oSomfyLight)

//! Field sizes

#define SFY_OS_AS_TASKS_SIZE  (sizeof(TsTask)*OS_TASKS_NUMBER)
#define SFY_OS_E_CURRENT_TASK_SIZE  (sizeof(TeTask))
#define SFY_OS_AS_MESSAGES_SIZE  (sizeof(TsMessage)*OS_MAXIMUM_NUMBER_OF_MESSAGES)
#define SFY_OS_AS_MSG_BOXES_SIZE  (sizeof(TsMessageBox)*OS_MESSAGE_BOXES_NUMBER)
#define SFY_OS_U8_MESSAGE_POSITION_SIZE  (sizeof(TU8))
#define SFY_OS_AS_TIMERS_SIZE  (sizeof(TsTimer)*OS_TIMERS_NUMBER)
#define SFY_OS_U16_TASK_WAITING_COUNTER_SIZE  (sizeof(TU16))

//! Object structure declaration
typedef struct
{
  TsTask asTasks[OS_TASKS_NUMBER];
  TeTask eCurrentTask;
#ifdef YES_I_AM_USING_MESSAGE_BOXES
  TsMessage asMessages[OS_MAXIMUM_NUMBER_OF_MESSAGES];
#endif // YES_I_AM_USING_MESSAGE_BOXES
#ifdef YES_I_AM_USING_MESSAGE_BOXES
  TsMessageBox asMsgBoxes[OS_MESSAGE_BOXES_NUMBER];
#endif // YES_I_AM_USING_MESSAGE_BOXES
#ifdef YES_I_AM_USING_MESSAGE_BOXES
  TU8 u8MessagePosition;
#endif // YES_I_AM_USING_MESSAGE_BOXES
#ifdef YES_I_AM_USING_TIMERS
  TsTimer asTimers[OS_TIMERS_NUMBER];
#endif // YES_I_AM_USING_TIMERS
  volatile TU16 u16TaskWaitingCounter; //MG => Modif under OS tick IT
} TsSFY_OS;


// Private functions declaration
//---------------------------------

static void SFY_OS_PRV_SetDefaultValue(void);

// Interrupt routine that must be called to update the OS Tick
static void SFY_OS_PRV_Schedule(void);


// Private variables
//------------------

//! Object instanciation
TsSFY_OS oSomfyLight;

/*LDRA_HEADER_END*/

// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \brief      Internal initialization
* \fn         void SFY_OS_Init(void)
* \return     void 
***************************************************************************************************/
void SFY_OS_Init(void)
{  
  // Object attributes initialization
  SFY_OS_PRV_SetDefaultValue();
  
  // Implementation
  //---------------
  {
    /*## BEGIN[GUID 7fdae10a-0641-4427-ac90-9652104cc555] ##*/
    // Local variables
    TU16 loc_u16Counter;   
    
    // Init Task events
    for (loc_u16Counter=0 ; loc_u16Counter<OS_TASKS_NUMBER ; loc_u16Counter++)
    {
      me->asTasks[loc_u16Counter].u8Event = 0x00U;
    }       
    
    // Init message box
    #ifdef YES_I_AM_USING_MESSAGE_BOXES
    me->u8MessagePosition = 0x00U;  
    #endif     
    
    // Init Timers
    #ifdef YES_I_AM_USING_TIMERS
    for (loc_u16Counter=0 ; loc_u16Counter<OS_TIMERS_NUMBER ; loc_u16Counter++)
    {                    
      me->asTimers[loc_u16Counter].bRunning = FALSE;
    }
    #endif       
    
    // Create Timer task
    me->asTasks[TIMER_TASK].pfFunction = SFY_OS_PRV_Schedule;
    /*## END[GUID 7fdae10a-0641-4427-ac90-9652104cc555] ##*/
  }
}


/*!************************************************************************************************
* \brief      Start OS.
*             WARNING : Will never returned !
* \fn         void SFY_OS_Start(void)
* \return     void 
***************************************************************************************************/
void SFY_OS_Start(void)
{  
  /*## BEGIN[GUID 9f9a25e1-0e34-4a6c-9837-24acb300917c] ##*/
  // Infinite Loop
  {
    // Local variables
    TeTask loc_eTask;
    
    // Infinite loop
    for(;;)
    {
      // Restart Counter               
      loc_eTask = ((TeTask)(0));  
      
      // Check each task events
      while(loc_eTask<OS_TASKS_NUMBER)
      {
        // Events waiting ?
        if (me->asTasks[loc_eTask].u8Event != 0)
        {
          // YES ! Run task   
          me->eCurrentTask = loc_eTask;
          me->asTasks[loc_eTask].pfFunction();
          
          // Clear counter in order to restart high priority task
          loc_eTask = ((TeTask)(0)); 
        }	           
        else
        {
          // No event, next task
          loc_eTask++;
        }
      }
    }  
  }
  
  // Never return !!
  //----------------        
  /*## END[GUID 9f9a25e1-0e34-4a6c-9837-24acb300917c] ##*/
}


/*!************************************************************************************************
* \brief      Entry in an interrupt
* \fn         void SFY_OS_InterruptEntry(void)
* \return     void 
***************************************************************************************************/
void SFY_OS_InterruptEntry(void)
{  
  /*## BEGIN[GUID 06175153-6537-49fa-9d1f-7a748b916518] ##*/
  // Nothing to do in that implementation      
  /*## END[GUID 06175153-6537-49fa-9d1f-7a748b916518] ##*/
}


/*!************************************************************************************************
* \brief      Exit from an interrupt
* \fn         void SFY_OS_InterruptExit(void)
* \return     void 
***************************************************************************************************/
void SFY_OS_InterruptExit(void)
{  
  /*## BEGIN[GUID 6d795843-0300-4c7e-9c42-492e43b5784c] ##*/
  // Nothing to do in that implementation      
  /*## END[GUID 6d795843-0300-4c7e-9c42-492e43b5784c] ##*/
}


/*!************************************************************************************************
* \brief      Interrupt routine that must be called to update the OS Tick
* \fn         void SFY_OS_TickUpdate(void)
* \return     void 
***************************************************************************************************/
void SFY_OS_TickUpdate(void)
{  
  /*## BEGIN[GUID 8352dcee-da23-4782-b8e3-8bf35de2d04f] ##*/
  // Mark event
  //-----------
  me->asTasks[TIMER_TASK].u8Event++;
  
  // Decreasing task waiting counter
  //--------------------------------
  if (me->u16TaskWaitingCounter != 0)
  {
    me->u16TaskWaitingCounter--;	
  }
  /*## END[GUID 8352dcee-da23-4782-b8e3-8bf35de2d04f] ##*/
}


/*!************************************************************************************************
* \brief      Create a task
* \fn         OS_RETURN SFY_OS_TaskCreate(TeTask eTaskIdArg, Function_Pointer pfFunctionArg, TU16 u16StackSizeInWordsArg)
* \param[in]  eTaskIdArg
* \param[in]  pfFunctionArg
* \param[in]  u16StackSizeInWordsArg
* \return     OS_RETURN 
***************************************************************************************************/
OS_RETURN SFY_OS_TaskCreate(TeTask eTaskIdArg, Function_Pointer pfFunctionArg, TU16 u16StackSizeInWordsArg)
{  
  /*## BEGIN[GUID c944028e-8867-4038-8d77-c91a98070095] ##*/
  // Save function pointer  
  me->asTasks[eTaskIdArg].pfFunction = pfFunctionArg; 
  
  // Return OS_SUCCESS  
  return OS_SUCCESS;      
  /*## END[GUID c944028e-8867-4038-8d77-c91a98070095] ##*/
}


/*!************************************************************************************************
* \brief      Ask a task to wait a time in milliseconds
* \fn         OS_RETURN SFY_OS_TaskDelay(TU16 u16WaitingTimeArg)
* \param[in]  u16WaitingTimeArg
* \return     OS_RETURN 
***************************************************************************************************/
OS_RETURN SFY_OS_TaskDelay(TU16 u16WaitingTimeArg)
{  
  /*## BEGIN[GUID 0744853a-f95a-4f0f-8e2f-9c64069ae385] ##*/
  // Setup Counter
  me->u16TaskWaitingCounter = u16WaitingTimeArg;
  
  // Wait
  while (me->u16TaskWaitingCounter != 0);
  
  // Return status
  return OS_SUCCESS;      
  /*## END[GUID 0744853a-f95a-4f0f-8e2f-9c64069ae385] ##*/
}


/*!************************************************************************************************
* \fn         TU8 SFY_OS_EventWait(TU8 u8EventsArg, TU16 u16WaitingTimeArg, EventFlagsClearingMethod FlagClearanceMethodArg)
* \param[in]  u8EventsArg            ÊÂ¼þ±êÖ¾
* \param[in]  u16WaitingTimeArg      µÈ´ýÊÂ¼þ
* \param[in]  FlagClearanceMethodArg Çå²»ÇåÊÂ¼þ±êÖ¾
* \return     TU8 
* \           µÈ´ýÊÂ¼þ
***************************************************************************************************/
TU8 SFY_OS_EventWait(TU8 u8EventsArg, TU16 u16WaitingTimeArg, EventFlagsClearingMethod FlagClearanceMethodArg)
{  
  /*## BEGIN[GUID 0950ab0a-aa26-4558-bb44-f525af44af9b] ##*/
  TU8 loc_u8RetVal = 0U;
  volatile TU8* loc_pu8TaskEvent = NULL;
  
  // Calculate address of Task Events Bits field to modify
  loc_pu8TaskEvent = &(me->asTasks[me->eCurrentTask].u8Event);
  
  // No Interruption from now
  SFY_OS_CRITICAL_SECTION_ENTRY();
  
  // Read events
  loc_u8RetVal = (*loc_pu8TaskEvent) & u8EventsArg;      
  
  // If Flag Must be cleared            //ÊÇ·ñÇåÊÂ¼þ±êÖ¾
  if ((0!=loc_u8RetVal) && (FlagClearanceMethodArg == OS_CLEAR_EVENT_FLAGS))
  {
    // THEN Clear it !
    *loc_pu8TaskEvent &= ~loc_u8RetVal;
  }
  
  // Interruptions accepted now
  SFY_OS_CRITICAL_SECTION_EXIT();
  
  // Return read events
  return loc_u8RetVal;       
  /*## END[GUID 0950ab0a-aa26-4558-bb44-f525af44af9b] ##*/
}


/*!************************************************************************************************
* \fn         OS_RETURN SFY_OS_EventSignal(EventSignalMethod SignalMethodArg, TeTask eTaskIdArg, TU8 u8EventsArg)
* \param[in]  SignalMethodArg
* \param[in]  eTaskIdArg
* \param[in]  u8EventsArg
* \return     OS_RETURN 
* \          ÉèÖÃÊÂ¼þÐÅºÅ
***************************************************************************************************/
OS_RETURN SFY_OS_EventSignal(EventSignalMethod SignalMethodArg, TeTask eTaskIdArg, TU8 u8EventsArg)
{  
  /*## BEGIN[GUID 65c6569e-4d45-4550-9d4d-7ae9e22a9a68] ##*/
  volatile TU8* loc_pu8TaskEvent = NULL;
  
  // Calculate address of Task Events Bits field to modify
  loc_pu8TaskEvent = &(me->asTasks[eTaskIdArg].u8Event);
  
  // No Interruption from now
  SFY_OS_CRITICAL_SECTION_ENTRY();
  
  // Save Signal information
  *loc_pu8TaskEvent |= u8EventsArg; 
  
  // Interruptions accepted now
  SFY_OS_CRITICAL_SECTION_EXIT();
  
  // Return OS_SUCCESS  
  return OS_SUCCESS;      
  /*## END[GUID 65c6569e-4d45-4550-9d4d-7ae9e22a9a68] ##*/
}


/*!************************************************************************************************
* \fn         OS_RETURN SFY_OS_EventSignalFromInterruption(EventSignalMethod SignalMethodArg, TeTask eTaskIdArg, TU8 u8EventsArg)
* \param[in]  SignalMethodArg
* \param[in]  eTaskIdArg
* \param[in]  u8EventsArg
* \return     OS_RETURN 
* \           ÊÂ¼þÐÅºÅÀ´×ÔÓÚÖÐ¶Ï
***************************************************************************************************/
OS_RETURN SFY_OS_EventSignalFromInterruption(EventSignalMethod SignalMethodArg, TeTask eTaskIdArg, TU8 u8EventsArg)
{  
  /*## BEGIN[GUID 560d451a-905d-47d4-8bfe-a68e4474098b] ##*/
  volatile TU8* loc_pu8TaskEvent = NULL;
  
  // Calculate address of Task Events Bits field to modify
  loc_pu8TaskEvent = &(me->asTasks[eTaskIdArg].u8Event);
  
  // Save Signal information
  *loc_pu8TaskEvent |= u8EventsArg; 
  
  // Return OS_SUCCESS  
  return OS_SUCCESS;  
  /*## END[GUID 560d451a-905d-47d4-8bfe-a68e4474098b] ##*/
}


/*!************************************************************************************************
* \fn         OS_RETURN SFY_OS_EventReset(TeTask eTaskIdArg, TU8 u8EventsArg)
* \param[in]  eTaskIdArg
* \param[in]  u8EventsArg
* \return     OS_RETURN 
* \           Çå³ýÊÂ¼þ±êÖ¾
***************************************************************************************************/
OS_RETURN SFY_OS_EventReset(TeTask eTaskIdArg, TU8 u8EventsArg)
{  
  /*## BEGIN[GUID ca418615-d89b-4d0e-8422-ac70021b9901] ##*/
  volatile TU8* loc_pu8TaskEvent = NULL;
  
  // Calculate address of Task Events Bits field to modify
  loc_pu8TaskEvent = &(me->asTasks[eTaskIdArg].u8Event);
  
  // No Interruption from now
  SFY_OS_CRITICAL_SECTION_ENTRY();
  
  // Reset Events
  *loc_pu8TaskEvent &= ~u8EventsArg;
  
  // Interruptions accepted now
  SFY_OS_CRITICAL_SECTION_EXIT();
   
  // Return OS_SUCCESS  
  return OS_SUCCESS;      
  /*## END[GUID ca418615-d89b-4d0e-8422-ac70021b9901] ##*/
}


/*!************************************************************************************************
* \fn         OS_RETURN SFY_OS_MessageAssociateToEvent(TeMessageBox eMsgBoxIdArg, TeTask eTaskIdArg, TU8 u8EventArg)
* \param[in]  eMsgBoxIdArg
* \param[in]  eTaskIdArg
* \param[in]  u8EventArg
* \return     OS_RETURN 
***************************************************************************************************/
OS_RETURN SFY_OS_MessageAssociateToEvent(TeMessageBox eMsgBoxIdArg, TeTask eTaskIdArg, TU8 u8EventArg)
{  
  /*## BEGIN[GUID c98131d6-665c-4bb4-804f-04f95195a566] ##*/
  #ifdef YES_I_AM_USING_MESSAGE_BOXES 
  
  TsMessageBox * loc_psMsgBox = &me->asMsgBoxes[eMsgBoxIdArg];
  
  // Create association
  loc_psMsgBox->eAssociatedTask   = eTaskIdArg;
  loc_psMsgBox->u8AssociatedEvent = u8EventArg;     
  
  // Return status
  return OS_SUCCESS;
  
  #else
  
  // Return status
  return OS_ERROR;
  
  #endif      
  /*## END[GUID c98131d6-665c-4bb4-804f-04f95195a566] ##*/
}


/*!************************************************************************************************
* \fn         OS_RETURN SFY_OS_MessageSend(TeMessageBox eMsgBoxIdArg, void * pMessageArg)
* \param[in]  eMsgBoxIdArg
* \param[in]  pMessageArg
* \return     OS_RETURN 
***************************************************************************************************/
OS_RETURN SFY_OS_MessageSend(TeMessageBox eMsgBoxIdArg, void * pMessageArg)
{  
  /*## BEGIN[GUID 3f4df7ef-3bc3-4bdc-9052-d89e3f034118] ##*/
  #ifdef YES_I_AM_USING_MESSAGE_BOXES
  
  // Local variables
  OS_RETURN loc_eRetVal = OS_ERROR;
  TsMessage *loc_psMessage;
  
  // Get the current message
  loc_psMessage = &me->asMessages[me->u8MessagePosition];
  
  // IF Message queue IS NOT full
  if (me->u8MessagePosition != OS_MAXIMUM_NUMBER_OF_MESSAGES)
  {
    volatile TU8* loc_pu8TaskEvent = NULL;
   	TsMessageBox* loc_psMessageBox; 
  	TU8 loc_u8AssociatedEvent;
   
  	// THEN, there is at least 1 place
    // Save message	                    
    loc_psMessage->eAssociatedBox = eMsgBoxIdArg;
    loc_psMessage->pJoinedPiece   = pMessageArg;
    
    // Update flags             
    me->u8MessagePosition++;
  
    // Calculate address of Task Msg Box on which we work
    loc_psMessageBox = &(me->asMsgBoxes[eMsgBoxIdArg]);
  	
    // Calculate address of Task Events Bits field to modify
    loc_pu8TaskEvent = &(me->asTasks[loc_psMessageBox->eAssociatedTask].u8Event);
  	
    // Get the Task Msg Box associated Event 
  	loc_u8AssociatedEvent = loc_psMessageBox->u8AssociatedEvent;
    
    // No Interruption from now
    SFY_OS_CRITICAL_SECTION_ENTRY();
  
    // Signal to task
    *loc_pu8TaskEvent |= loc_u8AssociatedEvent;
  	
    // Interruptions accepted now
    SFY_OS_CRITICAL_SECTION_EXIT();
    
    loc_eRetVal = OS_SUCCESS;	
  }
  
  // Return status
  //--------------
  return loc_eRetVal;
  
  #else
  
  return OS_ERROR;
  
  #endif      
  /*## END[GUID 3f4df7ef-3bc3-4bdc-9052-d89e3f034118] ##*/
}


/*!************************************************************************************************
* \fn         void * SFY_OS_MessageGet(TeMessageBox eMsgBoxIdArg)
* \param[in]  eMsgBoxIdArg
* \return     void * 
***************************************************************************************************/
void * SFY_OS_MessageGet(TeMessageBox eMsgBoxIdArg)
{  
  /*## BEGIN[GUID 8385f2b2-5cbc-41f0-879e-743e1a1a53b4] ##*/
  #ifdef YES_I_AM_USING_MESSAGE_BOXES
  
  TU8 loc_u8Counter = 0U; 
  void * loc_pRetVal = NULL;  
  TsMessage * loc_psMessage;
  
  // Look for Message bor that MBox
  for (loc_u8Counter=0 ; loc_u8Counter<me->u8MessagePosition ; loc_u8Counter++)
  {                 
    // IF it the needed Message BOX
    if (me->asMessages[loc_u8Counter].eAssociatedBox == eMsgBoxIdArg)
    {
      // THEN Get the joined piece
      loc_pRetVal = me->asMessages[loc_u8Counter].pJoinedPiece;
  
      // stop for loop
      break;
    }
  }      
    
  // IF it is not the last message
  if (loc_u8Counter < me->u8MessagePosition)
  {        
    // THEN shift the next message with one position less
    for ( ; loc_u8Counter < (me->u8MessagePosition-1) ;loc_u8Counter++)
    {   
      loc_psMessage = &me->asMessages[loc_u8Counter];
      *loc_psMessage = *(loc_psMessage+1);
    } 
    
    me->u8MessagePosition--;   
  } 
  
  // Return result
  return loc_pRetVal;
  
  #else
  
  return NULL;
  
  #endif      
  /*## END[GUID 8385f2b2-5cbc-41f0-879e-743e1a1a53b4] ##*/
}


/*!************************************************************************************************
* \fn         OS_RETURN SFY_OS_TimerCreate(TeTimer eTimerIdArg, EventSignalMethod SignalMethodArg, TeTask eTaskIdArg, TU8 u8EventArg)
* \param[in]  eTimerIdArg
* \param[in]  SignalMethodArg
* \param[in]  eTaskIdArg
* \param[in]  u8EventArg
* \return     OS_RETURN 
***************************************************************************************************/
OS_RETURN SFY_OS_TimerCreate(TeTimer eTimerIdArg, EventSignalMethod SignalMethodArg, TeTask eTaskIdArg, TU8 u8EventArg)
{  
  /*## BEGIN[GUID 3a4e065d-a641-452a-be0e-eff73a2e3ab1] ##*/
  #ifdef YES_I_AM_USING_TIMERS     
  
  TsTimer *loc_psTimer = &me->asTimers[eTimerIdArg];
  // Create Timer
  
  loc_psTimer->eAssociatedTask = eTaskIdArg;
  loc_psTimer->u8EventToSet = u8EventArg;
  
  // Return result
  return OS_SUCCESS;
  
  #else
  
  // Return result
  return OS_ERROR;
  
  #endif      
  /*## END[GUID 3a4e065d-a641-452a-be0e-eff73a2e3ab1] ##*/
}


/*!************************************************************************************************
* \fn         OS_RETURN SFY_OS_TimerStart(TeTimer eTimerIdArg, TU16 u16InitialValueArg, TU16 u16CyclicValueArg)
* \param[in]  eTimerIdArg
* \param[in]  u16InitialValueArg
* \param[in]  u16CyclicValueArg
* \return     OS_RETURN 
***************************************************************************************************/
OS_RETURN SFY_OS_TimerStart(TeTimer eTimerIdArg, TU16 u16InitialValueArg, TU16 u16CyclicValueArg)
{  
  /*## BEGIN[GUID 7f8f60fe-6cec-4bdd-bca5-e33603bb823d] ##*/
  #ifdef YES_I_AM_USING_TIMERS
  
  TsTimer *loc_psTimer = &me->asTimers[eTimerIdArg];
  
  // Start Timer
  loc_psTimer->u16Value = u16InitialValueArg;
  loc_psTimer->u16CyclicValue = u16CyclicValueArg;
  loc_psTimer->bRunning = TRUE;         
  
  // Return result
  return OS_SUCCESS;
  
  #else
  
  // Return result
  return OS_ERROR;
  
  #endif      
  /*## END[GUID 7f8f60fe-6cec-4bdd-bca5-e33603bb823d] ##*/
}


/*!************************************************************************************************
* \fn         OS_RETURN SFY_OS_TimerRestart(TeTimer eTimerIdArg)
* \param[in]  eTimerIdArg
* \return     OS_RETURN 
***************************************************************************************************/
OS_RETURN SFY_OS_TimerRestart(TeTimer eTimerIdArg)
{  
  /*## BEGIN[GUID c43d1323-1c7e-4549-ac4c-a7b1747726ce] ##*/
  #ifdef YES_I_AM_USING_TIMERS
  
  // Start Timer
  me->asTimers[eTimerIdArg].bRunning = TRUE;           
  
  // Return result
  return OS_SUCCESS;  
  
  #else
  
  return OS_ERROR;
  
  #endif      
  /*## END[GUID c43d1323-1c7e-4549-ac4c-a7b1747726ce] ##*/
}


/*!************************************************************************************************
* \fn         OS_RETURN SFY_OS_TimerStop(TeTimer eTimerIdArg)
* \param[in]  eTimerIdArg
* \return     OS_RETURN 
***************************************************************************************************/
OS_RETURN SFY_OS_TimerStop(TeTimer eTimerIdArg)
{  
  /*## BEGIN[GUID 50981c71-5063-4ee2-bfcd-e78cae65c642] ##*/
  #ifdef YES_I_AM_USING_TIMERS
  
  // Stop Timer
  me->asTimers[eTimerIdArg].bRunning = FALSE;
  
  // Return result
  return OS_SUCCESS; 
  
  #else
  
  // Return result
  return OS_ERROR;
  
  #endif      
  /*## END[GUID 50981c71-5063-4ee2-bfcd-e78cae65c642] ##*/
}


/*!************************************************************************************************
* \fn         OS_RETURN SFY_OS_TimerNewCyclicValue(TeTimer eTimerIdArg, TU16 u16CyclicValueArg)
* \param[in]  eTimerIdArg
* \param[in]  u16CyclicValueArg
* \return     OS_RETURN 
***************************************************************************************************/
OS_RETURN SFY_OS_TimerNewCyclicValue(TeTimer eTimerIdArg, TU16 u16CyclicValueArg)
{  
  /*## BEGIN[GUID 113b49f3-cbef-4fc8-9cef-7b4485c7c030] ##*/
  #ifdef YES_I_AM_USING_TIMERS
  
  // Set the new cyclic Timer value
  me->asTimers[eTimerIdArg].u16CyclicValue = u16CyclicValueArg;
  
  // Return result
  return OS_SUCCESS;         
  
  #else
  
  // Return result
  return OS_ERROR;
  
  #endif      
  /*## END[GUID 113b49f3-cbef-4fc8-9cef-7b4485c7c030] ##*/
}


// Private functions implementation
//------------------------------------


/*!************************************************************************************************
* \fn         static void SFY_OS_PRV_SetDefaultValue(void)
* \return     void 
***************************************************************************************************/
static void SFY_OS_PRV_SetDefaultValue(void)
{  
  // Set the eCurrentTask attribute default value
  me->eCurrentTask = TIMER_TASK;
  // Set the u8MessagePosition attribute default value
  #ifdef YES_I_AM_USING_MESSAGE_BOXES
  me->u8MessagePosition = 0U;
  #endif // YES_I_AM_USING_MESSAGE_BOXES
  // Set the u16TaskWaitingCounter attribute default value
  me->u16TaskWaitingCounter = 0U;
}


/*!************************************************************************************************
* \brief      Interrupt routine that must be called to update the OS Tick
* \fn         static void SFY_OS_PRV_Schedule(void)
* \return     void 
***************************************************************************************************/
static void SFY_OS_PRV_Schedule(void)
{  
  /*## BEGIN[GUID 50b2c95a-7687-4c1f-990e-96b0e3790615] ##*/
  // Local variable
  //---------------  
  #ifdef YES_I_AM_USING_TIMERS
  TU8 loc_u8Counter;
  TsTimer * loc_psTimer;
  TU8 loc_u8EventToSet = 0U;
  volatile TU8*  loc_pu8TaskEvent = 0U;
  #endif
        
  // Decrease each Running Timer
  #ifdef YES_I_AM_USING_TIMERS
  for (loc_u8Counter=0 ; loc_u8Counter<OS_TIMERS_NUMBER ; loc_u8Counter++)
  {               
    // Lock Timer
    loc_psTimer = &me->asTimers[loc_u8Counter];
    
    // Test timer status
    if (loc_psTimer->bRunning == TRUE)
    {
      // Decrease value
      loc_psTimer->u16Value--;
      
      // Test if elapsed
      if (loc_psTimer->u16Value == 0)
      {
        loc_u8EventToSet = loc_psTimer->u8EventToSet;
        loc_pu8TaskEvent = &( me->asTasks[loc_psTimer->eAssociatedTask].u8Event);
  
        // No Interruption from now
        SFY_OS_CRITICAL_SECTION_ENTRY();
  		
          // Signal u8Event
        *loc_pu8TaskEvent |= loc_u8EventToSet;
  
        // Interruptions accepted now
        SFY_OS_CRITICAL_SECTION_EXIT();
  
        // Rearm timer ?
        if (loc_psTimer->u16CyclicValue == 0)
        {
          // Stop Timer
          loc_psTimer->bRunning = FALSE;
        }
        else
        {
          // Rearm
          loc_psTimer->u16Value = loc_psTimer->u16CyclicValue;
        }
      }
    }
  }
  #endif    
  
  // No Interruption from now
  SFY_OS_CRITICAL_SECTION_ENTRY();
  
  // Consume 1 u8Event  
  //----------------
  me->asTasks[TIMER_TASK].u8Event--;
  
  // Interruptions accepted now
  SFY_OS_CRITICAL_SECTION_EXIT();
  /*## END[GUID 50b2c95a-7687-4c1f-990e-96b0e3790615] ##*/
}

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
