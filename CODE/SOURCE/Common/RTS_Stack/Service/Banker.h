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
/ ********************************************************************* 
/ AUTHOR : Florian GERMAIN
/ DATE : 30/10/2006
/ ********************************************************************* */

#ifndef _BANKER_H_
#define _BANKER_H_

// I N T E R F A C E S
//
#include "Config.h"

// S T R U C T U R E S
// 


// C O N S T A N T S
//
//! State of the bank.
enum
{
   
  BANK_MODE_MEM_OPENED,        //! Bank is opened and ready to record or delete a RemoteControl.
  BANK_MODE_MEM_CLOSED,        //! Bank is closed, only possible action is Open.
  BANK_MODE_NO_REMOTE,         //! Default state.
  BANK_MODE_TEMPORARY_MODE     //! Only one remote control recorded temporary.
};

// C L A S S   M E T H O D S ( P U B L I C   P R O T O T Y P E S )
//

void  Banker_ApplyFactorySettings(void);//³õÊ¼»¯
void  Banker_Create(void);//Ã»ÓÃµ½
char Banker_DeleteTheCurrentRemote(void);//É¾³ýµ±Ç°Ò£¿ØÆ÷
char Banker_DeleteTheCurrentSensor(void);//ST30ÐèÒªÓÃµ½£¬²ÎÕÕÉÏÍ¼
void  Banker_DeleteOnlyRemotes(void);
void  Banker_DeleteOnlySensors(void);
void  Banker_DeleteSensorsAndRemotes(void);
TU8 Banker_GetSensorId(void);
void  Banker_InitClass(void);
void  Banker_OpenBank(TU16 wDurationSec);
void  Banker_PowerFailDefinitely(void);
void  Banker_ReplaceAllRemotes(void);
TBool Banker_RecordTheCurrentRemote(void);//¼ÇÂ¼µ±Ç°Ò£¿ØÆ÷
void  Banker_RecordTheCurrentRemoteAsTemporary(void);
TBool Banker_RecordTheCurrentSensor(void); //¼ÇÂ¼µ±Ç°sensor
void  Banker_Restore(void);
void  Banker_Save(void);
void  Banker_TimeElapsed(void);
TU8 Banker_WhatIsYourState(void);



#endif /* _BANKER_H_ */
