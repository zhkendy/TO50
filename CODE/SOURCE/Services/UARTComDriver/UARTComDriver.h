// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once

#include "stm8s.h" // for use of INTERRUPT
#include "Config.h"
#include "Stack.h"



typedef enum UARTComDriverMode_e
{
  UARTComDriver_Disable,
  UARTComDriver_Enable_RxTxToIndus,
  UARTComDriver_Enable_TxSQC,
  UARTComDriver_Enable_RxTxToDebug,
}UARTComDriverMode_e;



void UARTComDriver_Init();
void UARTComDriver_Cmd(UARTComDriverMode_e Dmode,TU32 BaudRate); //uart3
void UART1ComDriver_Cmd(UARTComDriverMode_e Dmode,TU32 BaudRate);//uart1

UARTComDriverMode_e UARTComDriver_GetDriverMode();
TBool UARTComDriver_TX_GetNextByte(TU8 *NextByte);


void UARTComDriver_BufferedPrint(const char* Msg,TBool WaitWhenBufferFull);
void UART1ComDriver_BufferedPrintByteInASCII(const TU8* Msg,TU16 MsgLenght,TBool WaitWhenBufferFull);
void UART1ComDriver_WaitBufferedWriteFinish();
void UART3ComDriver_WaitBufferedWriteFinish();

void UART1ComDriver_PoollingPrint(const char* Msg);
void UART3ComDriver_PoollingPrint(const char* Msg);
void UART1ComDriver_PoollingPrintByteInASCII(const TU8* Msg,TU16 MsgLenght);



