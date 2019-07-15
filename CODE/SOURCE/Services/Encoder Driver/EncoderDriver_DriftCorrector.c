// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#include "Config.h"
#include "stm8s_gpio.h"
#include "Pcb.h"
#include "EncoderDriver_DriftCorrector.h"


#ifdef ENCODER_DRIFT_PRINT_DEBUG	
#include "DebugPrint.h"
#endif

#ifdef ENCODER_DRIFT_PRINT_INFO
#include "DebugPrint_info.h"
#endif

#ifdef ENCODER_DRIFT_PRINT_LOG	
#include "DebugPrint_Log.h"
#endif



MagWheelState_e DriftCorrector_GetState()
{
  TU8 A = GPIO_ReadInputPin(Encoder1_PORT,Encoder1_PIN) != RESET ? (1):(0);
  TU8 B = GPIO_ReadInputPin(Encoder2_PORT,Encoder2_PIN) != RESET ? (1):(0);
  TU8 status = (A << 1) | B ;
  return (MagWheelState_e) status;
}

//      CountingInverted (default)           |        NotInverted
//      Dir A CW   | Dir B CCW               |  Dir A CW  | Dir B CCW
//      URo +      |  Ro -                   |    Ro -    | URo +

MagWheelTransition_e DriftCorrector_GetTransition(MagWheelState_e StoredState, MagWheelState_e CurrentState)
{
  MagWheelTransition_e returnvalue = MagWheelTransition_Err_NoTransition;
  switch(StoredState)
  {
    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////      
  case MagWheelState_00 :
    
    switch(CurrentState)
    {
      case MagWheelState_00 :
        returnvalue = MagWheelTransition_Err_NoTransition;
        break;
      case MagWheelState_01 :
        returnvalue = MagWheelTransition_DirA_00_to_01;
        break;
      case MagWheelState_11 :
        returnvalue = MagWheelTransition_Err_00_to_11;
        break;
      case MagWheelState_10 :
        returnvalue = MagWheelTransition_DirB_00_to_10;
        break;
      default:
        trap();
        break;
    }
    break;
    ////////////////////////////////////////////////////////////////////////////////////  
    ////////////////////////////////////////////////////////////////////////////////////  
  case MagWheelState_01 :
    switch(CurrentState)
    {
      case MagWheelState_00 :
        returnvalue = MagWheelTransition_DirB_01_to_00;
        break;
      case MagWheelState_01 :
        returnvalue = MagWheelTransition_Err_NoTransition;
        break;
      case MagWheelState_11 :
        returnvalue = MagWheelTransition_DirA_01_to_11;
        break;
      case MagWheelState_10 :
        returnvalue = MagWheelTransition_Err_01_to_10;
        break;
      default:
        trap();
        break;
    }
    break;
    ////////////////////////////////////////////////////////////////////////////////////  
    ////////////////////////////////////////////////////////////////////////////////////  
  case MagWheelState_11 :
    switch(CurrentState)
    {
      case MagWheelState_00 :
        returnvalue = MagWheelTransition_Err_11_to_00;
        break;
      case MagWheelState_01 :
        returnvalue = MagWheelTransition_DirB_11_to_01;
        break;
      case MagWheelState_11 :
        returnvalue = MagWheelTransition_Err_NoTransition;
        break;
      case MagWheelState_10 :
        returnvalue = MagWheelTransition_DirA_11_to_10;
        break;
      default:
        trap();
        break;
    }
    break;
    ////////////////////////////////////////////////////////////////////////////////////  
    ////////////////////////////////////////////////////////////////////////////////////  
  case MagWheelState_10 :
    switch(CurrentState)
    {
      case MagWheelState_00 :
        returnvalue = MagWheelTransition_DirA_10_to_00;
        break;
      case MagWheelState_01 :
        returnvalue = MagWheelTransition_Err_10_to_01;
        break;
      case MagWheelState_11 :
        returnvalue = MagWheelTransition_DirB_10_to_11;
        break;
      case MagWheelState_10 :
        returnvalue = MagWheelTransition_Err_NoTransition;
        break;
      default:
        trap();
        break;
    }
    break;
    ////////////////////////////////////////////////////////////////////////////////////  
    ////////////////////////////////////////////////////////////////////////////////////  
  default:
    trap();
    break;
    ////////////////////////////////////////////////////////////////////////////////////  
    ////////////////////////////////////////////////////////////////////////////////////  
  }
  
  return returnvalue;
  
}



TS8 DriftCorrector_GetCorrection(MagWheelTransition_e OnWkupTransition,TBool IsCountingDirInverted)
{ 
  TS8 Correction = 0U;
  switch(OnWkupTransition)
  {
  case MagWheelTransition_DirA_00_to_01 :
  case MagWheelTransition_DirA_01_to_11 :
  case MagWheelTransition_DirA_11_to_10 :
  case MagWheelTransition_DirA_10_to_00 :
    if(IsCountingDirInverted == TRUE)
    {
      Correction = 1;
    }
    else
    {
      Correction = -1;
    }
    break;
    
    
  case MagWheelTransition_DirB_00_to_10 :
  case MagWheelTransition_DirB_10_to_11 :
  case MagWheelTransition_DirB_11_to_01 :
  case MagWheelTransition_DirB_01_to_00 :
    if(IsCountingDirInverted == TRUE)
    {
      Correction = -1;
    }
    else
    {
      Correction = 1;
    }
    break;
    
    
  case MagWheelTransition_Err_00_to_11 :
  case MagWheelTransition_Err_11_to_00 :
  case MagWheelTransition_Err_10_to_01 :
  case MagWheelTransition_Err_01_to_10 :
  case MagWheelTransition_Err_NoTransition:
    Correction = 0U;
    break;
    
  default:
    trap();
    break;
  }
  return Correction;
}



