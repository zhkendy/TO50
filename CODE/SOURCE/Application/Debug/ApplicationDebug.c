// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

//---------------------------------------------------------------------
// File "ApplicationDebug.c"   Hand made...
//---------------------------------------------------------------------


// External inclusions
//--------------------
#include "SFY_Types.h"
#include "SFY_Assert.h"
#include "ApplicationDebug.h"



/*!************************************************************************************************
* \fn         void SignalErrorAndGoToReset(void)
* \return     void 
***************************************************************************************************/
static void SignalErrorAndGoToReset(void)
{
  RUNTIME_WARNING();
}



// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \fn         void DebugAbort(void)
* \return     void 
***************************************************************************************************/
void DebugAbort(void)
{
  SignalErrorAndGoToReset();
}


/*!************************************************************************************************
* \fn         void Breakpoint(void)
* \brief      Place debugger breakpoint here
* \return     void 
***************************************************************************************************/
void Breakpoint(void)
{
  // You can place here a debugger breakpoint, to analyse problem causes as soon at it is detected
}




#ifdef __ICCAVR__

/*!************************************************************************************************
* \brief      Catch invalid indirect calls (by null function pointer)
* \fn         void Trap_NullAddress(void)
* \return     void 
***************************************************************************************************/
#pragma location = "TRAP_NULL"                      // Place the next function in this segment
__root void Trap_NullAddress(void)
{
  SignalErrorAndGoToReset();                        // Signal an attemps to indirect call a function by NULL pointer
}

#endif // __ICCAVR__


