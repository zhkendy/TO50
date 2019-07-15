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
/ ********************************************************************* */

#ifndef _RTSPROTOCOL_H_
#define _RTSPROTOCOL_H_

// I N T E R F A C E S
//
#include "Config.h"
#include "RtsFrame.h"
#include "TransceiverBank.h"

// O B J E C T   S T R U C T U R E
//
//! Structure of a RTS stimuli.
typedef struct
{
  //! Type of transceiver : 3button, 1button, sensor...
  TU8 btTypeOfTransceiver;
  //! State of the transceiver : recorded, temporary ...
  TU8 btStateOfTransceiver;
  //! Flow in progress or end of flow.
  TU8 btStateOfCommand;
  //! Stimuli number.
  TU8 btStimuliDuration;
  //! The command received.
  TU8 btCommand1;
  //! Some command need a parameter.
  TU8 btCommand2;
  //! Some command need a parameter.
  TU8 btCommand3;
  //! Some command need a parameter.
  TU8 btCommand4;
}TStimuli;


// P U B L I C   C O N S T A N T S
//

//! Type of transceiver which send the stimuli.
enum
{
  RTSPROT_RC_UNKNOWN = 0,                  //!< Default value
  RTSPROT_RC_3BUTTON = 1,                  //!< Classic Remote control
  RTSPROT_RC_1BUTTON = 2,                  //!< Cycle 4 times remote controls
  RTSPROT_RC_SPRING2BUTTON = 3,             //  Spring 2.5 entry range remote
  RTSPROT_SENSOR = 4,                      //!< Sensor
  RTSPROT_FACTORY_CRYPTED = 5,             //!< Industrial crypted frame
  RTSPROT_FACTORY_UNCRYPTED = 6,           //!< industrial uncrypted frame
  RTSPROT_RC_RTDS = 7                      //!< RTDS frame
};
//! State of transceiver.
enum
{
  RTSPROT_UNRECORDED=0,                  //!< remote control or sensor is not recorded in the bank
  RTSPROT_RECORDED=1,                    //!< RC or sensor is recorded in the bank
  RTSPROT_TEMPORARY=2                    //!< RC or sensor is recoded as temporary transceiver
};
// State of the command
//! Command is activated if this bit is set in btStateOfCommand.
#define BIT_FLOW_IN_PROGRESS    0
//! The transceiver which send this stimuli is a nex transceiver if this bit is set in btStateOfCommand.
#define BIT_NEW_TRANSCEIVER     1
#define MASK_FLOW_IN_PROGRESS   (1<<BIT_FLOW_IN_PROGRESS)
#define MASK_NEW_TRANSCEIVER    (1<<BIT_NEW_TRANSCEIVER)

// C L A S S   M E T H O D S ( P U B L I C   P R O T O T Y P E S )
//

void RTSProtocol_Create(void);
void RTSProtocol_Disable(void);
void RTSProtocol_Enable(void);
void RTSProtocol_EventProcessFrame(void);     // If polyspace is defined we don't use the event
TBool RTSProtocol_IsBankEmpty(void);
void RTSProtocol_ProcessFrame(TRTSFrame * ptFrame);
void RTSProtocol_TimeElapsed(void);



// C L A S S   I N S T A N C E S
//

#endif /* _RTSPROTOCOL_H_ */
