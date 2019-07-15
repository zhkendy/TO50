// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

/**
* @mainpage
* @details 
  Source code documentation.
* @defgroup Application
* @defgroup Common
* @defgroup Platform
* @defgroup Service
*/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "SFY_Types.h"
#include "SFY_Macros.h"
#include "stm8s.h"
#include "DebugPrintDisable.h"

// Note
// Axx dev   => compiled with debug option, devellopment version, no traçability
// Axx RCx   => compiled with release option, not official software, can be tagued in git(release candidate)
// Axx R     => compiled with release option, official svn tagued software 

#define SOFTWARE_VersionNumber                          00
#define SOFTWARE_SerialNumber                           03
#define SOFTWARE_V                                      "A"VALUE(SOFTWARE_VersionNumber)"V"VALUE(SOFTWARE_SerialNumber)
//in QT30 project it is not needed  
#define USE_RETURNMOVE                                  (0u)  

#pragma section = "HEAP"
#pragma section = "CSTACK"
#define LUCIFER                                         (0x66u)

#ifdef  DEBUG
#define RETARGET_PRINTF_TO_UART 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////DEBUG PRINT//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////                                                                            
#define DEBUGPRINT                                      (1)
#define SQC_PRINT_DEBUG	
#define SQC_PRINT_INFO
#define SQC_PRINT_LOG
//
//#define MOTORCONTROL_PRINT_DEBUG
//#define MOTORCONTROL_PRINT_INFO
//#define MOTORCONTROL_PRINT_LOG
////
//#define CONTROLLER_PRINT_DEBUG	
//#define CONTROLLER_PRINT_INFO	
//#define CONTROLLER_PRINT_LOG	
//
//#define MOVEMENTMANAGER_PRINT_DEBUG	     
//#define MOVEMENTMANAGER_PRINT_INFO
//#define MOVEMENTMANAGER_PRINT_LOG	
//
//#define MOVEMENTPLANNER_PRINT_DEBUG    
//#define MOVEMENTPLANNER_PRINT_INFO
//#define MOVEMENTPLANNER_PRINT_LOG

#define ENCODER_PRINT_DEBUG	
#define ENCODER_PRINT_INFO
#define ENCODER_PRINT_LOG
#define ENCODER_DRIFT_PRINT_DEBUG


////#define ARCHIVER_PRINT_DEBUG		
////#define ARCHIVER_PRINT_INFO	
////#define ARCHIVER_PRINT_LOG

//#define POSITION_PRINT_DEBUG	
//#define POSITION_PRINT_INFO
//#define POSITION_PRINT_LOG	
//
//#define LIMITS_PRINT_DEBUG	
//#define LIMITS_PRINT_INFO
//#define LIMITS_PRINT_LOG	

////#define ERGONOMIC_PRINT_DEBUG		
////#define ERGONOMIC_PRINT_INFO
////#define ERGONOMIC_PRINT_LOG
//
////#define LP_PRINT_DEBUG		
////#define ERGONOMIC_PRINT_INFO
////#define ERGONOMIC_PRINT_LOG

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////DEBUG OPTION/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SOFTWARE_VERSION		                    SOFTWARE_V" Dev "
#define DISABLE_WATCHDOG
#define DEFAULT_ERGO_GLOBAL_MODE                            (ErgoGloabalMode_WorldWide_EntryRange)
#define ADDITIONAL_FACTORY_PRODUCT_TURN_BEETWEN_END_LIMIT   (3u)
#define FACTORY_ABS_POS                                     ((TU32) 0x7FFFFFFFu)
//´®¿Ú²¨ÌØÂÊ  38400u
#define UART_BAUD_RATE_DEBUG                                (38400u)
#define UART_BAUD_RATE_SQC                                  (38400u)
#define UART_BAUD_RATE_INDUS                                (38400u)

#define INDUSPROTOCOL_TIMEOUT_ms                            (OS_TIME_1_S)
#define ERGO_RESETTING_TIMEOUT_2MIN                         (120000u) // 2min
#define ERGO_RESETTING_TIMEOUT_15MIN                        (900000u) // 15min
#define SLEEPMODE_TIMELAST_15MIN                            (900000u) // 15in 
//#define FORBID_LOWPOWER                                     (FALSE)
#define FORBID_LOWPOWER                                     (TRUE) //KENDY
#define FORBID_ENCODER_PWROFF                               (FALSE)
#define USE_DRIFT_COMPENSATION_ALGO                         (TRUE)
#define ENCODER_SWTCH_OFF_TIME_AFTER_MOVE                    OS_TIME_3_S

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else /////////////////////////////////////////////////////////////   RELEASE OPTION  //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////// DO NOT CHANGE THOSE VALUE     /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DEBUGPRINT                                          (0u)//kendy 0u
#define SOFTWARE_VERSION		                    SOFTWARE_V" R "
#define ADDITIONAL_FACTORY_PRODUCT_TURN_BEETWEN_END_LIMIT   (0u)
#define FACTORY_ABS_POS                                     ((TU32) 0x7FFFFFFFu)
#define UART_BAUD_RATE_DEBUG                                (38400u)
#define UART_BAUD_RATE_SQC                                  (38400u)
#define UART_BAUD_RATE_INDUS                                (38400u)
    
#define INDUSPROTOCOL_TIMEOUT_ms                            (OS_TIME_1200_MS)
#define ERGO_RESETTING_TIMEOUT_2MIN                         (120000u) // 2min
#define ERGO_RESETTING_TIMEOUT_15MIN                        (900000u) // 15min
#define SLEEPMODE_TIMELAST_15MIN                            (900000u) // 15min                      

#define DEFAULT_ERGO_GLOBAL_MODE                            (ErgoGloabalMode_WorldWide_EntryRange)
//#define FORBID_LOWPOWER                                     (FALSE)
#define FORBID_LOWPOWER                                     (TRUE) //KENDY
#define FORBID_ENCODER_PWROFF                               (FALSE)
#define USE_DRIFT_COMPENSATION_ALGO                         (TRUE)
#define ENCODER_SWTCH_OFF_TIME_AFTER_MOVE                   (OS_TIME_6_S)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////HARDWARE SETUP///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define GEARBOX_REDUCTION		                    (158u)//(96u)   //RU24 new GEARBOX 
#define MAGNETIC_WHEEL_POLE_COUNT	                    (2u)    
#define TICK_PER_MOTOR_TURN		                    (2*MAGNETIC_WHEEL_POLE_COUNT)
#define ONE_PRODUCT_TURN	                            (158u)//(96u)   
#define H_BRIDGE_FREQ			                    (20000u)//16M/20K=800
#define ABS_MAX_PWM                                         (800u)  //PWM ×î´óÖµ 100%
#define POWERFAIL_ADCLEVEL_TRIGGER                          (465u)  //8.5V //(350u)  //6.4V

#define MINIMUM_VOLTAGE_FOR_MOVE                            (500u)  //9.3v   
#define MIDDLE_VOLTAGE_FOR_MOVE                             (561u)  //10.3v  
#define FULL_VOLTAGE_FOR_MOVE                               (661u)  //12.3v //(645u) 12.0v 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////MOVE SETUP///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
#define P                                                   (0.03) 
#define I                                                   (0.018)
#define MOVECONTROLLOOP_TIME_MS		                    (25u)
#define MOVECONTROLLOOP_TIME		                    (OS_TIME_25_MS)
#define MOVECONTROLLOOP_PERSECOND	                    (40u)    //(1000u/MOVECONTROLLOOP_TIME_MS)

#define LOWSPEED_RPM		                             (1422u) //9Rpm * 158   9Rpm  
#define STARTUP_SPEED                                        (2212u) //14Rpm * 158  14Rpm

#define STANDARD_28_SPEED                                    (4424u) //28Rpm * 158  28Rpm
#define MIDDLE_26_SPEED                                      (4108u) //26Rpm * 158  26Rpm
#define MIDDLE_24_SPEED                                      (3792u) //24Rpm * 158  24Rpm
#define MIDDLE_22_SPEED                                      (3476u) //22Rpm * 158  22Rpm
#define MIDDLE_20_SPEED                                      (3160u) //20Rpm * 158  20Rpm
#define MIDDLE_18_SPEED                                      (2844u) //18Rpm * 158  18Rpm
#define MIDDLE_16_SPEED                                      (2528u) //16Rpm * 158  16Rpm
#define MIDDLE_14_SPEED                                      (2212u) //14Rpm * 158  14Rpm
#define MIDDLE_12_SPEED                                      (1896u) //12Rpm * 158  12Rpm
#define MININUM_10_SPEED                                     (1580u) //10Rpm * 158  10Rpm


#define ACC_SOFT                                            (4000u)
#define ACC_FAST                                            (8000u)
#define ACC_BRUTAL                                          (16000u)

#define BLOCKINGTIME_MS			                    (OS_TIME_500_MS)
#define MOTORBLOCKED_PWM_MINIMUMVALUE                       (750u)
#define LOWSPEEDTIME_MS			                    (OS_TIME_1_S)
#define MOTORLOWSPEED_PWM_MINIMUMVALUE                      (750u)
#define MOVEFORCED_MS			                    (OS_TIME_1_S)
#define SECULOCK_RELEASE_TIME_MS			    (OS_TIME_1_S)
    
#define FACTORY_ENCODER_INVERTED	                    (FALSE)   
#define FACTORY_MOTOR_INVERTED		                    (TRUE)//(FALSE)   
#define FACTORY_ABS_POS_UP		                    ((TU32) ONE_PRODUCT_TURN)
#define MINIMUM_UEL_TO_DEL_DISTANCE	                    ((TU32) ONE_PRODUCT_TURN)  
#define FACTORY_ABS_POS_DOWN		                    ((TU32) ((TU32) 0xFFFFFFFFu - (TU32) ONE_PRODUCT_TURN))

#define GEARBOX_DEADZONE_COMPENSATION	                    (0u)       

#if USE_RETURNMOVE == 1
#define RETURNMOVELENGHT                                    (GEARBOX_DEADZONE_COMPENSATION)  
#else
#define RETURNMOVELENGHT                                    (0u)
#endif

//Tilt lengh is = SHORTMOVE_LENGHT_MOTORTURN * Tilt_size
#define MAX_TILT_SIZE                                   (22u)  // 92¶È
#define MIN_TILT_SIZE                                   (2u)   // 8¶È
#define DEFAULT_TILT_SIZE                               (6u)   // 25¶È 
//The default value for the tilting speed is 24¡ã
#define TILT_SIZE_CHANGE_INCREMENT                      (2u)   // 16¶È 

#define POSITION_TOLERANCE      	                (2u)   
#define POSITION_TOLERANCE_FOR_SEGMENT_MOTORTURN	(1u)
#define SPEED_TOLERANCE         	                (125u) //RU24 96u
      

#ifdef  RU24           
#define PRODUCT_TYPE			                "QT30 V2"
#define REF_5M				                 5039678
#define FEEDBACK_LENGHT_MOTORTURN	                (20u)//(12u)
#define SHORTMOVE_LENGHT_MOTORTURN	                (2u)
#endif
#ifdef  ST30           
#define PRODUCT_TYPE			                "ST30 V1"
#define REF_5M				                 5039678
#define FEEDBACK_LENGHT_MOTORTURN	                (20u)//(12u)
#define SHORTMOVE_LENGHT_MOTORTURN	                (2u)
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////MEMORIES SETUP///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SEGMENT_STACK_DEPTH		                (10u)
#define MOVE_STACK_DEPTH		                (16u)
//´®¿ÚÐ´»º´æ
#define UART_WRITEBUFFER_LENGHT                         (255u)
//´®¿Ú¶Á»º´æ
#define UART_READBUFFER_LENGHT                          (64u)
#define SQC_EVENT_LOG_DEPTH                             (44u)


//2MIN
#define LED_2MIN                                        (120u)
//15MIN
#define LED_15MIN                                       (900u)
//6MIN
#define MAXMOVETIME_MS                                  (360000u)
//30MIN
#define CHARGER_PERIOD_MS                               (1800000u)
//4H
#define AUTOSAVE_PERIOD_MS                              (14400000u)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////LowPower SETUP///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Number of ADC measure
#define ADC_SAMPLE_PER_MEASURE                          (1u)
#define ADC_MEASURE_PER_BURST                           (3u)
#define ADC_MEASURE_RSSI_SENSITIVITY                    (50u)
#define TIME_BETWEEN_BURST                              (AWU_TIMEBASE_64MS)
#define RTS_ACTIVITY_TIMEOUT                            (OS_TIME_60_MS)
#define BUTTON_ACTIVITY_TIMEOUT                          OS_TIME_1_MIN

#define BUTTON_150Ms                                    (150u)  
#define BUTTON_500Ms                                    (436u)  
#define BUTTON_1SEC                                     (1000u)
#define BUTTON_2SEC                                     (2000u)
#define BUTTON_7SEC                                     (7000u)
#define BUTTON_12SEC                                    (12000u)
#define BUTTON_13SEC                                    (13000u)

#define MOTOR_HOT_PROTECT_COUNT                         (240000u)//4min*60
#define MOTOR_HOT_RELEASE_COUNT                         (2000u)  //

#define sign(a) ( ((a) < 0) ? (-1) : ( ((a) > 0) ? (1) : (0) )  )


/**********************************************************************
Copyright©(2010),SomfySAS.Allrightsreserved.
Allreproduction,useordistributionofthissoftware,inwholeor
inpart,byanymeans,withoutSomfySASpriorwrittenapproval,is
strictlyforbidden.
***********************************************************************/
