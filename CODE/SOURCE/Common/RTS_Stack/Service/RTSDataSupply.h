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

#ifndef _RTS_DATA_SUPPLY_H_
#define _RTS_DATA_SUPPLY_H_

// I N T E R F A C E S
//
#include "SFY_Types.h"

// O B J E C T   S T R U C T U R E
// 

// C O N S T A N T S
//

// C L A S S   A T T R I B U T E S
//


// C L A S S   M E T H O D S ( P U B L I C   P R O T O T Y P E S )
//
void RTSDataSupply_Create( void );
void RTSDataSupply_EdgeDetected( void );//edge ´¥·¢³ÌÐò£¬ÅÐ¶ÏÉÏÏÂÉýÑØ
void RTSDataSupply_StartFrameReception( void );
void RTSDataSupply_StopFrameReception( void );
void RTSDataSupply_TimeoutDetected(void);
void RTSDataSupply_WaitSynchroSoftware( void );
TBool RTSDataSupply_PresenceOfRTSFragment( void );//ÓÐRTSÊý¾ÝÖ¡


// C L A S S   I N S T A N C E S
//


#endif /* _RTS_DATA_SUPPLY_H_ */






















