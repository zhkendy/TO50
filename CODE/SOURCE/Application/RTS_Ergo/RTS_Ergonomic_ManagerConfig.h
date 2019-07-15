// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once
#include "RTS_Ergonomic_Manager.h"


#define REMOTE_TYPE_LENGH       (8)

static const TBool EntryRange_AcceptedRemoteArray[]=
{
  FALSE,                                //RTSPROT_RC_UNKNOWN=0,
  TRUE,                                 //RTSPROT_RC_3BUTTON=1,
  FALSE,                                //RTSPROT_RC_1BUTTON=2,
  FALSE,                                 //RTSPROT_RC_SPRING2BUTTON=3,
  TRUE,                                //RTSPROT_SENSOR=4,//kendyRTS to be done
  TRUE,                                 //RTSPROT_FACTORY_CRYPTED=5,
  TRUE,                                 //RTSPROT_FACTORY_UNCRYPTED=6,
  FALSE,                                //RTSPROT_RC_RTDS=7
};

static const TBool Spring25_AcceptedRemoteArray[]=
{
  FALSE,                                //RTSPROT_RC_UNKNOWN=0,
  FALSE,                                //RTSPROT_RC_3BUTTON=1,
  FALSE,                                //RTSPROT_RC_1BUTTON=2,
  TRUE,                                 //RTSPROT_RC_SPRING2BUTTON=3,
  FALSE,                                //RTSPROT_SENSOR=4,
  TRUE,                                 //RTSPROT_FACTORY_CRYPTED=5,
  TRUE,                                 //RTSPROT_FACTORY_UNCRYPTED=6,
  FALSE,                                //RTSPROT_RC_RTDS=7
};

static const TBool None_AcceptedRemoteArray[]=
{
  FALSE,                                //RTSPROT_RC_UNKNOWN=0,
  FALSE,                                //RTSPROT_RC_3BUTTON=1,
  FALSE,                                //RTSPROT_RC_1BUTTON=2,
  FALSE,                                 //RTSPROT_RC_SPRING2BUTTON=3,
  FALSE,                                //RTSPROT_SENSOR=4,
  TRUE,                                 //RTSPROT_FACTORY_CRYPTED=5,
  TRUE,                                 //RTSPROT_FACTORY_UNCRYPTED=6,
  FALSE,                                //RTSPROT_RC_RTDS=7
};



static const TBool Spring25_U56_AcceptedCMDArray[]=
{
  TRUE,      //  ErgoRemoteEventType_Up=0x00,
  TRUE,      //  ErgoRemoteEventType_Down,
  TRUE,      //  ErgoRemoteEventType_Stop,
  TRUE,      //  ErgoRemoteEventType_ModeDown,
  TRUE,      //  ErgoRemoteEventType_ModeUp,
  TRUE,      //  ErgoRemoteEventType_Prog,
  FALSE,     //  ErgoRemoteEventType_Stop_Up,
  FALSE,     //  ErgoRemoteEventType_Stop_Down,
  TRUE,      //  ErgoRemoteEventType_Up_Down,
  FALSE,     //  ErgoRemoteEventType_Stop_Up_Down,
  FALSE,     //  ErgoRemoteEventType_Sun_On,
  FALSE,     //  ErgoRemoteEventType_Sun_Off,
  TRUE,      //  ErgoRemoteEventType_Execute_U80,
  FALSE,     //  ErgoRemoteEventType_PROG_RT,
  FALSE,     //  ErgoRemoteEventType_CYCLE_4_TIME,
  TRUE,      //  ErgoIndusRemoteEventType_Up,
  TRUE,      //  ErgoIndusRemoteEventType_Down,
  TRUE,      //  ErgoIndusRemoteEventType_Stop,
  TRUE,      //  ErgoIndusRemoteEventType_SoftVersion,
  TRUE,      //  ErgoIndusRemoteEventType_Autotest1,
  TRUE,      //  ErgoIndusRemoteEventType_Autotest2,
  TRUE,      //  ErgoIndusRemoteEventType_Autotest3,
  TRUE,      //  ErgoIndusRemoteEventType_Autotest4,
  TRUE,      //  ErgoIndusRemoteEventType_Autotest5,
  TRUE,      //  ErgoIndusRemoteEventType_Autotest6,
  FALSE,     //  NormalMode_NoSun_NoWind,
  FALSE,     //  NormalMode_NoSun_Wind,
  FALSE,     //  NormalMode_Sun_NoWind,
  FALSE,     //  NormalMode_Sun_Wind,
  FALSE,     //  NormalMode_Prog,
  FALSE,     //  DemoMode_NoSun_NoWind,
  FALSE,     //  DemoMode_NoSun_Wind,
  FALSE,     //  DemoMode_Sun_NoWind,
  FALSE,     //  DemoMode_Sun_Wind,
  FALSE,     //  RTDS_BP1,
  FALSE,     //  RTDS_BP2,
  FALSE,     //  RTDS_BP3,
  FALSE,     //  RTDS_BP4,
  FALSE,     //  ErgoRemoteEventUnkownCommand,
  FALSE,     //  ErgoRemoteEventType_LastID,
};

static const TBool EntryRangeWolrdWide_U56_AcceptedCMDArray[]=
{
  TRUE,      //  ErgoRemoteEventType_Up=0x00,
  TRUE,      //  ErgoRemoteEventType_Down,1
  TRUE,      //  ErgoRemoteEventType_Stop,2
  TRUE,      //  ErgoRemoteEventType_ModeDown,3
  TRUE,      //  ErgoRemoteEventType_ModeUp,4
  TRUE,      //  ErgoRemoteEventType_Prog,5
  TRUE,      //  ErgoRemoteEventType_Stop_Up,6
  TRUE,      //  ErgoRemoteEventType_Stop_Down,7
  TRUE,      //  ErgoRemoteEventType_Up_Down,8
  TRUE,     //  ErgoRemoteEventType_Stop_Up_Down,9
  TRUE,     //  ErgoRemoteEventType_Sun_On,//kendyRTS10
  TRUE,     //  ErgoRemoteEventType_Sun_Off,//kendyRTS11
  TRUE,      //  ErgoRemoteEventType_Execute_U80,12
  FALSE,     //  ErgoRemoteEventType_PROG_RT,13
  FALSE,     //  ErgoRemoteEventType_CYCLE_4_TIME,14
  TRUE,      //  ErgoIndusRemoteEventType_Up,15
  TRUE,      //  ErgoIndusRemoteEventType_Down,16
  TRUE,      //  ErgoIndusRemoteEventType_Stop,17
  TRUE,      //  ErgoIndusRemoteEventType_Stop_UP,18
  TRUE,      //  ErgoIndusRemoteEventType_Stop_DOWN,19
  TRUE,      //  ErgoIndusRemoteEventType_Stop_UP_DOWN,20
  TRUE,      //  ErgoIndusRemoteEventType_UP_DOWN,21
  TRUE,      //  ErgoIndusRemoteEventType_SoftVersion,22
  TRUE,      //  ErgoIndusRemoteEventType_Autotest1,23
  TRUE,      //  ErgoIndusRemoteEventType_Autotest2,24
  TRUE,      //  ErgoIndusRemoteEventType_Autotest3,25
  TRUE,      //  ErgoIndusRemoteEventType_Autotest4,26
  TRUE,      //  ErgoIndusRemoteEventType_Autotest5,27
  TRUE,      //  ErgoIndusRemoteEventType_Autotest6,28
  TRUE,     //  NormalMode_NoSun_NoWind,29
  TRUE,     //  NormalMode_NoSun_Wind,230
  TRUE,     //  NormalMode_Sun_NoWind,231
  TRUE,     //  NormalMode_Sun_Wind,32
  TRUE,     //  NormalMode_Prog,33
  TRUE,     //  DemoMode_NoSun_NoWind,34
  TRUE,     //  DemoMode_NoSun_Wind,35
  TRUE,     //  DemoMode_Sun_NoWind,36
  TRUE,     //  DemoMode_Sun_Wind,37
  FALSE,     //  RTDS_BP1,38
  FALSE,     //  RTDS_BP2,39
  FALSE,     //  RTDS_BP3,40
  FALSE,     //  RTDS_BP4,41
  FALSE,     //  ErgoRemoteEventUnkownCommand,42
  FALSE,     //  ErgoRemoteEventType_LastID,43
};

static const TBool None_U56_AcceptedCMDArray[]=
{
  FALSE,      //  ErgoRemoteEventType_Up=0x00,
  FALSE,      //  ErgoRemoteEventType_Down,
  FALSE,      //  ErgoRemoteEventType_Stop,
  FALSE,      //  ErgoRemoteEventType_ModeDown,
  FALSE,      //  ErgoRemoteEventType_ModeUp,
  FALSE,      //  ErgoRemoteEventType_Prog,
  FALSE,     //  ErgoRemoteEventType_Stop_Up,
  FALSE,     //  ErgoRemoteEventType_Stop_Down,
  FALSE,      //  ErgoRemoteEventType_Up_Down,
  FALSE,     //  ErgoRemoteEventType_Stop_Up_Down,
  FALSE,     //  ErgoRemoteEventType_Sun_On,
  FALSE,     //  ErgoRemoteEventType_Sun_Off,
  TRUE,      //  ErgoRemoteEventType_Execute_U80,
  FALSE,     //  ErgoRemoteEventType_PROG_RT,
  FALSE,     //  ErgoRemoteEventType_CYCLE_4_TIME,
  TRUE,      //  ErgoIndusRemoteEventType_Up,
  TRUE,      //  ErgoIndusRemoteEventType_Down,
  TRUE,      //  ErgoIndusRemoteEventType_Stop,
  TRUE,      //  ErgoIndusRemoteEventType_SoftVersion,
  TRUE,      //  ErgoIndusRemoteEventType_Autotest1,
  TRUE,      //  ErgoIndusRemoteEventType_Autotest2,
  TRUE,      //  ErgoIndusRemoteEventType_Autotest3,
  TRUE,      //  ErgoIndusRemoteEventType_Autotest4,
  TRUE,      //  ErgoIndusRemoteEventType_Autotest5,
  TRUE,      //  ErgoIndusRemoteEventType_Autotest6,
  FALSE,     //  NormalMode_NoSun_NoWind,
  FALSE,     //  NormalMode_NoSun_Wind,
  FALSE,     //  NormalMode_Sun_NoWind,
  FALSE,     //  NormalMode_Sun_Wind,
  FALSE,     //  NormalMode_Prog,
  FALSE,     //  DemoMode_NoSun_NoWind,
  FALSE,     //  DemoMode_NoSun_Wind,
  FALSE,     //  DemoMode_Sun_NoWind,
  FALSE,     //  DemoMode_Sun_Wind,
  FALSE,     //  RTDS_BP1,
  FALSE,     //  RTDS_BP2,
  FALSE,     //  RTDS_BP3,
  FALSE,     //  RTDS_BP4,
  FALSE,     //  ErgoRemoteEventUnkownCommand,
  FALSE,     //  ErgoRemoteEventType_LastID,
};





//AEG_STD199P24
//RTSPROT_RC_SPRING2BUTTON=3,//Spring2.5entryrangeremote
static const ErgoRemoteEventType_e SpringsExtendedCMDArray[]=
{
  ErgoRemoteEventUnkownCommand,            //NOP                   0x00
  ErgoRemoteEventType_ModeUp,           //UPCYCLE2TEMPS         0x01
  ErgoRemoteEventType_ModeDown,         //DWNCYCLE2TEMPS        0x02
  ErgoRemoteEventType_PROG_RT,          //PROGRT                0x03
  ErgoRemoteEventType_CYCLE_4_TIME,     //CYCLE4TEMPS           0x04
  ErgoRemoteEventType_Stop,             //STOP                  0x05
  ErgoRemoteEventType_Up,               //UP                    0x06
  ErgoRemoteEventType_Stop_Up,          //STOP+MONTEE           0x07
  ErgoRemoteEventType_Down,             //DOWN                  0x08
  ErgoRemoteEventType_Stop_Down,        //STOP+DOWN             0x09
  ErgoRemoteEventType_Up_Down,          //UP+DOWN               0x0A
  ErgoRemoteEventType_Stop_Up_Down,     //STOP+MONTEE+DESC.     0x0B
  ErgoRemoteEventType_Prog,             //PROG                  0x0C
  ErgoRemoteEventType_Sun_On,           //SUNAUTOON             0x0D
  ErgoRemoteEventType_Sun_Off,          //SUNAUTOOFF            0x0E
  ErgoRemoteEventType_Execute_U80,      //EXECUTEU80            0x0F
};



//RTSPROT_FACTORY_CRYPTED=5,//!<Industrialcryptedframe
//RTSPROT_FACTORY_UNCRYPTED=6,//!<industrialuncryptedframe
static const ErgoRemoteEventType_e IndusCmdArray[]=
{
  ErgoRemoteEventUnkownCommand,//0x00
  ErgoIndusRemoteEventType_Stop,//0x01
  ErgoIndusRemoteEventType_Up,//0x02
  ErgoIndusRemoteEventType_Stop_Up,//0x03
  ErgoIndusRemoteEventType_Down,//0x04
  ErgoIndusRemoteEventType_Stop_Down,//0x05
  ErgoIndusRemoteEventType_Up_Down,//0x06
  ErgoIndusRemoteEventType_Up_Stop_Down,//0x07
  ErgoRemoteEventUnkownCommand,//0x08
  ErgoIndusRemoteEventType_Autotest1,//0x09
  ErgoIndusRemoteEventType_Autotest2,//0x0A
  ErgoIndusRemoteEventType_Autotest3,//0x0B
  ErgoIndusRemoteEventType_Autotest4,//0x0C
  ErgoIndusRemoteEventType_Autotest5,//0x0D
  ErgoIndusRemoteEventType_Autotest6,//0x0E
  ErgoIndusRemoteEventType_SoftVersion,//0x0F
};



//RTSPROT_RC_3BUTTON=1,//!<ClassicRemotecontrol
static const ErgoRemoteEventType_e StandarRTS_FunctionArray[]=
{
  ErgoRemoteEventUnkownCommand,//NOP 0x00
  ErgoRemoteEventType_Stop,//STOP 0x01
  ErgoRemoteEventType_Up,//UP 0x02
  ErgoRemoteEventType_Stop_Up,//STOP+UP 0x03
  ErgoRemoteEventType_Down,//DOWN 0x04
  ErgoRemoteEventType_Stop_Down,//STOP+Down0 x05
  ErgoRemoteEventType_Up_Down,//MONTEE+DESCENTE 0x06
  ErgoRemoteEventType_Stop_Up_Down,//STOP+MONTEE+DESC 0x07
  ErgoRemoteEventType_Prog,//PROG 0x08
  ErgoRemoteEventType_Sun_On,//SUNAUTOON 0x09
  ErgoRemoteEventType_Sun_Off,//SUNAUTOOFF 0x0A
  ErgoRemoteEventType_Execute_U80,//EXECUTEU80 0x0B
  ErgoRemoteEventUnkownCommand,//NOP 0x0C
  ErgoRemoteEventUnkownCommand,//NOP 0x0D
  ErgoRemoteEventUnkownCommand,//PARAMFRAME 0x0E
  ErgoRemoteEventUnkownCommand,//EXTENDEDFRAME 0x0F
};



static const ErgoRemoteEventType_e SENSOR_U56_FunctionArray[]=
{
  NormalMode_NoSun_NoWind,//0x00
  NormalMode_NoSun_Wind,//0x01
  NormalMode_Sun_NoWind,//0x02
  NormalMode_Sun_Wind,//0x03
  DemoMode_NoSun_NoWind,//0x04
  DemoMode_NoSun_Wind,//0x05
  DemoMode_Sun_NoWind,//0x06
  DemoMode_Sun_Wind,//0x07
  NormalMode_Prog,//0x08
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
};


static const ErgoRemoteEventType_e RTDS_FunctionArray[]=
{
  ErgoRemoteEventUnkownCommand,//0x00
  RTDS_BP2,//0x01
  RTDS_BP1,//0x02
  RTDS_BP3,//0x03
  RTDS_BP4,//0x04
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
};

static const ErgoRemoteEventType_e SOMFY_Extended_FunctionArray[]=
{
  ErgoRemoteEventUnkownCommand,////0x00
  ErgoRemoteEventUnkownCommand,//0x01
  ErgoRemoteEventUnkownCommand,//0x02
  ErgoRemoteEventType_PROG_RT,          //PROGRT                0x03
  ErgoRemoteEventType_CYCLE_4_TIME,     //CYCLE4TEMPS           0x04
  ErgoRemoteEventUnkownCommand,//0x04
  ErgoRemoteEventUnkownCommand,//05
  ErgoRemoteEventUnkownCommand,//06
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
  ErgoRemoteEventUnkownCommand,//
};




static const TBool EntryRangeWolrdWide_U80_AcceptedCMDArray[]=
{
  TRUE,//  U80_FrameCounter=0,
  FALSE,//  U80_Transmit_12bits_data,
  TRUE,//  U80_Go_To,
  TRUE,//  U80_Move_Of,
  FALSE,//  U80_Activate_Scenario,
  FALSE,//  U80_Learn_Scenario,
  FALSE,//  U80_Record_Scenario,
  FALSE,//  U80_Delete_Scenario,
  FALSE,//  U80_Brand_ID,
  FALSE,//  U80_Sensor_PROG,
  FALSE,//  U80_Sensor_DECLARE,
  FALSE,//  U80_Sensor_CONTROL,
  FALSE,//  U80_Sensor_DETECT,
  FALSE,//  U80_Sensor_IS_ALIVED,
    FALSE,//  U80_None
  TRUE,//  U80_Software_Version_Index,
  TRUE,//  U80_U56,
};

static const TBool Spring25_U80_AcceptedCMDArray[]=
{
  TRUE,//  U80_FrameCounter=0,
  FALSE,//  U80_Transmit_12bits_data,
  TRUE,//  U80_Go_To,
  TRUE,//  U80_Move_Of,
  FALSE,//  U80_Activate_Scenario,
  FALSE,//  U80_Learn_Scenario,
  FALSE,//  U80_Record_Scenario,
  FALSE,//  U80_Delete_Scenario,
  FALSE,//  U80_Brand_ID,
  FALSE,//  U80_Sensor_PROG,
  FALSE,//  U80_Sensor_DECLARE,
  FALSE,//  U80_Sensor_CONTROL,
  FALSE,//  U80_Sensor_DETECT,
  FALSE,//  U80_Sensor_IS_ALIVED,
    FALSE,//  U80_None
  TRUE,//  U80_Software_Version_Index,
  TRUE,//  U80_U56,
};

static const TBool None_U80_AcceptedCMDArray[]=
{
  TRUE,//  U80_FrameCounter=0,
  FALSE,//  U80_Transmit_12bits_data,
  FALSE,//  U80_Go_To,
  FALSE,//  U80_Move_Of,
  FALSE,//  U80_Activate_Scenario,
  FALSE,//  U80_Learn_Scenario,
  FALSE,//  U80_Record_Scenario,
  FALSE,//  U80_Delete_Scenario,
  FALSE,//  U80_Brand_ID,
  FALSE,//  U80_Sensor_PROG,
  FALSE,//  U80_Sensor_DECLARE,
  FALSE,//  U80_Sensor_CONTROL,
  FALSE,//  U80_Sensor_DETECT,
  FALSE,//  U80_Sensor_IS_ALIVED,
  FALSE,//  U80_None
  TRUE,//  U80_Software_Version_Index,
  TRUE,//  U80_U56,
};

static const TU8 Press_DurationValue[]=
{
  1,    //BUTTON_PUSH_SINGLE
  4,    //BUTTON_PUSH_500ms
  6,    //BUTTON_PUSH_1SEC
  13,   //BUTTON_PUSH_2SEC
  19,   //BUTTON_PUSH_3SEC
  26,   //BUTTON_PUSH_4SEC
  33,   //BUTTON_PUSH_5SEC
  39,   //BUTTON_PUSH_6SEC
  45,   //BUTTON_PUSH_7SEC
  51,   //BUTTON_PUSH_8SEC
  58,   //BUTTON_PUSH_9SEC
  64,   //BUTTON_PUSH_10SEC
};

