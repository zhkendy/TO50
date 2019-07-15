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

#ifndef _TRANSCEIVER_BANK_H_
#define _TRANSCEIVER_BANK_H_

// I N T E R F A C E S
//
#include "Config.h"

// S T R U C T U R E S
// 
//! Address buffer.
typedef struct
{
  TU8 btAddress[3];
}TAdresseU56;

//! Structure of a sensor. 
typedef struct
{
  //! Address of the sensor.
  TAdresseU56 Address;
  //! Type of the sensor.
  TU8 btType; 
}TSensor;

//! Persistent datas of remote which are saved
typedef struct
{
  //! Address of the remote.
  TAdresseU56 Address; 
  //! Type of the remote.
  TU8 btType;
  //! Rolling code.
  TU16 wRollingCodeSaved; 
}TRemoteControlPersistent;

//! Volatiles data of Remote which are not saved.
typedef struct
{
  //! Last rolling code.
  TU16 wLastRollingCode;
}TRemoteControlVolatile;

//! Structure of a remote. 
typedef struct
{
  TRemoteControlPersistent RemoteControlPersistent;
  TRemoteControlVolatile   RemoteControlVolatile;
}TRemoteControl;

// C O N S T A N T S
//
//! Number max of sensors.
#define SENSOR_NBR                      (4u)    //sage should be changed to 0u  Wirefree motors are not compatible with sensors.
//! Number max of remotes.
#define REGISTERED_REMOTE_CONTROL_NBR   (12u)   //sage change to 12u  

//! Index of the current remote.
#define INDEX_CURRENT_REMOTE            (0u)
//! Index of the temporary remote.
#define INDEX_TEMPORARY_REMOTE          (1u)
//! Index of the first remote.
#define INDEX_FIRST_REMOTE_RECORDED     (2u)
//! Index of the last remote.
#define INDEX_LAST_REMOTE_RECORDED      (INDEX_FIRST_REMOTE_RECORDED + REGISTERED_REMOTE_CONTROL_NBR - 1u)//2+12-1=13
//! Index of the first sensor.
#define INDEX_FIRST_SENSOR_RECORDED     (INDEX_LAST_REMOTE_RECORDED + 1u)     //13+1 
//! Index of the last sensor.
#define INDEX_LAST_SENSOR_RECORDED      (INDEX_FIRST_SENSOR_RECORDED + SENSOR_NBR - 1u)//16
 
//! Total number of transceivers.
#define TOTAL_TRANSCEIVER_NBR           (REGISTERED_REMOTE_CONTROL_NBR + SENSOR_NBR + INDEX_FIRST_REMOTE_RECORDED)//12+4+2=18
//! Index of the first transceiver.
#define FIRST_INDEX                     (INDEX_CURRENT_REMOTE)//0
//! Index of the last transceiver.
#define LAST_INDEX                      (INDEX_LAST_SENSOR_RECORDED) //16


// C L A S S   M E T H O D S ( P U B L I C   P R O T O T Y P E S )
//
void TransceiverBank_AddCurrentTransceiver(TU8 btType,TU16 wRollingCode,TAdresseU56 Address);
void TransceiverBank_CopyTransceiver(TU8 btIndexSrc,TU8 btIndexDest);
void TransceiverBank_Create(void);
void TransceiverBank_PowerFailDefinitely(void);//TransceiverBank_RemoveTransceiver(INDEX_TEMPORARY_REMOTE);
TU8 TransceiverBank_GiveIndexCurrent(void);//// Search current location in the bank
TBool TransceiverBank_IsThisIndexEmpty(TU8 btIndex);
TU16 TransceiverBank_ReadLastRollingCode(TU8 btIndex);//return  Its rolling code, 0 if a sensor.
TU16 TransceiverBank_ReadRollingCodeSaved(TU8 btIndex);
void TransceiverBank_RemoveTransceiver(TU8 btIndex);
void TransceiverBank_Restore(void);
void TransceiverBank_Save(void);
TBool TransceiverBank_WriteLastRollingCode(TU8 btIndex,TU16 wRollingCode);
TBool TransceiverBank_WriteRollingCodeSaved(TU8 btIndex,TU16 wRollingCode);
TRemoteControl*  TransceiverBank_GetRemoteControlBank(TU8 index);

// C L A S S   I N S T A N C E S
//

#endif /* _TRANSCEIVER_BANK_H_ */
