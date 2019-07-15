// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************



#include <stdlib.h>
#include "Stack.h"
#include "stm8s_iwdg.h"
#include "stm8s_awu.h"
#include "stm8s_adc2.h"
#include "stm8s_flash.h"
#include "stm8s_tim3.h"
#include "stm8s_clk.h"
#include "stm8s_gpio.h"
#include "pcb.h"
#include "SFY_OS.h"
#include "Platform_Config.h"
#include "UARTComDriver.h"
#include "Chrono.h"
#include "RtsLowPowerManager.h"
#include "Stm8_Adc.h"
#include "STM8_Archiver.h"
#include "SQC_DataProvider.h"
#include "LongTimer.h"
#include "SQC.h"
#include "PowerMonitor.h"
#include "RTS_Ergonomic_Manager.h"



#ifdef LP_PRINT_DEBUG	
#include "DebugPrint.h"
#endif

#ifdef LP_PRINT_INFO
#include "DebugPrint_info.h"
#endif

#ifdef LP_PRINT_LOG	
#include "DebugPrint_Log.h"
#endif

TU8   Prior_charger=0;
TU8   Current_charger=0;
TU8   Charger_LED=0;
bool  Charger_mode=false;
TU32  SleepWakeTime=0;
TBool SleepWakeEnable=false;
TBool SleepModeChange=false;
extern TU8   User_LedMode;
extern TU8   LedMode;
extern bool RADIO_OUTLINE;
extern TU8  Butt_Press;
extern TU16 Butt_Press_last;
extern bool Motor_Protect_Enable;
extern TU8  LowpowerLedMode;
extern TU8  Motor_HotProtect;

typedef struct RtsLowPowerManager_Pdata_SQC_s
{
  TU32 LowPowerCtr;
  TU32 WakeUpByLongTimer;
  TU32 WakeUpByNoise;
  TU32 WakeUpByunpairedRTS;
  TU32 WakeUpByPairedRTS;
  TU32 WakeByPartialRTSframe;
}RtsLowPowerManager_Pdata_SQC_s;



typedef struct RtsLowPowerManager_t
{
  TBool LongTimerWakeup;
  TBool UnPairedRTSFlag;
  TBool PairedRTSFlag;
  TBool RTStiming;
  RtsLowPowerManager_Pdata_SQC_s SQC;
  RSSI_s CurrentRSSI;
  RSSI_s WakeUpRSSI;
  TU16 MeasureArray_RSSI[ADC_MEASURE_PER_BURST];
  TU32 MeasuredLSIFrequency;
}RtsLowPowerManager_t;




static RtsLowPowerManager_t RtsLowPowerManager;
#define me (RtsLowPowerManager)



TBool RtsLowPowerManager_IsRadioActivityDetected();
static TU32 RtsLowPowerManager_MeasureLsiFrequency(void);
static void RtsLowPowerManager_CalibrateAWU();
static void RtsLowPowerManager_SetPdataToFactory();
static TU8* Get_SQC_data(TU8* DataSize);
static void SQC_Counter();
static TU16 Measure_RSSI();
static void AdjustFloor();
static void SleepUntilNextBurst();


void RtsLowPowerManager_Set_UnpairedRTS_Flag()
{
  me.UnPairedRTSFlag = TRUE;
}
void RtsLowPowerManager_Set_PairedRTS_Flag()
{
  me.PairedRTSFlag = TRUE;
}


void RtsLowPowerManager_Set_RTS_Timing_Flag()
{
  me.RTStiming = TRUE;
}

void RtsLowPowerManager_Init()
{
  Archiver_RegisterObject(&me.SQC,
                          RtsLowPowerManager_SetPdataToFactory,
                          sizeof(me.SQC),
                          FALSE);
  
    //SQC 
  SQC_DataProviderNameInterface_s intf;
  intf.Fptr_Get = Get_SQC_data;
  SQC_DataProvider_RegisterDataSource(SQC_DataProvider_LowPower ,&intf);
  me.LongTimerWakeup = FALSE;
  me.PairedRTSFlag   = FALSE;
  me.UnPairedRTSFlag = FALSE;
  me.RTStiming = FALSE;
}



static TU8* Get_SQC_data(TU8* DataSize)
{
  *DataSize = sizeof(me.SQC);
  return (TU8*) &me.SQC;
}

void RtsLowPowerManager_Create()
{
  RtsLowPowerManager_CalibrateAWU();
  RtsLowPowerManager_SetUpHardware_HighPower();
  Archiver_LoadMe(&me.SQC);
}


static void RtsLowPowerManager_SetPdataToFactory()
{
  me.SQC.LowPowerCtr= 0U;
  me.SQC.WakeUpByNoise= 0U;
  me.SQC.WakeUpByunpairedRTS= 0U;
  me.SQC.WakeUpByPairedRTS= 0U;
  me.SQC.WakeUpByLongTimer = 0U;
  me.SQC.WakeByPartialRTSframe = 0U;
}

void RtsLowPowerManager_SetUpHardware_LowPower()
{
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU,ENABLE);
  AWU_DeInit();

  FLASH_SetLowPowerMode(FLASH_LPMODE_POWERDOWN);
  CLK_SlowActiveHaltWakeUpCmd(ENABLE);
  CLK_FastHaltWakeUpCmd(ENABLE);
  
  GPIO_Init(Radio_ChipSelect_PORT,Radio_ChipSelect_PIN,Radio_ChipSelect_MODE);
  GPIO_Init(MVR_ChipSelect_PORT,MVR_ChipSelect_PIN,MVR_ChipSelect_MODE);
  
  GPIO_WriteLow(Radio_ChipSelect_PORT,Radio_ChipSelect_PIN);
  GPIO_WriteHigh(MVR_ChipSelect_PORT,MVR_ChipSelect_PIN);
  
  GPIO_Init(BUTTON_PORT,BUTTON_PIN,BUTTON_MODE);
}

void RtsLowPowerManager_SetUpHardware_HighPower()
{
  AWU_DeInit();
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU,DISABLE);
  GPIO_Init(MVR_ChipSelect_PORT,MVR_ChipSelect_PIN,MVR_ChipSelect_MODE);
  GPIO_WriteHigh(MVR_ChipSelect_PORT,MVR_ChipSelect_PIN);
}

static void SQC_Counter()
{
  SQC_Count(&me.SQC.LowPowerCtr,sizeof(me.SQC.LowPowerCtr));
  
  if(me.LongTimerWakeup == TRUE)
  {
    SQC_Count(&me.SQC.WakeUpByLongTimer,sizeof(me.SQC.WakeUpByLongTimer));
  }
  else if(me.PairedRTSFlag == FALSE && me.UnPairedRTSFlag == FALSE && me.RTStiming == TRUE)
  {
    SQC_Count(&me.SQC.WakeByPartialRTSframe,sizeof(me.SQC.WakeByPartialRTSframe));
  }
  else if(me.PairedRTSFlag == FALSE && me.UnPairedRTSFlag == FALSE && me.RTStiming == FALSE)
  {
    SQC_Count(&me.SQC.WakeUpByNoise,sizeof(me.SQC.WakeUpByNoise));
  }
  else if( me.PairedRTSFlag == TRUE)
  {
    SQC_Count(&me.SQC.WakeUpByPairedRTS,sizeof(me.SQC.WakeUpByPairedRTS));
  }
  else if( me.UnPairedRTSFlag == TRUE)
  {
    SQC_Count(&me.SQC.WakeUpByunpairedRTS,sizeof(me.SQC.WakeUpByunpairedRTS));
  }
  //else => strange, don't handle
  
  me.LongTimerWakeup = FALSE;
  me.PairedRTSFlag = FALSE;
  me.UnPairedRTSFlag = FALSE;
  me.RTStiming = FALSE;
}




static TU16 Measure_RSSI()
{
  TU16 Measure = 0U;
  static u8 count=0;
  
  //Power On Radio chip    RADIO上电
  GPIO_Init(INPUT_CAPTURE_RADIO_PORT,INPUT_CAPTURE_RADIO_PIN,INPUT_CAPTURE_RADIO_MODE);
  
  /*LDRA_INSPECTED 9 S*/
   SET_BIT((Radio_ChipSelect_PORT->ODR),7u);



  
  //WakeUp ADC
  ADC2->CSR &= (uint8_t)(~ADC2_CSR_EOC);//ADC2_ClearFlag()
  ADC2->CR1 |= ADC2_CR1_ADON;//ADC2_StartConversion()
  // Wait 250祍 RF chip stabilisation time
//  AWU_Init(AWU_TIMEBASE_250US);
//  halt();
   if(count==0)
  {
    AWU_Init(AWU_TIMEBASE_500US);
  }else
  {
    AWU_Init(AWU_TIMEBASE_250US);
  } 
    count++;
  if(count>2)
    count=0;
  
  halt();
  AWU_Init(AWU_TIMEBASE_250US);
  
  
 
  // Perform ADC_MEASURE_PER_BURST Measure, return average
  for(TU8 i = 0u; i< ADC_SAMPLE_PER_MEASURE;++i)
  {
    ADC2->CSR &= (uint8_t)(~ADC2_CSR_EOC);//ADC2_ClearFlag()
    ADC2->CR1 |= ADC2_CR1_ADON;//ADC2_StartConversion()
    
    while(((FlagStatus)(ADC2->CSR & ADC2_CSR_EOC))== RESET){
      //This is a pooling waiting loop
    };
    
    if(i == 0u)
    {
      Measure = ADC2_GetConversionValue();
    }
    else
    {
      Measure = (Measure + ADC2_GetConversionValue())/2u;
    }
    
  }
    /*LDRA_INSPECTED 9 S*/
    CLEAR_BIT(Radio_ChipSelect_PORT->ODR,7u);



  GPIO_Init(INPUT_CAPTURE_RADIO_PORT,INPUT_CAPTURE_RADIO_PIN,GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_WriteLow(INPUT_CAPTURE_RADIO_PORT,INPUT_CAPTURE_RADIO_PIN);
  return Measure;
}


static void SleepUntilNextBurst()
{
  //Disable Radio
  
  /*LDRA_INSPECTED 9 S*/ 
  CLEAR_BIT(Radio_ChipSelect_PORT->ODR,7u);//GPIO_WriteLow(Radio_ChipSelect_PORT,Radio_ChipSelect_PIN)
 
  
  //Setup AWU to halt for 64ms
  AWU_Init(TIME_BETWEEN_BURST);
  //Set MVR in LowPower
  
  /*LDRA_INSPECTED 9 S*/
  CLEAR_BIT(MVR_ChipSelect_PORT->ODR,6u);//GPIO_WriteLow(MVR_ChipSelect_PORT,MVR_ChipSelect_PIN)

  
  //Reload watchdog
  IWDG_ReloadCounter();
  
  halt();
  
  //Set MVR in High Power
  /*LDRA_INSPECTED 9 S*/
  SET_BIT(MVR_ChipSelect_PORT->ODR,6u);//GPIO_WriteHigh(MVR_ChipSelect_PORT,MVR_ChipSelect_PIN)
 
  
  //Reload watchdog
  IWDG_ReloadCounter();
}


static void AdjustFloor()
{
   me.CurrentRSSI.Floor = ( (me.CurrentRSSI.Floor * 3u) + me.CurrentRSSI.LastMeasure) / 4u;
}

/*!************************************************************************************************
* \param[in]  void
* \brief      Trap in Low Power Loop Until Rssi manager decide that Rssi level is high enouth to wake up
* \return     void
***************************************************************************************************/

void RtsLowPowerManager_SleepLoop()
{
  TBool Wakeup = FALSE;
  TU8   button_times=0;
  static TU8 count=0;
  
  ADC_Setup(ADC_Mode_Off);
  RtsLowPowerManager_SetUpHardware_LowPower();
  SQC_Counter();
  me.CurrentRSSI.Floor = me.CurrentRSSI.RSSI_HP;
  
  do
  {    
    if(RADIO_OUTLINE == true)
    { //Redio closed  
                  Chrono_TimeAdd(64);             //时间累计 增加64ms
                  LongTimer_TimeAdd(64);          //关键时间
                  MotorProtTimeDele(64);          //用户模式下电机冷却计时

                  me.LongTimerWakeup = LongTimer_DidTimerTimeout();
                  Wakeup = me.LongTimerWakeup; //长定时 唤醒
                  
                  if(ADC_GetValue_Polling(ADC_Mode_PowerCharge) > 500u) 
                  {
                      Current_charger = 0;  
                  }else if(ADC_GetValue_Polling(ADC_Mode_PowerCharge) < 100u)
                  {
                      Current_charger = 1; 
                  }
              
                  if(Current_charger != Prior_charger)//充电器状态改变
                  {
                     Prior_charger = Current_charger;
                     if(Current_charger ==1)          //是否插上充电器
                      {
                          Charger_mode = true;
                          
                          Wakeup = TRUE;
                          RADIO_OUTLINE=FALSE;
                          if(SleepWakeEnable == true)   //15min唤醒
                          {
                            SleepWakeTime =0;
                            SleepWakeEnable = false;     
                          }
                      }    
                  } 
                  
                if(SleepWakeEnable == true) //sleep mode 15min后自动唤醒并开启无线功能
                {
                    SleepWakeTime +=64u;
                    if(SleepWakeTime >= SLEEPMODE_TIMELAST_15MIN )//15min
                    {//15min时间到达 接收无线唤醒
                       SleepWakeTime =0u;
                       SleepWakeEnable = false;
                       SleepModeChange = true;
                       Wakeup = TRUE; 
                       RADIO_OUTLINE = FALSE; 
                    }
                }else//永久休眠 不接收无线唤醒
                {
                     //null
                }
              
                //按键触发唤醒
                if((GPIO_ReadInputData(BUTTON_PORT) & BUTTON_PIN) == 0x00)
                  {button_times++;}
                else
                  {button_times=0;}  
                
                 if(button_times>2)
                 {//持续120ms 唤醒
                    button_times=0u;
                    Wakeup = TRUE;
                    
                   if(SleepWakeEnable == true) //15min唤醒
                    {
                       SleepWakeTime =0;
                       SleepWakeEnable = false;     
                    }
                 }  
    }else 
    { //redio open    
                    ADC_Setup(ADC_Mode_RSSI);
                    AWU_Init(AWU_TIMEBASE_250US);
                    Chrono_TimeAdd(64);             //时间累计 增加64ms
                    LongTimer_TimeAdd(64);          //关键时间
                    MotorProtTimeDele(64);
                    
                    me.LongTimerWakeup = LongTimer_DidTimerTimeout();
                    Wakeup = me.LongTimerWakeup; //长定时 唤醒
                     //redio open         接收无线信号
                    for(TU8 i = 0U; i<ADC_MEASURE_PER_BURST && Wakeup == FALSE;++i)
                    {//采样3次
                        me.CurrentRSSI.LastMeasure = Measure_RSSI();
                        AdjustFloor();     
                        if(me.CurrentRSSI.LastMeasure > (me.CurrentRSSI.Floor + ADC_MEASURE_RSSI_SENSITIVITY)  )
                        {
                          Wakeup = TRUE;
                        }else
                        {
                          halt();
                        } 
                    }
                    
                    if(ADC_GetValue_Polling(ADC_Mode_PowerCharge) > 500u) 
                      {
                          Current_charger = 0;  
                      }
                    else if(ADC_GetValue_Polling(ADC_Mode_PowerCharge) < 100u)
                      {
                          Current_charger = 1; 
                      }
                
                    if(Current_charger != Prior_charger)//充电器状态改变
                      {
                         Prior_charger = Current_charger;
                         if(Current_charger ==1)  //是否插上充电器
                          {
                            Charger_mode = true;
                            Wakeup = TRUE;
                            LowpowerLedMode=Green_2_Economy;
                          }else
                          {
                            LowpowerLedMode=AllClose;
                          }
                      } 
                        
                    if((GPIO_ReadInputData(BUTTON_PORT) & BUTTON_PIN) == 0x00)//电机按键按下
                      {button_times++;}
                    else
                      {button_times=0;}
                    
                      if(button_times>2)
                      {
                        Wakeup = TRUE;
                        button_times=0;
                      }             
    }
    if(Wakeup == FALSE)
    {  
       if(Motor_HotProtect == true)
       {//LowpowerLedMode=RedSlowBlink
           count++;
           if(count==1)
           {
              GPIO_WriteLow(GPIOB,GPIO_PIN_3);  //LED_GREEN  灭
              GPIO_WriteHigh(GPIOB,GPIO_PIN_1);//LED_RED  light
           }else if(count==16)
           {
             GPIO_WriteLow(GPIOB,GPIO_PIN_3);  //LED_GREEN  灭
             GPIO_WriteLow(GPIOB,GPIO_PIN_1);//LED_RED    灭 
           }
           
           if(count>=30)
              count=0;
       }else
       {   
            if(LowpowerLedMode==GreenLight)
            {
               GPIO_WriteHigh(GPIOB,GPIO_PIN_3); //LED_GREEN  light
               GPIO_WriteLow(GPIOB,GPIO_PIN_1);  //LED_RED    灭 
            }
            else if(LowpowerLedMode==RedLight)
            {
               GPIO_WriteLow(GPIOB,GPIO_PIN_3);  //LED_GREEN  灭
               GPIO_WriteHigh(GPIOB,GPIO_PIN_1); //LED_RED    light 
            }
            else if(LowpowerLedMode==Green_2_Economy)
            {
                count++;
                if(count%166<2&&count%166>0)
                  GPIO_WriteHigh(GPIOB,GPIO_PIN_3); //LED_GREEN  light
                else if(count%166==3)
                  GPIO_WriteLow(GPIOB,GPIO_PIN_3);  //LED_GREEN  灭
                else if(count%166==9)
                  GPIO_WriteHigh(GPIOB,GPIO_PIN_3); //LED_GREEN  light
                else if(count%166==11)
                  GPIO_WriteLow(GPIOB,GPIO_PIN_3);  //LED_GREEN  灭
                
                if(count>=166)
                  count=0;
            }
            else if(LowpowerLedMode==Yellow_1_Flash)
            {
                count++;
                if(count%25<2&&count%25>0)
                {
                  GPIO_WriteHigh(GPIOB,GPIO_PIN_3);
                  GPIO_WriteHigh(GPIOB,GPIO_PIN_1);
                }
                else if(count%25==2)
                {
                  GPIO_WriteLow(GPIOB,GPIO_PIN_3);  
                  GPIO_WriteLow(GPIOB,GPIO_PIN_1);  
                }
              
                if(count>=25)
                  count=0;
            }else if(LowpowerLedMode==RedSlowBlink)
            {
                LowpowerLedMode=AllClose; 
                LedMode=AllClose;         
            }
            else
            {
               GPIO_WriteLow(GPIOB,GPIO_PIN_3);//LED_GREEN  灭
               GPIO_WriteLow(GPIOB,GPIO_PIN_1);//LED_RED    灭 
            }     
       }
       
        SleepUntilNextBurst();
    }   
  }while(Wakeup == FALSE);
    
  AWU_IdleModeEnable();
  RtsLowPowerManager_SetUpHardware_HighPower();
}

TU16 RtsLowPowerManager_GetRSSI()
{
  return me.CurrentRSSI.RSSI_HP;
}

void RtsLowPowerManager_MeasureRSSIFromHighPower()
{
  ADC_Setup(ADC_Mode_RSSI);
  ADC_GetValue_Polling(ADC_Mode_RSSI);
  TU16 Measure = 0U;
  Measure = ADC2_GetConversionValue();
  me.CurrentRSSI.RSSI_HP = ((me.CurrentRSSI.RSSI_HP * 3) + Measure)/4;
}

/*!************************************************************************************************
* \param[in]  void
* \brief      Mesasure LSI and re-compute AWU register value
* \return     void
***************************************************************************************************/
//
static void RtsLowPowerManager_CalibrateAWU()
{
  me.MeasuredLSIFrequency = RtsLowPowerManager_MeasureLsiFrequency();
  AWU_LSICalibrationConfig(me.MeasuredLSIFrequency);
}

/*!************************************************************************************************
* \param[in]  void
* \brief      Measurz LSI FRequency
* \return     LSI frequency in Hz
***************************************************************************************************/
static TU32 RtsLowPowerManager_MeasureLsiFrequency(void)
{
  RtsLowPowerManager_SetUpHardware_LowPower();
  TU32 lsi_freq_hz = 0x0;
  TU32 fmaster = 0x0;
  TU16 ICValue1 = 0x0;
  TU16 ICValue2 = 0x0;
  TU16 Pres = 0U;
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3,ENABLE);
  TIM3_DeInit();
  
  /* Enable the LSI measurement: LSI clock connected to timer 3 Input Capture 1 */
  AWU->CSR |= AWU_CSR_MSR;
  
  /* Measure the LSI frequency with TIMER3 Input Capture 1 */
  
  /* Capture only every 8 events!!! */
  /* Enable capture of TI1 */
  TIM3_ICInit(TIM3_CHANNEL_1, TIM3_ICPOLARITY_RISING, TIM3_ICSELECTION_DIRECTTI, TIM3_ICPSC_DIV8, 0);
  TIM3_PrescalerConfig(TIM3_PRESCALER_1,
                       TIM3_PSCRELOADMODE_IMMEDIATE);
  TIM3_ClearFlag(TIM3_FLAG_CC1);
  
  Pres = (TU16) TIM3_GetPrescaler() + 1;//TIM3_PRESCALER_1 == 0
  /* Get master frequency */
  fmaster = CLK_GetClockFreq();
  
  /* Enable TIM3 */
  TIM3_Cmd(ENABLE);
  
  /* wait for stabilization...*/
    /*LDRA_INSPECTED 120 S*/
  while ((TIM3->SR1 & TIM3_FLAG_CC1) != TIM3_FLAG_CC1){
      //This is a pooling waiting loop
    };
  /* Get CCR1 value*/
  TIM3_ClearFlag(TIM3_FLAG_CC1);
  
  /* wait a capture on cc1 */
  /*LDRA_INSPECTED 120 S*/
  while ((TIM3->SR1 & TIM3_FLAG_CC1) != TIM3_FLAG_CC1){
      //This is a pooling waiting loop
    };
  /* Get CCR1 value*/
  ICValue1 = TIM3_GetCapture1();
  TIM3_ClearFlag(TIM3_FLAG_CC1);
  
  /* wait a capture on cc1 */
  /*LDRA_INSPECTED 120 S*/
  while ((TIM3->SR1 & TIM3_FLAG_CC1) != TIM3_FLAG_CC1){
      //This is a pooling waiting loop
    };
  /* Get CCR1 value*/
  ICValue2 = TIM3_GetCapture1();
  TIM3_ClearFlag(TIM3_FLAG_CC1);
  
  /* Disable IC1 input capture */
  TIM3->CCER1 &= (TU8)(~TIM3_CCER1_CC1E);
  /* Disable timer3 */
  TIM3_Cmd(DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3,DISABLE);
  TIM3_DeInit();
  /* Compute LSI clock frequency */
  lsi_freq_hz = ((fmaster) / ((ICValue2 - ICValue1) * Pres))*8;
  
  /* Disable the LSI measurement: LSI clock disconnected from timer Input Capture 1 */
  AWU->CSR &= (TU8)(~AWU_CSR_MSR);
  
  return (lsi_freq_hz);
}

 /*LDRA_HEADER_END*/