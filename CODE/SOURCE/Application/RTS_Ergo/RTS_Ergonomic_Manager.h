// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once


#include "RTS_Ergonomic_ManagerSharedType.h"
#include "Config.h"
#include "RTSProtocol.h"//TStimuli
#include "Stack.h"

#define APPLI_SOFTWARE_NUMBER               "REF_5M"
#define APPLI_SOFTWARE_VERSION              "A00V00"
#define APPLI_SOFTWARE_BUILD_REVISION       0x563030
#define APPLI_SOFTWARE_BUILD_REVISION_HIGH  (APPLI_SOFTWARE_BUILD_REVISION >> 16 & 0xFF)
#define APPLI_SOFTWARE_BUILD_REVISION_MID   (APPLI_SOFTWARE_BUILD_REVISION >> 8 & 0xFF)
#define APPLI_SOFTWARE_BUILD_REVISION_LOW   (APPLI_SOFTWARE_BUILD_REVISION & 0xFF)
#define APPLI_SOFTWARE_SIZE                 0xD7EEul //0x1D7EEul     // Size is 0x27FEF (end of appli) - 0xA800 (start of appli) + 1
#define APPLI_SOFTWARE_SIZE_HIGH            (APPLI_SOFTWARE_SIZE >> 16 & 0xFF)
#define APPLI_SOFTWARE_SIZE_MID             (APPLI_SOFTWARE_SIZE >> 8 & 0xFF)
#define APPLI_SOFTWARE_SIZE_LOW             (APPLI_SOFTWARE_SIZE & 0xFF)

#define CST_APPLI_SOFT_NUMBER_SIZE          7
#define CST_APPLI_SOFT_VERSION_SIZE         6
//#define CST_APPLI_BUILD_SIZE                3
#define CST_APPLI_FLASH_APP_SIZE            3

typedef struct
{
    char softNumber[CST_APPLI_SOFT_NUMBER_SIZE];
    char softVersion[CST_APPLI_SOFT_VERSION_SIZE];
 //   char BuildRevision[CST_APPLI_BUILD_SIZE];
    char FlashAppSize[CST_APPLI_FLASH_APP_SIZE];
} APPLI_INFO;

#ifdef ENABLE_BOOTLOADER
#define CST_BOOT_SOFT_NUMBER_SIZE  7
#define CST_BOOT_SOFT_VERSION_SIZE 3

typedef struct
{
    char softNumber[CST_BOOT_SOFT_NUMBER_SIZE];
    char softVersion[CST_BOOT_SOFT_VERSION_SIZE];
    TU32 BuildRevision;
} BOOT_INFO;

#define BOOTLOADER_INFO_ADDR    0xA7F2  // Address of bootloader software version -> linked to icf file of bootloader
#endif




typedef enum ErgoRemoteEventType_e
{
  ErgoRemoteEventType_Up=0x00,
  ErgoRemoteEventType_Down,
  ErgoRemoteEventType_Stop,
  ErgoRemoteEventType_ModeDown,
  ErgoRemoteEventType_ModeUp,
  ErgoRemoteEventType_Prog,
  ErgoRemoteEventType_Stop_Up,
  ErgoRemoteEventType_Stop_Down,
  ErgoRemoteEventType_Up_Down,
  ErgoRemoteEventType_Stop_Up_Down,
  ErgoRemoteEventType_Sun_On,
  ErgoRemoteEventType_Sun_Off,
  ErgoRemoteEventType_Execute_U80,
  ErgoRemoteEventType_PROG_RT,
  ErgoRemoteEventType_CYCLE_4_TIME,
  ErgoIndusRemoteEventType_Up,
  ErgoIndusRemoteEventType_Down,
  ErgoIndusRemoteEventType_Stop,
  ErgoIndusRemoteEventType_Stop_Up,
  ErgoIndusRemoteEventType_Stop_Down,
  ErgoIndusRemoteEventType_Up_Stop_Down,
  ErgoIndusRemoteEventType_Up_Down,
  ErgoIndusRemoteEventType_SoftVersion,
  ErgoIndusRemoteEventType_Autotest1,
  ErgoIndusRemoteEventType_Autotest2,
  ErgoIndusRemoteEventType_Autotest3,
  ErgoIndusRemoteEventType_Autotest4,
  ErgoIndusRemoteEventType_Autotest5,
  ErgoIndusRemoteEventType_Autotest6,
  NormalMode_NoSun_NoWind,
  NormalMode_NoSun_Wind,
  NormalMode_Sun_NoWind,
  NormalMode_Sun_Wind,
  NormalMode_Prog,
  DemoMode_NoSun_NoWind,
  DemoMode_NoSun_Wind,
  DemoMode_Sun_NoWind,
  DemoMode_Sun_Wind,
  RTDS_BP1,
  RTDS_BP2,
  RTDS_BP3,
  RTDS_BP4,
  ErgoRemoteEventUnkownCommand,
  ErgoRemoteEventType_LastID,
}ErgoRemoteEventType_e;


typedef enum U80_Function_e
{
  U80_FrameCounter=0,
  U80_Transmit_12bits_data,
  U80_Go_To,
  U80_Move_Of,
  U80_Activate_Scenario,
  U80_Learn_Scenario,
  U80_Record_Scenario,
  U80_Delete_Scenario,
  U80_Brand_ID,
  U80_Sensor_PROG,
  U80_Sensor_DECLARE,
  U80_Sensor_CONTROL,
  U80_Sensor_DETECT,
  U80_Sensor_IS_ALIVED,
  U80_None,
  U80_Software_Version_Index,
  U80_U56,
  U80_LASTID,
}U80_Function_e;

typedef enum Press_Duration_e
{
  BUTTON_PUSH_SINGLE,
  BUTTON_PUSH_500ms,
  BUTTON_PUSH_1SEC,
  BUTTON_PUSH_2SEC,
  BUTTON_PUSH_3SEC,
  BUTTON_PUSH_4SEC,
  BUTTON_PUSH_5SEC,
  BUTTON_PUSH_6SEC,
  BUTTON_PUSH_7SEC,
  BUTTON_PUSH_8SEC,
  BUTTON_PUSH_9SEC,
  BUTTON_PUSH_10SEC,
}Press_Duration_e;



typedef enum Led_Mode
{
  AllClose, 
  GreenLight,    
  YellowLight,  
  RedLight,   
  Green_2_Economy,
  Yellow_2_Economy,
  GreenSlowBlink,    
  YellowSlowBlink,   
  RedSlowBlink, 
  GreenFastBlink,    
  YellowFastBlink,   
  RedFastBlink, 
  Green_1_Flash,    
  Yellow_1_Flash,     
  Red_1_Flash,   
  Green_2_Flash,    
  Yellow_2_Flash,   
  Red_2_Flash,  
  Green_3_Flash,     
  Red_3_Flash,  
  Green_4_Flash,     
}Led_Mode;

typedef enum Charger_Led_Mode
{
  Close, 
  Green_Economy,
  GreenStatic,    
}Charger_Led_Mode;

typedef enum ErgoRemoteBP_State_e
{
  ErgoRemote_BpPress=1,
  ErgoRemote_BpRelease=0,
}ErgoRemoteBP_State_e;

typedef enum ErgoModeState_e
{
  ErgoModeState_WaitEvent,
  ErgoModeState_WaitRelease,
}ErgoModeState_e;

typedef enum WheelDir_e
{
  WheelDir_Normal = 1,
  WheelDir_Inverted = -1,
}WheelDir_e;



void ErgonomicManager_ReadParameter();
void ErgonomicManager_Create();
void ErgonomicManager_Init();
void ErgonomicManager_Reset();
void ErgonomicManager_SetGlobalMode();
//ErgoGlobalMode_e ErgonomicManager_GetGlobalMode();
ErgoMode_e ErgonomicManager_GetCurrentMode();
 void ErgonomicManager_ChangeErgoMode(ErgoMode_e NewMode);
TBool ErgonomicManager_IsRTSFlowFlagSet();
void ErgonomicManager_TranslateStimulus(TStimuli *Stimuli);
void ErgonomicManager_DeadManTimer_Timeout();
TBool ErgonomicManager_AllowLowPower();

