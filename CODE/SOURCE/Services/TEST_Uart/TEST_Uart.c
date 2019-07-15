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
// File "TEST_Uart.c"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// External inclusions
//--------------------
#include "Config.h"
#include "stm8s_itc.h"
#include "stm8s_uart3.h"
#include "stm8s_clk.h"
#include "stm8s_gpio.h"
#include "Pcb.h"
#include "SFY_Includes.h"
#include "TestFrame.h"
#include "TestProtocol.h"
#include "SFY_OS_Config.h"
#include "SFY_OS.h"
#include "UARTComDriver.h"

// Internal inclusion
//------------------
#include "TEST_Uart.h"


// Private types
//-------------

ToTestFrameId TransmitFrame;
ToTestFrameId ReceiveFrame;

//#define me (&oTEST_Uart)



// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \fn         void TEST_Uart_Init(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_Init(void)
{  
  TestProtocol_GetFrameHdl(&ReceiveFrame,&TransmitFrame);
}


/*!************************************************************************************************
* \fn         void TEST_Uart_Cleanup(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_Cleanup(void)
{  

}


/*!************************************************************************************************
* \brief      Check if UART peripharal is ready to take data to transmit ; mainly called in polling mode
* \fn         TBool TEST_Uart_AreYouReadyToTransmitData(void)
* \return     TBool 
***************************************************************************************************/
TBool TEST_Uart_AreYouReadyToTransmitData(void)
{  
  return (TBool) (UART3_GetFlagStatus(UART3_FLAG_TXE) == SET);
}


/*!************************************************************************************************
* \brief      Deactivate hardware peripheral : UART RX
* \fn         void TEST_Uart_DisableReceive(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_DisableReceive(void)
{  
  UART3_ITConfig(UART3_IT_RXNE, DISABLE);
}


/*!************************************************************************************************
* \brief      Deactivate hardware peripheral : UART TX
* \fn         void TEST_Uart_DisableTransmit(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_DisableTransmit(void)
{  
}


/*!************************************************************************************************
* \brief      Activate hardware peripheral : UART RX
* \fn         void TEST_Uart_EnableReceive(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_EnableReceive(void)
{  
  UART3_ITConfig(UART3_IT_RXNE, ENABLE);
}


/*!************************************************************************************************
* \brief      Activate hardware peripheral : UART TX
* \fn         void TEST_Uart_EnableTransmit(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_EnableTransmit(void)
{  
}


/*!************************************************************************************************
* \brief      Interrupt service routine : a new byte can be pushed into fifo uart tx ; call a secondary function, in view to allow operating system to modify stack, and avoid compiler optimisation
* \fn         void TEST_Uart_ISR_DataRegisterEmpty(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_ISR_DataRegisterEmpty(void)
{  
}


/*!************************************************************************************************
* \fn         void TEST_Uart_ManageReceiveFrame(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_ManageReceiveFrame(void)
{  
  SFY_OS_EventSignalFromInterruption( OS_SIGNAL_TO_SPECIFIC_TASK, OS_TASK_Controller, TASK_Controller_IndusUARTRX_EVENT); 
}


/*!************************************************************************************************
* \brief      Wake up object from low power mode to normal running mode
* \fn         void TEST_Uart_PowerUp(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_PowerUp(void)
{  
}


/*!************************************************************************************************
* \brief      Sleep down object from normal running mode to low power mode
* \fn         void TEST_Uart_PowerDown(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_PowerDown(void)
{  
}


/*!************************************************************************************************
* \brief      Signal to hardware peripheral that TX interrupt process can be started : at least one byte is ready to be transmitted ; CAUTION : only in interrupt mode
* \fn         void TEST_Uart_StartTxInterrupts(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_StartTxInterrupts(void)
{  
}


/*!************************************************************************************************
* \brief      Signal to hardware peripheral that TX interrupt must be stoped : no more data to transmit ; CAUTION : only in interrupt mode
* \fn         void TEST_Uart_StopTxInterrupts(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_StopTxInterrupts(void)
{  
}


/*!************************************************************************************************
* \brief      Transmit a byte by uart link ; can be called either from interrupts, either from a task waiting by polling
* \fn         void TEST_Uart_TakeDataToTransmit(TU8 _u8DataArg)
* \param[in]  _u8DataArg
* \return     void 
***************************************************************************************************/
void TEST_Uart_TakeDataToTransmit(TU8 _u8DataArg)
{  
  UART3_SendData8(_u8DataArg);
}


/*!************************************************************************************************
* \fn         void TEST_Uart_TransmissionComplete(void)
* \return     void 
***************************************************************************************************/
void TEST_Uart_TransmissionComplete(void)
{  
  /*## BEGIN[GUID e19091c6-bf3e-4cb7-9605-77ed93cc429d] ##*/  
    while (UART3_GetFlagStatus(UART3_FLAG_TC) == RESET)
    {
      //wait loop
    };
  /*## END[GUID e19091c6-bf3e-4cb7-9605-77ed93cc429d] ##*/
}


/*!************************************************************************************************
* \brief      Function to update baud rate register when UART is running
* \fn         void TEST_Uart_UpdateYouBaudRateRegister(TU16 _u16NewBRRArg)
* \param[in]  _u16NewBRRArg
* \return     void 
***************************************************************************************************/
void TEST_Uart_UpdateYouBaudRateRegister(TU16 _u16NewBRRArg)
{  
  /*## BEGIN[GUID 0dd08d66-b3a2-4638-b764-7558c44f6aba] ##*/
  /*## END[GUID 0dd08d66-b3a2-4638-b764-7558c44f6aba] ##*/
}

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
