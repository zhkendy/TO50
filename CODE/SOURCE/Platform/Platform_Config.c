/**********************************************************************
This document and/or file is SOMFY’s property. All information it
contains is strictly confidential. This document and/or file shall
not be used, reproduced or passed on in any way, in full or in part
without SOMFY’s prior written approval. All rights reserved.
Ce document et/ou fichier est la propriété de SOMFY. Les informations
qu’il contient sont strictement confidentielles. Toute reproduction,
utilisation, transmission de ce document et/ou fichier, partielle ou
intégrale, non autorisée préalablement par SOMFY par écrit est
interdite. Tous droits réservés.
***********************************************************************/


//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------


// External inclusion
//------------------
#include "Config.h"
#include "stm8s_adc2.h"
#include "stm8s_awu.h"
#include "stm8s_beep.h"
#include "stm8s_clk.h"
#include "stm8s_exti.h"
#include "stm8s_flash.h"
#include "stm8s_gpio.h"
#include "stm8s_i2c.h"
#include "stm8s_itc.h"
#include "stm8s_iwdg.h"
#include "stm8s_rst.h"
#include "stm8s_spi.h"
#include "stm8s_tim1.h"
#include "stm8s_tim2.h"
#include "stm8s_tim3.h"
#include "stm8s_tim4.h"
#include "stm8s_uart1.h"
#include "stm8s_uart3.h"
#include "stm8s_wwdg.h"

#include "STM8_Archiver.h"
#include "UARTComDriver.h"
#include "Motor_Driver.h"
#include "Encoder_Driver.h"
#include "Pcb.h"
#include "Stm8_Adc.h"
#include "PowerMonitor.h"
#include "LongTimer.h"
#include "OSTick.h"
#include "RTSProtocol.h"
#include "RTS_InputCapture.h"
#include "Platform_Config.h"

/*!************************************************************************************************
* \fn         void CLK_Configuration(void)
* \param[in]  void
* \brief     CLK_Configuration method configures the core clock prescalers
* \return     void
***************************************************************************************************/

void CLK_Configuration()
{
    CLK_DeInit();
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
    CLK_HSICmd(ENABLE);
    CLK_LSICmd(ENABLE);
    CLK_HSECmd(DISABLE);
    CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
}


void Setup_And_Start_Watchdog()
{
#if defined(DISABLE_WATCHDOG) == 0
  IWDG_Enable();
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  IWDG_SetReload(250);//500ms 
  IWDG_ReloadCounter();
   
//  IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable)
#endif
}


//Æ½Ì¨ÉèÖÃÎªµÍ¹¦ºÄÄ£Ê½
void PlatformConfig_SetUpHardWareForLowPower()
{
  OSTick_Disable();                           //²Ù×÷ÏµÍ³Ê±»ù
  RTSProtocol_Disable();                      //RTSÐ­Òé¹Ø±Õ
  InputCapture_SwitchToLowPower();
  MotorDriver_SwitchToLowPower();
  EncoderDriver_SetUpHardware_LowPower();
  UARTComDriver_Cmd(UARTComDriver_Disable,0);
  UART1ComDriver_Cmd(UARTComDriver_Disable,0);
  PowerMonitor_SetupHardware_LowPower();
  ADC_Setup(ADC_Mode_Off);
  PlatformConfig_DisableAllPeriph();          //Æ½Ì¨¹Ø±ÕËùÓÐÍâÉè
  PlatformConfig_SetGPIOToLowPower();         //¹Ü½ÅÉèÖÃµÍµçÆ½
}



//Õý³£Ä£Ê½ Ó²¼þ»Ö¸´
void PlatformConfig_SetUpHardWareForHighPower()
{
  CLK_Configuration();  
  OSTick_Init();
  InputCapture_SetUpHarware();
  MotorDriver_RestoreFromLowpower();
  EncoderDriver_SetUpHardware_HighPower();
  PowerMonitor_CMD(TRUE);
  ADC_Init();   
}

//¹Ø±ÕÍâÉèÊ±ÖÓ
void PlatformConfig_DisableAllPeriph()
{
  ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_ALL,DISABLE);
  ADC2_Cmd(DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC,DISABLE);
  
  AWU_Cmd(DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU,DISABLE);
    
  BEEP_DeInit();
  BEEP_Cmd(DISABLE);
  
  EXTI_DeInit();   
  
  FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
  FLASH_SetLowPowerMode(FLASH_LPMODE_POWERDOWN);

  I2C_DeInit();
  I2C_Cmd(DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C,DISABLE);
  
  ITC_DeInit();
  
  SPI_DeInit();
  SPI_Cmd(DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI,DISABLE);
  
  TIM1_DeInit();
  TIM1_Cmd(DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1,DISABLE);
  
  TIM2_DeInit();
  TIM2_Cmd(DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2,DISABLE);
  
  TIM3_DeInit();
  TIM3_Cmd(DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3,DISABLE);
      
  TIM4_DeInit();
  TIM4_Cmd(DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4,DISABLE);
  
  UART3_DeInit();
  UART3_Cmd(DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART3,DISABLE);
  
  UART1_DeInit();
  UART1_Cmd(DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1,DISABLE); 
}



//Íâ²¿¹Ü½Å µÍ¹¦ºÄÄ£Ê½ÉèÖÃ sage
void PlatformConfig_SetGPIOToLowPower()
{
  //Floating pin 
  GPIO_Init(GPIOA,GPIO_PIN_1,GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOA,GPIO_PIN_2,GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_WriteHigh(GPIOA,GPIO_PIN_1);
  GPIO_WriteHigh(GPIOA,GPIO_PIN_2);
  
  GPIO_Init(GPIOB,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_SLOW); 
  GPIO_Init(GPIOB,GPIO_PIN_0,GPIO_MODE_OUT_PP_LOW_SLOW); 
  GPIO_WriteHigh(GPIOB,GPIO_PIN_7);
  GPIO_WriteHigh(GPIOB,GPIO_PIN_0);
  
  GPIO_Init(GPIOC,GPIO_PIN_4,GPIO_MODE_OUT_PP_LOW_SLOW); 
  GPIO_Init(GPIOC,GPIO_PIN_5,GPIO_MODE_OUT_PP_LOW_SLOW); 
  GPIO_Init(GPIOC,GPIO_PIN_6,GPIO_MODE_OUT_PP_LOW_SLOW); 
  GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_WriteHigh(GPIOC,GPIO_PIN_4);
  GPIO_WriteHigh(GPIOC,GPIO_PIN_5);
  GPIO_WriteHigh(GPIOC,GPIO_PIN_6);
  GPIO_WriteHigh(GPIOC,GPIO_PIN_7);
  
  GPIO_Init(GPIOD,GPIO_PIN_1,GPIO_MODE_OUT_PP_LOW_SLOW); //SWIM 
  GPIO_WriteHigh(GPIOD,GPIO_PIN_1);
  
  GPIO_Init(GPIOE,GPIO_PIN_0,GPIO_MODE_OUT_PP_LOW_SLOW); 
  GPIO_Init(GPIOE,GPIO_PIN_1,GPIO_MODE_OUT_PP_LOW_SLOW); 
  GPIO_Init(GPIOE,GPIO_PIN_2,GPIO_MODE_OUT_PP_LOW_SLOW); 
  GPIO_Init(GPIOE,GPIO_PIN_3,GPIO_MODE_OUT_PP_LOW_SLOW); 
  GPIO_Init(GPIOE,GPIO_PIN_5,GPIO_MODE_OUT_PP_LOW_SLOW); 
  GPIO_Init(GPIOE,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_SLOW); 
  GPIO_WriteHigh(GPIOE,GPIO_PIN_0);
  GPIO_WriteHigh(GPIOE,GPIO_PIN_1);
  GPIO_WriteHigh(GPIOE,GPIO_PIN_2);
  GPIO_WriteHigh(GPIOE,GPIO_PIN_3);
  GPIO_WriteHigh(GPIOE,GPIO_PIN_5);
  GPIO_WriteHigh(GPIOE,GPIO_PIN_7);
  
  GPIO_Init(GPIOG,GPIO_PIN_1,GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_WriteHigh(GPIOG,GPIO_PIN_1);
  
  //POWER_CHARGER¡¡  
  GPIO_Init(POWERCH_PORT,POWERCH_PIN,POWERCH_MODE);
  
  GPIO_Init(UART1_TX_PORT,UART1_TX_PIN,UART1_TX_MODE);
  GPIO_WriteHigh(UART1_TX_PORT,UART1_TX_PIN);
  
  GPIO_Init(UART1_RX_PORT,UART1_RX_PIN,GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_WriteHigh(UART1_RX_PORT,UART1_RX_PIN);  
  
  GPIO_Init(UART_TX_PORT,UART_TX_PIN,UART_TX_MODE);
  GPIO_WriteHigh(UART_TX_PORT,UART_TX_PIN);
  
  GPIO_Init(UART_RX_PORT,UART_RX_PIN,GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_WriteHigh(UART_RX_PORT,UART_RX_PIN);  

}

/**********************************************************************
Copyright © (2008), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
