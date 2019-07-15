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
/ ATTRIBUTS : 
/ Public : 
/ 
/ Private : 
/
/ ********************************************************************* 
/ INLINE METHODS : 
/ Public : 
/ 
/ Private : 
/
/ ********************************************************************* 
/ METHODS : 
/ Public : 
/ 
/ Private : 
/
/ ********************************************************************* 
/ AUTHOR : FlG
/ DATE :11/08/2006
/ ********************************************************************* */

#ifndef _RTS_RECEIVER_H_
#define _RTS_RECEIVER_H_

// I N T E R F A C E S
//
#include "Config.h"
#include "RTSFrame.h"

// O B J E C T   S T R U C T U R E
// 

// C O N S T A N T S
//
//! Macro to add a 0 bit in the frame.
#define  RTSReceiver_Bit0Detected()  RTSReceiver_AddABit(0)
//! Macro to add a 1 bit in the frame.
#define  RTSReceiver_Bit1Detected()	 RTSReceiver_AddABit(1)

// C L A S S   M E T H O D S   ( P U B L I C   P R O T O T Y P E S )
//
void  RTSReceiver_AddABit(TU8 btBitValue);//½ÓÊÕ×ã¹»µÄ×Ö½Úºó½âÂë
TBool RTSReceiver_AreYourBitsIdenticalToThisBits(TU8 * abtBitsToTest , TU8 btNumberOfBits);//ÅÐ¶ÏÍ¬²½P_RTSProtocol_ACTION_CheckIfFrameReceptionInProgress
void  RTSReceiver_Create( void);
void  RTSReceiver_Disable( void );
void  RTSReceiver_Enable( void );
void  RTSReceiver_EndOfFrame(void);
TU8   RTSReceiver_HowMuchBitAreReceived(void);
void  RTSReceiver_SynchroSoftware( void );//The software synchronisation is detected in the RF signal.


// C L A S S   I N S T A N C E S
//

#endif /* _RTS_RECEIVER_H_ */
