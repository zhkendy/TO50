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
#include "TransceiverBank.h"
#include "RTSProtocol.h"
//#include "PersistentDataGateway.h"
#include "STM8_Archiver.h"
#include "RTSProtocolInterface.h"

// C O N S T A N T S
//

// S T A T E   M A C H I N E ( S )
//


// V A R I A B L E S
//
#ifdef __ICCAVR__
__no_init TSensor        oSensorBank[SENSOR_NBR];
__no_init TRemoteControl oRemoteControlBank[TOTAL_TRANSCEIVER_NBR];
#else
//! Buffer of recorded sensors.
TSensor        oSensorBank[SENSOR_NBR];
//! Buffer of recorded remotes.
TRemoteControl oRemoteControlBank[TOTAL_TRANSCEIVER_NBR];
//#pragma DATA_SEG DEFAULT
#endif


//extern __no_init TPersistentDataGateway oPersistentDataGateway;

// P R I V A T E   P R O T O T Y P E S
//
void  P_TransceiverBank_ACTION_CopyTAddress    (TAdresseU56 * AddressDest,TAdresseU56 * AddressSrc);
TBool P_TransceiverBank_ACTION_AddressAreEqual (TAdresseU56 * Address1,TAdresseU56 * Address2);


// F U N C T I O N S / M E T H O D S
//
//_________________________________________________________________________________________________
/*!
 * \fn      void TransceiverBank_Create(void)
 * \brief   Empty all the bank.
 *
 */
void TransceiverBank_Create(void)
{
  //Reset all the bank
    for (TU8 i=FIRST_INDEX;i<=LAST_INDEX;i++)
    {
      TransceiverBank_RemoveTransceiver(i);
    }
}


TRemoteControl*  TransceiverBank_GetRemoteControlBank(TU8 index)
{
  return &oRemoteControlBank[index];
}


//_________________________________________________________________________________________________
/*!
 * \fn      void TransceiverBank_Restore(void)
 * \brief   Restore persistent datas, be carreful, this message must be send before Banker create, because the banker state is function of bank state.
 *
 */
void TransceiverBank_Restore(void)
{
  Archiver_LoadMe(TransceiverBank_GetRemoteControlBank(0));
}

//_________________________________________________________________________________________________
/*!
 * \fn      void TransceiverBank_Save(void)
 * \brief   Save persistent datas in segment.
 *
 */
void TransceiverBank_Save(void)
{     
}

//_________________________________________________________________________________________________
/*!
 * \fn      void TransceiverBank_PowerFailDefinitely(void)
 * \brief   A real power cut has occured.
 *
 */
void TransceiverBank_PowerFailDefinitely(void)
{
  TransceiverBank_RemoveTransceiver(INDEX_TEMPORARY_REMOTE);
}

//_________________________________________________________________________________________________
/*!
 * \fn      void  P_TransceiverBank_ACTION_CopyTAddress (TAdresseU56 * AddressDest,TAdresseU56 * AddressSrc)
 * \brief   copy a U56 address from a source to a destination.
 * \param   AddressDest : Destiantion address.
 * \param   AddressSrc : Source address.
 *
 */
void  P_TransceiverBank_ACTION_CopyTAddress (TAdresseU56 * AddressDest,TAdresseU56 * AddressSrc)
{
  memcpy(AddressDest->btAddress,AddressSrc->btAddress,sizeof(TAdresseU56));
}

//_________________________________________________________________________________________________
/*!
 * \fn      TBool P_TransceiverBank_ACTION_AddressAreEqual (TAdresseU56 * Address1,TAdresseU56 * Address2)
 * \brief   Compare 2 address : if there are equal return TRUE else FALSE.
 * \param   Address1 : first address.
 * \param   Address2 : second address.
 * \return  TRUE if equal, else FALSE.
 *
 */
TBool P_TransceiverBank_ACTION_AddressAreEqual (TAdresseU56 * Address1,TAdresseU56 * Address2)
{
  if(memcmp(Address1,Address2,sizeof(TAdresseU56))==FALSE)
  {
    return (TRUE);
  }
  return(FALSE);
}

//______________________________________________________________________________________________
/*!
 * \fn      void TransceiverBank_AddCurrentTransceiver(TU8 btType,TU16 wRollingCode,TAdresseU56 Address)
 * \brief   Add a transceiver caracterized by its type, rolling code and address in the current location of the bank.
 * \param   btType : Type
 * \param   wRollingCode : Rolling code.
 * \param   Address : Address of the transceiver.
 *
 */
void TransceiverBank_AddCurrentTransceiver(TU8 btType,TU16 wRollingCode,TAdresseU56 Address)
{
  oRemoteControlBank[INDEX_CURRENT_REMOTE].RemoteControlPersistent.btType = btType;
  oRemoteControlBank[INDEX_CURRENT_REMOTE].RemoteControlVolatile.wLastRollingCode = wRollingCode;
  oRemoteControlBank[INDEX_CURRENT_REMOTE].RemoteControlPersistent.wRollingCodeSaved = wRollingCode;
  P_TransceiverBank_ACTION_CopyTAddress (&oRemoteControlBank[INDEX_CURRENT_REMOTE].RemoteControlPersistent.Address,&Address);

  RTSProtocolListener_PersistenDataHaveChanged();
}

//______________________________________________________________________________________________
/*!
 * \fn      void TransceiverBank_RemoveTransceiver(TU8 btIndex)
 * \brief   Remove the transceiver located at the given index.
 * \param   btIndex : Index of the transceiver to remove.
 *
 */
void TransceiverBank_RemoveTransceiver(TU8 btIndex)
{
  if (btIndex <= INDEX_LAST_REMOTE_RECORDED)         // Index is a remote control
  {
    memset(&oRemoteControlBank[btIndex],0,sizeof (TRemoteControl));
    oRemoteControlBank[btIndex].RemoteControlPersistent.btType = RTSPROT_RC_UNKNOWN ;
    RTSProtocolListener_PersistenDataHaveChanged();
  }
  else if ( btIndex <= INDEX_LAST_SENSOR_RECORDED)  // Index is a sensor
  {
  //  memset(&oSensorBank[btIndex-INDEX_FIRST_SENSOR_RECORDED],0,sizeof(TSensor));
    //oSensorBank[btIndex-INDEX_FIRST_SENSOR_RECORDED].btType = RTSPROT_RC_UNKNOWN ;
        memset(&oRemoteControlBank[btIndex],0,sizeof (TRemoteControl));//kendy
    oRemoteControlBank[btIndex].RemoteControlPersistent.btType = RTSPROT_RC_UNKNOWN ;
    RTSProtocolListener_PersistenDataHaveChanged();
  }
  // else : bad btIndex
}

//______________________________________________________________________________________________
/*!
 * \fn      TBool TransceiverBank_IsThisIndexEmpty(TU8 btIndex)
 * \brief   Check if a transceiver index is free.
 * \param   btIndex : The index of the transceiver to test.
 * \return  TRUE if free, FALSE if full.
 *
 */
TBool TransceiverBank_IsThisIndexEmpty(TU8 btIndex)
{
  TBool btTest = FALSE;

  if (btIndex <= INDEX_LAST_REMOTE_RECORDED)         // Index is a remote control
  {
    if (oRemoteControlBank[btIndex].RemoteControlPersistent.btType == RTSPROT_RC_UNKNOWN )
    {
      btTest = TRUE;
    }
  }
  else if ( btIndex <= INDEX_LAST_SENSOR_RECORDED)  // Index is a sensor
  {
    btIndex -= INDEX_FIRST_SENSOR_RECORDED;
    if (oSensorBank[btIndex].btType == RTSPROT_RC_UNKNOWN )
    {
      btTest = TRUE;
    }
  }
  // else : bad btIndex
  return (btTest);
}

//______________________________________________________________________________________________
/*!
 * \fn      void TransceiverBank_CopyTransceiver(TU8 btIndexSrc,TU8 btIndexDest)
 * \brief   Copy a transceiver from a location to an other.
 * \param   btIndexSrc : The source index.
 * \param   btIndexDest : The destination index.
 *
 */
void TransceiverBank_CopyTransceiver(TU8 btIndexSrc,TU8 btIndexDest)
{
  if (btIndexSrc <= INDEX_LAST_REMOTE_RECORDED)          // btIndexSrc is a remote control
  {
    if ( btIndexDest <= INDEX_LAST_REMOTE_RECORDED)      // btIndexDest is a remote control
    {
      memcpy(&oRemoteControlBank[btIndexDest],&oRemoteControlBank[btIndexSrc],sizeof(TRemoteControl));
      RTSProtocolListener_PersistenDataHaveChanged();
    }
    else if (btIndexDest <= INDEX_LAST_SENSOR_RECORDED) // btIndexDest is a sensor
    {
     // btIndexDest -= INDEX_FIRST_SENSOR_RECORDED;
      
     // oSensorBank[btIndexDest].btType  = oRemoteControlBank[btIndexSrc].RemoteControlPersistent.btType;
    //  P_TransceiverBank_ACTION_CopyTAddress (&oSensorBank[btIndexDest].Address,&oRemoteControlBank[btIndexSrc].RemoteControlPersistent.Address);
      memcpy(&oRemoteControlBank[btIndexDest],&oRemoteControlBank[btIndexSrc],sizeof(TRemoteControl));//kendy
      RTSProtocolListener_PersistenDataHaveChanged();
    }
    // else bad btIndexDest
  }
}

//______________________________________________________________________________________________
/*!
 * \fn      TU8 TransceiverBank_GiveIndexCurrent(void)
 * \brief   Search the current transceiver in the bank.
 * \return  The index of the transceiver.
 *
 */
TU8 TransceiverBank_GiveIndexCurrent(void)
{
  TU8 j,i;
  TU8 btIndexFound;

  btIndexFound= 0U;
  i=INDEX_TEMPORARY_REMOTE;

  while (i<=INDEX_LAST_REMOTE_RECORDED)    // Search in remote control bank
  {
    if (  (oRemoteControlBank[i].RemoteControlPersistent.btType == oRemoteControlBank[INDEX_CURRENT_REMOTE].RemoteControlPersistent.btType)
        &&(P_TransceiverBank_ACTION_AddressAreEqual(&oRemoteControlBank[i].RemoteControlPersistent.Address,&oRemoteControlBank[INDEX_CURRENT_REMOTE].RemoteControlPersistent.Address)==TRUE))
    {
      btIndexFound=i;      // Same address and types
      i=LAST_INDEX+1;     // To stop this while loop
    }
    i++;
  }
  while (i<=INDEX_LAST_SENSOR_RECORDED)    // Search in sensor bank
  {
//    j=i-INDEX_FIRST_SENSOR_RECORDED;
//    if (  (oSensorBank[j].btType == oRemoteControlBank[INDEX_CURRENT_REMOTE].RemoteControlPersistent.btType)
//        &&(P_TransceiverBank_ACTION_AddressAreEqual(&oSensorBank[j].Address,&oRemoteControlBank[INDEX_CURRENT_REMOTE].RemoteControlPersistent.Address)==TRUE))
//    {
//      btIndexFound=i;      // Same address and types
//      i=LAST_INDEX+1;     // To stop this while loop
//    }//kendy RTS
        if (  (oRemoteControlBank[i].RemoteControlPersistent.btType == oRemoteControlBank[INDEX_CURRENT_REMOTE].RemoteControlPersistent.btType)
        &&(P_TransceiverBank_ACTION_AddressAreEqual(&oRemoteControlBank[i].RemoteControlPersistent.Address,&oRemoteControlBank[INDEX_CURRENT_REMOTE].RemoteControlPersistent.Address)==TRUE))
    {
      btIndexFound=i;      // Same address and types
      i=LAST_INDEX+1;     // To stop this while loop
    }
    i++;
  }
  return (btIndexFound);
}

//______________________________________________________________________________________________
/*!
 * \fn      TU16 TransceiverBank_ReadLastRollingCode(TU8 btIndex)
 * \brief   Read the last rolling code of the transceiver located at an index of the bank.
 * \param   btIndex : Index of the transceiver.
 * \return  Its rolling code, 0 if a sensor.
 *
 */
TU16 TransceiverBank_ReadLastRollingCode(TU8 btIndex)
{
  if (btIndex <= INDEX_LAST_REMOTE_RECORDED)         // Index is a remote control
  {
    return (oRemoteControlBank[btIndex].RemoteControlVolatile.wLastRollingCode);
  }
  // else Index is a sensor : no wLastRollingCode
  return(0);
}

//______________________________________________________________________________________________
/*!
 * \fn      TU16 TransceiverBank_ReadRollingCodeSaved(TU8 btIndex)
 * \brief   Read the last rolling code saved of the transceiver located at an index of the bank.
 * \param   btIndex : Index of the transceiver.
 * \return  Its rolling code, 0 if a sensor.
 *
 */
TU16 TransceiverBank_ReadRollingCodeSaved(TU8 btIndex)
{
  if (btIndex <= INDEX_LAST_REMOTE_RECORDED)         // Index is a remote control
  {
    return (oRemoteControlBank[btIndex].RemoteControlPersistent.wRollingCodeSaved);
  }
  // else Index is a sensor : no wRollingCodeSaved
  return (0);
}

//______________________________________________________________________________________________
/*!
 * \fn      TBool TransceiverBank_WriteLastRollingCode(TU8 btIndex,TU16 wRollingCode)
 * \brief   Write last rolling code of a transceiver.
 * \param   btIndex : Index of the transceiver.
 * \param   wRollingCode : Rollign code to write.
 * \return  TRUE if index is a remote control, else FALSE.
 *
 */
TBool TransceiverBank_WriteLastRollingCode(TU8 btIndex,TU16 wRollingCode)
{
  if (btIndex <= INDEX_LAST_REMOTE_RECORDED)         // Index is a remote control
  {
    oRemoteControlBank[btIndex].RemoteControlVolatile.wLastRollingCode = wRollingCode;
    return (TRUE);
  }
  // else Index is a sensor : no wLastRollingCode
  return (FALSE);
}

//______________________________________________________________________________________________
/*!
 * \fn      TBool TransceiverBank_WriteRollingCodeSaved(TU8 btIndex,TU16 wRollingCode)
 * \brief   Write last rolling code saved of a transceiver.
 * \param   btIndex : Index of the transceiver.
 * \param   wRollingCode : Rollign code to write.
 * \return  TRUE if index is a remote control, else FALSE.
 *
 */
TBool TransceiverBank_WriteRollingCodeSaved(TU8 btIndex,TU16 wRollingCode)
{
  if (btIndex <= INDEX_LAST_REMOTE_RECORDED)         // Index is a remote control
  {
    oRemoteControlBank[btIndex].RemoteControlPersistent.wRollingCodeSaved = wRollingCode;
    RTSProtocolListener_PersistenDataHaveChanged();
    return (TRUE);
  }
  // else Index is a sensor : no wRollingCodeSaved
  return (FALSE);
}

#endif
