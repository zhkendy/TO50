// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#include <String.h>
#include "stm8s_itc.h"
#include "stm8s_uart1.h"
#include "stm8s_uart3.h"
#include "stm8s_clk.h"
#include "stm8s_gpio.h"
#include "Pcb.h"

#include "UARTComDriver.h"
TU8 Uart1_mode=UARTComDriver_Enable_TxSQC;
typedef struct
{
  UARTComDriverMode_e CurrentMode;
  
  TU8 WriteBuffer[UART_WRITEBUFFER_LENGHT];
  StackObjRef WriteStack;

  TU8 ReadBuffer[UART_READBUFFER_LENGHT];
  StackObjRef ReadStack;
  
}UARTComDriver_t;

#define me (UARTComDriver)

UARTComDriver_t UARTComDriver;

static void To_HexaString(char *out,TU8 in);


void UARTComDriver_Init()
{
  me.WriteStack  = Stack_Create_StaticAllocation(sizeof(TU8),UART_WRITEBUFFER_LENGHT,me.WriteBuffer,UART_WriteStack);
  me.ReadStack   = Stack_Create_StaticAllocation(sizeof(TU8),UART_READBUFFER_LENGHT,me.ReadBuffer,UART_ReadStack);
  me.CurrentMode = UARTComDriver_Disable;
}
  

UARTComDriverMode_e UARTComDriver_GetDriverMode()
{
  return me.CurrentMode;
}


TBool UARTComDriver_TX_GetNextByte(TU8 *NextByte)
{
  return Stack_PopHead(UARTComDriver.WriteStack,NextByte);
}



void UARTComDriver_Cmd(UARTComDriverMode_e Dmode,TU32 BaudRate)
{
  if(me.CurrentMode != Dmode)
  {
    me.CurrentMode = Dmode;
    if( (DEBUGPRINT== 1 && me.CurrentMode == UARTComDriver_Enable_RxTxToDebug) ||
       me.CurrentMode == UARTComDriver_Enable_RxTxToIndus ||
       me.CurrentMode == UARTComDriver_Enable_TxSQC)
    {
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART3,ENABLE);
      UART3_DeInit();
      
      GPIO_Init(UART_TX_PORT,UART_TX_PIN,UART_TX_MODE);
      GPIO_WriteHigh(UART_TX_PORT,UART_TX_PIN);
      GPIO_Init(UART_RX_PORT,UART_RX_PIN,UART_RX_MODE);
      
      UART3_Init(BaudRate,
                 UART3_WORDLENGTH_8D, 
                 UART3_STOPBITS_1,
                 UART3_PARITY_NO, 
                 UART3_MODE_TXRX_ENABLE);
      
      //UART3_ITConfig(UART3_IT_RXNE, ENABLE);
      UART3_Cmd(ENABLE);
    }
    else
    {
      UART3_DeInit();
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART3,DISABLE);
      Stack_Purge(me.WriteStack);
      
      UART3_ITConfig(UART3_IT_TXE,DISABLE);
      UART3_ITConfig(UART3_IT_TC,DISABLE);
      UART3_ITConfig(UART3_IT_RXNE,DISABLE);
      
      GPIO_Init(UART_TX_PORT,UART_TX_PIN,UART_TX_MODE);
      GPIO_WriteHigh(UART_TX_PORT,UART_TX_PIN);
      GPIO_Init(UART_RX_PORT,UART_RX_PIN,GPIO_MODE_OUT_PP_LOW_FAST);
      GPIO_WriteHigh(UART_RX_PORT,UART_RX_PIN);  
    }
  }
}


void UART1ComDriver_Cmd(UARTComDriverMode_e Dmode,TU32 BaudRate)
{
   if(UARTComDriver_Enable_RxTxToIndus == Dmode)
    {
      Uart1_mode=UARTComDriver_Enable_RxTxToIndus;
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1,ENABLE);
      UART1_DeInit();    
      GPIO_Init(UART1_TX_PORT,UART1_TX_PIN,UART1_TX_MODE);
      GPIO_WriteHigh(UART1_TX_PORT,UART1_TX_PIN);
      GPIO_Init(UART1_RX_PORT,UART1_RX_PIN,UART1_RX_MODE);  
      UART1_Init(BaudRate,
                 UART1_WORDLENGTH_9D, 
                 UART1_STOPBITS_1,
                 UART1_PARITY_EVEN, 
                 UART1_SYNCMODE_CLOCK_DISABLE,
                 UART1_MODE_TXRX_ENABLE);
           
      UART1_ITConfig(UART1_IT_RXNE, ENABLE);
      UART1_Cmd(ENABLE);
    }else if(UARTComDriver_Enable_TxSQC == Dmode)
    {
      Uart1_mode=UARTComDriver_Enable_TxSQC;
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1,ENABLE);
      UART1_DeInit();    
      GPIO_Init(UART1_TX_PORT,UART1_TX_PIN,UART1_TX_MODE);
      GPIO_WriteHigh(UART1_TX_PORT,UART1_TX_PIN);
      GPIO_Init(UART1_RX_PORT,UART1_RX_PIN,UART1_RX_MODE);  
      
      UART1_Init(BaudRate,
           UART1_WORDLENGTH_8D, 
           UART1_STOPBITS_1,
           UART1_PARITY_NO, 
           UART1_SYNCMODE_CLOCK_DISABLE,
           UART1_MODE_TXRX_ENABLE);
      
      UART1_ITConfig(UART1_IT_RXNE, ENABLE);
      UART1_Cmd(ENABLE);
    }else if(UARTComDriver_Disable == Dmode)
    {      
      Uart1_mode=UARTComDriver_Enable_TxSQC;
      UART1_DeInit();
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1,DISABLE);
      
      UART1_ITConfig(UART1_IT_TXE,DISABLE);
      UART1_ITConfig(UART1_IT_TC,DISABLE);
      UART1_ITConfig(UART1_IT_RXNE,DISABLE);
      
      GPIO_Init(UART1_TX_PORT,UART1_TX_PIN,UART1_TX_MODE);
      GPIO_WriteHigh(UART1_TX_PORT,UART1_TX_PIN);
      GPIO_Init(UART1_RX_PORT,UART1_RX_PIN,GPIO_MODE_OUT_PP_LOW_FAST);
      GPIO_WriteHigh(UART1_RX_PORT,UART1_RX_PIN);  
    }
}




const char *hex = "0123456789ABCDEF";
static void To_HexaString(char *out,TU8 in)
{
  for(TU8 i = 0U; i < 2U; ++i)
  {
    //Yes it's ugly, but LDRA loves ugly code
    *out = hex[(in >> 4U) & 0x0F];
    out++;
    *out = hex[(in) & 0x0F];
    out++;
    in++;
  }
}


static void WaitWriteBufferSpace(TU16 NeededSpace);


void UART1ComDriver_BufferedPrint(const char* Msg,TBool WaitWhenBufferFull)
{
  TBool PushSuccess = TRUE;
  if(me.CurrentMode != UARTComDriver_Disable)
  {
    TU16 MsgLenght = strlen(Msg);
    if(MsgLenght < UART_WRITEBUFFER_LENGHT)
    {
      if(MsgLenght >= Stack_GetFreeRoomLeft(me.WriteStack) && WaitWhenBufferFull == TRUE)
      {
        WaitWriteBufferSpace(MsgLenght);
      }
      
      UART1_ITConfig(UART1_IT_TXE, DISABLE);
      for(TU16 i = 0U; i<MsgLenght && PushSuccess == TRUE; ++i)
      {
        PushSuccess = Stack_PushTail(me.WriteStack,&Msg[i]);
      }
      UART1_ITConfig(UART1_IT_TXE, ENABLE);
    }
  }
}

void UART1ComDriver_BufferedPrintByteInASCII(const TU8* Msg,TU16 MsgLenght,TBool WaitWhenBufferFull)
{
  TBool PushSuccess = TRUE;
  if(me.CurrentMode != UARTComDriver_Disable)
  {
    if(MsgLenght < UART_WRITEBUFFER_LENGHT)
    {
      if(MsgLenght >= Stack_GetFreeRoomLeft(me.WriteStack) && WaitWhenBufferFull == TRUE)
      {
        WaitWriteBufferSpace(MsgLenght);
      }
      char Hexa[2];
      UART1_ITConfig(UART1_IT_TXE, DISABLE);
      for(TU16 i = 0U; i<MsgLenght && PushSuccess == TRUE; ++i)
      {
        To_HexaString(Hexa,Msg[i]);
        PushSuccess = Stack_PushTail(me.WriteStack,&Hexa[0]);
        PushSuccess = Stack_PushTail(me.WriteStack,&Hexa[1]);
      }
      UART1_ITConfig(UART1_IT_TXE, ENABLE);
    }
  }
}


static void WaitWriteBufferSpace(TU16 NeededSpace)
{
  UART1_ITConfig(UART1_IT_TXE, ENABLE);
  while(Stack_GetFreeRoomLeft(me.WriteStack) < NeededSpace)
  {
    //wait loop
  };
  
}

void UART1ComDriver_WaitBufferedWriteFinish()
{
  if(me.CurrentMode != UARTComDriver_Disable)
  {
    if(Stack_GetObjectCount(me.WriteStack) != 0)
    {
      while(UART1_GetFlagStatus(UART1_FLAG_TC) != SET)  {
    //wait loop
  };
    }
  }
}


void UART3ComDriver_WaitBufferedWriteFinish()
{
  if(me.CurrentMode != UARTComDriver_Disable)
  {
    if(Stack_GetObjectCount(me.WriteStack) != 0)
    {
      while(UART3_GetFlagStatus(UART3_FLAG_TC) != SET)  {
    //wait loop
  };
    }
  }
}


void UART3ComDriver_PoollingPrint(const char* Msg)//uart3
{
  if(me.CurrentMode != UARTComDriver_Disable)
  {
    UART3ComDriver_WaitBufferedWriteFinish();
    TU16 MsgLenght = strlen(Msg);
    if(MsgLenght < UART_WRITEBUFFER_LENGHT)
    {
      for(TU16 i = 0U; i<MsgLenght;++i)
      {
        UART3_SendData8(Msg[i]);
        while(UART3_GetFlagStatus(UART3_FLAG_TXE) == RESET)  {
    //wait loop
  };
        UART3_ClearFlag(UART3_FLAG_TXE);
      }
      while(UART3_GetFlagStatus(UART3_FLAG_TC) == RESET)  {
    //wait loop
  };
      UART3_ClearFlag(UART3_FLAG_TC);
    }
  }
}


void UART1ComDriver_PoollingPrint(const char* Msg)//uart1
{
  if(me.CurrentMode != UARTComDriver_Disable)
  {
    UART1ComDriver_WaitBufferedWriteFinish();
    TU16 MsgLenght = strlen(Msg);
    if(MsgLenght < UART_WRITEBUFFER_LENGHT)
    {
      for(TU16 i = 0U; i<MsgLenght;++i)
      {
        UART1_SendData8(Msg[i]);
        while(UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)  {
    //wait loop
  };
        UART1_ClearFlag(UART1_FLAG_TXE);
      }
      while(UART1_GetFlagStatus(UART1_FLAG_TC) == RESET)  {
    //wait loop
  };
      UART1_ClearFlag(UART1_FLAG_TC);
    }
  }
}




void UART1ComDriver_PoollingPrintByteInASCII(const TU8* Msg,TU16 MsgLenght)
{
  if(me.CurrentMode != UARTComDriver_Disable)
  {
    UART1ComDriver_WaitBufferedWriteFinish();
    if(MsgLenght < UART_WRITEBUFFER_LENGHT)
    {
      char Hexa[2];
      for(TU16 i = 0U; i<MsgLenght;++i)
      {
        To_HexaString(Hexa,Msg[i]);
        UART1_SendData8(Hexa[0]);
        UART1_SendData8(Hexa[1]);
        while(UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)  {
    //wait loop
  };
        UART1_ClearFlag(UART1_FLAG_TXE);
      }
      while(UART1_GetFlagStatus(UART1_FLAG_TC) == RESET)  {
    //wait loop
  };
      UART1_ClearFlag(UART1_FLAG_TC);
    }
  }
}


#ifdef RETARGET_PRINTF_TO_UART
int putchar(int c)
{
  char casted = (char) c;
  
  if(me.CurrentMode > UARTComDriver_Enable_RxTxToIndus)
  {
    UART3_ITConfig(UART3_IT_TXE, DISABLE);
    Stack_PushTail(me.WriteStack,&casted);
    UART3_ITConfig(UART3_IT_TXE, ENABLE);
  }
  return c;
}
#endif



