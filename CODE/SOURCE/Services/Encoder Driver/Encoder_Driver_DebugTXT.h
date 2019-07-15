#pragma once

#include "EncoderDriver_DriftCorrector.h"


static const char*  MagWheelState_TXT[] = 
{
  "MgWheelState_00\0",
  "MgWheelState_01\0",
  "MgWheelState_11\0",
  "MgWheelState_10\0",
};


static const char* MagWheelTransition_TXT[] =
{
 "MgWheelTrans_DirA_00_to_01\0",
 "MgWheelTrans_DirA_01_to_11\0",
 "MgWheelTrans_DirA_11_to_10\0",
 "MgWheelTrans_DirA_10_to_00\0",
 "MgWheelTrans_DirB_00_to_10\0",
 "MgWheelTrans_DirB_10_to_11\0",
 "MgWheelTrans_DirB_11_to_01\0",
 "MgWheelTrans_DirB_01_to_00\0",
 "MgWheelTrans_Err_00_to_11\0",
 "MgWheelTrans_Err_11_to_00\0",
 "MgWheelTrans_Err_10_to_01\0",
 "MgWheelTrans_Err_01_to_10\0",
 "MgWheelTrans_NoTransition\0",
};

