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
// @author MGu 2016
//---------------------------------------------------------------------



#include "SFY_Includes.h"
#include "TestProtocol.h"



#include "GetExtPeriphPowerMode.h"




TeIndustrialTestCmdError GetExtPeriphPowerMode_Execute(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg, TU8 _u8DataSizeArg, TU8* _pu8DataArg)
{  
  
//  TU8                                   loc_u8DataSize              = 0U;
//  TU8                                   loc_u8Data[ET_GET_PLINE_ANS_DATA_LENGHT];
//  TeIndustrialTestCmdError              loc_eRetVal                 = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
//  TeIndustrialTestCommandListenerError  loc_eReturnValue            = INDUSTRIAL_TEST_CMD_LISTENER_ERROR__LISTENER_ERROR;
//  TeTestProtocolError                   loc_eTestProtocolReturValue = TEST_PROTOCOL_ERROR__NO_ERROR;
//  TeTestFrameError                      loc_eTestFrameRetVal        = TEST_FRAME_ERROR__NO_ERROR;
//  TU8                                   loc_u8ReceiveDataSize       = 0U;
//  TU8                                   loc_u8ReceiveData[ET_GET_PLINE_REQ_LENGTH];
//  TU8                                   loc_u8AckValue              = ACK_KO;
//  TU8                                   loc_u8FrameHeader = 0U;
//  
//  //Initialize charts
//  MEMSET(loc_u8Data, 0, sizeof(loc_u8Data));
//  MEMSET(loc_u8ReceiveData,0,sizeof(loc_u8ReceiveData));
//  
//  //Get complete frame
//  TestFrame_GetFrameData(_oReceiveFrameArg, &loc_u8ReceiveDataSize, loc_u8ReceiveData);
//  TestFrame_GetFrameHeader(_oReceiveFrameArg, &loc_u8FrameHeader);
//  
//  //Call Handler
//  loc_eReturnValue = IndustrialTestCommandListener_GetInfo(_poListenerArg, ET_GET_PLINE_REQ, &(loc_u8ReceiveData[COMMAND_DATA_POSITION]), &loc_u8DataSize, loc_u8Data);
//  if(loc_eReturnValue != INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_ERROR)
//  {
//    //ERROR
//    //build ack answer
//    loc_u8AckValue = ACK_KO;
//    TestFrame_UpdateTestFrame(_oTransmitFrameArg, ET_ACK, sizeof(loc_u8AckValue), &loc_u8AckValue,
//                              FALSE,// No Multiframe
//                              FALSE ); //No Ack
//    //Send Ack
//    loc_eTestProtocolReturValue = TestProtocol_SendFrame();
//    if(loc_eTestProtocolReturValue != TEST_PROTOCOL_ERROR__NO_ERROR )
//    {
//      loc_eRetVal = INDUSTRIAL_TEST_CMD_ERROR__CMD_FAILED;
//    }
//    else
//    {
//      //Update return value if listener command failed
//      loc_eRetVal = INDUSTRIAL_TEST_CMD_ERROR__LISTENER_FAILED;
//    }
//  }
//  else
//  {
//    //Build and send acknoledge if needed
//    if(NO_ACK_NEEDED != (loc_u8FrameHeader & NO_ACK_NEEDED))
//    {
//      //ACK needed
//      loc_u8AckValue = ACK_OK;
//      //build ack answer
//      TestFrame_UpdateTestFrame(_oTransmitFrameArg, ET_ACK, sizeof(loc_u8AckValue), &loc_u8AckValue, FALSE, FALSE);
//      
//      //Send Ack
//      loc_eTestProtocolReturValue = TestProtocol_SendFrame();
//      if(TEST_PROTOCOL_ERROR__NO_ERROR != loc_eTestProtocolReturValue)
//      {
//        loc_eRetVal = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
//      }
//      else
//      {
//        // test protocol command succeed
//      }
//    }
//    else
//    {
//      //No Ack needed
//    }
//    
//    //Build answer frame
//    loc_eTestFrameRetVal = TestFrame_UpdateTestFrame(_oTransmitFrameArg, ET_GET_PLINE_ANS, loc_u8DataSize, loc_u8Data, FALSE, FALSE);
//    
//    if(loc_eTestFrameRetVal == TEST_FRAME_ERROR__NO_ERROR)
//    {
//      loc_eTestProtocolReturValue = TestProtocol_SendFrame();
//      if(loc_eTestProtocolReturValue != TEST_PROTOCOL_ERROR__NO_ERROR)
//      {
//        loc_eRetVal = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
//      }
//      else
//      {
//        // test protocol command succeed
//      }
//    }
//    else
//    {
//      loc_eRetVal = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
//    }
//  }
  
  return INDUSTRIAL_TEST_CMD_ERROR__CMD_FAILED;
  
}



TeIndustrialTestCmdError GetExtPeriphPowerMode_ValidateAndFillAck(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg)
{  
//  TeIndustrialTestCmdError  loc_eReturnValue            = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
//  TeTestProtocolError       loc_eTestProtocolReturValue = TEST_PROTOCOL_ERROR__NO_ERROR;
//  TU8                       loc_u8FrameCmdId            = 0U;
//  TBool                     loc_bCksumStatus            = FALSE;
//  TU8                       loc_u8AckValue              = ACK_KO;
//  TU8                       loc_u8FrameHeader           = 0U;
//  
//  TestFrame_GetFrameHeader(_oReceiveFrameArg, &loc_u8FrameHeader);
//  TestFrame_GetFrameId(_oReceiveFrameArg, &loc_u8FrameCmdId);
//  
//  if(ET_GET_PLINE_REQ == loc_u8FrameCmdId)
//  {
//    loc_bCksumStatus = TestFrame_CksumIsValid(_oReceiveFrameArg);
//    if(TRUE == loc_bCksumStatus)
//    {
//      loc_u8AckValue = ACK_OK;
//    }
//    else
//    {
//      //Bad Ckecksum
//      loc_u8AckValue = ACK_BAD_CKSUM;
//      loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
//    }
//  }
//  else
//  {
//    //Bad Receive frame Id
//    loc_u8AckValue = ACK_KO;
//    loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
//  }
//  
//  //Build and send acknoledge if acknoledge is not OK
//  if(ACK_OK != loc_u8AckValue)
//  {
//    //build ack answer
//    TestFrame_UpdateTestFrame(_oTransmitFrameArg, ET_ACK, sizeof(loc_u8AckValue), &loc_u8AckValue, FALSE, FALSE);
//    
//    //Send Ack
//    loc_eTestProtocolReturValue = TestProtocol_SendFrame();
//    if(TEST_PROTOCOL_ERROR__NO_ERROR != loc_eTestProtocolReturValue)
//    {
//      loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
//    }
//    else
//    {
//      // test protocol command succeed
//    }
//  }
//  else
//  {
//    //Nothing to do
//  }
//  
  return INDUSTRIAL_TEST_CMD_ERROR__CMD_FAILED;
}

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
