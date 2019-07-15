/**********************************************************************
This document and/or file is SOMFY�s property. All information it
contains is strictly confidential. This document and/or file shall
not be used, reproduced or passed on in any way, in full or in part
without SOMFY�s prior written approval. All rights reserved.
Ce document et/ou fichier est la propri�t� de SOMFY. Les informations
qu�il contient sont strictement confidentielles. Toute reproduction,
utilisation, transmission de ce document et/ou fichier, partielle ou
int�grale, non autoris�e pr�alablement par SOMFY par �crit est
interdite. Tous droits r�serv�s.
***********************************************************************/


//---------------------------------------------------------------------
// File "DoAlgoReq.c"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// External inclusions
//--------------------
#include "SFY_Includes.h"
#include "TestProtocol.h"
#include "IndustrialTestCommandListener.h"


// Internal inclusion
//------------------
#include "DoAlgoReq.h"

/*LDRA_HEADER_END*/

// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \fn         TeIndustrialTestCmdError DoAlgoReq_Execute(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg, TU8 _u8DataSizeArg, TU8* _pu8DataArg)
* \param[in]  _poListenerArg
* \param[in]  _oTransmitFrameArg
* \param[in]  _oReceiveFrameArg
* \param[in]  _u8DataSizeArg
* \param[in]  _pu8DataArg
* \return     TeIndustrialTestCmdError 
***************************************************************************************************/
TeIndustrialTestCmdError DoAlgoReq_Execute(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg, TU8 _u8DataSizeArg, TU8* _pu8DataArg)
{  
  /*## BEGIN[GUID 7c2fe6a6-2831-4b5e-aa75-9ca210d4c051] ##*/
  TeIndustrialTestCommandListenerError  loc_eTestCmdListenerRetVal  = INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_ERROR;
  TeIndustrialTestCmdError              loc_eReturnValue            = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
  TeTestFrameError                      loc_eTestFrameRetVal        = TEST_FRAME_ERROR__NO_ERROR;
  TU8                                   loc_u8ReceiveDataSize       = 0U;
  TU8                                   loc_u8ReceiveData[MAX_DATA_FRAME_SIZE];
  TU8                                   loc_u8DataSize              = 0U;
  TU8                                   loc_u8Data[MAX_DATA_FRAME_SIZE];
  TeTestProtocolError                   loc_eTestProtocolReturValue = TEST_PROTOCOL_ERROR__NO_ERROR;
  TU8                                   loc_u8FrameHeader = 0U;
  TU8                                   loc_u8AckValue              = ACK_KO;
  
  MEMSET(loc_u8Data,0,MAX_DATA_FRAME_SIZE);
  MEMSET(loc_u8ReceiveData,0,MAX_DATA_FRAME_SIZE);
  
  TestFrame_GetFrameData(_oReceiveFrameArg, &loc_u8ReceiveDataSize, loc_u8ReceiveData);
  TestFrame_GetFrameHeader(_oReceiveFrameArg, &loc_u8FrameHeader);
  
  //Do algo Command is managed by product and must send an answer
  loc_eTestCmdListenerRetVal = IndustrialTestCommandListener_GetInfo(_poListenerArg, ET_DO_ALGO_REQ, loc_u8ReceiveData, &loc_u8DataSize, loc_u8Data);
  if(INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_ERROR != loc_eTestCmdListenerRetVal)
  {
    //build ack answer
    TestFrame_UpdateTestFrame(_oTransmitFrameArg, ET_ACK, sizeof(loc_u8AckValue), &loc_u8AckValue, FALSE, FALSE);
    
    //Send Ack
    loc_eTestProtocolReturValue = TestProtocol_SendFrame();
    if(TEST_PROTOCOL_ERROR__NO_ERROR != loc_eTestProtocolReturValue)
    {
      loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__CMD_FAILED;
    }
    else
    {
      //Update return value if listener command failed
      loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__LISTENER_FAILED;
    }
    loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
  }
  else
  {
    //Build and send acknoledge if needed
    if(NO_ACK_NEEDED != (loc_u8FrameHeader & NO_ACK_NEEDED))
    {
      loc_u8AckValue = ACK_OK;
      //build ack answer
      TestFrame_UpdateTestFrame(_oTransmitFrameArg, ET_ACK, sizeof(loc_u8AckValue), &loc_u8AckValue, FALSE, FALSE);
  	    
      //Send Ack
      loc_eTestProtocolReturValue = TestProtocol_SendFrame();
      if(TEST_PROTOCOL_ERROR__NO_ERROR != loc_eTestProtocolReturValue)
      {
        loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
      }
      else
      {
        // test protocol command succeed
      }
    }
    else
    {
      //No Ack needed
    }
  
    //Build answer frame
    loc_eTestFrameRetVal = TestFrame_UpdateTestFrame(_oTransmitFrameArg, ET_DO_ALGO_ANS, loc_u8DataSize, loc_u8Data, FALSE, FALSE);
    
    if(TEST_FRAME_ERROR__NO_ERROR == loc_eTestFrameRetVal)
    {
      loc_eTestProtocolReturValue = TestProtocol_SendFrame();
      if(TEST_PROTOCOL_ERROR__NO_ERROR != loc_eTestProtocolReturValue)
      {
        loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
      }
      else
      {
        //test protocol command succeed
      }
    }
    else
    {
      loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
    }
  }
  
  return loc_eReturnValue;
  /*## END[GUID 7c2fe6a6-2831-4b5e-aa75-9ca210d4c051] ##*/
}


/*!************************************************************************************************
* \fn         TeIndustrialTestCmdError DoAlgoReq_ValidateAndFillAck(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg)
* \param[in]  _poListenerArg
* \param[in]  _oTransmitFrameArg
* \param[in]  _oReceiveFrameArg
* \return     TeIndustrialTestCmdError 
***************************************************************************************************/
TeIndustrialTestCmdError DoAlgoReq_ValidateAndFillAck(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg)
{  
  /*## BEGIN[GUID 1769d539-01cb-4d0e-8095-4b4e9e4e6104] ##*/
  TeIndustrialTestCmdError  loc_eReturnValue            = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
  TeTestProtocolError       loc_eTestProtocolReturValue = TEST_PROTOCOL_ERROR__NO_ERROR;
  TU8                       loc_u8FrameCmdId            = 0U;
  TBool                     loc_bCksumStatus            = FALSE;
  TU8                       loc_u8AckValue              = ACK_KO;
  TU8                       loc_u8FrameHeader           = 0U;
  
  TestFrame_GetFrameHeader(_oReceiveFrameArg, &loc_u8FrameHeader);
  TestFrame_GetFrameId(_oReceiveFrameArg, &loc_u8FrameCmdId);
  
  if(ET_DO_ALGO_REQ == loc_u8FrameCmdId)
  {
    loc_bCksumStatus = TestFrame_CksumIsValid(_oReceiveFrameArg);
    if(TRUE == loc_bCksumStatus)
    {
      loc_u8AckValue = ACK_OK;
    }
    else
    {
      //Bad Ckecksum
      loc_u8AckValue = ACK_BAD_CKSUM;
      loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
    }
  }
  else
  {
    //Bad Receive frame Id
  }
  
  //Build and send acknoledge if acknoledge is not OK
  if(ACK_OK != loc_u8AckValue)
  {
      //build ack answer
      TestFrame_UpdateTestFrame(_oTransmitFrameArg, ET_ACK, sizeof(loc_u8AckValue), &loc_u8AckValue, FALSE, FALSE);
      
      //Send Ack
      loc_eTestProtocolReturValue = TestProtocol_SendFrame();
      if(TEST_PROTOCOL_ERROR__NO_ERROR != loc_eTestProtocolReturValue)
      {
        loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
      }
      else
      {
        // test protocol command succeed
      }
  }
  else
  {
    //Nothing to do
  }
  
  return loc_eReturnValue;
  /*## END[GUID 1769d539-01cb-4d0e-8095-4b4e9e4e6104] ##*/
}

/**********************************************************************
Copyright � (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
