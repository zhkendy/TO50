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
/ DATE :11/08/2006
/ DESCRIPTION :
/ ***********************************************************************/
// E X T E R N A L   D E F I N I T I O N S
//
#include "Config.h"
#include <string.h>

// I N T E R F A C E S
//
#include "RTSFrame.h"
#include "RTSReceiver.h"
#include "RTSDataSupply.h"
#include "RTS_InputCapture.h"
#include "RTSReceiverInterface.h"
#include "RtsLowPowerManager.h"

// C O N S T A N T S
//
//! States of RTSReceiver_eState.
typedef enum
{
  RTSRECEIVER_OFF,							//!< Receiver is disabled.
  RTSRECEIVER_WAIT_SYNCHRO,			//!< Receiver waits for synchro software.
  RTSRECEIVER_BITS_RECEPTION		//!< Receiver is in bit reception.
}TRTSReceiverCurrentState;

// S T A T E   M A C H I N E ( S )
//

// V A R I A B L E S
//
//! Pointer on current frame.
TRTSFrame * RTSReceiver_psCurrentFrame;
//! State machine of RTSReceiver.
TRTSReceiverCurrentState RTSReceiver_eState;


// P R I V A T E   P R O T O T Y P E S
//

// F U N C T I O N S / M E T H O D S
//

//_________________________________________________________________________________________________
/*!
 * \fn      void  RTSReceiver_Create( void )
 * \brief   Create the data supplier and the output.
 *
 */
void RTSReceiver_Create( void ) ///³õÊ¼»¯Êý¾ÝÖ¡ Ó²¼þ£¬ºÍ½ÓÊÕ×´Ì¬Îª¿ª±Õ
{
  // instanciation
  RTSReceiver_psCurrentFrame = RTSFrame_Create ();//³õÊ¼»¯Êý¾ÝÖ¡
  RTSDataSupply_Create();
  // TRANSITION
  RTSReceiver_eState = RTSRECEIVER_OFF;
  // ENTRY_STATE RTSRECEIVER_OFF
}

//_________________________________________________________________________________________________
/*!
 * \fn      void RTSReceiver_Enable( void )
 * \brief   Switch ON the CS_RF and ask to the data supplier to start frame reception.
 *
 */
void RTSReceiver_Enable( void )
{
  RTSReceiverListenerCS_RF_Activate();
  RTSDataSupply_StartFrameReception();
  // TRANSITION
  RTSReceiver_eState = RTSRECEIVER_WAIT_SYNCHRO;
}

//_________________________________________________________________________________________________
/*!
 * \fn      void  RTSReceiver_SynchroSoftware( void )
 * \brief   The software synchronisation is detected in the RF signal.
 *
 */
void  RTSReceiver_SynchroSoftware( void )
{
  TU8 i;

  if (RTSReceiver_eState == RTSRECEIVER_WAIT_SYNCHRO )//ÔÚÍ¬²½ÖÐ
  {
    RTSReceiver_psCurrentFrame->btNbBitReceived= 0U;
    for ( i = 0 ; i < FRAME_BYTE_NBR; i++ )
    {
      RTSReceiver_psCurrentFrame->abtBuffer[i].value= 0U;//clear buffer
    }
    RTSReceiverListenerIM_RF_FRAME_Activate();
    // TRANSITION
    RTSReceiver_eState = RTSRECEIVER_BITS_RECEPTION;
  }
}

//_________________________________________________________________________________________________
/*!
 * \fn      void  RTSReceiver_EndOfFrame(void)
 * \brief   End of frame is detected.
 *
 */
void  RTSReceiver_EndOfFrame(void)
{
  if (RTSReceiver_psCurrentFrame->btNbBitReceived >= U56_FRAME_BYTE_NBR*8)
  {
    RTSReceiverListener_ProcessFrame(RTSReceiver_psCurrentFrame);
  }
  RTSReceiver_psCurrentFrame->btNbBitReceived= 0U;

  RTSReceiverListenerIM_RF_FRAME_Deactivate();
  RTSDataSupply_WaitSynchroSoftware();
  // TRANSITION
  RTSReceiver_eState = RTSRECEIVER_WAIT_SYNCHRO;
}

//_________________________________________________________________________________________________
/*!
 * \fn      void  RTSReceiver_Disable(void)
 * \brief   Ask to the data supplier to stop frame reception and disable the physical receiver switching off CS_RF.
 *
 */
void RTSReceiver_Disable( void )
{
  RTSDataSupply_StopFrameReception();
  RTSReceiverListenerCS_RF_Deactivate();
  RTSReceiver_eState = RTSRECEIVER_OFF;
}

//_________________________________________________________________________________________________
/*!
 * \fn      TU8 RTSReceiver_HowMuchBitAreReceived(void)
 * \brief   Ask the number of bits actually received.
 * \return  The number of bits actually received.
 *
 */
TU8 RTSReceiver_HowMuchBitAreReceived(void)
{
  return (RTSReceiver_psCurrentFrame->btNbBitReceived);
}

//_________________________________________________________________________________________________
/*!
 * \fn      TBool RTSReceiver_AreYourBitsIdenticalToThisBits(TU8 * abtBitsToTest , TU8 btNumberOfBits)
 * \brief   Ask to compare a buffer with the current frame.
 * \param   abtBitsToTest : a pointer on the data to compare
 * \param   btNumberOfBits : the number of bits to test
 * \return  TRUE if bits are identical.//Í¬²½
 *
 */
TBool RTSReceiver_AreYourBitsIdenticalToThisBits(TU8 * abtBitsToTest , TU8 btNumberOfBits)
{
  TBool bTest;

  RTSReceiverListener_Save_Interrupts();
  bTest = FALSE;
  if (memcmp(RTSReceiver_psCurrentFrame->abtBuffer,abtBitsToTest,btNumberOfBits)==0)
  {
    bTest = TRUE;
  }
  RTSReceiverListener_Restore_Interrupts();
  return (bTest);
}

//_________________________________________________________________________________________________
/*!
 * \fn      void RTSReceiver_AddABit(TU8 btBitValue)
 * \brief   Add a bit in the frame.
 * \param   btBitValue : Value to add.
 *
 */
void RTSReceiver_AddABit(TU8 btBitValue)
{
  if (RTSReceiver_eState == RTSRECEIVER_BITS_RECEPTION)// RTS is in receiver state reception
  {
    if ( RTSReceiver_psCurrentFrame->btNbBitReceived >= (FRAME_BYTE_NBR*8) ) // Security  //! Number of bits currently in the frame buffer.
    {
      RTSReceiver_EndOfFrame(); //end of frame
    }
    else
    {
      if ( btBitValue == 1)
      {
        RTSReceiver_psCurrentFrame->abtBuffer[RTSReceiver_psCurrentFrame->btNbBitReceived/8].value |= (0x80>>(RTSReceiver_psCurrentFrame->btNbBitReceived%8));
      }
      RTSReceiver_psCurrentFrame->btNbBitReceived++;
      if ( RTSReceiver_psCurrentFrame->btNbBitReceived >= (FRAME_BYTE_NBR*8) )
      {
        RTSReceiver_EndOfFrame(); //end of frame
      }
    }
  }
}

#endif
