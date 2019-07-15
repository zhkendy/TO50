
/* *********************************************************************
/ This document and/or file is SOMFY’s property. All information
/ it contains is strictly confidential. This document and/or file
/ shall not be used, reproduced or passed on in any way, in full
/ or in part without SOMFY’s prior written approval.
/ All rights reserved.
/ Ce document et/ou fichier est la propriété de SOMFY.
/ Les informations qu’il contient sont strictement confidentielles.
/ Toute reproduction, utilisation, transmission de ce document
/ et/ou fichier, partielle ou intégrale, non autorisée
/ préalablement par SOMFY par écrit est interdite.
/ Tous droits réservés.
/ ***********************************************************************
/ CLASS DEFINITION FILE
/ ***********************************************************************
/ ASSOCIATED HEADER FILE :
/ ***********************************************************************
/ AUTHOR : FlG
/ DATE :11/08/2006
/ DESCRIPTION : 
/ ********************************************************************* */
// E X T E R N A L   D E F I N I T I O N S
//

// I N T E R F A C E S
//

#include "Pcb.h"
#include "RTS_InputCapture.h"
// C O N S T A N T S
//

// S T A T E   M A C H I N E ( S )
//

// V A R I A B L E S
//
TInputCapture oInputCapture ;

// P R I V A T E   P R O T O T Y P E S
//

// F U N C T I O N S / M E T H O D S   R F
//
//_________________________________________________________________________________________________
/*!
* \fn      __interrupt void InputCapture_Edge( void )
* \brief   This interrupt occure when an edge is detected or CC2 match
*
*/
void InputCapture_Edge_IRQHandler(void)
{
  if( (oInputCapture_btInputLevelExpected == HIGH_LEVEL))// For Robustness
  {
    InputCapture_TriggerOnFallingEdge();
    RTSDataSupply_EdgeDetected();
  }
  else if( (oInputCapture_btInputLevelExpected == LOW_LEVEL))//For Robustness
  {
    InputCapture_TriggerOnRisingEdge();
    RTSDataSupply_EdgeDetected();
  }
  else
  {
    oInputCapture_btInputLevelExpected = !oInputCapture_btInputLevelExpected;
  }
}

void InputCapture_Timeout_IRQHandler(void)
{
  InputCapture_DeactivateTimeout();
  RTSDataSupply_TimeoutDetected();
}






//_________________________________________________________________________________________________
/*!
* \fn      void InputCapture_Create( void )
* \brief   Initialize the input capture. 
*
*/
void InputCapture_SetUpHarware( void )
{

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2,ENABLE);
  TIM2_DeInit();  
  TIM2_Cmd(DISABLE);
  
  GPIO_Init(Radio_ChipSelect_PORT,Radio_ChipSelect_PIN,Radio_ChipSelect_MODE);
  GPIO_Init(INPUT_CAPTURE_RADIO_PORT,INPUT_CAPTURE_RADIO_PIN,INPUT_CAPTURE_RADIO_MODE);
  
  TIM2_CCxCmd(INPUT_CAPTURE_CHANNEL,DISABLE);
  TIM2_CCxCmd(RADIO_TIMER_CHANNEL,DISABLE);
  TIM2_CCxCmd(TIM2_CHANNEL_3,DISABLE);
  
  
  TIM2_PrescalerConfig(TIM2_PRESCALER_16,
                       TIM2_PSCRELOADMODE_IMMEDIATE);
  TIM2_UpdateDisableConfig(DISABLE);
  
  TIM2_ICInit(INPUT_CAPTURE_CHANNEL,
              TIM2_ICPOLARITY_RISING,
              TIM2_ICSELECTION_DIRECTTI,// to IC2
              TIM2_ICPSC_DIV1, //each front is counted
              TMR2_INPUTCAPTURE_FILTER_LEVEL); //Filter value, used to filter "radio noise".
  

  TIM2_OC1Init(TIM2_OCMODE_TIMING,
                 TIM2_OUTPUTSTATE_DISABLE,
                 0xFFFF,
                 TIM2_OCPOLARITY_HIGH);
  
  TIM2_ITConfig(TIM2_IT_CC1,ENABLE);
  TIM2_ITConfig(TIM2_IT_CC2,ENABLE);
  TIM2_ITConfig(TIM2_IT_CC3,DISABLE);
  TIM2_ITConfig(TIM2_IT_UPDATE,ENABLE);
  
  oInputCapture_btInputLevelExpected = HIGH_LEVEL;
  
  TIM2_CCxCmd(INPUT_CAPTURE_CHANNEL,ENABLE);

  TIM2_Cmd(ENABLE);


  
}

void InputCapture_SwitchToLowPower( void )
{
  TIM2_DeInit(); 
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2,DISABLE);
  GPIO_Init(INPUT_CAPTURE_RADIO_PORT,INPUT_CAPTURE_RADIO_PIN,GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_WriteLow(INPUT_CAPTURE_RADIO_PORT,INPUT_CAPTURE_RADIO_PIN);
}


//_________________________________________________________________________________________________
/*!
* \fn      void InputCapture_GiveTimerValue( void )
* \brief   Give the timer value corresponding to the event catch.
* \return  The timer value.
*
*/

TU16 InputCapture_GiveTimerValue( void )
{
  //STM8 automaticaly buffer LSB on read, no need to stop the timer for read
  return (TIM2_GetCounter());
}

//_________________________________________________________________________________________________
/*!
* \fn      TU8 InputCapture_Read( void )
* \brief   Give the level of the input.
* \return  1 if HIGH_LEVEL and 0 if not.
*
*/
TU8 InputCapture_Read( void )
{
  return GPIO_ReadInputPin(INPUT_CAPTURE_RADIO_PORT, INPUT_CAPTURE_RADIO_PIN) != RESET;
}

// Reload time out timer
// TIM2 is 16bits UP counter.
// Setup TIM2 channel 2 to generate an interup in wTimeoutValue µs.
// 0<TU16<0xFFFF

void Input_Capture_ReloadTimeout(TU16 wTimeoutValue)
{
  /*There is no buffering when writing to the counter. Both TIM1_CNTRH and TIM1_CNTRL
  //  can be written at any time, so it is suggested not to write a new value into the counter while
  //  it is running to avoid loading an incorrect intermediate content.*/

  TU16 Tim2Value = TIM2_GetCounter();
  //Tick left before timer overflow
  TU16 tickleft = (0xFFFFu - TIM2_GetCounter());
  
  
  if(tickleft > wTimeoutValue)
  {
    TIM2_SetCompare1(Tim2Value + wTimeoutValue);
  }
  else// [T] --tickleft-- 0xFFFF => 0 ----[Timeout = wTimeoutValue - tickleft]
  {
    TIM2_SetCompare1(wTimeoutValue - tickleft);
  }
  
}

