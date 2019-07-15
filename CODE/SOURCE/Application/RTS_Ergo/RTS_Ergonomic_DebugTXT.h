// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once

static const char *RTSErgoSpecialRequest_TXT[]=
{
  "RemoteEepromBackup\0",
  "InvertRotationDirection\0",
  "MotorReset\0",
  "RunAutotest1\0",
  "RunAutotest2\0",
  "RunAutotest3\0",
  "RunAutotest4\0",
  "RunAutotest5\0",
  "RunAutotest6\0",
  "SensitivityFrame\0",
};

static const char *ErgoGlobalMode_TXT[]=
{
   "None\0",
   "Springs25_EntryRange\0",
   "WorldWide_EntryRange\0",
};


static const char *RemoteType_TXT[]=
{
  "UNKNOWN\0",
  "3BUTTON\0",
  "1BUTTON\0",
  "SPRING2BUTTON\0",
  "SENSOR\0",
  "FACTORY_CRYPTED\0",
  "FACTORY_UNCRYPTED\0",
  "RTDS\0",
};

static const char *RemotePairingState_TXT[]=
{
  "UNRECORDED\0",              
  "RECORDED\0",                   
  "TEMPORARY\0",                 
};

  
static const char *ErgoRemoteEventType_TXT[] =
{
  "Up\0",
  "Down\0",
  "Stop\0",
  "ModeDown\0",
  "ModeUp\0",
  "Prog\0",
  "Stop_Up\0",
  "Stop_Down\0",
  "Up_Down\0",
  "Stop_Up_Down\0",
  "Sun_On\0",
  "Sun_Off\0",
  "Execute_U80\0",
  "PROG_RT\0",
  "CYCLE_4_TIME\0",
  "Ind_Up\0",
  "Ind_Down\0",
  "Ind_Stop\0",
  "Ind_SoftVersion\0",
  "Ind_Autotest1\0",
  "Ind_Autotest2\0",
  "Ind_Autotest3\0",
  "Ind_Autotest4\0",
  "Ind_Autotest5\0",
  "Ind_Autotest6\0",
  "NormalMode_NoSun_NoWind\0",
  "NormalMode_NoSun_Wind\0",
  "NormalMode_Sun_NoWind\0",
  "NormalMode_Sun_Wind\0",
  "NormalMode_Prog\0",
  "DemoMode_NoSun_NoWind\0",
  "DemoMode_NoSun_Wind\0",
  "DemoMode_Sun_NoWind\0",
  "DemoMode_Sun_Wind\0",
  "RTDS_BP1\0",
  "RTDS_BP2\0",
  "RTDS_BP3\0",
  "RTDS_BP4\0",
  "ErgoRemoteEventUnkownCommand\0",
};

static const char *U80_FunctionTXT[] =
{
  "FCo\0",
  "Trt_12bits\0",
  "Go_To\0",
  "Move_Of\0",
  "Act_Scen\0",
  "Learn_Scen\0",
  "Record_Scen\0",
  "Del_Scen\0",
  "Brand_ID\0",
  "SensPROG\0",
  "SensDECE\0",
  "SensCTRL\0",
  "SensDETECT\0",
  "SensALVD\0",
  "SoftVIdx\0",
  "ERR_LASTID\0",
  "U56\0",
};


static const char *Press_Duration_TXT[] =
{
  "SINGLE\0",
  "500ms\0",
  "1S\0",
  "2S\0",
  "3S\0",
  "4S\0",
  "5S\0",
  "6S\0",
  "7S\0",
  "8S\0",
  "9S\0",
  "10S\0",
};


static const char *ErgoMode_TXT[] =
{
  "SetAndPaired\0",
  "SetAndNotPaired\0",
  "UserMode\0",
  "ReSettingDEL\0",
  "ReSettingUEL\0",
  "ReSettingRotationDirection\0",
  "EMS\0",
  "EMS_Sensitivity_Off\0",
  "ReSettingWheelDirection\0",
};

static const char *RemoteBP_State_TXT[] =
{
  "BpRelease\0",
  "BpPress\0",
};

