#ifndef __DO_NOT_COMPILE_THIS__
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
/ CLASS DEFINITION FILE
/ ***********************************************************************
/ ASSOCIATED HEADER FILE :
/ ***********************************************************************
/ AUTHOR : Florian GERMAIN
/ DATE :12/10/2006
/ DESCRIPTION :
/ ********************************************************************* */
#ifdef __DEBUG_TARGET__
  //#define DBG_THIS_CLASS        // define to debug this class
#endif // ifdef GLOBAL_DEBUG

// E X T E R N A L   D E F I N I T I O N S
//
#include <stdio.h>
#include "Config.h"
#include <string.h>


// I N T E R F A C E S
//
#include "Banker.h"
#include "RTSProtocolInterface.h"
#ifdef DBG_THIS_CLASS
  #include "TestInterface.h"
#endif // #ifdef DBG_THIS_CLASS

// C O N S T A N T S
//

// S T A T E   M A C H I N E ( S )
//

/*-- MACROS FOR DEBUG --*/
#ifdef BANKER_DEBUG
  #define BIRTH_OBJECT_BANKER\
    PutString("<o n=\"&bkr;\"></o>");
#else//BANKER_DEBUG
  #define BIRTH_OBJECT_BANKER
#endif//BANKER_DEBUG

#ifdef BANKER_DEBUG
  #define  START_DEBUG_DISPLAY_BANKER(text)\
    PutString("<m o=\"&bkr;\" n=\"&" #text ";\"><st st=\"&en;\">");\
    BANKER_PRINT_STATE()\
    PutString("</st>");
#else // BANKER_DEBUG
  #define  START_DEBUG_DISPLAY_BANKER(text)
#endif // BANKER_DEBUG

#ifdef BANKER_DEBUG
  #define  END_DEBUG_DISPLAY_BANKER\
    PutString("<st st=\"&ex;\">");\
    BANKER_PRINT_STATE()\
    PutString("</st></m>");
#else // BANKER_DEBUG
  #define  END_DEBUG_DISPLAY_BANKER
#endif // BANKER_DEBUG

#ifdef BANKER_DEBUG
  #define DEBUG_DISPLAY_STRING_BANKER(text)\
    PutString("{BKR "#text"}");
#else // BANKER_DEBUG
  #define DEBUG_DISPLAY_STRING_BANKER(text)
#endif // BANKER_DEBUG

#ifdef BANKER_DEBUG
  #define DEBUG_DISPLAY_BYTE_BANKER(byteToDisp)\
    PutHex(byteToDisp);\
    PutChar(' ');
  #else // BANKER_DEBUG
    #define DEBUG_DISPLAY_BYTE_BANKER(byteToDisp)
#endif // BANKER_DEBUG

#ifdef BANKER_DEBUG
  #define BANKER_PRINT_STATE()\
    Banker_PrintState();
#else // BANKER_DEBUG
  #define BANKER_PRINT_STATE()
#endif // BANKER_DEBUG

#ifdef BANKER_DEBUG
  void  Banker_PrintState( void );
#endif//BANKER_DEBUG

// V A R I A B L E S
//! Context of the bank : OPENED, CLOSED, TEMPORARY.
TU8 Banker_Context;
//! Index in the bank of the opener.
TU8 Banker_IndexOpener;

TU8 debug_btNbrOfRCRecorded;//kendy 
TU8 debug_btIndex;//kendy 
// F U N C T I O N S / M E T H O D S
//
//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_Create(void)
 * \brief   Creator of banker class.
 *
 */
void Banker_Create(void)
{
  BIRTH_OBJECT_BANKER
  BankerLongCounter_Create();
  Banker_InitClass();
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_InitClass(void)
 * \brief   Init banker class according to the context of the bank.
 *
 */
void Banker_InitClass(void)
{
  TU8 i;

  START_DEBUG_DISPLAY_BANKER(incl)
  Banker_IndexOpener = INDEX_CURRENT_REMOTE;//ÏÖÔÚµÄÒ£¿ØÆ÷

  Banker_Context = BANK_MODE_NO_REMOTE;           // Bank is empty
  for (i=INDEX_FIRST_REMOTE_RECORDED;i<=INDEX_LAST_SENSOR_RECORDED;i++)//clear remote boXs
  {
    if (TransceiverBank_IsThisIndexEmpty(i)==FALSE)   // One is not empty
    {
      Banker_Context = BANK_MODE_MEM_CLOSED;                    // Bank is closed
      BankerLongCounter_StopCounting();
    }
  }
  END_DEBUG_DISPLAY_BANKER
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_Save(void)
 * \brief   Ask the bank to save its persistent data.
 *
 */
void Banker_Save(void)
{
  TransceiverBank_Save();

}

//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_Restore(void)
 * \brief   Ask the bank to restore its data and Banker state is updated
 *
 */
void Banker_Restore(void)
{
  TransceiverBank_Restore();
  Banker_InitClass();
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_ApplyFactorySettings(void)
 * \brief   Ask the bank to restore factory settings : delete all remote controls.
 *
 */
void Banker_ApplyFactorySettings(void)
{
  START_DEBUG_DISPLAY_BANKER(fs)
  Banker_DeleteSensorsAndRemotes();
  Banker_Save();
  Banker_InitClass();
  END_DEBUG_DISPLAY_BANKER
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_PowerFailDefinitely(void)
 * \brief   A real power cut has occured.
 *
 */
void Banker_PowerFailDefinitely(void)
{
  TransceiverBank_PowerFailDefinitely();
  Banker_InitClass();
}

//_________________________________________________________________________________________________
/*!
 * \fn      TU8 Banker_WhatIsYourState(void)
 * \brief   Return the state of the bank.
 * \return  TU8 : BANKER state OPENED_BANK, CLOSED_BANK, EMPTY_BANK...
 *
 */
TU8 Banker_WhatIsYourState(void)
{
  START_DEBUG_DISPLAY_BANKER(wct)
  END_DEBUG_DISPLAY_BANKER
  return (Banker_Context);
}

//_________________________________________________________________________________________________
/*!
 * \fn      TBool Banker_RecordTheCurrentRemote(void)
 * \brief   Try to record the current transceiver.
 * \return  TBool : TRUE if recording is possible, FALSE if not.
 *
 */
TBool Banker_RecordTheCurrentRemote(void)
{
  TU8 btIndex,btOneFree;
  TBool btReturn;


  START_DEBUG_DISPLAY_BANKER(rrc)
  btReturn = FALSE;
  btOneFree = FALSE;
  btIndex=INDEX_FIRST_REMOTE_RECORDED;
  while ((btIndex <= INDEX_LAST_REMOTE_RECORDED)&&(btOneFree == FALSE))  // Find the first free
  {
    if (TransceiverBank_IsThisIndexEmpty(btIndex)==TRUE)
    {
      btOneFree = TRUE;
    }
    else
    {
      btIndex++;
    }
  }

  if ( Banker_Context == BANK_MODE_TEMPORARY_MODE )     // Record Temporary in the bank
  {
    if ( btOneFree == TRUE )
    {
      TransceiverBank_CopyTransceiver( INDEX_TEMPORARY_REMOTE,btIndex);           // temporary -> free location
      TransceiverBank_RemoveTransceiver(INDEX_TEMPORARY_REMOTE);                   // remove the temporary RC
      btReturn = TRUE;                                                        // Recording possible
    }
  }
  else                                        // Record Current in the bank
  {
    if ( btOneFree == TRUE )
    {
      TransceiverBank_CopyTransceiver( INDEX_CURRENT_REMOTE,btIndex);             // Current -> free location//kendy
      btReturn = TRUE;                                                        // Recording possible
    }
  }
  Banker_Context = BANK_MODE_MEM_CLOSED;        //! Bank is closed
  BankerLongCounter_StopCounting();    //! Stop opening window
  END_DEBUG_DISPLAY_BANKER
  return (btReturn);                  //! Return TRUE or FALSE if recording is possible or note
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_RecordTheCurrentRemoteAsTemporary(void)
 * \brief   Record the current transceiver as temporary transceiver
 *
 */
void Banker_RecordTheCurrentRemoteAsTemporary(void)
{
  START_DEBUG_DISPLAY_BANKER(rt)
  TransceiverBank_CopyTransceiver( INDEX_CURRENT_REMOTE,INDEX_TEMPORARY_REMOTE);           // current -> temporary
  Banker_Context = BANK_MODE_TEMPORARY_MODE;                                            // Bank is closed
  END_DEBUG_DISPLAY_BANKER
}

//_________________________________________________________________________________________________
/*!
 * \fn      char Banker_DeleteTheCurrentRemote(void)
 * \brief   ask to remove current transceiver
 *
 */
char Banker_DeleteTheCurrentRemote(void)
{
  TU8 btIndex;
  TU8 btNbrOfRCRecorded;
  START_DEBUG_DISPLAY_BANKER(drc)

  btNbrOfRCRecorded = 0U;

  
      for (btIndex = INDEX_FIRST_REMOTE_RECORDED;btIndex <= INDEX_LAST_REMOTE_RECORDED;btIndex++)
      {
        if (TransceiverBank_IsThisIndexEmpty(btIndex)==FALSE)//\return  TRUE if free, FALSE if full.
        {
          btNbrOfRCRecorded++;
        }
      }
      if(btNbrOfRCRecorded>1)
      {
          btIndex = TransceiverBank_GiveIndexCurrent();                                 // Search current location in the bank
          if (( btIndex >= INDEX_FIRST_REMOTE_RECORDED )                                // Index is corect
           && ( btIndex <= INDEX_LAST_REMOTE_RECORDED ) )
          {
            if ((Banker_IndexOpener != btIndex) && (btNbrOfRCRecorded >= 1))            // Opener can't be removed
            {
              TransceiverBank_RemoveTransceiver(btIndex);         // remove the RC
            }
          }
          if (Banker_Context == BANK_MODE_MEM_OPENED )
          {
            BankerLongCounter_StopCounting();
            Banker_Context = BANK_MODE_MEM_CLOSED;    // Bank is closed
          }
   
          return 0;
      }
     else
          return 1;
  

  END_DEBUG_DISPLAY_BANKER
}


//_________________________________________________________________________________________________
/*!
 * \fn      TU8 Banker_RecordTheCurrentSensor(void)
 * \brief   Try to record current sensor.
 * \return  TU8 : 1,2 or 3 (the index of the sensor) or 0 if bank is full.
 *
 */
TBool Banker_RecordTheCurrentSensor(void)
{
  TU8 btIndex,btOneFree;
  TU8 btSensorId = 0U;
  TU8 btNbrOfRCRecorded;
  START_DEBUG_DISPLAY_BANKER(rs)

  btNbrOfRCRecorded = 0U;
  for (btIndex = INDEX_FIRST_REMOTE_RECORDED;btIndex <= INDEX_LAST_REMOTE_RECORDED;btIndex++)
  {
    if (TransceiverBank_IsThisIndexEmpty(btIndex)==FALSE)
    {
      btNbrOfRCRecorded++;
    }
  }
  
  btOneFree = FALSE;
  btIndex=INDEX_FIRST_SENSOR_RECORDED;
  while ((btIndex <= INDEX_LAST_SENSOR_RECORDED)&&(btOneFree == FALSE))  //! Find the first free location
  {
    if (TransceiverBank_IsThisIndexEmpty(btIndex)==TRUE)
    {
      btOneFree = TRUE;
    }
    else
    {
      btIndex++;
    }
  }

  if ((btOneFree == TRUE) && (btNbrOfRCRecorded>0))                              //! if one free location in the bank
  {
     //debug_btNbrOfRCRecorded=btNbrOfRCRecorded;//kendy 
 //debug_btIndex=btIndex;//kendy 
    TransceiverBank_CopyTransceiver( INDEX_CURRENT_REMOTE, btIndex);        //! copy : current -> free location
    btSensorId = (btIndex-INDEX_FIRST_SENSOR_RECORDED+1);
  }
  BankerLongCounter_StopCounting();                          //! Stop opening window
  Banker_Context = BANK_MODE_MEM_CLOSED;                    //! Bank is closed
  END_DEBUG_DISPLAY_BANKER
  return (btSensorId);                            //! return sensor id in the bank : 1 2 or 3
}

//_________________________________________________________________________________________________
/*!
 * \fn      TU8 Banker_DeleteTheCurrentSensor(void)
 * \brief   Try to remove the current sensor.
 * \return  TU8 : the index (1,2,3) of the sensor removed, 0 if sensor doesn't exist.
 *
 */
char Banker_DeleteTheCurrentSensor(void)
{
  TU8 btIndex;
  TU8 btRetour;

  START_DEBUG_DISPLAY_BANKER(ds)
  btRetour = 0U;
  btIndex = TransceiverBank_GiveIndexCurrent();         // Search current location in the bank
  if (( btIndex >= INDEX_FIRST_SENSOR_RECORDED )        // Index is corect
   && ( btIndex <= INDEX_LAST_SENSOR_RECORDED ) )
  {
    TransceiverBank_RemoveTransceiver(btIndex);         // remove the Sensor
    btRetour = btIndex-INDEX_FIRST_SENSOR_RECORDED+1;   // Return sensor removed nbr
  }
  Banker_Context = BANK_MODE_MEM_CLOSED;                            // Bank is closed
  BankerLongCounter_StopCounting();
  END_DEBUG_DISPLAY_BANKER
  return (btRetour);                                               // Return 0
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_OpenBank(TU16 wDurationSec)
 * \brief   Open the bank for recording.
 * \param   wDurationSec : Duration of the opening in seconds.
 *
 */
void Banker_OpenBank(TU16 wDurationSec)
{
  START_DEBUG_DISPLAY_BANKER(obk)
  Banker_IndexOpener = TransceiverBank_GiveIndexCurrent();
  Banker_Context = BANK_MODE_MEM_OPENED;
  BankerLongCounter_NotifyMeAfter( wDurationSec );
  END_DEBUG_DISPLAY_BANKER
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_TimeElapsed (void)
 * \brief   Close the bank after recording.
 *
 */
void Banker_TimeElapsed (void)
{
  START_DEBUG_DISPLAY_BANKER(tiel)
  Banker_Context = BANK_MODE_MEM_CLOSED;      // Timeout elapsed the bank is closed
  RTSProtocolListener_BankClosed();           // In order to have a feedback
  Banker_IndexOpener = INDEX_CURRENT_REMOTE;      // remove index opener
  BankerLongCounter_StopCounting();
  END_DEBUG_DISPLAY_BANKER
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_DeleteOnlySensors(void)
 * \brief   Delete all sensors but not the remotes.
 *
 */
void Banker_DeleteOnlySensors(void)
{
  TU8 i;

  START_DEBUG_DISPLAY_BANKER(das)
  for (i=INDEX_FIRST_SENSOR_RECORDED;i<=INDEX_LAST_SENSOR_RECORDED;i++)
  {
    TransceiverBank_RemoveTransceiver(i);         // remove the SENSOR
  }
  BankerLongCounter_StopCounting();
  Banker_Context = BANK_MODE_MEM_CLOSED;    // Bank is closed
  END_DEBUG_DISPLAY_BANKER
}

//_________________________________________________________________________________________________
/*!
 * \fn      TU8 Banker_GetSensorId(void)
 * \brief   Give sensor index : 1,2,3 of the current sensor.
 * \return  TU8 : the index of the current sensor (1 to 3), 0 if sensor is not paired.
 *
 */
TU8 Banker_GetSensorId(void)
{
  TU8 btIndexCurrent;
  TU8 btSensorId;

  START_DEBUG_DISPLAY_BANKER(gsi)
  btSensorId = 0U;
  btIndexCurrent = TransceiverBank_GiveIndexCurrent();
  if (( btIndexCurrent >= INDEX_FIRST_SENSOR_RECORDED )
  && ( btIndexCurrent <= INDEX_LAST_SENSOR_RECORDED ))
  {
    btSensorId = (btIndexCurrent-INDEX_FIRST_SENSOR_RECORDED+1);
  }
  END_DEBUG_DISPLAY_BANKER
  return (btSensorId);
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_DeleteOnlyRemotes(void)
 * \brief   All remote are removed but not the sensors.
 *É¾³ýËùÓÐÒ£¿ØÆ÷£¬²»°üÀ¨´«¸ÐÆ÷
 */
void Banker_DeleteOnlyRemotes(void)
{
  TU8 i;

  TransceiverBank_RemoveTransceiver(INDEX_TEMPORARY_REMOTE);
  START_DEBUG_DISPLAY_BANKER(rar)
  for (i=INDEX_FIRST_REMOTE_RECORDED;i<=INDEX_LAST_REMOTE_RECORDED;i++)
  {
    TransceiverBank_RemoveTransceiver(i);         // remove the RC
  }
  BankerLongCounter_StopCounting();
  Banker_Context = BANK_MODE_NO_REMOTE;    // Bank is empty
  END_DEBUG_DISPLAY_BANKER
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_DeleteSensorsAndRemotes(void)
 * \brief   Ask the bank to delete sensors and remotes
 *É¾³ýËùÓÐ´«¸ÐÆ÷
 */
void Banker_DeleteSensorsAndRemotes(void)
{
  START_DEBUG_DISPLAY_BANKER(dsr)
  Banker_DeleteOnlySensors(); 		   // Remove all sensors  //kendy sensor5/30 change open
  Banker_DeleteOnlyRemotes();              // Remove all remotes
  Banker_Context = BANK_MODE_NO_REMOTE;    // Bank is empty
  END_DEBUG_DISPLAY_BANKER
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Banker_ReplaceAllRemotes(void)
 * \brief   Remove all remote control and current is added.
 *
 */
void Banker_ReplaceAllRemotes(void)
{
  TU8 i;
  START_DEBUG_DISPLAY_BANKER(rpar)

  for (i=INDEX_FIRST_REMOTE_RECORDED;i<=INDEX_LAST_REMOTE_RECORDED;i++)
  {
    TransceiverBank_RemoveTransceiver(i);         // remove the RC
  }
  TransceiverBank_CopyTransceiver( INDEX_CURRENT_REMOTE,INDEX_FIRST_REMOTE_RECORDED);    // Current -> first TR
  BankerLongCounter_StopCounting();
  Banker_Context = BANK_MODE_MEM_CLOSED;    // Bank is closed
  END_DEBUG_DISPLAY_BANKER
}

#ifdef BANKER_DEBUG
void Banker_PrintState( void )
{
    switch(Banker_Context){
		case BANK_MODE_MEM_OPENED:PutString("&mop;");break;
		case BANK_MODE_MEM_CLOSED:PutString("&mcl;");break;
		case BANK_MODE_NO_REMOTE:PutString("&nrm;");break;
		case BANK_MODE_TEMPORARY_MODE:PutString("&tmd;");break;
    default:break;}
}
#endif//BANKER_DEBUG

#endif
