// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

/**
* @file Encoder_Driver.c
* @addtogroup Encoder
* @ingroup Services
* @{
* @author Gabriel Mula
* @date 2016
* @brief Encoder, compute speed and position.
* @details By using STM8 Timer 1 encoder decoder hardware, this object compute speed and position

*/

#include "Config.h"
#include "SFY_OS.h"
#include "stm8s_tim1.h"
#include "stm8s_tim2.h"
#include "stm8s_gpio.h"
#include "Pcb.h"
#include "stm8s_clk.h"
#include "RTS_InputCapture.h"
#include "Encoder_Driver.h"
#include "STM8_Archiver.h"
#include "SQC_DataProvider.h"
#include "EncoderDriver_DriftCorrector.h"

#ifdef ENCODER_PRINT_DEBUG	
#include "DebugPrint.h"
#include "Encoder_Driver_DebugTXT.h"
#endif

#ifdef ENCODER_PRINT_INFO
#include "DebugPrint_info.h"
#include "Encoder_Driver_DebugTXT.h"
#endif

#ifdef ENCODER_PRINT_LOG	
#include "DebugPrint_Log.h"
#include "Encoder_Driver_DebugTXT.h"
#endif

typedef struct
{
  TBool Started;
  TU16  StartTimerValue;
  TU16  StopTimerValue;
  TBool Overflow;
  EncoderDriverMovingDirection_e Dir;
}MotorRotTimeTimer_s;


typedef struct EncoderDriver_PersistantData_t
{
  TU32 MotorTurnDown;
  TU32 MotorTurnUp;
  TU16 StateMissMatch;
  TU16 StateMatch;
  TU16 DirASum;
  TU16 DirA_00_to_01;
  TU16 DirA_01_to_11;
  TU16 DirA_11_to_10;
  TU16 DirA_10_to_00;  
  TU16 DirBSum;
  TU16 DirB_00_to_10;
  TU16 DirB_10_to_11;
  TU16 DirB_11_to_01;
  TU16 DirB_01_to_00;  
  TU16 Err_00_to_11 ; 
  TU16 Err_11_to_00 ; 
  TU16 Err_10_to_01 ; 
  TU16 Err_01_to_10 ; 
  TS16 DriftCompensationSum;
  EncoderPosition_s Position_s;
  TBool IsInverted;
  EncoderDriverMovingDirection_e LastMoveDirection;
  MagWheelState_e OnSleepState;
  MagWheelState_e OnWakeUpState;
  TBool FirstPowerOn;
}EncoderDriver_PersistantData_t;

typedef struct EncoderDriver_SQC_s
{
  TU32 MotorTurnDown;   //向下
  TU32 MotorTurnUp;     //向上
  TU16 StateMissMatch;  //状态丢失
  TU16 StateMatch;      //状态符合
  TU16 DirASum;
  TU16 DirA_00_to_01;
  TU16 DirA_01_to_11;
  TU16 DirA_11_to_10;
  TU16 DirA_10_to_00;  
  TU16 DirBSum;
  TU16 DirB_00_to_10;
  TU16 DirB_10_to_11;
  TU16 DirB_11_to_01;
  TU16 DirB_01_to_00;  
  TU16 Err_00_to_11 ; 
  TU16 Err_11_to_00 ; 
  TU16 Err_10_to_01; 
  TU16 Err_01_to_10 ; 
  TS16 DriftCompensationSum;                              //漂移补偿
  TU32 Position;                                          //位置
  TBool IsInverted;                                       //是否反向
  EncoderDriverMovingDirection_e LastMoveDirection;       //上次运行方向
  EncoderDriverMovingDirection_e CurrentMovingDirection;  //当前运行方向 
}EncoderDriver_SQC_s;



typedef struct EncoderDriver_t
{
  EncoderPosition_s OnInit_Position;
  EncoderDriver_PersistantData_t Pdata;
  EncoderDriver_SQC_s SQC;
  TBool EnablePosCounTing;
  
  TS16  CurrentSpeed;
  TS16  LastSpeed;
  
  EncoderDriverMovingDirection_e MovingDirection;
  
  MotorRotTimeTimer_s  PreviousRotTimer;
  MotorRotTimeTimer_s  CurrentRotTimer;
  
}EncoderDriver_t;




__no_init EncoderDriver_t EncoderDriver;
#define me EncoderDriver
//#define TIMEOUT_ROTTIME (40000/US_PER_TIMER_TICK)
#define TIMEOUT_ROTTIME (65000/US_PER_TIMER_TICK)   //sage  100ms


// ****************************************************************************
// Private METHOD
// ****************************************************************************
/**
* @brief Power off sensor (cut off power supply)
*/
static void PowerOff_Sensor();

/**
* @brief Power On sensor 
*/
static void PowerOn_Sensor();

/**
* @brief Get timer 1 counter register. represent the magnetic wheel angular position within a motor turn
* @return Timer 1 counter register [0-7]
*/
static TU16 EncoderDriver_GetRegister();

static void IT_CountMotorTurnAndComputeDirection();

static void IT_StartMotorRotationTimeTimer();
static void IT_SwitchMotorRotationTimeTimer();
static void IT_StopMotorRotationTimeTimer();

static void IT_SetMotorRotationTimeTimerTimeout(TU32 Dt);
static void IT_DisableMotorRotationTimeTimerTimeout();

static void IT_ComputeMotorCurrentSpeedRpm(MotorRotTimeTimer_s *Timer);

static TU8* Get_SQC_data(TU8* DataSize);
static EncoderPosition_s CorrectPosition(EncoderPosition_s Pos, TS8 Correction);


/**
* @brief Initialize object, register to interface ext...
*/
void EncoderDriver_Init()
{
  //Note that position in RAM is store on init, to be restored in case of hot reset
  me.OnInit_Position = me.Pdata.Position_s;
  me.EnablePosCounTing = FALSE;
  me.MovingDirection = EncoderDriver_Stop;
  me.CurrentSpeed = 0U;
  me.LastSpeed = 0U;
  
  me.CurrentRotTimer.Started = FALSE;
  
  Archiver_RegisterObject(&me.Pdata,
                          EncoderDriver_SetPersistantDataToFactoryValue,
                          sizeof(me.Pdata),
                          TRUE);
  //SQC 
  SQC_DataProviderNameInterface_s intf;
  intf.Fptr_Get = Get_SQC_data;
  SQC_DataProvider_RegisterDataSource(SQC_DataProvider_Encoder ,&intf);
  
  
  EncoderDriver_SetUpHardware_LowPower();
}


/**
* @brief Create object. In case of hot reset, position in ram is taken
*/
void EncoderDriver_Create(TBool HotReset)
{ 
  Archiver_LoadMe(&me.Pdata);  //me EncoderDriver
  
  if(HotReset == TRUE)
  {
    EncoderDriver_SetPositionFine(me.OnInit_Position);
    // In case of hot reset, position in ram is taken
  }
  
  EncoderDriver_SensorCmd(FALSE);
}

/**
* @brief SQC_DataProvider implementation. Fill SQC struct with Pdata
*/
static TU8* Get_SQC_data(TU8* DataSize)
{
  //Take and return snapshot
  *DataSize = sizeof(me.SQC);
  me.SQC.MotorTurnDown  = me.Pdata.MotorTurnDown;   //向下转
  me.SQC.MotorTurnUp    = me.Pdata.MotorTurnUp;     //向上转
  
  me.SQC.StateMissMatch = me.Pdata.StateMissMatch; //状态不匹配
  me.SQC.StateMatch     = me.Pdata.StateMatch;     //状态
  me.SQC.DirASum        = me.Pdata.DirASum;
  me.SQC.DirA_00_to_01  = me.Pdata.DirA_00_to_01;
  me.SQC.DirA_01_to_11  = me.Pdata.DirA_01_to_11;
  me.SQC.DirA_11_to_10  = me.Pdata.DirA_11_to_10;
  me.SQC.DirA_10_to_00  = me.Pdata.DirA_10_to_00;
  
  me.SQC.DirBSum        = me.Pdata.DirBSum;
  me.SQC.DirB_00_to_10  = me.Pdata.DirB_00_to_10;
  me.SQC.DirB_10_to_11  = me.Pdata.DirB_10_to_11;
  me.SQC.DirB_11_to_01  = me.Pdata.DirB_11_to_01;
  me.SQC.DirB_01_to_00  = me.Pdata.DirB_01_to_00;
  
  me.SQC.Err_00_to_11   = me.Pdata.Err_00_to_11;
  me.SQC.Err_11_to_00   = me.Pdata.Err_11_to_00;
  me.SQC.Err_10_to_01   = me.Pdata.Err_10_to_01;
  me.SQC.Err_01_to_10   = me.Pdata.Err_01_to_10;
  
  me.SQC.DriftCompensationSum = me.Pdata.DriftCompensationSum; //漂移补偿
  me.SQC.Position   = me.Pdata.Position_s.MotorTurn;           //电机运行圈数
  me.SQC.IsInverted = me.Pdata.IsInverted;                     //encode 是否反向
  me.SQC.LastMoveDirection = me.Pdata.LastMoveDirection;       //上次的运行方向
  me.SQC.CurrentMovingDirection = me.MovingDirection;          //当前的运行方向
  return (TU8*) &me.SQC;
}




/**
* @brief Set Pdata default value
* 编码器数据 设置默认参数
*/
void  EncoderDriver_SetPersistantDataToFactoryValue()
{
  me.Pdata.MotorTurnDown  = 0U;
  me.Pdata.MotorTurnUp    = 0U;
  me.Pdata.StateMissMatch = 0U;
  me.Pdata.StateMatch     = 0U;
  me.Pdata.DirASum        = 0U;
  me.Pdata.DirA_00_to_01 = 0U;
  me.Pdata.DirA_01_to_11 = 0U;
  me.Pdata.DirA_11_to_10 = 0U;
  me.Pdata.DirA_10_to_00 = 0U;  
  me.Pdata.DirBSum       = 0U;
  me.Pdata.DirB_00_to_10 = 0U;
  me.Pdata.DirB_10_to_11 = 0U;
  me.Pdata.DirB_11_to_01 = 0U;
  me.Pdata.DirB_01_to_00 = 0U;  
  me.Pdata.Err_00_to_11  = 0U;  
  me.Pdata.Err_11_to_00  = 0U;  
  me.Pdata.Err_10_to_01  = 0U;  
  me.Pdata.Err_01_to_10  = 0U;  
  me.Pdata.DriftCompensationSum = 0U;
  me.Pdata.LastMoveDirection = EncoderDriver_UnRoll;
  me.Pdata.IsInverted = FACTORY_ENCODER_INVERTED;
  EncoderDriver_SetPositionCoarse(FACTORY_ABS_POS) ; //当前位置重设  设置为出厂位置坐标
  me.Pdata.FirstPowerOn = TRUE;
}



/**
* @brief invert counting direction   是否反转编码器方向
*/
void EncoderDriver_InvertCountingDirection()
{
  EncoderDriver_SensorCmd(FALSE);
  me.Pdata.IsInverted = (me.Pdata.IsInverted == TRUE) ? (FALSE):(TRUE);
  
  EncoderDriver_SensorCmd(TRUE);
}


/**
* @brief Set position, fine counter set to the defult value  
* 限位中间值 ((TU32) 0x7FFFFFFFu)
*/
__monitor void EncoderDriver_SetPositionCoarse(TU32 NewPos)
{
  //set new position                       //初始化起始位置 电机圈数初始化
  me.Pdata.Position_s.MotorTurn = NewPos;
  //reset fine counter in Pdata and Hardware register
  me.Pdata.Position_s.MotorPosition = 0U;  //初始化电机角度 ( 0 1 2 3 4 ) 5个刻度450度 一个刻度即为90度  sage 
  TIM1_SetCounter(me.Pdata.Position_s.MotorPosition);//time1 捕捉编码器数据
  
  TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
  TIM2_ClearITPendingBit(TIM2_IT_UPDATE);  
}


/**
* @brief Set position,coarse and fine
*/
__monitor void EncoderDriver_SetPositionFine(EncoderPosition_s NewPos)
{
  me.Pdata.Position_s = NewPos;
  TIM1_SetCounter(me.Pdata.Position_s.MotorPosition);
  
  TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
  TIM2_ClearITPendingBit(TIM2_IT_UPDATE); 
}

/**
* @brief Setup GPIO and disable timer 1 periph
*/
void EncoderDriver_SetUpHardware_LowPower()
{
  TIM1_DeInit();
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1,DISABLE);
  
  GPIO_Init(Encoder_ChipSelect_PORT,Encoder_ChipSelect_PIN,Encoder_ChipSelect_MODE);
  GPIO_WriteLow(Encoder_ChipSelect_PORT,Encoder_ChipSelect_PIN);
  
//  GPIO_Init(Encoder1_PORT,Encoder1_PIN,GPIO_MODE_OUT_PP_LOW_SLOW);
//  GPIO_Init(Encoder2_PORT,Encoder2_PIN,GPIO_MODE_OUT_PP_LOW_SLOW);
//  GPIO_WriteLow(Encoder1_PORT,Encoder1_PIN);
//  GPIO_WriteLow(Encoder2_PORT,Encoder2_PIN);  sage

}

/**
* @brief Setup timer 1 periph
*/
void EncoderDriver_SetUpHardware_HighPower()
{
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1,ENABLE);
  TIM1_DeInit();
  
  GPIO_Init(Encoder_ChipSelect_PORT,Encoder_ChipSelect_PIN,Encoder_ChipSelect_MODE);
  GPIO_Init(Encoder1_PORT,Encoder1_PIN,Encoder1_MODE);
  GPIO_Init(Encoder2_PORT,Encoder2_PIN,Encoder2_MODE);
  GPIO_WriteLow(Encoder_ChipSelect_PORT,Encoder_ChipSelect_PIN);
  
  if(me.Pdata.IsInverted == FALSE)
  {
    TIM1_EncoderInterfaceConfig(TIM1_ENCODERMODE_TI12,TIM1_ICPOLARITY_RISING,TIM1_ICPOLARITY_RISING);
  }
  else
  {
    TIM1_EncoderInterfaceConfig(TIM1_ENCODERMODE_TI12,TIM1_ICPOLARITY_FALLING,TIM1_ICPOLARITY_RISING);
  }
  
  TIM2_SelectOCxM(TIM2_CHANNEL_3,TIM2_OCMODE_TIMING);
  TIM2_CCxCmd(TIM2_CHANNEL_3,DISABLE);//关闭比较器3
  
  // 0 count => 8 position  8次改变
  TIM1_SetAutoreload(TICK_PER_MOTOR_TURN - 1);
 // TIM1_SetAutoreload(TICK_PER_MOTOR_TURN);//修改 sage  5
  TIM1_ARRPreloadConfig(DISABLE);
  TIM1_ITConfig(TIM1_IT_UPDATE ,ENABLE);    
  TIM1_Cmd(ENABLE);
}

/**
* @brief Reset position to factory value. Direction is not reset
* 当前位置重设  把当前位置设置为上限位位置
*/
void EncoderDriver_Reset()
{
  EncoderDriver_SetPositionCoarse(FACTORY_ABS_POS) ; //当前位置重设  设置为出厂位置坐标
}


/**
* @brief Get timer 1 counter register or, when encoder are power off, the last saved value
*/
static TU16 EncoderDriver_GetRegister()
{
  return (EncoderDriver_IsPowerOn() == TRUE &&  me.EnablePosCounTing == TRUE) ? ((TU16) TIM1_GetCounter()) : (me.Pdata.Position_s.MotorPosition);
}

/**
* @brief Getter, get only corse position
*/
__monitor TU32 EncoderDriver_GetPosition()
{
  return me.Pdata.Position_s.MotorTurn;
}

/**
* @brief Getter, get fine position 
*/
__monitor EncoderPosition_s EncoderDriver_GetFinePosition()
{
  me.Pdata.Position_s.MotorPosition = EncoderDriver_GetRegister();
  return me.Pdata.Position_s;
}

/**
* @brief Force update fine position
*/
__monitor void EncoderDriver_UpdateFinePosition()
{
  me.Pdata.Position_s.MotorPosition = EncoderDriver_GetRegister();
}


/**
* @brief High level function to disable/enable encoder.
* @details Power on and off encoder safely. A 10 ms delays is perform on power up to avoid jitter transition.
* It also monitor encoder state Before/after wake
*/
void EncoderDriver_SensorCmd(TBool State)
{
  if(State == TRUE && EncoderDriver_IsPowerOn() == FALSE)
  { //编码器功能打开  之前是关闭状态
    DEBUG_PRINT("\n[DBG][Encoder] Power On Encoder");
    //Store position
    EncoderPosition_s Pos = me.Pdata.Position_s;
    
    //position may change dud to jitter when power on hall sensor
    PowerOn_Sensor();
    SFY_OS_TaskDelay(OS_TIME_10_MS);
    
    //Get encoder state 获取漂移编码器当前状态
    me.Pdata.OnWakeUpState = DriftCorrector_GetState();
    
    //To avoid false state missmatch on first power on, or when performing move with counter disable (indus)
    if(me.Pdata.FirstPowerOn == TRUE || me.EnablePosCounTing == FALSE)
    {
      DEBUG_PRINT("\n[DBG][Encoder] Compensation Disable this time");
      me.Pdata.FirstPowerOn = FALSE;
      me.Pdata.OnSleepState = me.Pdata.OnWakeUpState;
    }
    
    //Count missmatch         醒来状态和睡眠时不同
    if(me.Pdata.OnWakeUpState != me.Pdata.OnSleepState)
    {
      SQC_Count(&me.Pdata.StateMissMatch,sizeof(me.Pdata.StateMissMatch));
    }
    
    //Get transition and count it  获取转化并计算
    MagWheelTransition_e Transition =  MagWheelTransition_Err_NoTransition;
    //漂移转化 霍尔信号转化
    Transition = DriftCorrector_GetTransition( me.Pdata.OnSleepState, me.Pdata.OnWakeUpState );
    DEBUG_PRINT2("\n[DBG][Encoder] Pwr on State = %s, Pwr off State = %s" ,MagWheelState_TXT[me.Pdata.OnWakeUpState],MagWheelState_TXT[me.Pdata.OnSleepState]);
    DEBUG_PRINT1("\n[DBG][Encoder] Transition = %s" ,MagWheelTransition_TXT[Transition]);
    
    switch(Transition)
    {
      case MagWheelTransition_DirA_00_to_01 :
        SQC_Count(&me.Pdata.DirA_00_to_01,sizeof(me.Pdata.StateMissMatch));
        SQC_Count(&me.Pdata.DirASum,sizeof(me.Pdata.StateMissMatch));
        break;
      case MagWheelTransition_DirA_01_to_11 :
        SQC_Count(&me.Pdata.DirA_01_to_11,sizeof(me.Pdata.StateMissMatch));
        SQC_Count(&me.Pdata.DirASum,sizeof(me.Pdata.StateMissMatch));
        break;
      case MagWheelTransition_DirA_11_to_10 :
        SQC_Count(&me.Pdata.DirA_11_to_10,sizeof(me.Pdata.StateMissMatch));
        SQC_Count(&me.Pdata.DirASum,sizeof(me.Pdata.StateMissMatch));
        break;
      case MagWheelTransition_DirA_10_to_00 :
        SQC_Count(&me.Pdata.DirA_10_to_00,sizeof(me.Pdata.StateMissMatch));
        SQC_Count(&me.Pdata.DirASum,sizeof(me.Pdata.StateMissMatch));
        break;
        
      case MagWheelTransition_DirB_00_to_10 :
        SQC_Count(&me.Pdata.DirB_00_to_10,sizeof(me.Pdata.StateMissMatch));
        SQC_Count(&me.Pdata.DirBSum,sizeof(me.Pdata.StateMissMatch));
        break;
      case MagWheelTransition_DirB_10_to_11 :       
        SQC_Count(&me.Pdata.DirB_10_to_11,sizeof(me.Pdata.StateMissMatch));
        SQC_Count(&me.Pdata.DirBSum,sizeof(me.Pdata.StateMissMatch));
        break;
      case MagWheelTransition_DirB_11_to_01 :
        SQC_Count(&me.Pdata.DirB_11_to_01,sizeof(me.Pdata.StateMissMatch));
        SQC_Count(&me.Pdata.DirBSum,sizeof(me.Pdata.StateMissMatch));
        break;
      case MagWheelTransition_DirB_01_to_00 :
        SQC_Count(&me.Pdata.DirB_01_to_00,sizeof(me.Pdata.StateMissMatch));
        SQC_Count(&me.Pdata.DirBSum,sizeof(me.Pdata.StateMissMatch));
        break;
        
      case MagWheelTransition_Err_00_to_11 :
        SQC_Count(&me.Pdata.Err_00_to_11,sizeof(me.Pdata.StateMissMatch));
        break;
      case MagWheelTransition_Err_11_to_00 :
        SQC_Count(&me.Pdata.Err_11_to_00,sizeof(me.Pdata.StateMissMatch));
        break;
      case MagWheelTransition_Err_10_to_01 :
        SQC_Count(&me.Pdata.Err_10_to_01,sizeof(me.Pdata.StateMissMatch));
        break;
      case MagWheelTransition_Err_01_to_10 :
        SQC_Count(&me.Pdata.Err_01_to_10,sizeof(me.Pdata.StateMissMatch));
        break;
      case MagWheelTransition_Err_NoTransition:
        SQC_Count(&me.Pdata.StateMatch,sizeof(me.Pdata.StateMissMatch));
        break;
        
      default:
        trap();
        break;
    }
    
    //Based on transition deteted and inversion status, get the correction to applied in tick.
    //基于转化检测和反向状态，获取校正应用
    TS8 Correction = 0;
    Correction = DriftCorrector_GetCorrection(Transition,me.Pdata.IsInverted);
    DEBUG_PRINT1("\n[DBG][Encoder] Correction to add %d" ,Correction);
    //Compute corrected position
    //DEBUG_PRINT2("\n[DBG][Encoder] Position initial [%ld,%d]" ,Pos.MotorTurn,Pos.MotorPosition);

    EncoderPosition_s CorrectedPos = CorrectPosition(Pos,Correction);
    // DEBUG_PRINT2("\n[DBG][Encoder] Corected Position [%ld,%d]" ,CorrectedPos.MotorTurn,CorrectedPos.MotorPosition);
    DEBUG_PRINT("\n");
    //Set corrected position
    EncoderDriver_SetPositionFine(CorrectedPos);
    //Sum corection for SQC diag
    me.Pdata.DriftCompensationSum += Correction;
    
  }
  else if (State == FALSE && EncoderDriver_IsPowerOn() == TRUE)
  {
    DEBUG_PRINT("\n[DBG][Encoder] Power Off Encoder");
    EncoderPosition_s Pos = EncoderDriver_GetFinePosition();
    me.Pdata.OnSleepState = DriftCorrector_GetState();
    PowerOff_Sensor();
    SFY_OS_TaskDelay(OS_TIME_10_MS);
    me.Pdata.Position_s = Pos;
    DEBUG_PRINT2("\n[DBG][Encoder] Position Final [%ld,%d]" ,Pos.MotorTurn,Pos.MotorPosition);
    DEBUG_PRINT1("\n[DBG][Encoder] Pwr off State = %s" ,MagWheelState_TXT[me.Pdata.OnSleepState]);
    DEBUG_PRINT("\n");
    me.CurrentSpeed = 0;
    me.LastSpeed = 0;
    me.MovingDirection = EncoderDriver_Stop;
  }
}

//当前位置 ok sage
static EncoderPosition_s CorrectPosition(EncoderPosition_s Pos, TS8 Correction)
{
  EncoderPosition_s CorrectedPos = Pos;
  

#if (USE_DRIFT_COMPENSATION_ALGO == TRUE)
  // TickSum : note that MotorPosition [0;TICK_PER_MOTOR_TURN]
  TS8 TickSum = (Pos.MotorPosition + Correction);
  TU8 absTickSum = (TU8) abs( (int) TickSum);
  
  //Correction make MotorPosition overflow (++ side  TICK_PER_MOTOR_TURN... => ... 0) => MotorTurn must also be update
  if(TickSum >= 0) //值大于0
  {
    TU16 MotorTurnToAdd = TickSum / TICK_PER_MOTOR_TURN;           //增加了多少圈
    CorrectedPos.MotorPosition = TickSum % TICK_PER_MOTOR_TURN;    //余下多少角度
    CorrectedPos.MotorTurn += MotorTurnToAdd;
  }
  //Correction make MotorPosition overflow (-- side  0... => ... TICK_PER_MOTOR_TURN) => MotorTurn must also be update
  else 
  {//值小于0
    TU16 MotorTurnToAdd = (absTickSum / TICK_PER_MOTOR_TURN) + 1;  //改变了多少圈
    CorrectedPos.MotorPosition = ((TICK_PER_MOTOR_TURN ) - (absTickSum % TICK_PER_MOTOR_TURN)) % TICK_PER_MOTOR_TURN;//余下多少角度
    CorrectedPos.MotorTurn -= MotorTurnToAdd;
  }
#endif
  return CorrectedPos;
}


void EncoderDriver_PosCounterCMD(TBool State)
{
  static EncoderPosition_s MemPos = {0,0};
  
  if(MemPos.MotorTurn == 0)
  {
    MemPos = EncoderDriver_GetFinePosition();  //初始化相位角
  }
    
  //When enable poscounter, restore position
  if(me.EnablePosCounTing == FALSE && State == TRUE)
  {
    EncoderDriver_SetPositionFine(MemPos);//
  }
  //when disable poscounter, backup position
  else if(me.EnablePosCounTing == TRUE && State == FALSE)
  {
     MemPos = EncoderDriver_GetFinePosition();
  }
  me.EnablePosCounTing = State;
  
}

static void PowerOn_Sensor()
{
  EncoderDriver_SetUpHardware_HighPower();
  GPIO_WriteHigh(Encoder_ChipSelect_PORT,Encoder_ChipSelect_PIN);
}

static void PowerOff_Sensor()
{
  EncoderDriver_SetUpHardware_LowPower();
  GPIO_WriteLow(Encoder_ChipSelect_PORT,Encoder_ChipSelect_PIN);
}

TBool EncoderDriver_IsPowerOn()
{
  BitStatus Pin =  GPIO_ReadInputPin(Encoder_ChipSelect_PORT,Encoder_ChipSelect_PIN);
  return (Pin == RESET ? (FALSE) : (TRUE));
}

__monitor TS16 EncoderDriver_GetSpeed()
{
  return me.CurrentSpeed;
}

__monitor EncoderDriverMovingDirection_e EncoderDriver_GetMoveDirection()
{
  return me.MovingDirection;
}


void EncoderDriver_IT_Handler_Timer2_Update()
{
  me.CurrentRotTimer.Overflow = TRUE;
}

//定时器1数据更新
void EncoderDriver_IT_Handler_Timer1_Update()
{
  IT_CountMotorTurnAndComputeDirection();
  me.CurrentRotTimer.Dir = me.MovingDirection;
  
  if(TIM2_GetITStatus(TIM2_IT_UPDATE) == SET)
  {
    TIM2_ClearITPendingBit(TIM2_IT_UPDATE); 
    me.CurrentRotTimer.Overflow = TRUE;
  }
  
  if(me.CurrentRotTimer.Started == TRUE)
  {
    IT_SwitchMotorRotationTimeTimer();//停止数值 然后计算速度
    IT_ComputeMotorCurrentSpeedRpm(&me.PreviousRotTimer);//计算速度
  }else
  {
    IT_StartMotorRotationTimeTimer();//初始数值
    me.CurrentSpeed = 0U;
  }
  
  IT_SetMotorRotationTimeTimerTimeout(TIMEOUT_ROTTIME);
}



static void IT_CountMotorTurnAndComputeDirection()
{
  TU16 FinePos = TIM1_GetCounter();

  
  if(FinePos > 2u)
  {
    //Underflow  向下溢出
    if(me.EnablePosCounTing == TRUE)//位置计算 使能
    {
      --me.Pdata.Position_s.MotorTurn;//圈数减少
      me.Pdata.Position_s.MotorPosition = FinePos;
    }
    me.Pdata.MotorTurnDown++;
    me.MovingDirection = EncoderDriver_Roll;//up roll
  }else
  {
    //overflow  向上溢出
    if(me.EnablePosCounTing == TRUE)//位置计算 使能
    {
      ++me.Pdata.Position_s.MotorTurn;//圈数增加
      me.Pdata.Position_s.MotorPosition = FinePos; 
    }
    me.Pdata.MotorTurnUp++;
    me.MovingDirection = EncoderDriver_UnRoll; //down roll 
  }
  
  me.Pdata.LastMoveDirection = me.MovingDirection;//运行方向
}

static void IT_StartMotorRotationTimeTimer()
{
  me.CurrentRotTimer.Started = TRUE;
  me.CurrentRotTimer.StartTimerValue = TIM2_GetCounter();
  me.CurrentRotTimer.Overflow = FALSE;
}

static void IT_SwitchMotorRotationTimeTimer()
{
  me.CurrentRotTimer.StopTimerValue = TIM2_GetCounter();
  me.PreviousRotTimer = me.CurrentRotTimer;
  me.CurrentRotTimer.Started = TRUE;
  me.CurrentRotTimer.StartTimerValue = me.PreviousRotTimer.StopTimerValue;
  me.CurrentRotTimer.StopTimerValue = 0U;
  me.CurrentRotTimer.Overflow = FALSE;
}

static void IT_StopMotorRotationTimeTimer()
{
  me.CurrentRotTimer.Started = FALSE;
}

static void IT_SetMotorRotationTimeTimerTimeout(TU32 Dt)
{
  TIM2_ITConfig(TIM2_IT_CC3,DISABLE);
  
  TU32 TimeOutv = (TU32) TIM2_GetCounter() + Dt;
  if(TimeOutv > 0xFFFFu)
  {
    TimeOutv %= 0xFFFFu;
  } 
  TIM2_SetCompare3((TU16) TimeOutv);
  TIM2_ITConfig(TIM2_IT_CC3,ENABLE);
  TIM2_ClearITPendingBit(TIM2_IT_CC3);
}

static void IT_DisableMotorRotationTimeTimerTimeout()
{
  TIM2_ITConfig(TIM2_IT_CC3,DISABLE);
  TIM2_ClearITPendingBit(TIM2_IT_CC3);
}

void EncoderDriver_IT_MotorRotationTimeTimerTimeout()
{
  IT_StopMotorRotationTimeTimer();
  IT_DisableMotorRotationTimeTimerTimeout();
  me.CurrentSpeed = 0U;
  me.MovingDirection = EncoderDriver_Stop;
}


#define OneMinuteInUS   (60000000u)
//计算当前速度
static void IT_ComputeMotorCurrentSpeedRpm(MotorRotTimeTimer_s *Timer)
{
  me.LastSpeed = me.CurrentSpeed;
  TU32 DT_us = 0U;
  
  if(Timer->Overflow == FALSE)//时钟没有溢出
  {
    DT_us = Timer->StopTimerValue - Timer->StartTimerValue;
  }else                       //时钟溢出
  {
    DT_us = ((0xFFFFu - Timer->StartTimerValue) + Timer->StopTimerValue);
  }
  DT_us *= US_PER_TIMER_TICK;
  
  me.CurrentSpeed =  (TS16) ((OneMinuteInUS / DT_us) * (TS8) Timer->Dir) ;
}



/** @}*/