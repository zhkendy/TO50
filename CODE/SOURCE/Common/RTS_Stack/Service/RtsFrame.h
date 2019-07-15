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

#ifndef _RTS_FRAME_H_
#define _RTS_FRAME_H_

// I N T E R F A C E S
//
#include "Config.h"

// O B J E C T   S T R U C T U R E
// 

// C O N S T A N T S
//Max number of bytes in a frame.
#define FRAME_BYTE_NBR  10            
//! Number of bytes in a U56 frame.
#define U56_FRAME_BYTE_NBR  7          
//! Number of bytes in a U80 frame.
#define U80_FRAME_BYTE_NBR  10        

//! Type of frame.
enum 
{
  UNKNOWN_FRAME,      //!< Unknown type of frame.
  U56_FRAME,          //!< Frame is a U56 frame.
  U80_FRAME           //!< Frame is a U80 frame.
};

//! Byte in the frame buffer.
typedef union
{
  //! Value of the byte.
  TU8 value;        
  struct{              
  //! Value of the bit 0.
  TU8 b0 : 1;       
  //! Value of bit2 to bit6.
  TU8    : 6;       
  //! Value of the bit 7.
  TU8 b7 : 1;       
  }Bit;
}TByteFrame;

//! Attributes of RTSFrame.
typedef struct RTSFrameAttributes
{
  //! Number of bits currently in the frame buffer.
  TU8 btNbBitReceived;                  
  //! Buffer which contain current datas.
  TByteFrame abtBuffer[FRAME_BYTE_NBR];    
} TRTSFrame;

// C L A S S   M E T H O D S ( P U B L I C   P R O T O T Y P E S )
//
TRTSFrame * RTSFrame_Create (void);


// C L A S S   I N S T A N C E S
//

#endif /* _RTS_FRAME_H_ */
