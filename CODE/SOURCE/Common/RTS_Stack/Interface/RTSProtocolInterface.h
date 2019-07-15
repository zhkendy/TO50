/* *********************************************************************
/ This document and/or file is SOMFY’s property. All information
/ it contains is strictly confidential. This document and/or file
/ shall not be used, reproduced or passed on in any way, in full
/ or in part without SOMFY’s prior written approval.
/ All rights reserved.
/ Ce document et/ou fichier est la propriété de SOMFY.
/ Les informations qu’il contient sont strictement confidentielles.
/ Toute reproduction, utilisation, transmission de ce document
/ et/ou fichier, partielle ou intégrale, non autorisée
/ préalablement par SOMFY par écrit est interdite.
/ Tous droits réservés.
/ ***********************************************************************
/ CLASS HEADER FILE
/ ***********************************************************************
/ NAME :
/ BASE CLASS :
/ DESCRIPTION :
/
/ *********************************************************************
/ AUTHOR : Florian GERMAIN
/ DATE : 12/10/2006
/ Modify by Mula Gabriel 04/2015
/ ********************************************************************* */

#include "SFY_OS.h"
#include "Counter.h"
#include "RTSProtocol.h"
#include "RTSReceiver.h"
//#include "STM8_RSSI_LowPower_Interface.h"
#include "RTS_Ergonomic_Manager.h" //RTSProtocolListener_ProcessStimuli
#include "Controller.h"

//! Interface with OS
#define RTSProtocol_SendEventProcessFrame()\
{\
 SFY_OS_EventSignalFromInterruption( OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_RTS, TASK_RTS_FRAME_RX_EVENT);\
  Controller_RTSActivityNotify();\
}

//! Interface with RTSProtocolListener
#define RTSProtocolListener_ProcessStimuli(ptStimuli) \
ErgonomicManager_TranslateStimulus(ptStimuli)



#define RTSProtocolListener_BankClosed()
              // Not used by current RTSOrderProvider

// Called each time a persitent data has been changed in order to signal that a save has to be done
#define RTSProtocolListener_PersistenDataHaveChanged()\
              SFY_OS_EventSignal( OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_RTS, TASK_RTS_DATA_CHANGE)

//! Interface with RTSReceiver
#define RTSProtocolRTSReceiverListener_Create() \
              RTSReceiver_Create()

#define RTSProtocolRTSReceiverListener_Enable() \
              RTSReceiver_Enable()

#define RTSProtocolRTSReceiverListener_Disable() \
              RTSReceiver_Disable()

//! Interface with the OS Timer
#define RTSProtocolShortCounter_NotifyMeAfter(wDurationMs) \
              SFY_OS_TimerStart(RTSPROTOCOL_TIMER_ID, wDurationMs, 0)

#define RTSProtocolShortCounter_StopCounting() \
              SFY_OS_TimerStop(RTSPROTOCOL_TIMER_ID)


//! Interface with Counter
#define BankerLongCounter_Create() 
#define BankerLongCounter_StopCounting() 
#define BankerLongCounter_NotifyMeAfter(wDurationSec) 

////! Interface with Counter
//#define BankerLongCounter_Create() \
//              Counter_Create(BANKER_LONGCOUNTER_ID,Banker_TimeElapsed)
//
//#define BankerLongCounter_StopCounting() \
//              Counter_StopCounting(BANKER_LONGCOUNTER_ID)
//
//#define BankerLongCounter_NotifyMeAfter(wDurationSec) \
//              Counter_NotifyMeAfter(BANKER_LONGCOUNTER_ID, wDurationSec)

//! Interface with OS
#define RTSProtocolListener_K_OS_Save_Interrupts() \
__disable_interrupt()

#define RTSProtocolListener_K_OS_Restore_Interrupts() \
__enable_interrupt()

// Value=100ms , we check if 16bits are received after this duration
#define DURATION_FOR_RX_CHECKING                       (OS_TIME_100_MS)
// Value=54ms, max duration between 2 frames = 140ms+10% = 154ms, -100ms = 54ms
#define TIME_TO_WAIT_BETWEEN_RX_CHECK_AND_NEW_FRAME    OS_TIME_60_MS

