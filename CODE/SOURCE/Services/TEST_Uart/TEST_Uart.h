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
// File "TEST_Uart.h"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef __TEST_UART_H__
#define __TEST_UART_H__


// Public functions declaration
//-----------------------------------

void TEST_Uart_Init(void);

void TEST_Uart_Cleanup(void);

// Check if UART peripharal is ready to take data to transmit ; mainly called in polling mode
TBool TEST_Uart_AreYouReadyToTransmitData(void);

// Deactivate hardware peripheral : UART RX
void TEST_Uart_DisableReceive(void);

// Deactivate hardware peripheral : UART TX
void TEST_Uart_DisableTransmit(void);

// Activate hardware peripheral : UART RX
void TEST_Uart_EnableReceive(void);

// Activate hardware peripheral : UART TX
void TEST_Uart_EnableTransmit(void);

// Interrupt service routine : a new byte can be pushed into fifo uart tx ; call a secondary function, in view to allow operating system to modify stack, and avoid compiler optimisation
void TEST_Uart_ISR_DataRegisterEmpty(void);

void TEST_Uart_ManageReceiveFrame(void);

// Wake up object from low power mode to normal running mode
void TEST_Uart_PowerUp(void);

// Sleep down object from normal running mode to low power mode
void TEST_Uart_PowerDown(void);

// Signal to hardware peripheral that TX interrupt process can be started : at least one byte is ready to be transmitted ; CAUTION : only in interrupt mode
void TEST_Uart_StartTxInterrupts(void);

// Signal to hardware peripheral that TX interrupt must be stoped : no more data to transmit ; CAUTION : only in interrupt mode
void TEST_Uart_StopTxInterrupts(void);

// Transmit a byte by uart link ; can be called either from interrupts, either from a task waiting by polling
void TEST_Uart_TakeDataToTransmit(TU8 _u8DataArg);

void TEST_Uart_TransmissionComplete(void);

// Function to update baud rate register when UART is running
void TEST_Uart_UpdateYouBaudRateRegister(TU16 _u16NewBRRArg);


// Header end
//--------------
#endif // __TEST_UART_H__

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
