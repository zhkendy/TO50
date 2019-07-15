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
/ **********************************************************************/


//---------------------------------------------------------------------
// File "PCB.h"  
// Mula Gabriel 02/2015
// All GPIO definition
//---------------------------------------------------------------------


// Header safety
//---------------------
#pragma once


//--------------------------------------------
// PCB clock speed
//--------------------------------------------
#define PCB_OSC_FREQ                            (16000000)


//--------------------------------------------
// INPUT CAPTURE RADIO RTS
// INPUT_CAPTURE  PD3
//--------------------------------------------
#define INPUT_CAPTURE_RADIO_PORT                (GPIOD)
#define INPUT_CAPTURE_RADIO_PIN                 (GPIO_PIN_3) 
#define INPUT_CAPTURE_RADIO_MODE                (GPIO_MODE_IN_FL_NO_IT)
#define INPUT_CAPTURE_CHANNEL                   (TIM2_CHANNEL_2)
#define RADIO_TIMER_CHANNEL                     (TIM2_CHANNEL_1)

//--------------------------------------------
// Chips Select
//--------------------------------------------
//Radio CS PB5->PD7
#define Radio_ChipSelect_PORT                   (GPIOD)
#define Radio_ChipSelect_PIN                    (GPIO_PIN_7)
#define Radio_ChipSelect_MODE                   (GPIO_MODE_OUT_PP_LOW_SLOW)

//Encoder EN PG0
#define Encoder_ChipSelect_PORT                 (GPIOG)
#define Encoder_ChipSelect_PIN                  (GPIO_PIN_0)
#define Encoder_ChipSelect_MODE                 (GPIO_MODE_OUT_PP_LOW_SLOW)

//BATMeasure EN PC3
#define BATMeasure_ChipSelect_PORT              (GPIOC)
#define BATMeasure_ChipSelect_PIN               (GPIO_PIN_3)
#define BATMeasure_ChipSelect_MODE              (GPIO_MODE_OUT_PP_LOW_SLOW)

//REG sleep  PB6
#define MVR_ChipSelect_PORT                     (GPIOB)
#define MVR_ChipSelect_PIN                      (GPIO_PIN_6)
#define MVR_ChipSelect_MODE                     (GPIO_MODE_OUT_PP_LOW_SLOW)

//--------------------------------------------
// Motor Driver
// H_Bridge PD0
// H_Bridge PD2
//--------------------------------------------
#define H_Bridge_PWMA_PORT                    (GPIOD)
#define H_Bridge_PWMA_PIN                     (GPIO_PIN_0)
#define H_Bridge_PWMA_MODE                    (GPIO_MODE_OUT_PP_LOW_SLOW)

#define H_Bridge_PWMB_PORT                    (GPIOD)
#define H_Bridge_PWMB_PIN                     (GPIO_PIN_2)
#define H_Bridge_PWMB_MODE                    (GPIO_MODE_OUT_PP_LOW_SLOW)

//--------------------------------------------
// Encoder Driver
// Encoder1 PC1
// Encoder2 PC2
//--------------------------------------------
#define Encoder1_PORT                         (GPIOC)
#define Encoder1_PIN                          (GPIO_PIN_1)
#define Encoder1_MODE                         (GPIO_MODE_IN_FL_NO_IT)

#define Encoder2_PORT                         (GPIOC)
#define Encoder2_PIN                          (GPIO_PIN_2)
#define Encoder2_MODE                         (GPIO_MODE_IN_FL_NO_IT)


//--------------------------------------------
// ADC        
// BATMeasure  PE6  
// Radio_RSSI  PB2
//--------------------------------------------
#define Radio_RSSI_MEASURE_CHANNEL            (ADC2_CHANNEL_2)
#define Radio_RSSI_MEASURE_PORT               (GPIOB)
#define Radio_RSSI_MEASURE_PIN                (GPIO_PIN_2)
#define Radio_RSSI_MEASURE_MODE               (GPIO_MODE_IN_FL_NO_IT)


#define BATMeasure_CHANNEL                    (ADC2_CHANNEL_9)
#define BATMeasure_PORT                       (GPIOE)
#define BATMeasure_PIN                        (GPIO_PIN_6)
#define BATMeasure_MODE                       (GPIO_MODE_IN_FL_NO_IT)

//--------------------------------------------
// UART1
//--------------------------------------------
#define UART1_TX_PORT                         (GPIOA)
#define UART1_TX_PIN                          (GPIO_PIN_5)
#define UART1_TX_MODE                         (GPIO_MODE_OUT_PP_LOW_FAST)

#define UART1_RX_PORT                         (GPIOA)
#define UART1_RX_PIN                          (GPIO_PIN_4)
#define UART1_RX_MODE                         (GPIO_MODE_IN_FL_NO_IT)

//--------------------------------------------
// UART3
//--------------------------------------------
#define UART_TX_PORT                          (GPIOD)
#define UART_TX_PIN                           (GPIO_PIN_5)
#define UART_TX_MODE                          (GPIO_MODE_OUT_PP_LOW_FAST)

#define UART_RX_PORT                          (GPIOD)
#define UART_RX_PIN                           (GPIO_PIN_6)
#define UART_RX_MODE                          (GPIO_MODE_IN_FL_NO_IT)

//--------------------------------------------
// BUTTON  PD4
//--------------------------------------------
#define BUTTON_PORT                       (GPIOD)
#define BUTTON_PIN                        (GPIO_PIN_4)
#define BUTTON_MODE                       (GPIO_MODE_IN_PU_NO_IT)

//--------------------------------------------
// Boot_IO  PA6
//--------------------------------------------
#define EN_SC_DET_PORT                      (GPIOA)
#define EN_SC_DET_PIN                       (GPIO_PIN_6)
#define EN_SC_DET_MODE                      (GPIO_MODE_OUT_PP_LOW_SLOW)

//--------------------------------------------
// LED   green PB3  red PB1
//--------------------------------------------
#define LED_GREEN_PORT                    (GPIOB)
#define LED_GREEN_PIN                     (GPIO_PIN_3)
#define LED_GREEN_MODE                    (GPIO_MODE_OUT_PP_LOW_SLOW)

#define LED_RED_PORT                      (GPIOB)
#define LED_RED_PIN                       (GPIO_PIN_1)
#define LED_RED_MODE                      (GPIO_MODE_OUT_PP_LOW_SLOW)
  
//--------------------------------------------
// Power charge PB4
//--------------------------------------------
#define POWERCH_MEASURE_CHANNEL           (ADC2_CHANNEL_4)
#define POWERCH_PORT                      (GPIOB)
#define POWERCH_PIN                       (GPIO_PIN_4)
#define POWERCH_MODE                      (GPIO_MODE_IN_FL_NO_IT)

//--------------------------------------------
// Brake control  PB0->PE5
//--------------------------------------------
#define BRAKECONTROL_PORT                 (GPIOB)
#define BRAKECONTROL_PIN                  (GPIO_PIN_5)
#define BRAKECONTROL_MODE                 (GPIO_MODE_OUT_PP_LOW_SLOW)


/**********************************************************************
Copyright © (2010), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
