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
// File "OSTick.c"   STM32F10x
//---------------------------------------------------------------------


// External inclusions
//--------------------

#include "Config.h"
#include "Encoder_Driver.h"
#include "stm8s_tim4.h"
#include "RTS_Ergonomic_ManagerSharedType.h"
#include "RTS_Ergonomic_Manager.h"
#include "SFY_OS.h"

// Internal inclusion
//------------------
#include "OSTick.h"

// Private types
//-------------

// Private variables
//------------------
TBool Led_flash_once = FALSE;
TS8   Led_repet_time    = 0U;
TU16  SpeedCycle_last   = 0U;
TU16  Led_time_count    = 0U;
TU16  Led_time_last     = 0U;
TU16  Ledtime_outsecond = 0U;
TU16  Butt_tim_last = 0U;
TU16  Bool_tim_last = 0U;
TBool Butt_Press = FALSE;
TU16  Butt_Press_last = 0U;

extern TU8   Button_flage;
extern TU8   LedMode;
extern TU8   NextLedMode;
extern bool   Speed_MoveCycle;
extern bool   Speed_MoveCycleRelod;
extern TU16  SpeedADJCycle_timems;
extern bool Switch_Button_Enable;

extern void ErgonomicManager_ChangeErgoMode(ErgoMode_e NewMode);

// Public functions implementation
//--------------------------------------

/*!************************************************************************************************
* \fn         void OSTick_Init( void )
* \param[in]  void
* \return     void
***************************************************************************************************/
void OSTick_Init( void )
{
  
    /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example                                                                                                                                                                                                                                                                              we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */
  
  /* Reset TIM4 */
  TIM4_DeInit();
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);

//  generate a time base equal to 2 ms
//   so TIM4_PERIOD = (0.002 * 125000 - 1) = 249 
  TIM4_TimeBaseInit(TIM4_PRESCALER_128,124);

  /* Generate an interrupt on timer count overflow */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

  /* Enable TIM1 */
  TIM4_Cmd(ENABLE);
}


void OSTick_Disable( void )
{
  TIM4_ITConfig(TIM4_IT_UPDATE, DISABLE);
  TIM4_Cmd(DISABLE);
  TIM4_DeInit();
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
}

//µç»ú¶Ë°´¼üÏû³ý¶¶¶¯
void Motor_Prog_Moniter( void )
{
  if(Switch_Button_Enable== true)
  {
      if((GPIO_ReadInputData(BUTTON_PORT) & BUTTON_PIN) == 0x00)
         Button_flage=1;
          
      if(Button_flage==1)
      {
        Butt_tim_last++;
        if(Butt_tim_last > BUTTON_13SEC)
           Butt_tim_last = BUTTON_13SEC;
        
        if(Butt_tim_last>20)
        {
            if((GPIO_ReadInputData(BUTTON_PORT) & BUTTON_PIN) == 0x00)
            {
              Butt_Press_last=Butt_tim_last;
              Butt_Press = TRUE;
            }else
            {
              Button_flage=0;
              Butt_tim_last=0;
            }  
        }   
      }
  }
}


/**************************************************************************************************
* Description    Tjis interrupt is set every 1ms
*
* Fonction       __interrupt void OSTick_OSTick_IRQHandler(void)
* Retour         __interrupt void
* ÏµÍ³1ms ÖÐ¶Ï
***************************************************************************************************/

void OSTick_IRQHandler(void)
{
  SFY_OS_InterruptEntry();               //¿Õ
  TIM4_ClearITPendingBit(TIM4_IT_UPDATE);//ÇåÖÐ¶Ï±êÖ¾
  SFY_OS_TickUpdate();                   //Event++  TaskWaitingCounter--
  SFY_OS_InterruptExit();                //¿Õ
  
  Motor_Prog_Moniter();                  //µç»ú¶Ë°´¼ü
  
   if(Speed_MoveCycle == true)
   {
     if(SpeedCycle_last%SpeedADJCycle_timems ==0)
     {
       Speed_MoveCycleRelod=true;//Ò»¸öÖÜÆÚ½áÊøÖ®ºóÖØÐÂ¼ÓÔØÔË¶¯ÖÜÆÚ
     }
     SpeedCycle_last++;
   }else
   {
     SpeedCycle_last=0;
     Speed_MoveCycleRelod=false;
   }
  

    Led_time_last++;
    Led_time_count++;    
    if(LedMode == GreenLight||LedMode == YellowLight||LedMode == RedLight)
    {
          if(LedMode == GreenLight)
          {
            GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);   // High light ON
            GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);  
          }else if(LedMode == YellowLight)
          {
            GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);   
            GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);     
          }else if(LedMode == RedLight)
          {
            GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);  
            GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
          }  
    }
    
    else if(LedMode == GreenFastBlink)
    {
         if(Led_repet_time>0)
         {     
              Led_flash_once =TRUE;
              if(Led_time_last%600==1) 
              {
                  GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);   
                  GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);  
              }else if(Led_time_last%600==301)
              {
                  GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                  GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
              }
              
              if(Led_time_last >= 600)
              {
                Led_time_last=0;
                Led_repet_time--; 
              }
         }else if(Led_repet_time<=0)
          {
              Led_flash_once=FALSE;  //for safety
              if(NextLedMode!=AllClose)
               {
                  Led_time_last=0;
                  LedMode= NextLedMode;
                  NextLedMode=AllClose;
               }else
                  LedMode=AllClose;
          }
    }
    
    else if(LedMode == YellowFastBlink )
    {
          if(Led_time_last%600==1)  
          {
              GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);  
              GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
          }else if(Led_time_last%600==301)
          {
              GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
              GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
          }
          
          if(Led_time_last >= 600)
          {
            Led_time_last=0;
          }
    }
    
   else if(LedMode == GreenSlowBlink||LedMode == YellowSlowBlink||LedMode == RedSlowBlink)
   {
          if(Led_time_last%2000==1) 
          {
              if(LedMode == GreenSlowBlink)
              {
                GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);   
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);  
              }else if(LedMode == YellowSlowBlink)
              {
                GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);  
                GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
              }else if(LedMode == RedSlowBlink)
              {
                GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN); 
                GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
              }
          }else if(Led_time_last%2000==1001)
          {
              GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN); 
              GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
          }
         
          if(Led_time_last>=2000)
             Led_time_last=0;
      
   } 
    
    else if(LedMode == Green_1_Flash||LedMode == Yellow_1_Flash||LedMode == Red_1_Flash)
     {
           if(Led_repet_time>0)
           {
               if(Led_time_last%1600==1)
               {
                  if(LedMode == Green_1_Flash)
                  {
                    GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);   
                    GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);  
                  }else if(LedMode == Yellow_1_Flash)
                  {
                    GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);  
                    GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
                  }else if(LedMode == Red_1_Flash)
                  {
                    Led_flash_once =TRUE;
                    GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);   
                    GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
                  }
               }else if(Led_time_last%1600==101)
               {
                  GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                  GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
               }
              
              if(Led_time_last>=1600)
              {
                Led_repet_time--;
                Led_time_last=0;
              }
           }else if(Led_repet_time==0)
            {
                Led_flash_once=FALSE;  //for safety
                if(NextLedMode!=AllClose)
                 {  
                    Led_time_last=0;
                    LedMode=NextLedMode;
                    NextLedMode=AllClose;
                 }else
                    LedMode=AllClose;
            }
       }
           
      else if(LedMode == Red_2_Flash)
      {
           if(Led_time_last%2100==1) 
           {
                GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);   
                GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
           }else if(Led_time_last%2100==101)
           {
                GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
           }else if(Led_time_last%2100==501) 
            {
                GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);  
                GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
           }else if(Led_time_last%2100==601)
           {
                GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
           }
          
          if(Led_time_last>=2100)
          {
             Led_time_last=0;
          }
      }
     else if(LedMode == Green_2_Flash)
      {
           if(Led_time_last%2100==1) 
           {
                GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);   
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);  
           }else if(Led_time_last%2100==101)
           {
                GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
           }else if(Led_time_last%2100==501) 
            {
                GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);   
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);  

           }else if(Led_time_last%2100==601)
           {
                GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
           }
          
          if(Led_time_last>=700)
          {
             Led_time_last=0;
             Ledtime_outsecond=0;
          }
      }
          
      else if(LedMode == Red_3_Flash)
      {
          if(Led_repet_time>0)
          {
                if(Led_time_last%2600==1)
                {
                    GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);   
                    GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
                }else if(Led_time_last%2600==101)
                {
                    GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                    GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
                }else if(Led_time_last%2600==501) 
                {
                    GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN); 
                    GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
                }else if(Led_time_last%2600==601)
                {
                    GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                    GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
                }else if(Led_time_last%2600==1001) 
                {
                    GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);   
                    GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
                }else if(Led_time_last%2600==1101)
                {
                    GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                    GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
                }
                
                if(Led_time_last>=2600)
                {
                  Led_repet_time--;
                  Led_time_last=0;
                }
          }else if(Led_repet_time==0)
           {
              if(NextLedMode!=AllClose)
               {
                  Led_time_last=0;
                  LedMode=NextLedMode;
                  NextLedMode=AllClose;
               }else
                  LedMode=AllClose;
           }
      } 
    
     else if(LedMode == Green_3_Flash)
      {
            if(Led_time_last%2600==1)
            {
                GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);   
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);  
            }else if(Led_time_last%2600==101)
            {
                GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
            }else if(Led_time_last%2600==501) 
            {
                GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN); 
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);  
            }else if(Led_time_last%2600==601)
            {
                GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
            }else if(Led_time_last%2600==1001) 
            {
                GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);  
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);  
            }else if(Led_time_last%2600==1101)
            {
                GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
            }
            
            if(Led_time_last>=1200)
            {
              Led_time_last=0;
              Ledtime_outsecond=0;
            }
      } 
            
      else if(LedMode == Green_2_Economy||LedMode == Yellow_2_Economy)
       {
            if(Led_time_last%10600==1) 
            {
                  if(LedMode == Green_2_Economy)
                  {
                    GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);   
                    GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);  
                  }else if(LedMode == Yellow_2_Economy)
                  {
                    GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);   
                    GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
                  }
            }else if(Led_time_last%10600==101)
             {
                  GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                  GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
             }else if(Led_time_last%10600==501) 
              {
                  if(LedMode == Green_2_Economy)
                  {
                    GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);  
                    GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);  
                  }else if(LedMode == Yellow_2_Economy)
                  {
                    GPIO_WriteHigh(LED_GREEN_PORT,LED_GREEN_PIN);   
                    GPIO_WriteHigh(LED_RED_PORT,LED_RED_PIN);  
                  }
              }else if(Led_time_last%10600==601)
               {
                    GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
                    GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
               }
            
            if(Led_time_last>=10600)
            {
              Led_time_last=0;
            }
      }
    
     else if(LedMode==AllClose)
      {
          GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
          GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
      }
    
      if(Led_time_last>=10700)   //·ÀÖ¹Led_time_lastÊý¾ÝÏòÉÏÒç³ö
      {
          Led_time_last=0;
      }
    
     if(Led_time_count>=1000) 
      {
           Led_time_count=0; 
          
        if(Ledtime_outsecond>0)
           Ledtime_outsecond--;//·ÀÖ¹Ledtime_outsecondÊý¾ÝÏòÏÂÒç³ö
      }
    
     if(Ledtime_outsecond == 0)
      {
          Led_flash_once=FALSE;  //for safety
          Led_time_count=0;
          
          LedMode    =AllClose;
          NextLedMode=AllClose;
          
          GPIO_WriteLow(LED_GREEN_PORT,LED_GREEN_PIN);
          GPIO_WriteLow(LED_RED_PORT,LED_RED_PIN);
      }   
}

/**********************************************************************
Copyright © (2010), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
