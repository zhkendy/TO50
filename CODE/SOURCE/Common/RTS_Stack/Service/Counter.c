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
/ **********************************************************************/
#include <stdio.h>
#include "Counter.h"



//! Pool of instances.
struct CounterAttributes
{
  //! Current value of a running counter.
  TU16 wDuration;
  //! Function to call after expiration.
  void (*fpCallBack)(void);
} aoCounter[MAX_NB_OF_COUNTER];


//_________________________________________________________________________________________________
/*!
 * \fn      void Counter_InitClass(void)
 * \brief   Initialize the class. To be called before other class funcs to avoid any spurious countdown.
 *
 */
void Counter_InitClass(void)
{
  TU8 btLoop;

  for(btLoop = 0U; btLoop < MAX_NB_OF_COUNTER; btLoop++)
  {
    aoCounter[btLoop].wDuration = 0U;
    aoCounter[btLoop].fpCallBack = (void(*)())NULL;
  }
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Counter_Create(TU8 btCounterIdArg, void (*fpCallBackArg)(void))
 * \brief   Initialize the class. To be called before other class funcs to avoid any spurious countdown.
 * \param   btCounterIdArg : Identifier of the created counter.
 * \param   fpCallBackArg : Function to call when it expires.
 *
 */
void Counter_Create(TU8 btCounterIdArg, void (*fpCallBackArg)(void))
{
  aoCounter[btCounterIdArg].wDuration = 0U;
  aoCounter[btCounterIdArg].fpCallBack = fpCallBackArg;
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Counter_NotifyMeAfter(TU8 btCounterIdArg, TU16 wDurationArg)
 * \brief   Start a counting on a given counter.
 * \param   btCounterIdArg : Identifier of the counter.
 * \param   wDurationArg : Duration.
 *
 */
void Counter_NotifyMeAfter(TU8 btCounterIdArg, TU16 wDurationArg)
{
  aoCounter[btCounterIdArg].wDuration = wDurationArg;
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Counter_StopCounting(TU8 btCounterIdArg)
 * \brief   Stop a counting on a given counter.
 * \param   btCounterIdArg : Identifier of the counter.
 *
 */
void Counter_StopCounting(TU8 btCounterIdArg)
{
  aoCounter[btCounterIdArg].wDuration = 0U;
}

//_________________________________________________________________________________________________
/*!
 * \fn      void Counter_Decrement(void)
 * \brief   Main task of counters. Decrement all running counters and call their callback if elapsed.
 *
 */
void Counter_Decrement(void)
{
  TU8 btLoop = 0U;

  do
  {
    if(aoCounter[btLoop].wDuration != 0)
    {
      aoCounter[btLoop].wDuration --;

      if( aoCounter[btLoop].wDuration == 0 )
      {
        if (aoCounter[btLoop].fpCallBack)
        {
          aoCounter[btLoop].fpCallBack();
        }
      }
    }
  }while( ++btLoop < MAX_NB_OF_COUNTER );
}

#endif
