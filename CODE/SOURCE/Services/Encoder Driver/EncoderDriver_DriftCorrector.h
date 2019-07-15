// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once

typedef enum MagWheelState_e
{
  MagWheelState_00 = 0,
  MagWheelState_01 = 1,
  MagWheelState_11 = 3,
  MagWheelState_10 = 2,
}MagWheelState_e;


typedef enum MagWheelTransition_e
{
  MagWheelTransition_DirA_00_to_01 ,
  MagWheelTransition_DirA_01_to_11 ,
  MagWheelTransition_DirA_11_to_10 ,
  MagWheelTransition_DirA_10_to_00 ,
  MagWheelTransition_DirB_00_to_10 ,
  MagWheelTransition_DirB_10_to_11 ,
  MagWheelTransition_DirB_11_to_01 ,
  MagWheelTransition_DirB_01_to_00 ,
  MagWheelTransition_Err_00_to_11 ,
  MagWheelTransition_Err_11_to_00 ,
  MagWheelTransition_Err_10_to_01 ,
  MagWheelTransition_Err_01_to_10 ,
  MagWheelTransition_Err_NoTransition,
}MagWheelTransition_e;

MagWheelState_e DriftCorrector_GetState();
MagWheelTransition_e DriftCorrector_GetTransition(MagWheelState_e StoredState, MagWheelState_e CurrentState); 
TS8 DriftCorrector_GetCorrection(MagWheelTransition_e OnWkupTransition,TBool IsCountingDirInverted);









