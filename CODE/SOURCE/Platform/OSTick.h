/**********************************************************************
This document and/or file is SOMFY’s property. All information it
contains is strictly confidential. This document and/or file shall
not be used, reproduced or passed on in any way, in full or in part
without SOMFY’s prior written approval. All rights reserved.
Ce document et/ou fichier est la propriété de SOMFY. Les informations
qu’il contient sont strictement confidentielles. Toute reproduction,
utilisation, transmission de ce document et/ou fichier, partielle ou
intégrale, non autorisée préalablement par SOMFY par écrit est
interdite. Tous droits réservés.
***********************************************************************/


//---------------------------------------------------------------------
// File "OSTick.h"
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef OSTICK_H
#define OSTICK_H

#include "stm8s_tim4.h"
#include "stm8s_clk.h"
#include "stm8s_itc.h"




// Constants
//--------------
// Timer prescalers
//#define OS_TIMER_PRESCALER      1
//
//#define DURATION_TIMER_OS_1MS   (((CPU_CLOCK_SPEED)/1000)/OS_TIMER_PRESCALER)
//#define DURATION_TIMER_OS_500uS (DURATION_TIMER_OS_1MS/2)
//#define DURATION_TIMER_100uS    (((CPU_CLOCK_SPEED)/1000)/10)
//#define DURATION_TIMER_500uS    (((CPU_CLOCK_SPEED)/1000)/2)


// Public types
//--------------


// Public functions declaration
//-----------------------------------

void OSTick_Init( void );
void OSTick_Disable( void );


void OSTick_IRQHandler(void);


// Header end
//--------------

#endif // OSTICK_H

/**********************************************************************
Copyright © (2010), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
