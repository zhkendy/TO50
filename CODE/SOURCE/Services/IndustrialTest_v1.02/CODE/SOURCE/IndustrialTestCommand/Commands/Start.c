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
// File "Start.c"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// External inclusions
//--------------------
#include "SFY_Includes.h"
#include "TestProtocol.h"


// Internal inclusion
//------------------
#include "Start.h"


// Private types
//-------------

CONST TU8 u8StartCmdData[ET_START_DATA_LENGTH] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07};

/*LDRA_HEADER_END*/

// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \fn         TeIndustrialTestCmdError Start_Execute(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg, TU8 _u8DataSizeArg, TU8* _pu8DataArg)
* \param[in]  _poListenerArg
* \param[in]  _oTransmitFrameArg
* \param[in]  _oReceiveFrameArg
* \param[in]  _u8DataSizeArg
* \param[in]  _pu8DataArg
* \return     TeIndustrialTestCmdError 
***************************************************************************************************/
TeIndustrialTestCmdError Start_Execute(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg, TU8 _u8DataSizeArg, TU8* _pu8DataArg)
{  
  /*## BEGIN[GUID eab71c52-5929-47e6-a983-80cb70734eed] ##*/
  TeIndustrialTestCmdError loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
  
  return loc_eReturnValue;
  /*## END[GUID eab71c52-5929-47e6-a983-80cb70734eed] ##*/
}


/*!************************************************************************************************
* \fn         TeIndustrialTestCmdError Start_ValidateAndFillAck(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg)
* \param[in]  _poListenerArg
* \param[in]  _oTransmitFrameArg
* \param[in]  _oReceiveFrameArg
* \return     TeIndustrialTestCmdError 
***************************************************************************************************/
TeIndustrialTestCmdError Start_ValidateAndFillAck(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg)
{  
  /*## BEGIN[GUID 9e1e8996-6dba-435c-8536-5e593b4f0b40] ##*/
  TeIndustrialTestCmdError  loc_eRetVal           = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
  TeTestProtocolError       loc_eTestProtocolReturValue = TEST_PROTOCOL_ERROR__NO_ERROR;
  TU8                       loc_u8FrameHeader     = 0U;
  TU8                       loc_u8FrameLength     = 0U;
  TU8                       loc_u8FrameCmdId      = 0U;
  TU8                       loc_u8Data[ET_START_LENGTH];
  TU8                       loc_u8FrameIndex      = 0U;
  TBool                     loc_bCksumStatus      = FALSE;
  TU8                       loc_u8AckValue        = ACK_KO;
  
  //Initialize loc_u8Data
  MEMSET(loc_u8Data, 0, ET_START_LENGTH);
  
  TestFrame_GetFrameHeader(_oReceiveFrameArg, &loc_u8FrameHeader);
  TestFrame_GetFrameId(_oReceiveFrameArg, &loc_u8FrameCmdId);
  TestFrame_GetFrameData(_oReceiveFrameArg, &loc_u8FrameLength, loc_u8Data);
  
  if(ET_START == loc_u8FrameCmdId)
  {
    loc_bCksumStatus = TestFrame_CksumIsValid(_oReceiveFrameArg);
    if(TRUE == loc_bCksumStatus)
    {
      for(loc_u8FrameIndex= 0U; loc_u8FrameIndex < ET_START_DATA_LENGTH; loc_u8FrameIndex++)
      {
        if(loc_u8Data[COMMAND_DATA_POSITION + loc_u8FrameIndex] != u8StartCmdData[loc_u8FrameIndex])
        {
          loc_u8FrameIndex = ET_START_DATA_LENGTH;
          loc_eRetVal = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
        }
      }
  
      //Update Ack value
      if(INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR == loc_eRetVal)
      {
        loc_u8AckValue = ACK_OK;
        IndustrialTestCommandListener_Notify(_poListenerArg, ET_START, NULL);
      }
    }
    else
    {
      //Bad Ckecksum
      loc_u8AckValue = ACK_BAD_CKSUM;
      loc_eRetVal = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
    }
  }
  else
  {
    //Receive frame is not start frame
    loc_u8AckValue = ACK_KO;
    loc_eRetVal = INDUSTRIAL_TEST_CMD_ERROR__BAD_COMMAND_ERROR;
  }
  
  //Build and send acknoledge if needed
  if(NO_ACK_NEEDED != (loc_u8FrameHeader & NO_ACK_NEEDED))
  {
      //build ack answer
      TestFrame_UpdateTestFrame(_oTransmitFrameArg, ET_ACK, 1, &loc_u8AckValue, FALSE, FALSE);
      
      //Send Ack
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
  else
  {
    //No Ack needed
  }
  
  return loc_eRetVal;
  /*## END[GUID 9e1e8996-6dba-435c-8536-5e593b4f0b40] ##*/
}

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
