// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once


typedef enum
{
  ErgoMode_FactoryMode,                                
  ErgoMode_FirstPairingMode,                  
  ErgoMode_SettingPart1Mode,                          
  ErgoMode_SettingPart1Modebis,                  
  ErgoMode_SetAndTemporaryRemote,     
  ErgoMode_SetAndNotPaired,   
  ErgoMode_SetNoTemporaryRemote,//kendy
  ErgoMode_Tiltingspeedadjustment,//kendy
  ErgoMode_ReSettingSDP,//kendy   
  ErgoMode_SleepMode,                 
  ErgoMode_ConfigMode,              
  ErgoMode_UserMode,                  
  ErgoMode_SpeedAdjustmentMode,                       
  ErgoMode_ReSettingDEL,              
  ErgoMode_ReSettingUEL,                
  ErgoMode_MaintenanceResetingMode,     
  ErgoMode_Delivery1Mode,
  ErgoMode_Delivery2Mode,
  ErgoMode_Delivery3Mode,
  ErgoMode_IndustrialMode,
  ErgoMode_EMS_Sensitivity_On,
  ErgoMode_EMS_Sensitivity_Off,
}ErgoMode_e;

typedef enum
{
  SPEED_28RPM=1,
  SPEED_26RPM,
  SPEED_24RPM,
  SPEED_22RPM,
  SPEED_20RPM,
  SPEED_18RPM,  
  SPEED_16RPM,
  SPEED_14RPM,
  SPEED_12RPM,
  SPEED_10RPM,
}SpeedMode_e;

typedef enum User_Mode
{
  User_ModeB_Roller, //roller mode 
  User_ModeA_Us_Tilit, //tilt US 
  User_ModeC_Eur_Tilit,
}User_Mode_e;

extern TU16 Roller_Speed;
extern TU16 Current_Speed;
extern TU16 Tilting_Speed;
extern TU8  User_Mode;
extern TU8  Curr_SpeedMode;
extern TU8  Pre_SpeedMode;

