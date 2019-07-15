// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#pragma once
#include "Config.h"
#include "Stack.h"

typedef enum LED_Event_e
{
  LED_Event_None,                               //0
  LED_Event_NoBlinkUserMode,                    //1
  LED_Event_TempPaired,                         //2
  LED_Event_AddTeleco,                          //3
  LED_Event_ToUserMode,                         //4 
  LED_Event_CantAddOrDeletTeleco,               //5
  LED_Event_RemoveAllTeleco,                    //6
  LED_Event_Ergo_FactoryMode,                   //8
  LED_Event_Ergo_IndustrialMode,                //9
  LED_Event_Ergo_FirstPairingMode,              //10
  LED_Event_Ergo_SettingPart1Mode,              //11
  LED_Event_Ergo_SettingPart1Modebis,           //12
  LED_Event_Ergo_ConfigMode,                    //13 
  LED_Event_Ergo_SpeedAdjustmentMode,           //14
  LED_Event_Ergo_MaintenanceResetingMode,       //15  
  LED_Event_PowerOn_SetAndNotPaired,            //17   //kendy???
  LED_Event_ErgoEnterInLimitResetting,          //18
  LED_Event_ErgoEnterInResetingRotationDir,     //19
  LED_Event_Ergo_ToSleepMode,                   //20
  LED_Event_PwrCut,                             //21
  LED_Event_3MinMoveTimeout,                    //22
  LED_Event_ThermalProtect,                     //23
  LED_Event_MotorBlocked,                       //24
  LED_Event_ResetDEL,                           //25
  LED_Event_ResetUEL,                           //26
  LED_Event_ResetMY,                            //27
  LED_Event_SpeedINC,                           //28
  LED_Event_SpeedDEC,                           //29
  LED_Event_TiltSpeedINC,                       //30
  LED_Event_TiltSpeedDEC,                       //31 
  LED_Event_Set_ToSetNotTemMode,                //32 
  LED_Event_RadioOn_FactMode,                   //33
  LED_Event_RadioOn_SetNotMode,                 //34
  LED_Event_RadioOn_UserMode,                   //35
  LED_Event_RadioOff,                           //36
  LED_Event_ResetRotationDir,                   //37   
  LED_Event_SetImpossibleUserMode,              //39  
  LED_Event_SetImpossibleSetMode,               //39 
  LED_Event_BackUserMode,                       //40  
  LED_Event_ChangeToUserModeA,                  //41  
  LED_Event_ChangeToUserModeB,                  //42   
  LED_Event_ErrorFeedbackReq,                   //43        
  LED_Event_StopOnUL,                           //44
  LED_Event_StopOnDL,                           //45
  LED_Event_StopOnMy,                           //46
  LED_Event_StopBetweenLimits,                  //47
  LED_Event_StopAboveLimits,                    //48
  LED_Event_StopBelowLimits,                    //49       
  LED_Event_UserLowPower,                       //50  
  LED_Event_UserRemotecontral,                  //51
  LED_Event_UserChargerFull,                    //52  
  LED_Event_UserChargerGoing,                   //53 
  LED_Event_RollReq,                            //54
  LED_Event_UnRollReq,                          //55
  LED_Event_Empty,                              //56
}LED_Event_e;


typedef enum SQC_Event_e
{
  SQC_Event_None,                               //0
  SQC_Event_Reset,                              //1
  SQC_Event_AddTeleco,                          //2
  SQC_Event_CantAddTeleco,                      //3
  SQC_Event_RemoveTeleco,                       //4
  SQC_Event_RemoveAllTeleco,                    //5
  SQC_Event_ErgoEnterInUserMode,                //6
  SQC_Event_ErgoEnterInSetAndNotPaired,         //7
  SQC_Event_ErgoEnterInSetAndTemporaryPaired,   //8
  SQC_Event_ErgoEnterInLimitResetting,          //9
  SQC_Event_ErgoEnterInResetingRotationDir,     //10
  SQC_Event_ControllerInEMSMode,                //11
  SQC_Event_SendSQC,                            //12
  SQC_Event_SaveAll,                            //13
  SQC_Event_PwrCut,                             //14
  SQC_Event_3MinMoveTimeout,                    //15
  SQC_Event_FeedbackReq,                        //16
  SQC_Event_DoubleFeedbackReq,                  //17
  SQC_Event_MotorBlocked,                       //18
  SQC_Event_ResetDEL,                           //19
  SQC_Event_ResetUEL,                           //20
  SQC_Event_ResetRotationDir,                   //21       
  SQC_Event_ErrorFeedbackReq,                   //22
  SQC_Event_ControllerInHighPower,              //23
  SQC_Event_ErgoTimeOut,                        //24
  SQC_Event_StopOnUL,                           //25
  SQC_Event_StopOnDL,                           //26
  SQC_Event_StopOnMy,                           //27
  SQC_Event_StopBetweenLimits,                  //28
  SQC_Event_StopAboveLimits,                    //29
  SQC_Event_StopBelowLimits,                    //30
  SQC_Event_StopReq,                            //31
  SQC_Event_OpenEMSWindow,                      //32
  SQC_Event_CloseEMSWindow,                     //33
  SQC_Event_Indus_Perso_None,                   //34
  SQC_Event_Indus_Perso_Spring,                 //35
  SQC_Event_Indus_Perso_WorldWide,              //36
  SQC_Event_ControllerInAutoTest,               //37
  SQC_Event_RollReq,                            //38
  SQC_Event_UnRollReq,                          //39
  SQC_Event_ErgoEnterInSensitivityTest,         //40
  SQC_Event_MoveActionRequestRejected,          //41
  SQC_Event_Empty,                              //42
  SQC_Event_ErgoWheelDirchange,                 //43
  SQC_Event_ErgoEnterInSetNoTemporaryRemote,     //44 kendy
}SQC_Event_e;





void  SQC_Init();
void  SQC_Create(TBool HotReset);
char* SQC_Get_SoftVersionString();
void  Version_Print(void);
void  SQCmesage_Print(void);
void  LED_LogEvent(LED_Event_e Evt);
void  SQC_LogEvent(SQC_Event_e Evt);
