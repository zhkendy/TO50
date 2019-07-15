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
*/

/**
* @file RTSProtocol.c
* @brief ...
* @author Florian GERMAIN
* @version xx
* @date 12/10/2006
*
*/


#ifdef __DEBUG_TARGET__
#define DBG_THIS_CLASS        // define to debug this class
#endif // ifdef GLOBAL_DEBUG

// E X T E R N A L   D E F I N I T I O N S
//
#include <stdio.h>
#include "Config.h"
#include <string.h>
#include "stm8s.h"

// I N T E R F A C E S
//
#include "RTSProtocol.h"
#include "RTSProtocolInterface.h"
#include "SFY_Macros.h"

// O B J E C T   S T R U C T U R E
typedef union
{
  TU8 btByte[U80_FRAME_BYTE_NBR];
  struct
  {
    TU8 RANDOM;
    TU8 CKSTR;
    TU8 CPTH;
    TU8 CPTL;
    TAdresseU56 ADDR;
    TU8 U80_BYTE1;
    TU8 U80_BYTE2;
    TU8 U80_BYTE3;
  }Field;
}TFrameStructure;


// P R I V A T E   C O N S T A N T S
//
//! Different event which imply statechart evolution.
enum
{
  EVENT_NO_EVENT,                   //!< No event.
  EVENT_TIME_ELAPSED,               //!< Timer elapsed : first event which can imply an evolution of statechart.
  EVENT_FRAME_RECEPTION             //!< Second event : a frame reception.
};

//! Type of frame.
enum
{
  RTS_FRAME_INVALID,                //!< Default value.
  RTS_FACTORY_FRAME_CRYPTED,        //!< Factory crypted frame : Random=0 and adress<0x300.
  RTS_FACTORY_FRAME_UNCRYPTED,      //!< Factory uncrypted frame : Random=0 and 0xFE0000<=adress<=0xFEFFFF.
  RTDS_FRAME_NO_RC_CHECK,           //!< 0xD70000<=adress<=0xD7FFFF.
  RTDS_FRAME_WITH_RC_CHECK,         //!< 0xCC0000<=adress<=0xCCFFFF.
  RTS_CHANGING_CODE,                //!< Command=E : sensor.
  RTS_EXTENDED_COMMAND,             //!< Command=F : 1 button remote control.
  RTS_STANDARD                      //!< Classic remote control 3 button.
};

//! Mask to test if relay bit is activate in U80 frame.
#define MASK_U80_BIT_RELAY                      0x80
//! Mask to test if released bit is activate in U80 frame.
#define MASK_U80_BIT_RELEASED                   0x40
//! Number of bits to test after 100ms to say : frame reception in progress.
#define NB_BIT_CHECK_RX_U56                     16
//! Mask to find the mark identifiant in the random.
#define MASK_MARK_IN_RANDOM                     0xF0
//! SOMFY identifiant in the random.
#define MARK_SOMFY_IN_RANDOM                    0xA0
//Spring 2.5 brandID
#define MARK_SPRING_IN_RANDOM                    0xB0
//! Offset to find the command in CKSTR.
#define OFFSET_COMMAND_IN_CKSTR                 4
//! Mask to find the command in CKSTR.
#define MASK_COMMAND_IN_CKSTR                   0xF0
//! Mask to find the command in the random.
#define MASK_COMMAND_IN_RANDOM                  0x0F
//! Command PROG on 1 button remote control.
#define PROG_INIS_RT                            0x03
//! 4 time command stop-roll-stop-unroll for 1 button RC.
#define CYCLE_4CMD_INIS_RT                      0x04
//! Index of first byte of address in btAddress[].
#define INDEX_ADDR1                             2
//! Index of second byte of address in btAddress[].
#define INDEX_ADDR2                             1
//! Index of third byte of address in btAddress[].
#define INDEX_ADDR3                             0

// S T A T E   M A C H I N E ( S )
//
//! States of RTSProtocol statechart.
typedef enum
{
  STATE_NO_FLOW,                                        //!< No flow in progress.
  STATE_FLOW_WITHOUT_ERROR_WAIT_TO_CHECK_RX,            //!< Wait 100ms to verify if reception in progress.
  STATE_FLOW_WITHOUT_ERROR_WAIT_NEW_FRAME,              //!< Wait a new frame.
  STATE_FLOW_WITH_AN_ERROR_WAIT_TO_CHECK_RX,            //!< Wait 100ms to verify if reception in progress.
  STATE_FLOW_WITH_AN_ERROR_WAIT_NEW_FRAME,              //!< Wait a new frame.
}TStateRTSProtocol;

#ifdef POLYSPACE                                // Polyspace don't like structures

TStateRTSProtocol oRTSProtocol_eProtocolState;
TU8 oRTSProtocol_btThisFrameIsU80;
TU8 oRTSProtocol_btStimuliCounterToRestore;
TAdresseU56     oRTSProtocol_sLastAddress;
TFrameStructure oRTSProtocol_sFrameExpected;
TRTSFrame       oRTSProtocol_sPendingFrame;
TFrameStructure oRTSProtocol_sCurrentFrame;
TStimuli        oRTSProtocol_sStimuliToSend;

#else

//! Attributes of class RTSProtocol.
struct RTSProtocolAttributes
{
  //! State machine.
  TStateRTSProtocol           btProtocolState;
  //! Record if current frame is U80.
  TU8           btThisFrameIsU80;
  //! Duration of the stimuli to restore after an error in the stream.
  TU8           btStimuliCounterToRestore;
  //! Record adress of the last transceiver.
  TAdresseU56     sLastAddress;
  //! Record the frame expected.
  TFrameStructure sFrameExpected;
  //! Duplication of received frame in IT.
  TRTSFrame       sPendingFrame;
  //! Current frame.
  TFrameStructure sCurrentFrame;
  //! Stimuli which will be send to RTSOrderProvider.
  TStimuli        StimuliToSend;
}oRTSProtocol;

#define oRTSProtocol_eProtocolState              oRTSProtocol.btProtocolState
#define oRTSProtocol_btThisFrameIsU80             oRTSProtocol.btThisFrameIsU80
#define oRTSProtocol_btStimuliCounterToRestore    oRTSProtocol.btStimuliCounterToRestore
#define oRTSProtocol_sLastAddress                 oRTSProtocol.sLastAddress
#define oRTSProtocol_sFrameExpected               oRTSProtocol.sFrameExpected
#define oRTSProtocol_sPendingFrame                oRTSProtocol.sPendingFrame
#define oRTSProtocol_sCurrentFrame                oRTSProtocol.sCurrentFrame
#define oRTSProtocol_sStimuliToSend                oRTSProtocol.StimuliToSend

#endif


/*-- MACROS FOR DEBUG --*/
#ifdef RTSPR_DEBUG
#define BIRTH_OBJECT_RTSPR\
PutString("<o n=\"&rpr;\"></o>");
#else//RTSPR_DEBUG
#define BIRTH_OBJECT_RTSPR
#endif//RTSPR_DEBUG

#ifdef RTSPR_DEBUG
#define  START_DEBUG_DISPLAY_RTSPR(text)\
PutString("<m o=\"&rpr;\" n=\"&" #text ";\"><st st=\"&en;\">");\
  RTSPR_PRINT_STATE()\
    PutString("</st>");
#else // RTSPR_DEBUG
#define  START_DEBUG_DISPLAY_RTSPR(text)
#endif // RTSPR_DEBUG
    
#ifdef RTSPR_DEBUG
#define  END_DEBUG_DISPLAY_RTSPR\
    PutString("<st st=\"&ex;\">");\
      RTSPR_PRINT_STATE()\
        PutString("</st></m>");
#else // RTSPR_DEBUG
#define  END_DEBUG_DISPLAY_RTSPR
#endif // RTSPR_DEBUG
        
#ifdef RTSPR_DEBUG
#define DEBUG_DISPLAY_STRING_RTSPR(text)\
        PutString("{RTSPR " #text "}");
#else // RTSPR_DEBUG
#define DEBUG_DISPLAY_STRING_RTSPR(text)
#endif // RTSPR_DEBUG
        
#ifdef RTSPR_DEBUG
#define DEBUG_DISPLAY_WORD_RTSPR(word)\
        PutWord(word);
#else // RTSPR_DEBUG
#define DEBUG_DISPLAY_WORD_RTSPR(word)
#endif // RTSPR_DEBUG
        
#ifdef RTSPR_DEBUG
#define RTSPR_PRINT_STATE()\
        RTSProtocol_PrintState();
#else // RTSPR_DEBUG
#define RTSPR_PRINT_STATE()
#endif // RTSPR_DEBUG
        
#ifdef RTSPR_DEBUG
#define RTSProtocol_PrintState() PutString("&nost;");
#endif // RTSPR_DEBUG
        
        // P R I V A T E   P R O T O T Y P E S
        //
        TU8 P_RTSProtocol_ACTION_GiveTypeOfTransceiver(void);
        TBool P_RTSProtocol_ACTION_CheckNewRollingCode(void);
        TBool P_RTSProtocol_ACTION_DecryptAndVerifyChecksum(void);
        TBool P_RTSProtocol_ACTION_CheckIfFrameReceptionInProgress(void);
        TBool P_RTSProtocol_ACTION_CheckIfFrameIsThisExpected(void);
        void  P_RTSProtocol_ACTION_SendReleasedStimuli(void);
        void  P_RTSProtocol_ACTION_SendStimuli(TU8 btTypeOfFrame);
        void  P_RTSProtocol_ACTION_Statechart(TU8 Event);
        void  P_RTSProtocol_ACTION_SendAgainLastStimuli(void);
        void  P_RTSProtocol_ACTION_EndOfFlow(void);
        void  P_RTSProtocol_ACTION_TakeAgainCurrentFlow(void);
        void  P_RTSProtocol_ACTION_AnErrorIsDetectedCheckReceptionAfterDuration(TU16 wDuratioToWait);
        
        
        // F U N C T I O N S / M E T H O D S
        //
        //_________________________________________________________________________________________________
        /*!
        * \fn      void  RTSProtocol_TimeElapsed( void )
        * \brief   Event coming from the timer to say that duration asked is elapsed.
        *
        */
        void RTSProtocol_TimeElapsed(void)
        {
          START_DEBUG_DISPLAY_RTSPR(tiel)
          P_RTSProtocol_ACTION_Statechart(EVENT_TIME_ELAPSED);  //! Execute the statechart
          END_DEBUG_DISPLAY_RTSPR
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      void  RTSProtocol_Create( void )
        * \brief   Initialize the object.
        *
        */
        void RTSProtocol_Create(void)
        {
          BIRTH_OBJECT_RTSPR
          memset(&oRTSProtocol_sCurrentFrame,0,sizeof(TFrameStructure));
          memset(&oRTSProtocol_sFrameExpected,0,sizeof(TFrameStructure));
          memset(&oRTSProtocol_sLastAddress.btAddress,0xFF,sizeof(TAdresseU56));
          memset(&oRTSProtocol_sPendingFrame,0,sizeof(TRTSFrame));
          oRTSProtocol_sStimuliToSend.btTypeOfTransceiver  = RTSPROT_RC_UNKNOWN;
          oRTSProtocol_sStimuliToSend.btStateOfTransceiver = RTSPROT_UNRECORDED;
          oRTSProtocol_sStimuliToSend.btStateOfCommand     = MASK_FLOW_IN_PROGRESS | MASK_NEW_TRANSCEIVER ;
          oRTSProtocol_sStimuliToSend.btStimuliDuration    = 0U;
          oRTSProtocol_sStimuliToSend.btCommand1           = 0U;
          oRTSProtocol_sStimuliToSend.btCommand2           = 0U;
          oRTSProtocol_sStimuliToSend.btCommand3           = 0U;
          oRTSProtocol_sStimuliToSend.btCommand4           = 0U;
          oRTSProtocol_eProtocolState = STATE_NO_FLOW;
          
          // Instanctation of the bank
          TransceiverBank_Create();
          RTSProtocolRTSReceiverListener_Create();
        }
        
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      void RTSProtocol_Enable(void)
        * \brief   Start the radio receiver.
        *          ¿ªÊ¼ÎÞÏß½ÓÊÕ
        */
        void RTSProtocol_Enable(void)
        {
          START_DEBUG_DISPLAY_RTSPR(sta)
          RTSProtocolListener_K_OS_Save_Interrupts();   //¹Ø±ÕÖÐ¶Ï
          RTSProtocolRTSReceiverListener_Enable();      // Send the message to the listener
          RTSProtocolListener_K_OS_Restore_Interrupts();//Ê¹ÄÜÖÐ¶Ï
          END_DEBUG_DISPLAY_RTSPR
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      void RTSProtocol_Disable(void)
        * \brief   Stop the radio receiver.
        *          ¹Ø±ÕÎÞÏß½ÓÊÕ
        */
        void RTSProtocol_Disable(void)
        {
          START_DEBUG_DISPLAY_RTSPR(sto)
          RTSProtocolListener_K_OS_Save_Interrupts();
          RTSProtocolRTSReceiverListener_Disable();     // Send the message to the listener
          RTSProtocolListener_K_OS_Restore_Interrupts();
          END_DEBUG_DISPLAY_RTSPR
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      void RTSProtocol_EventProcessFrame(void)
        * \brief   Process the frame that has been copied in RTSProtocol_ProcessFrame.
        *
        */
        void RTSProtocol_EventProcessFrame(void)
        {
          START_DEBUG_DISPLAY_RTSPR(utfr)
          RTSProtocolListener_K_OS_Save_Interrupts();
          memcpy(&oRTSProtocol_sCurrentFrame,&oRTSProtocol_sPendingFrame.abtBuffer,sizeof(TFrameStructure));  // Copy the frame
          
          if (oRTSProtocol_sPendingFrame.btNbBitReceived >= (U80_FRAME_BYTE_NBR*8) )        // 80 bits -> it is a U80 frame
          {
            oRTSProtocol_btThisFrameIsU80 = TRUE;
          }
          else if (oRTSProtocol_sPendingFrame.btNbBitReceived >= (U56_FRAME_BYTE_NBR*8) )   // less than 80 but more than 56 -> U56
          {
            oRTSProtocol_btThisFrameIsU80 = FALSE;
          }
          
          RTSProtocolListener_K_OS_Restore_Interrupts();
          P_RTSProtocol_ACTION_Statechart(EVENT_FRAME_RECEPTION);     // Ask to execute the statechart
          END_DEBUG_DISPLAY_RTSPR
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      void RTSProtocol_ProcessFrame(TRTSFrame * ptFrame)
        * \brief   Called by RTSReceived interrupts. Say to protocol to keep this frame to use it later.
        *          An event is posted to process the frame in Task3.
        * \param   ptFrame : The frame to process.
        *
        */
        void RTSProtocol_ProcessFrame(TRTSFrame * ptFrame)
        {
          START_DEBUG_DISPLAY_RTSPR(ttf)
          memcpy(&oRTSProtocol_sPendingFrame,ptFrame,sizeof(TRTSFrame));  // Copy the frame
          RTSProtocol_SendEventProcessFrame(); // In order to stop time inside Interrupt
          END_DEBUG_DISPLAY_RTSPR
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      TBool RTSProtocol_IsBankEmpty(void)
        * \brief   Tells if the bank is empty.
        * \return  TRUE is bank is empty, FALSE if not.
        *
        */
        TBool RTSProtocol_IsBankEmpty(void)
        {
          TBool btReturn;
          TU8 i;
          START_DEBUG_DISPLAY_RTSPR(ibe)
            
            btReturn = TRUE;
          for (i=INDEX_FIRST_REMOTE_RECORDED;i<=INDEX_LAST_SENSOR_RECORDED;i++)
          {
            if (TransceiverBank_IsThisIndexEmpty(i)==FALSE)
            {
              btReturn = FALSE;
            }
          }
          END_DEBUG_DISPLAY_RTSPR
            return (btReturn);
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      TBool P_RTSProtocol_ACTION_DecryptAndVerifyChecksum(void)
        * \brief   Decrypt and verify the checksum of the frame.
        * \return  TRUE if the decrypted frame checksum is correct, else FALSE.
        *
        */
        TBool P_RTSProtocol_ACTION_DecryptAndVerifyChecksum(void)
        {
          TU8 i;
          TU8 btChecksum = 0U;
          TBool btRetVal = FALSE;    // Default result : bad checksum
          
          for ( i = U56_FRAME_BYTE_NBR-1; i > 0; i-- )
          {
            // Decrypt the frame and calculate the checksum
            oRTSProtocol_sCurrentFrame.btByte[i] = oRTSProtocol_sCurrentFrame.btByte[i] ^ oRTSProtocol_sCurrentFrame.btByte[i-1];
            btChecksum ^= oRTSProtocol_sCurrentFrame.btByte[i];
          }
          btChecksum ^= oRTSProtocol_sCurrentFrame.btByte[0];   // First byte is not crypted
          btChecksum ^= (( btChecksum << 4) | ( btChecksum >> 4));
          
          if ( btChecksum == 0 )     // result is 0
          {
            
            btRetVal = TRUE;         // -> Checksum is correct
          }
          //		    SET_BWUPFlag(!btRetVal);
          return( btRetVal );        // Return the test result
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      TBool P_RTSProtocol_ACTION_CheckNewRollingCode(void)
        * \brief   Check if the new rolling code can be accepted.
        * \return  TRUE : new rolling code is correct, else FALSE.
        *
        */
        TBool P_RTSProtocol_ACTION_CheckNewRollingCode(void)
        {
          TBool btRollingCodeCorrect = FALSE;
          TU8 btRetour;
          TU16 wLastRC;
          TU16 wRCSaved;
          TU16 wCurrentRC;
          TU16 wDeltaRC;
          
          btRetour = TransceiverBank_GiveIndexCurrent();
          if ( btRetour != 0)    // Current transceiver is recorded or temporary
          {
            wLastRC = TransceiverBank_ReadLastRollingCode(btRetour);
            wRCSaved =  TransceiverBank_ReadRollingCodeSaved(btRetour);
            wCurrentRC = ((TU16)oRTSProtocol_sCurrentFrame.Field.CPTH << 8);
            wCurrentRC += (oRTSProtocol_sCurrentFrame.Field.CPTL);
            wDeltaRC = wCurrentRC - wRCSaved;
            
            if ( (0 < wDeltaRC) && ( wDeltaRC < 500))
            {
              if (wDeltaRC < 10)
              {
                btRollingCodeCorrect = TRUE;
                TransceiverBank_WriteRollingCodeSaved(btRetour,wCurrentRC);
              }
              else
              {
                if (wCurrentRC != wLastRC) //anti-spy
                {
                  btRollingCodeCorrect = TRUE;
                }
                if (wCurrentRC == wLastRC+1)
                {
                  TransceiverBank_WriteRollingCodeSaved(btRetour,wCurrentRC);
                }
              }
              TransceiverBank_WriteLastRollingCode(btRetour,wCurrentRC);
            }
            else
            {
              // (wDeltaRC > 499 ) OU ( wDeltaRC == 0 ) :
              if ( ( (oRTSProtocol_sCurrentFrame.Field.RANDOM & MASK_COMMAND_IN_RANDOM) == PROG_INIS_RT)
                  && ( wCurrentRC <= 3))
              {
                TransceiverBank_WriteRollingCodeSaved(btRetour,wCurrentRC);
                TransceiverBank_WriteLastRollingCode(btRetour,wCurrentRC);  // Command is forbidden
                btRollingCodeCorrect = TRUE;
              }
              if ( ( (oRTSProtocol_sCurrentFrame.Field.RANDOM & MASK_COMMAND_IN_RANDOM) == CYCLE_4CMD_INIS_RT)
                  && ( wCurrentRC <= 10))
              {
                if (wCurrentRC == wLastRC+1)
                {
                  btRollingCodeCorrect = TRUE;
                  TransceiverBank_WriteRollingCodeSaved(btRetour,wCurrentRC);
                }
                TransceiverBank_WriteLastRollingCode(btRetour,wCurrentRC);  // Command is forbidden
              }
            }
          }
          else
          {
            btRollingCodeCorrect = TRUE;  // Command is accepted but transceiver is not recorded
          }
          return (btRollingCodeCorrect);
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      TU8 P_RTSProtocol_ACTION_GiveTypeOfTransceiver(void)
        * \brief   Give the type of transceiver which send the frame.
        * \return  TU8 : the type of transceiver.
        *
        */
        TU8 P_RTSProtocol_ACTION_GiveTypeOfTransceiver(void)
        {
          TU8 btRetourType;
          TU8 btCurrentTransceiver;
          TU16 wTemp;
          
          // Analyse the frame and set btRetour
          btRetourType = RTS_FRAME_INVALID;
          btCurrentTransceiver = RTSPROT_RC_UNKNOWN;
          
          if ((oRTSProtocol_sCurrentFrame.Field.RANDOM == 0)      // Random = 0
              &&(oRTSProtocol_sCurrentFrame.Field.ADDR.btAddress[INDEX_ADDR1] == 0x00)   // 0x00 <= Adress <= 0x2FF
                &&(oRTSProtocol_sCurrentFrame.Field.ADDR.btAddress[INDEX_ADDR2] <  0x30))
          {
            btRetourType = RTS_FACTORY_FRAME_CRYPTED;
            btCurrentTransceiver = RTSPROT_FACTORY_CRYPTED;
          }
          
          //RTDS frame are ignored
          else if (oRTSProtocol_sCurrentFrame.Field.ADDR.btAddress[INDEX_ADDR1] == 0xD7)   // 0xD70000 <= Adress <= 0xD7FFFF
          {
            btRetourType = RTDS_FRAME_NO_RC_CHECK;
          }
          else if (oRTSProtocol_sCurrentFrame.Field.ADDR.btAddress[INDEX_ADDR1] == 0xCC)   // 0xCC0000 <= Adress <= 0xCCFFFF
          {
            btRetourType = RTDS_FRAME_WITH_RC_CHECK;
          }
          
          else if ((oRTSProtocol_sCurrentFrame.Field.CKSTR & MASK_COMMAND_IN_CKSTR) == 0xE0)   // SENSOR frame
          {
            if (oRTSProtocol_sCurrentFrame.Field.RANDOM == MARK_SOMFY_IN_RANDOM)       // Random == 0xA0 for SOMFY sensors
            {
              btRetourType = RTS_CHANGING_CODE;          // Only SOMFY Sensor are accepted
              btCurrentTransceiver = RTSPROT_SENSOR;
            }
          }
          else if ((oRTSProtocol_sCurrentFrame.Field.CKSTR & MASK_COMMAND_IN_CKSTR) == 0xF0)   // 1 Button frame
          {
            //extended frame
            if ((oRTSProtocol_sCurrentFrame.Field.RANDOM & MASK_MARK_IN_RANDOM) == MARK_SOMFY_IN_RANDOM)
            {
              btRetourType = RTS_EXTENDED_COMMAND;      // Only SOMFY 1 button Remote control are accepted
              btCurrentTransceiver = RTSPROT_RC_1BUTTON;
            }
            else if((oRTSProtocol_sCurrentFrame.Field.RANDOM & MASK_MARK_IN_RANDOM) == MARK_SPRING_IN_RANDOM)
            {
              btRetourType = RTS_EXTENDED_COMMAND;      // Only Spring 2 button Remote control are accepted
              btCurrentTransceiver = RTSPROT_RC_SPRING2BUTTON;
            }
            
          }
          else                                          // RTS standard frame
          {
            btRetourType = RTS_STANDARD;
            btCurrentTransceiver = RTSPROT_RC_3BUTTON;
          }
          
          // Add the current transceiver in the bank
          wTemp = ((TU16)oRTSProtocol_sCurrentFrame.Field.CPTH << 8);
          wTemp += oRTSProtocol_sCurrentFrame.Field.CPTL;
          TransceiverBank_AddCurrentTransceiver(btCurrentTransceiver,wTemp,oRTSProtocol_sCurrentFrame.Field.ADDR);
          return (btRetourType);        // Return transceiver type
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      TBool P_RTSProtocol_ACTION_CheckIfFrameReceptionInProgress(void)
        * \brief   Check if the frame reception in progress correspond to last frame.
        * \return  TRUE if this frame is corresponding, else FALSE.
        *
        */
        TBool P_RTSProtocol_ACTION_CheckIfFrameReceptionInProgress(void)
        {
          TBool btReturn;
          
          btReturn=FALSE;             // Default : frame are differents
          
          if (RTSReceiver_HowMuchBitAreReceived() > NB_BIT_CHECK_RX_U56 )  // 30 bits should be received
          {
            if (RTSReceiver_AreYourBitsIdenticalToThisBits(oRTSProtocol_sFrameExpected.btByte ,NB_BIT_CHECK_RX_U56/8)==TRUE)
            {
              btReturn=TRUE;           // Same 30 first bits : same frames
            }
          }
          return (btReturn);
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      TBool P_RTSProtocol_ACTION_CheckIfFrameIsThisExpected(void)
        * \brief   Verify if the new frame is similar to frame expected.
        * \return  TRUE if frame is similar, else FALSE.
        *
        */
        TBool P_RTSProtocol_ACTION_CheckIfFrameIsThisExpected(void)
        {
          TU8 btTemp;
          TBool btReturn;
          
          btReturn = TRUE;
          
          if (memcmp (&oRTSProtocol_sCurrentFrame,&oRTSProtocol_sFrameExpected,U56_FRAME_BYTE_NBR)!=0)
          {
            btReturn = FALSE;
          }
          else if ( oRTSProtocol_btThisFrameIsU80 == TRUE )  // U80
          {
            btTemp = oRTSProtocol_sCurrentFrame.Field.U80_BYTE1 ^ oRTSProtocol_sCurrentFrame.Field.U80_BYTE2;
            btTemp ^= oRTSProtocol_sCurrentFrame.Field.U80_BYTE3;
            if ( (btTemp & 0x0F) == (btTemp>>4) )    // Chk correct
            {
              if ((oRTSProtocol_sCurrentFrame.Field.U80_BYTE1 & MASK_U80_BIT_RELEASED) != MASK_U80_BIT_RELEASED)
              {
                btReturn = FALSE;   // Bit is reset : frame is not active
              }
            }
            else
            {
              btReturn = FALSE;     // Checksum is incorrect
            }
          }
          return (btReturn);
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      void P_RTSProtocol_ACTION_SendReleasedStimuli(void)
        * \brief   Send released stimuli.
        *
        */
        void P_RTSProtocol_ACTION_SendReleasedStimuli(void)
        {
          oRTSProtocol_sStimuliToSend.btStateOfCommand &= ~MASK_FLOW_IN_PROGRESS ;  // Change state of the command
          P_RTSProtocol_ACTION_SendAgainLastStimuli();                            // Send the stimuli
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      void P_RTSProtocol_ACTION_SendStimuli(TU8 btTypeOfFrame)
        * \brief   Send a stimuli to order provider.
        * \param   btTypeOfFrame : The type of the frame.
        *
        */
        void P_RTSProtocol_ACTION_SendStimuli(TU8 btTypeOfFrame)
        {
          TU8 btIndex;
          
          oRTSProtocol_sStimuliToSend.btCommand1 = 0U;                // Reset last command
          oRTSProtocol_sStimuliToSend.btCommand2 = 0U;                // Reset last parameter
          oRTSProtocol_sStimuliToSend.btCommand3 = 0U;                // Reset last parameter
          oRTSProtocol_sStimuliToSend.btCommand4 = 0U;                // Reset last parameter
          oRTSProtocol_sStimuliToSend.btStateOfCommand = 0U;          // Reste flags of last command state
          //		    SET_BWUPFlag(TRUE);
          btIndex = TransceiverBank_GiveIndexCurrent();
          oRTSProtocol_sStimuliToSend.btStateOfTransceiver = RTSPROT_UNRECORDED;
          if (btIndex==INDEX_TEMPORARY_REMOTE)
          {
            oRTSProtocol_sStimuliToSend.btStateOfTransceiver = RTSPROT_TEMPORARY;
            //			SET_BWUPFlag(FALSE);
          }
          else if ((btIndex>=INDEX_FIRST_REMOTE_RECORDED) && (btIndex<=INDEX_LAST_REMOTE_RECORDED))    // Adress recorded as a remote
          {
            oRTSProtocol_sStimuliToSend.btStateOfTransceiver = RTSPROT_RECORDED;
            //			SET_BWUPFlag(FALSE);
          }
          else if ((btIndex>=INDEX_FIRST_SENSOR_RECORDED) && (btIndex<=INDEX_LAST_SENSOR_RECORDED)) // Address is recorded as a sensor
          {
            if (btTypeOfFrame == RTS_CHANGING_CODE )							// ans it is a sensor
            {
              oRTSProtocol_sStimuliToSend.btStateOfTransceiver = RTSPROT_RECORDED;
              //			    SET_BWUPFlag(FALSE);
            }
          }
          
          TRemoteControl *Rc = TransceiverBank_GetRemoteControlBank(INDEX_CURRENT_REMOTE);
          switch (btTypeOfFrame)
          {
          case RTS_FACTORY_FRAME_CRYPTED :
            oRTSProtocol_sStimuliToSend.btTypeOfTransceiver = RTSPROT_FACTORY_CRYPTED;
            oRTSProtocol_sStimuliToSend.btCommand1 = (MASK_COMMAND_IN_CKSTR & oRTSProtocol_sCurrentFrame.Field.CKSTR) >> OFFSET_COMMAND_IN_CKSTR;
            oRTSProtocol_sStimuliToSend.btCommand2 = oRTSProtocol_sCurrentFrame.Field.CPTH;
            oRTSProtocol_sStimuliToSend.btCommand3 = oRTSProtocol_sCurrentFrame.Field.CPTL;
            break;
            
          case RTS_FACTORY_FRAME_UNCRYPTED :
            oRTSProtocol_sStimuliToSend.btTypeOfTransceiver = RTSPROT_FACTORY_UNCRYPTED;
            oRTSProtocol_sStimuliToSend.btCommand1 = oRTSProtocol_sCurrentFrame.Field.ADDR.btAddress[INDEX_ADDR2];
            oRTSProtocol_sStimuliToSend.btCommand2 = oRTSProtocol_sCurrentFrame.Field.ADDR.btAddress[INDEX_ADDR3];
            oRTSProtocol_sStimuliToSend.btCommand3 = oRTSProtocol_sCurrentFrame.Field.CPTL;
            oRTSProtocol_sStimuliToSend.btCommand4 = oRTSProtocol_sCurrentFrame.Field.CPTH;
            break;
            
          case RTS_CHANGING_CODE :
            oRTSProtocol_sStimuliToSend.btTypeOfTransceiver = RTSPROT_SENSOR;
            oRTSProtocol_sStimuliToSend.btCommand1 = oRTSProtocol_sCurrentFrame.Field.CPTL;
            break;
            
          case RTS_EXTENDED_COMMAND :
            oRTSProtocol_sStimuliToSend.btTypeOfTransceiver =  Rc->RemoteControlPersistent.btType ;
            oRTSProtocol_sStimuliToSend.btCommand1 = (oRTSProtocol_sCurrentFrame.Field.RANDOM & MASK_COMMAND_IN_RANDOM);
#warning "Modfif MG 01/2016"
            // Extended cmd have U80 field to !
            oRTSProtocol_sStimuliToSend.btCommand2 = oRTSProtocol_sCurrentFrame.Field.U80_BYTE1;
            oRTSProtocol_sStimuliToSend.btCommand3 = oRTSProtocol_sCurrentFrame.Field.U80_BYTE2;
            oRTSProtocol_sStimuliToSend.btCommand4 = oRTSProtocol_sCurrentFrame.Field.U80_BYTE3;
            break;
            
          case RTDS_FRAME_NO_RC_CHECK :
          case RTDS_FRAME_WITH_RC_CHECK :
            oRTSProtocol_sStimuliToSend.btTypeOfTransceiver = RTSPROT_RC_RTDS;
            oRTSProtocol_sStimuliToSend.btCommand1 = (MASK_COMMAND_IN_CKSTR & oRTSProtocol_sCurrentFrame.Field.CKSTR) >> OFFSET_COMMAND_IN_CKSTR;
            break;
            
          case RTS_STANDARD :        //±ê×¼RTSÊý¾ÝÖ¡
            oRTSProtocol_sStimuliToSend.btTypeOfTransceiver = RTSPROT_RC_3BUTTON;
            oRTSProtocol_sStimuliToSend.btCommand1 = (MASK_COMMAND_IN_CKSTR & oRTSProtocol_sCurrentFrame.Field.CKSTR) >> OFFSET_COMMAND_IN_CKSTR;
            oRTSProtocol_sStimuliToSend.btCommand2 = oRTSProtocol_sCurrentFrame.Field.U80_BYTE1;
            oRTSProtocol_sStimuliToSend.btCommand3 = oRTSProtocol_sCurrentFrame.Field.U80_BYTE2;
            oRTSProtocol_sStimuliToSend.btCommand4 = oRTSProtocol_sCurrentFrame.Field.U80_BYTE3;
            break;
            
          case RTS_FRAME_INVALID :   //²»È«µÄÊý¾ÝÖ¡
          default :
            oRTSProtocol_sStimuliToSend.btTypeOfTransceiver = RTSPROT_RC_UNKNOWN;
            break;
          }
          
          if (oRTSProtocol_btThisFrameIsU80==FALSE)
          {
            oRTSProtocol_sStimuliToSend.btCommand2 = 0U;                // Reset last parameter
            oRTSProtocol_sStimuliToSend.btCommand3 = 0U;                // Reset last parameter
            oRTSProtocol_sStimuliToSend.btCommand4 = 0U;                // Reset last parameter
          }
          
          // Test if the transceiver is the same that last transceiver
          if (memcmp(&oRTSProtocol_sLastAddress,&oRTSProtocol_sCurrentFrame.Field.ADDR.btAddress[INDEX_ADDR3],sizeof(TAdresseU56)) != 0)
          {
            oRTSProtocol_sStimuliToSend.btStateOfCommand |= MASK_NEW_TRANSCEIVER;  // Set new transceiver
          }
          
          // Save adress of current transceiver
          memcpy(&oRTSProtocol_sLastAddress,&oRTSProtocol_sCurrentFrame.Field.ADDR.btAddress[INDEX_ADDR3],sizeof(TAdresseU56));     // Save the Address
          
          oRTSProtocol_sStimuliToSend.btStateOfCommand    |= MASK_FLOW_IN_PROGRESS;
          oRTSProtocol_sStimuliToSend.btStimuliDuration++;
          RTSProtocolListener_ProcessStimuli(&oRTSProtocol_sStimuliToSend);
          //ÎÞÏß½ÓÊÕÖ®ºó´¦Àí sage
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      void P_RTSProtocol_ACTION_SendAgainLastStimuli(void)
        * \brief   Send again last stimuli increasing frame counter if necessary.
        *
        */
        void P_RTSProtocol_ACTION_SendAgainLastStimuli(void)
        {
#warning "Modfif MG 01/2016"
          // Update U80 field 
          oRTSProtocol_sStimuliToSend.btCommand2 = oRTSProtocol.sCurrentFrame.Field.U80_BYTE1;
          oRTSProtocol_sStimuliToSend.btCommand3 = oRTSProtocol_sCurrentFrame.Field.U80_BYTE2;
          oRTSProtocol_sStimuliToSend.btCommand4 = oRTSProtocol_sCurrentFrame.Field.U80_BYTE3;
          // Update stimuli duration
          oRTSProtocol_sStimuliToSend.btStimuliDuration++;  
          RTSProtocolListener_ProcessStimuli(&oRTSProtocol_sStimuliToSend);
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      void P_RTSProtocol_ACTION_Statechart(TU8 btEvent)
        * \brief   Statechart of the protocol object.
        * \param   btEvent : The event which imply statechart evolution.
        *
        */
        void P_RTSProtocol_ACTION_Statechart(TU8 btEvent)
        {
          TU8 btReturn;
          
          switch (oRTSProtocol_eProtocolState)
          {
          case STATE_NO_FLOW :  //No flow in progress Ã»ÓÐÊý¾ÝÖ¡´¦Àí
            if (btEvent == EVENT_FRAME_RECEPTION)
            {
              if ((oRTSProtocol_btThisFrameIsU80==TRUE)
                  && ((oRTSProtocol_sCurrentFrame.Field.U80_BYTE1 & MASK_U80_BIT_RELAY) != MASK_U80_BIT_RELAY ))
              {
                // Nothing : relay bit is not correct
              }
              else
              {
                oRTSProtocol_sStimuliToSend.btStimuliDuration = 0U;                     // Reset frame counter
                oRTSProtocol_btStimuliCounterToRestore = 0U;
                memcpy(&oRTSProtocol_sFrameExpected,&oRTSProtocol_sCurrentFrame,sizeof(TFrameStructure) );
                if ( (oRTSProtocol_sCurrentFrame.Field.RANDOM == 0 )
                    && ( (oRTSProtocol_sCurrentFrame.Field.CKSTR & MASK_COMMAND_IN_CKSTR) == 0xF0 )
                      && (oRTSProtocol_sCurrentFrame.Field.ADDR.btAddress[INDEX_ADDR1] == 0xFE ) )
                {
                  // Frame is not crypted Êý¾ÝÖ¡Ã»ÓÐ¼ÓÃÜ
                  TransceiverBank_AddCurrentTransceiver(RTSPROT_FACTORY_UNCRYPTED,0,oRTSProtocol_sCurrentFrame.Field.ADDR);
                  P_RTSProtocol_ACTION_SendStimuli(RTS_FACTORY_FRAME_UNCRYPTED);
                  oRTSProtocol_eProtocolState = STATE_FLOW_WITHOUT_ERROR_WAIT_TO_CHECK_RX;
                  RTSProtocolShortCounter_NotifyMeAfter( DURATION_FOR_RX_CHECKING );   // Ask to the counter to send a message later
                }
                else   // Frame is crypted  Êý¾ÝÖ¡ÒÑ¾­½âÃÜ
                {
                  if (P_RTSProtocol_ACTION_DecryptAndVerifyChecksum() == TRUE)
                  {
                    btReturn = P_RTSProtocol_ACTION_GiveTypeOfTransceiver();
                    
                    if ((btReturn ==RTS_EXTENDED_COMMAND)
                        || (btReturn ==RTS_STANDARD)
                          || (btReturn ==RTDS_FRAME_WITH_RC_CHECK))
                    {
                      //// It is necessary to verify the RollingCode                      
                      if (P_RTSProtocol_ACTION_CheckNewRollingCode()==TRUE)
                      {//Ð£Ñé¹ö¶¯Âë
                        P_RTSProtocol_ACTION_SendStimuli(btReturn);
                        oRTSProtocol_eProtocolState = STATE_FLOW_WITHOUT_ERROR_WAIT_TO_CHECK_RX;
                        RTSProtocolShortCounter_NotifyMeAfter( DURATION_FOR_RX_CHECKING );   // Ask to the counter to send a message later
                      }
                    }
                    else if ( btReturn != RTS_FRAME_INVALID )  //No RollingCode checking
                    { //ÎÞÐ§µÄRTSÊý¾ÝÖ¡  
                      P_RTSProtocol_ACTION_SendStimuli(btReturn);
                      oRTSProtocol_eProtocolState = STATE_FLOW_WITHOUT_ERROR_WAIT_TO_CHECK_RX;
                      RTSProtocolShortCounter_NotifyMeAfter( DURATION_FOR_RX_CHECKING );   // Ask to the counter to send a message later
                    }
                  }
                }
              }
            }
            break;
            
          case STATE_FLOW_WITHOUT_ERROR_WAIT_TO_CHECK_RX :
            //!< Wait 100ms to verify if reception in progress.  Êý¾ÝÖ¡Ã»ÓÐ´íÎó
            if (btEvent == EVENT_TIME_ELAPSED)
            {
              if ( P_RTSProtocol_ACTION_CheckIfFrameReceptionInProgress()== TRUE)
              {
                RTSProtocolShortCounter_NotifyMeAfter(  TIME_TO_WAIT_BETWEEN_RX_CHECK_AND_NEW_FRAME );   // Ask to the counter to send a message later
                oRTSProtocol_eProtocolState = STATE_FLOW_WITHOUT_ERROR_WAIT_NEW_FRAME;
              }
              else     // No frame reception actually
              {
                P_RTSProtocol_ACTION_AnErrorIsDetectedCheckReceptionAfterDuration(DURATION_FOR_RX_CHECKING+TIME_TO_WAIT_BETWEEN_RX_CHECK_AND_NEW_FRAME);
              }
            }
            break;
            
          case STATE_FLOW_WITHOUT_ERROR_WAIT_NEW_FRAME :
            //!< Wait a new frame.   Êý¾ÝÖ¡Ã»ÓÐ´íÎó µÈ´ýÐÂµÄÖ¡
            if (btEvent == EVENT_FRAME_RECEPTION)
            {
              if ( P_RTSProtocol_ACTION_CheckIfFrameIsThisExpected() == TRUE)
              {
                P_RTSProtocol_ACTION_SendAgainLastStimuli();
                oRTSProtocol_eProtocolState = STATE_FLOW_WITHOUT_ERROR_WAIT_TO_CHECK_RX;
                RTSProtocolShortCounter_NotifyMeAfter( DURATION_FOR_RX_CHECKING );   // Ask to the counter to send a message later
              }
              else
              {
                P_RTSProtocol_ACTION_AnErrorIsDetectedCheckReceptionAfterDuration(DURATION_FOR_RX_CHECKING);
              }
            }
            else if ( btEvent == EVENT_TIME_ELAPSED )
            {
              P_RTSProtocol_ACTION_AnErrorIsDetectedCheckReceptionAfterDuration(DURATION_FOR_RX_CHECKING);
            }
            break;
            
          case STATE_FLOW_WITH_AN_ERROR_WAIT_TO_CHECK_RX :
            //!< Wait 100ms to verify if reception in progress.  Êý¾ÝÖ¡ÓÐ´íÎó
            if (btEvent == EVENT_TIME_ELAPSED)
            {
              if ( P_RTSProtocol_ACTION_CheckIfFrameReceptionInProgress()== TRUE)
              {
                RTSProtocolShortCounter_NotifyMeAfter(  TIME_TO_WAIT_BETWEEN_RX_CHECK_AND_NEW_FRAME );   // Ask to the counter to send a message later
                oRTSProtocol_eProtocolState = STATE_FLOW_WITH_AN_ERROR_WAIT_NEW_FRAME;
              }
              else     // No frame reception actually
              {
                P_RTSProtocol_ACTION_EndOfFlow();
              }
            }
            if (btEvent == EVENT_FRAME_RECEPTION)
            {
              if ( P_RTSProtocol_ACTION_CheckIfFrameIsThisExpected() == TRUE)
              {
                P_RTSProtocol_ACTION_TakeAgainCurrentFlow();
              }
              else
              {
                P_RTSProtocol_ACTION_EndOfFlow();
              }
            }
            break;
            
          case STATE_FLOW_WITH_AN_ERROR_WAIT_NEW_FRAME :
             //!< Wait a new frame.   Êý¾ÝÖ¡ÓÐ´íÎó µÈ´ýÐÂµÄÖ¡
            if (btEvent == EVENT_FRAME_RECEPTION)
            {
              if ( P_RTSProtocol_ACTION_CheckIfFrameIsThisExpected() == TRUE)
              {
                P_RTSProtocol_ACTION_TakeAgainCurrentFlow();
              }
              else
              {
                P_RTSProtocol_ACTION_EndOfFlow();
              }
            }
            else if ( btEvent == EVENT_TIME_ELAPSED )
            {
              P_RTSProtocol_ACTION_EndOfFlow();
            }
            break;
            
          default :
            break;
          }
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      void  P_RTSProtocol_ACTION_EndOfFlow(void)
        * \brief   End of flow is detected.
        *
        */
        void  P_RTSProtocol_ACTION_EndOfFlow(void)
        {
          oRTSProtocol_eProtocolState = STATE_NO_FLOW;
          P_RTSProtocol_ACTION_SendReleasedStimuli();
          RTSProtocolShortCounter_StopCounting();                  // Disable the timer
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      void  P_RTSProtocol_ACTION_TakeAgainCurrentFlow(void)
        * \brief   An error was deteted in last frame but this frame is correct flow is countinued.
        *
        */
        void  P_RTSProtocol_ACTION_TakeAgainCurrentFlow(void)
        {
          oRTSProtocol_sStimuliToSend.btStimuliDuration = oRTSProtocol_btStimuliCounterToRestore;
          SET_BIT(oRTSProtocol_sStimuliToSend.btStateOfCommand, BIT_FLOW_IN_PROGRESS); // Change state of the command
          P_RTSProtocol_ACTION_SendAgainLastStimuli();
          oRTSProtocol_eProtocolState = STATE_FLOW_WITHOUT_ERROR_WAIT_TO_CHECK_RX;
          RTSProtocolShortCounter_NotifyMeAfter( DURATION_FOR_RX_CHECKING );   // Ask to the counter to send a message later
        }
        
        //_________________________________________________________________________________________________
        /*!
        * \fn      void  P_RTSProtocol_ACTION_AnErrorIsDetectedCheckReceptionAfterDuration(TU16 wDuratioToWait)
        * \brief   An error is detected in the flow : we wait a duration to check if flow can countinue.
        * \param   wDuratioToWait : Duration to wait before reception checking.
        */
        void  P_RTSProtocol_ACTION_AnErrorIsDetectedCheckReceptionAfterDuration(TU16 wDuratioToWait)
        {
          oRTSProtocol_eProtocolState = STATE_FLOW_WITH_AN_ERROR_WAIT_TO_CHECK_RX;
          RTSProtocolShortCounter_NotifyMeAfter(  wDuratioToWait );   // Ask to the counter to send a message later
          oRTSProtocol_btStimuliCounterToRestore = oRTSProtocol_sStimuliToSend.btStimuliDuration;
          oRTSProtocol_sStimuliToSend.btStimuliDuration = 0U;                      // Reset frame counter
          P_RTSProtocol_ACTION_SendReleasedStimuli();
        }
        
#endif
        