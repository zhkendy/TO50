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
/ **********************************************************************/
#ifndef __COUNTER_H__
#define __COUNTER_H__

#include "Config.h"

//! Counter ids.
enum
{
  ACTUATOR_LONGCOUNTER_ID = 0x00,         //!< Actuator counter.
  EMSORDERPROVIDER_LONGCOUNTER_ID,        //!< EmsOrderProvider counter.
  BANKER_LONGCOUNTER_ID,									//!< Banker counter.
  RTSTESTORDERPROVIDER_LONGCOUNTER_ID,		//!< RTSTestOrderProvider counter.
  WEATHERCONTROL_LONGCOUNTER_ID,					//!< WeatherControl counter.
  SUNCONTROL_LONGCOUNTER_ID,							//!< SunControl counter.
  WINDCONTROL_LONGCOUNTER_ID,							//!< WindControl counter.
  KEYORDERPROVIDE_LONGCOUNTER_ID,         //!< KeyOrderProvider counter
  MAX_NB_OF_COUNTER												//!< Max number of counter.
};

//! Resolution of counter is 1 second.
#define COUNTER_RESOLUTION_MS         (1000)

#define COUNTER_DURATION_FOR_1S       (1000/COUNTER_RESOLUTION_MS      )
#define COUNTER_DURATION_FOR_5S       (5000/COUNTER_RESOLUTION_MS      )
#define COUNTER_DURATION_FOR_10S      (10000/COUNTER_RESOLUTION_MS     )
#define COUNTER_DURATION_FOR_12S      (12000/COUNTER_RESOLUTION_MS     )
#define COUNTER_DURATION_FOR_60S      (60000/COUNTER_RESOLUTION_MS     )
#define COUNTER_DURATION_FOR_1MN      (COUNTER_DURATION_FOR_60S        )
#define COUNTER_DURATION_FOR_2MN      (2*COUNTER_DURATION_FOR_60S      )
#define COUNTER_DURATION_FOR_3MN      (3*COUNTER_DURATION_FOR_60S      )
#define COUNTER_DURATION_FOR_6MN      (6*COUNTER_DURATION_FOR_60S      )
#define COUNTER_DURATION_FOR_10MN     (10*COUNTER_DURATION_FOR_60S     )
#define COUNTER_DURATION_FOR_11MN5S   (11.5*COUNTER_DURATION_FOR_60S   )
#define COUNTER_DURATION_FOR_15MN     (15*COUNTER_DURATION_FOR_60S     )
#define COUNTER_DURATION_FOR_20MN     (20*COUNTER_DURATION_FOR_60S     )
#define COUNTER_DURATION_FOR_25MN     (25*COUNTER_DURATION_FOR_60S     )
#define COUNTER_DURATION_FOR_30MN     (30*COUNTER_DURATION_FOR_60S     )


void Counter_InitClass(void);
void Counter_Create(TU8 btCounterIdArg, void (*fpCallBackArg)(void));
void Counter_NotifyMeAfter(TU8 btCounterIdArg, TU16 wDurationArg);
void Counter_StopCounting(TU8 btCounterIdArg);
void Counter_Decrement(void);

#endif //__COUNTER_H__

