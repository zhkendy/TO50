// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#include "Config.h"
#include "stm8s_tim3.h"
#include "stm8s_clk.h"
#include "stm8s_gpio.h"
#include "Pcb.h"
#include "STM8_Archiver.h"
#include "SQC_DataProvider.h"
#include "Motor_Driver.h"

/**
* @file Motor_Driver.c
* @brief blabla
* @details blabla
* @author Gabriel Mula
* @date Septembre 2016
*/

/**
*  @struct MotorDriverPersistantData_t
*  @brief Persistant data of the motor driver.
*  @details Those data are save/Load by Archiver. @see STM8_Archiver.c
*/
typedef struct MotorDriverPersistantData_t
{
  TBool IsMotorPolarityInverted;
  TBool IsMotorInverted;
  MotorDirection_e LastMoveDirection;
  TU32 MotorBlocked;
}MotorDriverPersistantData_t;

typedef struct
{
  TBool IsEnable;
  MotorDriverPersistantData_t Pdata;
  TU16 Periode;
  void (*f_GoUP)(TU16 DutyCycle);
  void (*f_GoDown)(TU16 DutyCycle);
}MotorDriver_t;


static void SetPWM(TU16 A, TU16 B);
static void MotorDriver_UpdateDirection();
 
static MotorDriver_t MotorDriver;
#define me MotorDriver
static void MotorDriver_SetUpHardware();
static TU8* Get_SQC_data(TU8* DataSize);



void MotorDriver_Create()
{
  Archiver_LoadMe(&me.Pdata);         //me MotorDriver
  MotorDriver_UpdateDirection();
  MotorDriver_RestoreFromLowpower();
}

void MotorDriver_Init()
{
  MotorDriver_SwitchToLowPower();
  Archiver_RegisterObject( &me.Pdata, //me MotorDriver
                          MotorDriver_SetPersistantDataToFactoryValue,
                          sizeof(me.Pdata),
                          TRUE); 
      //SQC 
  SQC_DataProviderNameInterface_s intf;
  intf.Fptr_Get = Get_SQC_data;
  SQC_DataProvider_RegisterDataSource(SQC_DataProvider_MotorDriver ,&intf);
}


void MotorDriver_CountMotorBloccked()
{
  SQC_Count(&me.Pdata.MotorBlocked,sizeof(me.Pdata.MotorBlocked));
}

static TU8* Get_SQC_data(TU8* DataSize)
{
  //Take and return snapshot
  *DataSize = sizeof(me.Pdata);
  return (TU8*) &me.Pdata;
}

bool Get_MotorDriver_Rotation()
{
  return ((me.Pdata.IsMotorInverted == TRUE) ?(TRUE): (FALSE));
}


void MotorDriver_InvertRotation()
{
  me.Pdata.IsMotorInverted = (me.Pdata.IsMotorInverted == TRUE) ? (FALSE) : (TRUE);
  MotorDriver_UpdateDirection();
}


static void MotorDriver_UpdateDirection()
{
  if(MotorDriver.Pdata.IsMotorInverted == true)
  {
    MotorDriver.f_GoUP   = &MotorDriver_RotateDirB;
    MotorDriver.f_GoDown = &MotorDriver_RotateDirA;
  }
  else
  {
    MotorDriver.f_GoUP   = &MotorDriver_RotateDirA;
    MotorDriver.f_GoDown = &MotorDriver_RotateDirB;
  }
}

//By convention, going down is counting/positive speed/Positive PWM
//向下 是正的占空比和速度
//占空比  当前速度
void MotorDriver_Move(TS16 DutyCycle,TS16 CurrentSpeed)
{
  if(DutyCycle > 0 && CurrentSpeed >= 0)
  {//占空比大于0  UnRoll
    me.Pdata.LastMoveDirection = MotorDirection_UnRoll;
    MotorDriver.f_GoDown(abs(DutyCycle));
  }
  else if(DutyCycle < 0 && CurrentSpeed <= 0)
  {//占空比小于0  Roll
    me.Pdata.LastMoveDirection = MotorDirection_Roll;
    MotorDriver.f_GoUP(abs(DutyCycle));
  }
  else
  {
    MotorDriver_Brake(ABS_MAX_PWM);//(0);//刹车     关键处 new sage
  }
}


void MotorDriver_Move2(TS16 DutyCycle)
{
    if(DutyCycle > 0 )
    {//占空比大于0  UnRoll
      MotorDriver.f_GoDown(abs(DutyCycle));
    }
    else if(DutyCycle < 0)
    {//占空比小于0  Roll
      MotorDriver.f_GoUP(abs(DutyCycle));
    }
    else
    {//刹车
      MotorDriver_Brake(ABS_MAX_PWM);
    }
}



MotorDirection_e MotorDriver_GetLastMoveDirection()
{
  return me.Pdata.LastMoveDirection;
}

void MotorDriver_SetPersistantDataToFactoryValue()
{
  me.Pdata.LastMoveDirection = MotorDirection_UnRoll;//
  me.Pdata.IsMotorInverted   = FACTORY_MOTOR_INVERTED;
  me.Pdata.MotorBlocked = 0U; 
}


static void MotorDriver_SetUpHardware()
{
  TU32 ClkF =  CLK_GetClockFreq();
  me.Periode = (TU16) (ClkF/H_BRIDGE_FREQ) ; //周期 800 16M/20K=800
   
  GPIO_Init(H_Bridge_PWMA_PORT,H_Bridge_PWMA_PIN,H_Bridge_PWMA_MODE);
  GPIO_Init(H_Bridge_PWMB_PORT,H_Bridge_PWMB_PIN,H_Bridge_PWMB_MODE);
    
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3,ENABLE);
  TIM3_DeInit();  
  
  /* Time base configuration */
  TIM3_PrescalerConfig(TIM3_PRESCALER_1,TIM3_PSCRELOADMODE_IMMEDIATE);//设置预分频值，无产生更新事件
  TIM3_TimeBaseInit(TIM3_PRESCALER_1, me.Periode-1); // 0 is count  //1分频，时钟   800-1开始计数
  
  /* PWM1 Mode configuration: Channel1 */ 
  TIM3_OC1Init(TIM3_OCMODE_PWM1, TIM3_OUTPUTSTATE_ENABLE,0, TIM3_OCPOLARITY_HIGH);
  TIM3_OC1PreloadConfig(ENABLE);//输出比较使能
  
  /* PWM1 Mode configuration: Channel2 */ 
  TIM3_OC2Init(TIM3_OCMODE_PWM1, TIM3_OUTPUTSTATE_ENABLE,0, TIM3_OCPOLARITY_HIGH);
  TIM3_OC2PreloadConfig(ENABLE);
  
  TIM3_ARRPreloadConfig(ENABLE);//自动预装载
  
  /* TIM3 enable counter */
  TIM3_Cmd(ENABLE);
  SetPWM(0,0);
}



void MotorDriver_RotateDirA(TU16 Cst)
{
  TU16 Reverted_Duty_Cycle = (me.Periode - Cst);
  SetPWM(me.Periode,Reverted_Duty_Cycle);
}

void MotorDriver_RotateDirB(TU16 Cst)
{
  TU16 Reverted_Duty_Cycle = (me.Periode - Cst);
  SetPWM(Reverted_Duty_Cycle,me.Periode);
}

void MotorDriver_Brake(TS16 DutyCycle)
{
  TU16 absPWM = (TU16) abs( (int) DutyCycle);
  SetPWM(absPWM, absPWM);
}

//PWM 0-100%
static void SetPWM(TU16 A, TU16 B)
{
  if(me.IsEnable == TRUE)
  {
    TU16 Limited_A = (A > me.Periode) ? (me.Periode) : (A);
    TU16 Limited_B = (B > me.Periode) ? (me.Periode) : (B);
    TIM3_SetCompare1(Limited_A);//设置捕捉比较寄存器。
    TIM3_SetCompare2(Limited_B);
  }
  else
  {
    GPIO_WriteLow(H_Bridge_PWMA_PORT,H_Bridge_PWMA_PIN);   
    GPIO_WriteLow(H_Bridge_PWMB_PORT,H_Bridge_PWMB_PIN);
  }
}


void MotorDriver_SwitchToLowPower()
{
  GPIO_Init(H_Bridge_PWMA_PORT,H_Bridge_PWMA_PIN,H_Bridge_PWMA_MODE);
  GPIO_Init(H_Bridge_PWMB_PORT,H_Bridge_PWMB_PIN,H_Bridge_PWMB_MODE);
  
  GPIO_WriteLow(H_Bridge_PWMA_PORT,H_Bridge_PWMA_PIN);
  GPIO_WriteLow(H_Bridge_PWMB_PORT,H_Bridge_PWMB_PIN);
  //刹车 
  GPIO_WriteLow(BRAKECONTROL_PORT,BRAKECONTROL_PIN);   

  me.IsEnable = FALSE;
  TIM3_DeInit(); 
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3,DISABLE);  
}

void MotorDriver_RestoreFromLowpower()
{
    MotorDriver_SetUpHardware();
    SetPWM(ABS_MAX_PWM,ABS_MAX_PWM);  
    me.IsEnable = TRUE;
}

