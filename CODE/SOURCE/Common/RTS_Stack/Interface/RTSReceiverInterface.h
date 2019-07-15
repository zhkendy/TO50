/* *********************************************************************
/ This document and/or file is SOMFY�s property. All information
/ it contains is strictly confidential. This document and/or file
/ shall not be used, reproduced or passed on in any way, in full
/ or in part without SOMFY�s prior written approval.
/ All rights reserved.
/ Ce document et/ou fichier est la propri�t� de SOMFY.
/ Les informations qu�il contient sont strictement confidentielles.
/ Toute reproduction, utilisation, transmission de ce document
/ et/ou fichier, partielle ou int�grale, non autoris�e
/ pr�alablement par SOMFY par �crit est interdite.
/ Tous droits r�serv�s.
/ ***********************************************************************
/ CLASS HEADER FILE
/ ***********************************************************************
/ NAME :
/ BASE CLASS :
/ DESCRIPTION :
/ *********************************************************************
/ AUTHOR : FlG
/ DATE :11/08/2006
/ ********************************************************************* */

#pragma once

// ****************************************************************************
// EXTERNAL HEADERS
// ****************************************************************************

#include "RTSProtocol.h"
#include "Pcb.h"
#include "SFY_OS.h"
#include "Controller.h"

// ****************************************************************************
// MACRO
// ****************************************************************************



// Interface with RTSProtocol
#define RTSReceiverListener_ProcessFrame(frame)\
RTSProtocol_ProcessFrame(frame)


#define RTSReceiverListenerIM_RF_FRAME_Activate()\
Controller_RTSActivityNotify()

#define RTSReceiverListenerIM_RF_FRAME_Deactivate()

#define RTSReceiverListenerCS_RF_Activate() 	\
GPIO_WriteHigh(Radio_ChipSelect_PORT, Radio_ChipSelect_PIN)
						
#define RTSReceiverListenerCS_RF_Deactivate() 	\
GPIO_WriteLow(Radio_ChipSelect_PORT, Radio_ChipSelect_PIN)


// Interface with OS
#define RTSReceiverListener_Restore_Interrupts() \
enableInterrupts()

#define RTSReceiverListener_Save_Interrupts() \
disableInterrupts()

// GPIO
#define RTSReceiverListenerINPUT_CAPTURE_RADIO_PORT  INPUT_CAPTURE_RADIO1_PORT
#define RTSReceiverListenerINPUT_CAPTURE_RADIO_BIT   INPUT_CAPTURE_RADIO1_BIT

