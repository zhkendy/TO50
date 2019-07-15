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
/ ********************************************************************* */
// E X T E R N A L   D E F I N I T I O N S
//

// I N T E R F A C E S
//
#include "Config.h"
#include "RTSDataSupply.h"
#include "RTS_InputCapture.h"
#include "RTSReceiver.h"
#include "RTSReceiverInterface.h"

                                                               
// C O N S T A N T S
//
//! Type of duration to wait.
enum{
  DURATION_ERROR = 0x00,     //!< Duration between two edge is unknown.
  DURATION_SHORT,            //!< Duration between two edge is a short level (+/- 640us).
  DURATION_LONG,             //!< Duration between two edge is a long level   (+/- 1280us).
  DURATION_SYNCHRO,          //!< Duration between two edge is a software synchro duration (+/- 4800us).
  DURATION_FIRST_SHORT,      //!< Duration between two edge is a first short bit (+/- 640us).
  DURATION_FIRST_LONG,       //!< Duration between two edge is a first long bit (+/- 1280us).
  DURATION_SYNCHRO_HW        //!< Duration between two edge is a hardware synchro duration (+/- 2500us).
};

/* Number of consecutive RTS fragment necessary to detect a RTS frame */
#define NB_OF_FRAGMENT_TO_DETECT_A_FRAME       4

// S T A T E   M A C H I N E ( S )
//
//! States of 
typedef enum{
  OFF_STATE,                //! state machine is switch OFF
  WAIT_BEGIN_SYNCHRO_SW,    //! state machine is waiting a begin of software synchro
  WAIT_END_SYNCHRO_SW,      //! state machine is waiting a end of software synchro
  WAIT_FIRST_EDGE,          //! state machine is waiting first edge
  LONG_LEVEL_DETECTED,      //! state machine has received a long level
  SHORT_LEVEL_DETECTED      //! state machine has received a short level
}TRTSDataSupplyStates;

// V A R I A B L E S
//
#ifdef POLYSPACE    
  TRTSDataSupplyStates oRTSDataSupply_eCurrentState;
  TU16 oRTSDataSupply_wPreviousTimerCounter;
#else
//! Attributes of RTSDataSupply.
struct RTSDataSupplyAttributes
{
  TRTSDataSupplyStates eCurrentState;     //! current state from statechart.
  TU16 wPreviousTimerCounter;            //! timer value when last edge occured.
  TU8 btRTSFragmentPresent;
} oRTSDataSupply;
  
#define oRTSDataSupply_eCurrentState                  oRTSDataSupply.eCurrentState
#define oRTSDataSupply_wPreviousTimerCounter          oRTSDataSupply.wPreviousTimerCounter  
#define oRTSDataSupply_btNbOfSuccessiveRTSFragments   oRTSDataSupply.btRTSFragmentPresent
#endif

// P R I V A T E   P R O T O T Y P E S
//
TBool P_RTSDataSupply_ACTION_VerifyTimerValue( TU16 TimerValue , TU8 TypeOfDuration );

// F U N C T I O N S / M E T H O D S
//
//_________________________________________________________________________________________________
/*!
 * \fn      void  RTSDataSupply_Create( void )
 * \brief   Create the Input RADIO and initialize attributes.
 * 
 */
void RTSDataSupply_Create( void )
{ 
  // instanciation
  InputCapture_SetUpHarware();
  
  // initialisation des attributs
  oRTSDataSupply_eCurrentState = OFF_STATE;
  //oRTSDataSupply_wTimerCounterToRestore = 0U;
  oRTSDataSupply_wPreviousTimerCounter = 0U;
  oRTSDataSupply_btNbOfSuccessiveRTSFragments = 0U;
}

//_________________________________________________________________________________________________
/*!
 * \fn      void  RTSDataSupply_WaitSynchroSoftware( void )
 * \brief   Say to the data supplier to wait a begin of synchro soft.
 * 
 */
void RTSDataSupply_WaitSynchroSoftware( void )
{
  
  InputCapture_TriggerOnRisingEdge();
  InputCapture_DeactivateTimeout();    
  oRTSDataSupply_eCurrentState = WAIT_BEGIN_SYNCHRO_SW;
}

//_________________________________________________________________________________________________
/*!
 * \fn      void  RTSDataSupply_TimeoutDetected( void )
 * \brief   A timeout is detected, say to RTSReceiver that an error is detected.
 * 
 */
void RTSDataSupply_TimeoutDetected(void)
{
  RTSReceiver_EndOfFrame();
  RTSDataSupply_StartFrameReception();
}

//_________________________________________________________________________________________________
/*!
 * \fn      void  RTSDataSupply_StartFrameReception( void )
 * \brief   Say to the data supplier to wait rising edge and initialize the statechart.
 * 
 */
void RTSDataSupply_StartFrameReception( void )
{
  RTSReceiverListener_Save_Interrupts();
  InputCapture_Disable();
  InputCapture_TriggerOnRisingEdge();
  InputCapture_Ack();
  InputCapture_DeactivateTimeout();
  InputCapture_Enable();
    
  oRTSDataSupply_eCurrentState = WAIT_BEGIN_SYNCHRO_SW;
  RTSReceiverListener_Restore_Interrupts();
  
  oRTSDataSupply_btNbOfSuccessiveRTSFragments = 0U;
}

//_________________________________________________________________________________________________
/*!
 * \fn      void  RTSDataSupply_StopFrameReception( void )
 * \brief   Disable input capture and set the statechart in a idle state.
 * 
 */
void RTSDataSupply_StopFrameReception( void )
{  
  RTSReceiverListener_Save_Interrupts();
  InputCapture_Disable();
  RTSReceiverListenerIM_RF_FRAME_Deactivate();
  InputCapture_DeactivateTimeout();
  oRTSDataSupply_eCurrentState = OFF_STATE;
  RTSReceiverListener_Restore_Interrupts();
}

//_________________________________________________________________________________________________
/*!
 * \fn      TBool P_RTSDataSupply_ACTION_VerifyTimerValue( TU16 wTimerValue , TU8 btTypeOfDuration )
 * \brief   Verify if the duration can be of the type passed in param.
 * \param   wTimerValue : Duration to check.
 * \param   btTypeOfDuration : Type of duration to check.
 * \return  TRUE if type of duration is correct, else FALSE.
 *
 */
TBool P_RTSDataSupply_ACTION_VerifyTimerValue( TU16 wTimerValue , TU8 btTypeOfDuration )
{
  TBool bRetval = FALSE;
  
  switch (btTypeOfDuration)
  {
    case DURATION_SHORT :
      if ( ( DURATION_SHORT_MIN < wTimerValue ) && ( wTimerValue < DURATION_SHORT_MAX ) )  
      {
        bRetval = TRUE;
      }
      else {}
    break;
    
    case DURATION_LONG :
      if ( ( DURATION_LONG_MIN < wTimerValue ) && ( wTimerValue < DURATION_LONG_MAX ) )  
      {
        bRetval = TRUE;
      }
      else {}
    break;

    case DURATION_FIRST_LONG :
      if ( ( DURATION_FIRST_LONG_MIN < wTimerValue ) && ( wTimerValue < DURATION_FIRST_LONG_MAX ) )  
      {
        bRetval = TRUE;
      }
      else {}
    break; 
    
    case DURATION_FIRST_SHORT :
      if ( ( DURATION_FIRST_SHORT_MIN < wTimerValue ) && ( wTimerValue < DURATION_FIRST_SHORT_MAX ) )  
      {
        bRetval = TRUE;
      }
      else {}
    break;
    
    case DURATION_SYNCHRO_HW :
      if ( ( DURATION_SYNCHRO_HW_MIN < wTimerValue ) && ( wTimerValue < DURATION_SYNCHRO_HW_MAX ) )  
      {
        bRetval = TRUE;
      }
      else {}
    break;
    default:
    break;               
  }
  return (bRetval);  
}

//_________________________________________________________________________________________________
/*!
 * \fn      void  RTSDataSupply_EdgeDetected( void )
 * \brief   Called when an edge is detected by InputCapture.
 * 
 */
void RTSDataSupply_EdgeDetected( void )
{
  TU16 wDuration;
  TU16 TimerValue;
  TU8  bInputLevel;
  
  TimerValue=InputCapture_GiveTimerValue();
  
  if(TimerValue > oRTSDataSupply_wPreviousTimerCounter) 
    { wDuration = TimerValue - oRTSDataSupply_wPreviousTimerCounter;}
  else 
    { wDuration = TimerValue + (0xFFFF - oRTSDataSupply_wPreviousTimerCounter); }//add sage
   
  //wDuration = InputCapture_GiveTimerValue() - oRTSDataSupply_wPreviousTimerCounter;
    
  //oRTSDataSupply_wPreviousTimerCounter = InputCapture_GiveTimerValue();
  oRTSDataSupply_wPreviousTimerCounter = TimerValue;
  bInputLevel = InputCapture_Read( );
    
  if (oRTSDataSupply_eCurrentState == WAIT_END_SYNCHRO_SW)
  {
    if ( wDuration > DURATION_SYNCHRO_MIN  )
    {
      if ( wDuration < DURATION_SYNCHRO_MAX  )
      {
        if (bInputLevel == LOW_LEVEL)
        {
          //InputCapture_ReloadTimeout(DURATION_FIRST_LONG_MAX);  sage
          InputCapture_ReloadTimeout(DURATION_LONG_TIMEOUT);   
          RTSReceiver_SynchroSoftware();
          oRTSDataSupply_eCurrentState = WAIT_FIRST_EDGE;
        }
        else
        {
          RTSReceiverListenerIM_RF_FRAME_Deactivate();
          RTSDataSupply_WaitSynchroSoftware();
          /* Error detected: not a RTS frame */
          oRTSDataSupply_btNbOfSuccessiveRTSFragments = 0U;
        }
      }
      else
      {
        RTSReceiverListenerIM_RF_FRAME_Deactivate();
        RTSDataSupply_WaitSynchroSoftware();
        /* Error detected: not a RTS frame */
        oRTSDataSupply_btNbOfSuccessiveRTSFragments = 0U;
      }
    }
    else
    {
      if(P_RTSDataSupply_ACTION_VerifyTimerValue( wDuration , DURATION_FIRST_SHORT ))
      {
        /* Bit reception */
        oRTSDataSupply_btNbOfSuccessiveRTSFragments++;
      }
      else if (P_RTSDataSupply_ACTION_VerifyTimerValue( wDuration , DURATION_FIRST_LONG ))
      {
        /* Bit reception */
        oRTSDataSupply_btNbOfSuccessiveRTSFragments++;
      }
      else if (P_RTSDataSupply_ACTION_VerifyTimerValue( wDuration , DURATION_SYNCHRO_HW ))
      {
        /* Synchro HW */
        oRTSDataSupply_btNbOfSuccessiveRTSFragments++;
      }
      else
      {
        /* Error detected: not a RTS frame */
        oRTSDataSupply_btNbOfSuccessiveRTSFragments = 0U;
      }
      //InputCapture_ReloadTimeout(DURATION_SYNCHRO_MAX);  sage
      InputCapture_ReloadTimeout(DURATION_SYNCHRO_TIMEOUT);
      InputCapture_ActivateTimeout();   
      RTSReceiverListenerIM_RF_FRAME_Deactivate();
      RTSDataSupply_WaitSynchroSoftware();
    }     
  }
  else if (oRTSDataSupply_eCurrentState == WAIT_BEGIN_SYNCHRO_SW)
  {
    if(P_RTSDataSupply_ACTION_VerifyTimerValue( wDuration , DURATION_FIRST_SHORT ))
    {
      /* Bit reception */
      oRTSDataSupply_btNbOfSuccessiveRTSFragments++;
    }
    else if (P_RTSDataSupply_ACTION_VerifyTimerValue( wDuration , DURATION_FIRST_LONG ))
    {
      /* Bit reception */
      oRTSDataSupply_btNbOfSuccessiveRTSFragments++;
    }
    else if (P_RTSDataSupply_ACTION_VerifyTimerValue( wDuration , DURATION_SYNCHRO_HW ))
    {
      /* Synchro HW */
      oRTSDataSupply_btNbOfSuccessiveRTSFragments++;
    }
    else
    {
      /* Error detected: not a RTS frame */
      oRTSDataSupply_btNbOfSuccessiveRTSFragments = 0U;
    }
    
    if ( bInputLevel == HIGH_LEVEL )
    {
      //InputCapture_ReloadTimeout(DURATION_SYNCHRO_MAX);  sage
      InputCapture_ReloadTimeout(DURATION_SYNCHRO_TIMEOUT);
      InputCapture_ActivateTimeout();
      oRTSDataSupply_eCurrentState = WAIT_END_SYNCHRO_SW;
    }   
    else
    {
      /* Nothing to do */
      //InputCapture_ReloadTimeout(DURATION_SYNCHRO_MAX);  sage
      InputCapture_ReloadTimeout(DURATION_SYNCHRO_TIMEOUT);
      InputCapture_ActivateTimeout();
    }
  }
  else
  {
    switch ( oRTSDataSupply_eCurrentState )
    {
      case OFF_STATE :
      break;
    
      case WAIT_FIRST_EDGE : 
        if (P_RTSDataSupply_ACTION_VerifyTimerValue( wDuration , DURATION_FIRST_LONG )) //!<  (+/- 1280us).
        {
          if (bInputLevel == HIGH_LEVEL )
          {
            //InputCapture_ReloadTimeout(DURATION_LONG_MAX); sage
            InputCapture_ReloadTimeout(DURATION_LONG_TIMEOUT);   
            RTSReceiver_Bit1Detected();
            oRTSDataSupply_eCurrentState = LONG_LEVEL_DETECTED;
          } 
          else
          {
            RTSReceiverListenerIM_RF_FRAME_Deactivate();
            RTSDataSupply_WaitSynchroSoftware();          
          }
        }
        else
        {
          if (P_RTSDataSupply_ACTION_VerifyTimerValue( wDuration , DURATION_FIRST_SHORT )) 
          {
            oRTSDataSupply_eCurrentState = SHORT_LEVEL_DETECTED;
            //InputCapture_ReloadTimeout(DURATION_LONG_MAX); sage
            InputCapture_ReloadTimeout(DURATION_LONG_TIMEOUT);   
          }
          else
          {
            RTSReceiver_EndOfFrame(); 
            /* Error detected: not a RTS frame */
            oRTSDataSupply_btNbOfSuccessiveRTSFragments = 0U;
          }
        }      
      break;
          
      case LONG_LEVEL_DETECTED :
        if (P_RTSDataSupply_ACTION_VerifyTimerValue( wDuration , DURATION_LONG ))
        {
          if (bInputLevel == HIGH_LEVEL )
          {
            //InputCapture_ReloadTimeout(DURATION_LONG_MAX);   sage
            InputCapture_ReloadTimeout(DURATION_LONG_TIMEOUT);   
            RTSReceiver_Bit1Detected();  
          } 
          else
          {
            //InputCapture_ReloadTimeout(DURATION_LONG_MAX);  sage
            InputCapture_ReloadTimeout(DURATION_LONG_TIMEOUT);   
            RTSReceiver_Bit0Detected(); 
          }
          oRTSDataSupply_eCurrentState = LONG_LEVEL_DETECTED;
        }
        else 
        {
          if (P_RTSDataSupply_ACTION_VerifyTimerValue( wDuration , DURATION_SHORT )) 
          {                 
            oRTSDataSupply_eCurrentState = SHORT_LEVEL_DETECTED;
            //InputCapture_ReloadTimeout(DURATION_LONG_MAX);  sage
            InputCapture_ReloadTimeout(DURATION_LONG_TIMEOUT);   
          }
          else
          {
            RTSReceiver_EndOfFrame(); 
            /* Error detected: not a RTS frame */
            oRTSDataSupply_btNbOfSuccessiveRTSFragments = 0U;
          }
        }
      break;
            
      case SHORT_LEVEL_DETECTED :
        if (P_RTSDataSupply_ACTION_VerifyTimerValue( wDuration , DURATION_SHORT ))
        {
          if (bInputLevel == HIGH_LEVEL )
          {
            //InputCapture_ReloadTimeout(DURATION_LONG_MAX); sage
            InputCapture_ReloadTimeout(DURATION_LONG_TIMEOUT);   
            RTSReceiver_Bit1Detected();  
          }
          else
          {
            //InputCapture_ReloadTimeout(DURATION_LONG_MAX); sage
            InputCapture_ReloadTimeout(DURATION_LONG_TIMEOUT);   
            RTSReceiver_Bit0Detected(); 
          }
          oRTSDataSupply_eCurrentState = LONG_LEVEL_DETECTED;
        }
        else
        {
          RTSReceiver_EndOfFrame();
          /* Error detected: not a RTS frame */
          oRTSDataSupply_btNbOfSuccessiveRTSFragments = 0U;
        }
      break;  
    }   // switch ( RTSDataSupply_eCurrentState )     
  }  		// if oRTSDataSupply_eCurrentState != SYNCHRO
}

//_________________________________________________________________________________________________
/*!
 * \fn      TBool RTSDataSupply_PresenceOfRTSFragment( void )
 * \brief   True if at least a piece of RTS frame has been detected
 * 
 */
TBool RTSDataSupply_PresenceOfRTSFragment( void )
{
  TBool bRet = FALSE;
  
  if(oRTSDataSupply_btNbOfSuccessiveRTSFragments >= NB_OF_FRAGMENT_TO_DETECT_A_FRAME)
  {
    bRet =  TRUE;
  }
  else
  {
    bRet = FALSE;
  }
  return bRet;
}

#endif
