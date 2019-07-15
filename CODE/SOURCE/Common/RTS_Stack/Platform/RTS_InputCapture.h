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
/ Modified by MG 02/2015
/ ********************************************************************* */

#pragma once

// I N T E R F A C E S
//
#include "stm8s_tim2.h"
#include "stm8s_gpio.h"
#include "stm8s_itc.h"
#include "stm8s_clk.h"

#include "RTSDataSupply.h"
#include "RTSReceiverInterface.h"



#define TMR2_INPUTCAPTURE_FILTER_LEVEL  0x01  //0x0F


// C O N S T A N T S
//

//! Tick duration of input capture timer.
#define US_PER_TIMER_TICK   (1)


////#define DURATION_FIRST_SHORT_TYP        (640)
////#define DURATION_LONG_TYP               (1280)
////#define DURATION_SYNCHRO_TYP            (4800)

////#define COEFF_FIRST_LONG_MIN        0.70   
////#define COEFF_FIRST_LONG_MAX        1.30 
//////! Coef to calculate min limit for long level. 
////#define COEFF_LONG_MIN              0.76     
//////! Coef to calculate max limit for long level. 
////#define COEFF_LONG_MAX              1.24    
//////! Coef to calculate min limit for first short level. 
////#define COEFF_FIRST_SHORT_MIN       0.70     
//////! Coef to calculate max limit for first short level. 
////#define COEFF_FIRST_SHORT_MAX       1.30  
//////! Coef to calculate min limit for short level. 
////#define COEFF_SHORT_MIN             0.73     
//////! Coef to calculate max limit for short level. 
////#define COEFF_SHORT_MAX             1.27 


//This stack doesn't measure Reco bit time, (because first RTS remote doesn't respect timing protocol),
//then, aditional tolerence was added on first bits of frame.
//So, it's normal to have a lab's test failure on reco bits timing

#define DEF_DURATION_LONG_MIN_US                   (930) //(920)
#define DEF_DURATION_LONG_MAX_US                   (1700)//(1640)

#define DEF_DURATION_SHORT_MIN_US                  (420)//(500)//(480)//(400)o//(350) //(455)
#define DEF_DURATION_SHORT_MAX_US                  (870)//(780)//(810)//(870)o//(925) //(820)

//6% more tolerence
#define DEF_DURATION_FIRST_LONG_MIN_US             (DEF_DURATION_LONG_MIN_US)//(865)
#define DEF_DURATION_FIRST_LONG_MAX_US             (DEF_DURATION_LONG_MAX_US)//(1739)
//3% more tolerence
#define DEF_DURATION_FIRST_SHORT_MIN_US            (DEF_DURATION_SHORT_MIN_US)//(440)
#define DEF_DURATION_FIRST_SHORT_MAX_US            (DEF_DURATION_SHORT_MAX_US)//(845)

#define DEF_DURATION_SYNCHRO_MIN_US                (3900)//(4000)
#define DEF_DURATION_SYNCHRO_MAX_US                (5550)//(5650) need

#define DEF_DURATION_SYNCHRO_HW_MIN_US             (2100)//(2150) need
#define DEF_DURATION_SYNCHRO_HW_MAX_US             (2900)//(2750) need

//Because of the 16µs resolution, short timing are placed in indeterminated zone
static const TU16 DURATION_SHORT_MAX =          (DEF_DURATION_SHORT_MAX_US)/(US_PER_TIMER_TICK);
static const TU16 DURATION_SHORT_MIN =          (DEF_DURATION_SHORT_MIN_US)/(US_PER_TIMER_TICK);

static const TU16 DURATION_FIRST_SHORT_MAX =    (DEF_DURATION_FIRST_LONG_MAX_US)/(US_PER_TIMER_TICK);
static const TU16 DURATION_FIRST_SHORT_MIN =    (DEF_DURATION_FIRST_SHORT_MIN_US)/(US_PER_TIMER_TICK);

static const TU16 DURATION_LONG_MAX =           (DEF_DURATION_LONG_MAX_US)/(US_PER_TIMER_TICK);
static const TU16 DURATION_LONG_MIN =           (DEF_DURATION_LONG_MIN_US)/(US_PER_TIMER_TICK);

static const TU16 DURATION_FIRST_LONG_MAX =     (DEF_DURATION_FIRST_LONG_MAX_US)/(US_PER_TIMER_TICK);
static const TU16 DURATION_FIRST_LONG_MIN =     (DEF_DURATION_FIRST_LONG_MIN_US)/(US_PER_TIMER_TICK);

static const TU16 DURATION_SYNCHRO_MAX =        (DEF_DURATION_SYNCHRO_MAX_US) /(US_PER_TIMER_TICK ) ;
static const TU16 DURATION_SYNCHRO_MIN =        (DEF_DURATION_SYNCHRO_MIN_US) / (US_PER_TIMER_TICK);

static const TU16 DURATION_SYNCHRO_HW_MIN =     (DEF_DURATION_SYNCHRO_HW_MIN_US) /(US_PER_TIMER_TICK) ;
static const TU16 DURATION_SYNCHRO_HW_MAX =     (DEF_DURATION_SYNCHRO_HW_MAX_US) /(US_PER_TIMER_TICK);

static const TU16 DURATION_LONG_TIMEOUT =       (2500) /(US_PER_TIMER_TICK);
static const TU16 DURATION_SYNCHRO_TIMEOUT =    (6000) /(US_PER_TIMER_TICK);

//! Low level on RADIO pin.
#define LOW_LEVEL (0u)
//! High level on RADIO pin.
#define HIGH_LEVEL (1u)


typedef struct InputCaptureAttributes {
  //! Next level expected on input capture RADIO pin.
  TU8 btInputLevelExpected;
} TInputCapture;

extern TInputCapture oInputCapture;
#define oInputCapture_btInputLevelExpected oInputCapture.btInputLevelExpected


// Macros objet INPUTCAPTURE_RADIO
//! Program the trigger to rising edge.
#define InputCapture_TriggerOnRisingEdge()\
{\
    TIM2_ICInit(INPUT_CAPTURE_CHANNEL,TIM2_ICPOLARITY_RISING,TIM2_ICSELECTION_DIRECTTI,TIM2_ICPSC_DIV1,TMR2_INPUTCAPTURE_FILTER_LEVEL);\
    oInputCapture_btInputLevelExpected = HIGH_LEVEL;\
}

//! Program the trigger to falling edge.
#define InputCapture_TriggerOnFallingEdge()\
{\
  TIM2_ICInit(INPUT_CAPTURE_CHANNEL,TIM2_ICPOLARITY_FALLING,TIM2_ICSELECTION_DIRECTTI,TIM2_ICPSC_DIV1,TMR2_INPUTCAPTURE_FILTER_LEVEL);\
  oInputCapture_btInputLevelExpected = LOW_LEVEL;\
}


// Enable capture
#define InputCapture_Enable()\
{\
      TIM2_CCxCmd(INPUT_CAPTURE_CHANNEL,ENABLE);\
      TIM2_ITConfig(TIM2_IT_CC2,ENABLE);/* Enable IT */\
}
//
// Disable capture
//

#define InputCapture_Disable()\
{\
      TIM2_CCxCmd(INPUT_CAPTURE_CHANNEL,DISABLE);\
      TIM2_ITConfig(TIM2_IT_CC2,DISABLE);/* Enable IT */\
}

// Acknowledge the IT
#define InputCapture_Ack()\
{\
         TIM2_ClearFlag(TIM2_FLAG_CC2);\
}

// Deactivate time out timer
//

#define InputCapture_DeactivateTimeout()\
{\
  TIM2_ClearFlag(TIM2_FLAG_CC1);\
  TIM2_ITConfig(TIM2_IT_CC1,DISABLE);/* Enable IT */\
}

// Activate time out timer
//

#define InputCapture_ActivateTimeout()\
{\
  TIM2_ClearFlag(TIM2_FLAG_CC1);\
  TIM2_ITConfig(TIM2_IT_CC1,ENABLE);/* Enable IT */\
}


//! Reload time out timer.
#define InputCapture_ReloadTimeout(TimeoutValue)\
 Input_Capture_ReloadTimeout(TimeoutValue)



// C L A S S   M E T H O D S ( P U B L I C   P R O T O T Y P E S )
//
   
   
void InputCapture_Refresh( void );
void InputCapture_SetUpHarware( void );
void InputCapture_SwitchToLowPower(void);
TU16 InputCapture_GiveTimerValue( void );
uint8_t InputCapture_Read( void );

void InputCapture_Edge_IRQHandler(void);
void InputCapture_Timeout_IRQHandler(void);
// reload Timeout Value for TIM2
void Input_Capture_ReloadTimeout(TU16 wTimeoutValue);

// C L A S S   I N S T A N C E S
//



