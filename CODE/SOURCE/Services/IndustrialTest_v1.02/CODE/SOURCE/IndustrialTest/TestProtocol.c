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
// File "TestProtocol.c"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// External inclusions
//--------------------
#include "SFY_Includes.h"
#include "TEST_Uart.h"
#include "IndustrialTestConfig.h"
#include "IndustrialTest.h"
#include "stm8s_uart1.h"
#include "Controller.h"

// Internal inclusion
//------------------
#include "TestProtocol.h"


// Private types
//-------------

#define MAX_FRAME_LENGTH 64U

#define TEST_PROTOCOL_TIMEOUT 0x02FFU

#define FRAME_LENGTH_MASK	0x3FU

#define HEADER_LENGTH		1U

extern TU8 Receive_Buffer[24];
extern TU8 current_index;
TU8     Uart_Command=0;
TU8     UartEvent_Command=UartCommand_NULL;
TU8     Pre_UartEvent_Command=UartCommand_NULL;
TU8     NbrOfDataToTransfer=0;
TU8     TxBuffer[20];
const TU8 Close5Turns_ACK[6]={0x10,0x02,0x2E,0xD5,0x10,0x03};
const TU8 Open_ACK[6] ={0x10,0x02,0x2F,0xD6,0x10,0x03};
const TU8 Close_ACK[6]={0x10,0x02,0x30,0xD7,0x10,0x03};
const TU8 Stop_ACK[6] ={0x10,0x02,0x31,0xD8,0x10,0x03};
TU8 SoftVersion_ACK[13]={0x10,0x02,0x80,0xF0,0xAB,0xE2,0x4C,0x41,0x00,0x04,0x35,0x10,0x03};

static ToTestFrameId TransmitFrameId;
static ToTestFrameId ReceiveFrameId;

/* me is a macro equivalent to this in C++ */
/* LDRA Rule Deactivation : 210 S Macro name is not upper case. */
/*LDRA_INSPECTED 210 S*/
#define me (&oTestProtocol)

//! State structure declaration
typedef enum
{
  TESTPROTOCOL_STATE_ROOT,
  TESTPROTOCOL_STATE_IDLE,
  TESTPROTOCOL_LAST_STATE = TESTPROTOCOL_STATE_IDLE //!< Last state of TestProtocol object used for state machine coherency
} TeTestProtocolState;

//! Field sizes
#define TESTPROTOCOL_STATE_SIZE  sizeof(TeTestProtocolState)

//! Object structure declaration
typedef struct
{
  //! State variable
  TeTestProtocolState eState;
} TsTestProtocol;


// Private functions declaration
//---------------------------------

static TeTestProtocolError TestProtocol_ACTION_OnSendFrame(void);

//static TeTestProtocolError TestProtocol_ACTION_OnFrameComplete(void);


// Private variables
//------------------
bool One_Frame_finish=false;
//! Object instanciation
TsTestProtocol oTestProtocol;

/*LDRA_HEADER_END*/

// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \fn         void TestProtocol_Init(void)
* \return     void 
***************************************************************************************************/
void TestProtocol_Init(void)
{  
  // Implementation
  //---------------
  {
    /*## BEGIN[GUID 444e3320-ad8b-4613-b041-9eb0ecee080f] ##*/
    TransmitFrameId = TestFrame_Init();
    ReceiveFrameId = TestFrame_Init();
    
    TEST_Uart_Init();
    TEST_Uart_PowerUp();
    TEST_Uart_EnableTransmit();
    TEST_Uart_EnableReceive();
    /*## END[GUID 444e3320-ad8b-4613-b041-9eb0ecee080f] ##*/
  }
  // Changing state
  //------------------
  me->eState = TESTPROTOCOL_STATE_IDLE;
}


/*!************************************************************************************************
* \fn         void TestProtocol_Cleanup(void)
* \return     void 
***************************************************************************************************/
void TestProtocol_Cleanup(void)
{  
  // Object state destruction
  //---------------------------------
  me->eState = TESTPROTOCOL_STATE_ROOT;
  
  // Implementation
  //---------------
  {
    /*## BEGIN[GUID 3a3cc7e2-8864-4789-8f03-15bc432a6e26] ##*/
    TestFrame_Cleanup(TransmitFrameId);
    TestFrame_Cleanup(ReceiveFrameId);
    TEST_Uart_DisableReceive();
    TEST_Uart_DisableTransmit();
    TEST_Uart_PowerDown();
    TEST_Uart_Cleanup();
    /*## END[GUID 3a3cc7e2-8864-4789-8f03-15bc432a6e26] ##*/
  }
}


/*!************************************************************************************************
* \fn         void TestProtocol_ReceiveData(void)
* \return     void 
***************************************************************************************************/
void TestProtocol_ReceiveData(void)
{  
  TU8 check_sum=0;
  if(Receive_Buffer[current_index-1]==0x03 && current_index >=7)
  {
     if(Receive_Buffer[current_index-2]==0x10)
     {
        if(Receive_Buffer[current_index-7]==0x10)
        {
            One_Frame_finish=true;
        }
        
        if(current_index >=9)
          if(Receive_Buffer[current_index-9]==0x10)
          {
            One_Frame_finish=true;
          }
     }
  }
  
  if(One_Frame_finish==true)
  {
    One_Frame_finish=false;
    Uart_Command=Receive_Buffer[current_index-4];
    current_index=0;
    if(Uart_Command==0x07)
    {
      //Down move 5turns
      NbrOfDataToTransfer=6;
      for(int i=0;i<NbrOfDataToTransfer;i++)
        TxBuffer[i]=Close5Turns_ACK[i];
      
      UartEvent_Command=UartCommand_DOWN5TURN;
    }else if(Uart_Command==0x08)
    {
      //upmove
      NbrOfDataToTransfer=6;
      for(int i=0;i<NbrOfDataToTransfer;i++)
        TxBuffer[i]=Open_ACK[i];
      
      UartEvent_Command=UartCommand_UP;
    }else if(Uart_Command==0x09)
    {
      //down
      NbrOfDataToTransfer=6;
      for(int i=0;i<NbrOfDataToTransfer;i++)
        TxBuffer[i]=Close_ACK[i];

      UartEvent_Command=UartCommand_DOWN;      
    }else if(Uart_Command==0x0A)
    {
      //stop
      NbrOfDataToTransfer=6;
      for(int i=0;i<NbrOfDataToTransfer;i++)
        TxBuffer[i]=Stop_ACK[i];  

      UartEvent_Command=UartCommand_STOP;      
    }else if(Uart_Command==0x06)
    {
      UartEvent_Command=UartCommand_RESTART;    
    }else if(Uart_Command==0x01)
    {
      //zhuangtai
      
    }else if(Uart_Command==0xBC)
    {
      //zhuangtai
      
    }else if(Uart_Command==0xB8)
    {
      //zhuangtai
      
    }else if(Uart_Command==0x0B)
    {
      UartEvent_Command=UartCommand_REST;        
    }else if(Uart_Command==0xF0)
    {
      //software version
      SoftVersion_ACK[9]=SOFTWARE_SerialNumber;
      SoftVersion_ACK[6]=REF_5M>>16;
      SoftVersion_ACK[5]=REF_5M>>8;
      SoftVersion_ACK[4]=REF_5M;
      
      for(int j=2;j<10;j++)
        check_sum+=SoftVersion_ACK[j];
      
      SoftVersion_ACK[10]=check_sum+0xa7;
      
      NbrOfDataToTransfer=13;
      for(int i=0;i<NbrOfDataToTransfer;i++)
        TxBuffer[i]=SoftVersion_ACK[i];  
    }
    
    if( NbrOfDataToTransfer!=0)
    UART1_ITConfig(UART1_IT_TXE, ENABLE);  //¿ªÊ¼·¢ËÍÖÐ¶Ï
    
  }
}


/*!************************************************************************************************
* \fn         TeTestProtocolError TestProtocol_SendFrame(void)
* \return     TeTestProtocolError 
***************************************************************************************************/
TeTestProtocolError TestProtocol_SendFrame(void)
{  
  // Local variables
  //------------------
  TeTestProtocolError ric_reply;
  ric_reply = TEST_PROTOCOL_ERROR__NO_ERROR;
  
  // State test
  //---------------
  switch (me->eState)
  {
    case TESTPROTOCOL_STATE_IDLE :
      // Transition action
      ric_reply = TestProtocol_ACTION_OnSendFrame();
      break;
      
    default:
      break;
  }
  
  // Return result
  //------------------
  return ric_reply;
}


/*!************************************************************************************************
* \fn         TeTestProtocolError TestProtocol_FrameIsComplete(TeFrameState* peFrameState)
* \param[in]  peFrameState
* \return     TeTestProtocolError 
***************************************************************************************************/
TeTestProtocolError TestProtocol_FrameIsComplete(TeFrameState* peFrameState)
{  
  /*## BEGIN[GUID e99e4332-3a21-43c2-8bda-6c298a0f4ca9] ##*/
  TeTestProtocolError loc_eRetVal             = TEST_PROTOCOL_ERROR__NO_ERROR;
  TU8                 loc_u8CurrentFrameSize  = 0U;
  TU8                 loc_u8FrameHeader       = 0U;
  TU8                 loc_u8FrameSize         = 0U;
  TU8                 loc_u8Frame[SIZE_OF_FRAME];
  TU8                 loc_u8FrameLength = 0x00;
  TU8                 loc_u8Index = 0x00;
  
  //Check pointer is not NULL
  if(NULL != peFrameState)
  {  
    //Initialise frame state to uncomplete frame
    *peFrameState = FRAME_IS_UNCOMPLETE;
    
    //Get Frame header
    TestFrame_GetFrameHeader(ReceiveFrameId, &loc_u8FrameHeader);
    
    //Get frame length in the header (length is coded under 6 bits)
    //Length never includes the header, but always the checksum
    //To get all frame length get frame length in the header and add header length
    loc_u8FrameSize = (FRAME_LENGTH_MASK & loc_u8FrameHeader) + HEADER_LENGTH;
    
    //Get the length of the receive frame
    TestFrame_GetFrameLength(ReceiveFrameId, &loc_u8CurrentFrameSize);
    
    //Check both length are the same
    if(loc_u8CurrentFrameSize == loc_u8FrameSize)
    {
      //Frame is complete
      *peFrameState = FRAME_IS_COMPLETE;
    }
    else
    {
      //Check if more data are already received by Test Usart
      if(loc_u8FrameSize < loc_u8CurrentFrameSize)
      {
        MEMSET(loc_u8Frame, 0, SIZE_OF_FRAME);
        TestFrame_GetFrameData(ReceiveFrameId, &loc_u8FrameLength, loc_u8Frame);
        TestFrame_ClearFrame(ReceiveFrameId);
        while(loc_u8Index < loc_u8FrameSize)
        {
          TestFrame_AddData(ReceiveFrameId, loc_u8Frame[loc_u8Index]);
          loc_u8Index++;
        }
        *peFrameState = FRAME_IS_COMPLETE;
      }
    }
  }
  else
  {
    //Null pointer parameter return an error
    loc_eRetVal = TEST_PROTOCOL_ERROR__BAD_FRAME;
  }
  
  return loc_eRetVal;
  /*## END[GUID e99e4332-3a21-43c2-8bda-6c298a0f4ca9] ##*/
}


/*!************************************************************************************************
* \fn         void TestProtocol_GetFrameHdl(ToTestFrameId* piReceiveFrameId, ToTestFrameId* piTransmitFrameId)
* \param[in]  piReceiveFrameId
* \param[in]  piTransmitFrameId
* \return     void 
***************************************************************************************************/
void TestProtocol_GetFrameHdl(ToTestFrameId* piReceiveFrameId, ToTestFrameId* piTransmitFrameId)
{  
  /*## BEGIN[GUID 3daf5c10-0d97-40f8-9de5-7e52ea87aeb6] ##*/
  //Set receive and transmit frame id
  //Check pointers are not NULL
  if((NULL != piReceiveFrameId)
    &&(NULL != piTransmitFrameId))
  {
    *piReceiveFrameId = ReceiveFrameId;
    *piTransmitFrameId = TransmitFrameId;
  }
  else
  {
    //TO DO Manage error
  }
  /*## END[GUID 3daf5c10-0d97-40f8-9de5-7e52ea87aeb6] ##*/
}


// Private functions implementation
//------------------------------------


/*!************************************************************************************************
* \fn         static TeTestProtocolError TestProtocol_ACTION_OnSendFrame(void)
* \return     TeTestProtocolError 
***************************************************************************************************/
static TeTestProtocolError TestProtocol_ACTION_OnSendFrame(void)
{  
  /*## BEGIN[GUID f67f6fa1-36d1-4ffd-9e25-6128694053bd] ##*/
  TeTestProtocolError loc_eReturnValue = TEST_PROTOCOL_ERROR__NO_ERROR;
  TU8                 loc_u8FrameSize = 0U;
  TU8                 loc_u8FrameData[MAX_FRAME_LENGTH];
  TU8                 loc_u8DataFrameIndex = 0U;
  //Check if test USART is alloweded to transmit data
  TBool               loc_bReadyToTransmit = FALSE;
  //Timeout is used to avoid to stay blocked (if last medium transmition takes too much time)
  TU32                loc_u32LocTimeout = TEST_PROTOCOL_TIMEOUT;
  
  MEMSET(loc_u8FrameData, 0, MAX_FRAME_LENGTH);
  
  //Get frame data
  TestFrame_GetFrameData(TransmitFrameId, &loc_u8FrameSize, loc_u8FrameData);
  
  //Send all data bytes
  for(loc_u8DataFrameIndex= 0U; loc_u8DataFrameIndex < loc_u8FrameSize; loc_u8DataFrameIndex++)
  {
    //Ensure USART is available to transmit or timeout occurs
    while((TRUE != loc_bReadyToTransmit)
      &&(0U != loc_u32LocTimeout))
    {
      // Wait for end transmit of previous char
      loc_bReadyToTransmit = TEST_Uart_AreYouReadyToTransmitData();
      loc_u32LocTimeout--;
    }
  
    if(0U != loc_u32LocTimeout)
    {
      //Transmit current data byte
      TEST_Uart_TakeDataToTransmit(loc_u8FrameData[loc_u8DataFrameIndex]);
      //Restart timeout
      loc_u32LocTimeout = TEST_PROTOCOL_TIMEOUT;
      //reset transmit flag
      loc_bReadyToTransmit = FALSE;
    }
    else
    {
      //Timeout occurs
      loc_eReturnValue = TEST_PROTOCOL_ERROR__UNABLE_TO_TRANSMIT;
      loc_u8DataFrameIndex = loc_u8FrameSize;
    }
  }
  
  //Ensure USART is available to transmit or timeout occurs
  while((TRUE != loc_bReadyToTransmit)
    &&(0U != loc_u32LocTimeout))
  {
    // Wait for end transmit of previous char
    loc_bReadyToTransmit = TEST_Uart_AreYouReadyToTransmitData();
    loc_u32LocTimeout--;
  }
  
  if(TEST_PROTOCOL_ERROR__NO_ERROR == loc_eReturnValue)
  {
    if(loc_u8DataFrameIndex == loc_u8FrameSize)
    {
      TEST_Uart_TransmissionComplete();
    }
    else
    {
      //NOTHING to do
    }
  }
  else
  {
    //NOTHING to do
  }
  
  //Clear frame after sent it
  TestFrame_ClearFrame(TransmitFrameId);
  
  return loc_eReturnValue;
  /*## END[GUID f67f6fa1-36d1-4ffd-9e25-6128694053bd] ##*/
}


/*!************************************************************************************************
* \fn         static TeTestProtocolError TestProtocol_ACTION_OnFrameComplete(void)
* \return     TeTestProtocolError 
***************************************************************************************************/
//static TeTestProtocolError TestProtocol_ACTION_OnFrameComplete(void)
//{  
//  /*## BEGIN[GUID 2e3ff4cd-9b92-47c1-a9df-35e017a30a71] ##*/
//  TeIndustrialTestError loc_eReturnValue = INDUSTRIAL_TEST_ERROR__NO_ERROR;
//  TeTestProtocolError   loc_eRetValue = TEST_PROTOCOL_ERROR__NO_ERROR;
//  
//  loc_eReturnValue = IndustrialTest_NewFrameIsReceived();
//  if(INDUSTRIAL_TEST_ERROR__NO_ERROR != loc_eReturnValue)
//  {
//    IndustrialTest_UnknownCmd();
//  }
//  else
//  {
//    //Frame management succeed
//    loc_eReturnValue = IndustrialTest_CmdProcessed();
//    if(INDUSTRIAL_TEST_ERROR__NO_ERROR != loc_eReturnValue)
//    {
//    	loc_eRetValue = TEST_PROTOCOL_ERROR__BAD_FRAME;
//    }
//    else
//    {
//      //Nothing to do
//    }
//  }
//  
//  TestFrame_ClearFrame(ReceiveFrameId);
//  TestFrame_FrameIsUnlock(ReceiveFrameId);
//  
//  return loc_eRetValue;
//  /*## END[GUID 2e3ff4cd-9b92-47c1-a9df-35e017a30a71] ##*/
//}

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
