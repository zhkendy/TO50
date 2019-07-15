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
// File "KeyPressed.c"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// External inclusions
//--------------------
#include "SFY_Includes.h"
#include "TestProtocol.h"
#include "IndustrialTestConfig.h"


// Internal inclusion
//------------------
#include "KeyPressed.h"

/*LDRA_HEADER_END*/

// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \fn         TeIndustrialTestCmdError KeyPressed_Execute(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg, TU8 _u8DataSizeArg, TU8* _pu8DataArg)
* \param[in]  _poListenerArg
* \param[in]  _oTransmitFrameArg
* \param[in]  _oReceiveFrameArg
* \param[in]  _u8DataSizeArg
* \param[in]  _pu8DataArg
* \return     TeIndustrialTestCmdError 
***************************************************************************************************/
TeIndustrialTestCmdError KeyPressed_Execute(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg, TU8 _u8DataSizeArg, TU8* _pu8DataArg)
{  
  /*## BEGIN[GUID bafb9f34-14b8-4f87-9262-fa0d1f95e8a9] ##*/
  TeIndustrialTestCmdError  loc_eRetVal           = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
  TeTestFrameError          loc_eTestFrameRetVal  = TEST_FRAME_ERROR__NO_ERROR;
  TeTestProtocolError       loc_eTestProtocolReturValue = TEST_PROTOCOL_ERROR__NO_ERROR;
  
  loc_eTestFrameRetVal = TestFrame_UpdateTestFrame(_oTransmitFrameArg, ET_KEY_PRESSED, _u8DataSizeArg, _pu8DataArg, FALSE, FALSE);
  if(TEST_FRAME_ERROR__NO_ERROR != loc_eTestFrameRetVal)
  {
    loc_eRetVal = INDUSTRIAL_TEST_CMD_ERROR__UPDATE_FRAME_ERROR;
  }
  else
  {
      loc_eTestProtocolReturValue = TestProtocol_SendFrame();
      if(TEST_PROTOCOL_ERROR__NO_ERROR != loc_eTestProtocolReturValue)
      {
        loc_eRetVal = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
      }
      else
      {
        // test protocol command succeed
      }
  }
  
  return loc_eRetVal;
  /*## END[GUID bafb9f34-14b8-4f87-9262-fa0d1f95e8a9] ##*/
}


/*!************************************************************************************************
* \fn         TeIndustrialTestCmdError KeyPressed_ValidateAndFillAck(TsIndustrialTestCommandListenerInfo* _poListenerArgArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg)
* \param[in]  _poListenerArgArg
* \param[in]  _oTransmitFrameArg
* \param[in]  _oReceiveFrameArg
* \return     TeIndustrialTestCmdError 
***************************************************************************************************/
TeIndustrialTestCmdError KeyPressed_ValidateAndFillAck(TsIndustrialTestCommandListenerInfo* _poListenerArgArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg)
{  
  /*## BEGIN[GUID 8e929524-bc82-4f08-8865-ddb6ef38486c] ##*/
  TeIndustrialTestCmdError  loc_eReturnValue            = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
  TeTestProtocolError       loc_eTestProtocolReturValue = TEST_PROTOCOL_ERROR__NO_ERROR;
  TU8                       loc_u8FrameCmdId            = 0U;
  TBool                     loc_bCksumStatus            = FALSE;
  TU8                       loc_u8AckValue              = ACK_KO;
  
  TestFrame_GetFrameId(_oReceiveFrameArg, &loc_u8FrameCmdId);
  
  if(ET_KEY_PRESSED == loc_u8FrameCmdId)
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
    //No Ack needed
  }
  
  return loc_eReturnValue;
  /*## END[GUID 8e929524-bc82-4f08-8865-ddb6ef38486c] ##*/
}

/**********************************************************************
Copyright � (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
