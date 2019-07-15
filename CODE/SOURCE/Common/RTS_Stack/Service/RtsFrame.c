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
#include <stdio.h> 
#include "Config.h"
#include <string.h>

// I N T E R F A C E S
//
#include "RTSFrame.h"


// C O N S T A N T S
//

// S T A T E   M A C H I N E ( S )
//


// V A R I A B L E S
//
//! The frame buffer.
TRTSFrame oRTSFrame;

// P R I V A T E   P R O T O T Y P E S
//

// F U N C T I O N S / M E T H O D S
//
//_________________________________________________________________________________________________
/*!
 * \fn      TRTSFrame * RTSFrame_Create (void)
 * \brief   Initilialize and return the frame buffer.
 * \return  A pointer on the frame buffer.
 */
TRTSFrame * RTSFrame_Create (void)
{  
  oRTSFrame.btNbBitReceived = 0U;
  memset(oRTSFrame.abtBuffer,0,FRAME_BYTE_NBR);
  return ( &oRTSFrame);  
}

#endif
