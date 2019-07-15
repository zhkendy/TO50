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
// File "IndustrialTest.c"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// External inclusions
//--------------------
#include "SFY_Includes.h"
#include "IndustrialTestConfig.h"
#include "TestProtocol.h"
#include "TestFrame.h"


// Internal inclusion
//------------------
#include "IndustrialTest.h"


// Private types
//-------------

static ToTestFrameId TransmitFrameHdl;
static ToTestFrameId ReceiveFrameHdl;

CONST TsIndustrialTestCommandListenerIf sIndustrialTestAsIndustrialTestCommandListener =
{ 
  IndustrialTest_GetInfo
  ,IndustrialTest_Notify
};

/* me is a macro equivalent to this in C++ */
/* LDRA Rule Deactivation : 210 S Macro name is not upper case. */
/*LDRA_INSPECTED 210 S*/
#define me (&oIndustrialTest)

//! State structure declaration
typedef enum
{
  INDUSTRIALTEST_STATE_ROOT,
  INDUSTRIALTEST_STATE_IDLE,
  INDUSTRIALTEST_STATE_WAIT_FOR_START_COMMAND,
  INDUSTRIALTEST_STATE_WAIT_FOR_COMMAND,
  INDUSTRIALTEST_STATE_COMMAND_PROCESSING,
  INDUSTRIALTEST_LAST_STATE = INDUSTRIALTEST_STATE_COMMAND_PROCESSING //!< Last state of IndustrialTest object used for state machine coherency
} TeIndustrialTestState;

//! Field sizes
#define INDUSTRIALTEST_STATE_SIZE  sizeof(TeIndustrialTestState)
#define INDUSTRIALTEST_S_LISTENER_INFO_SIZE  (sizeof(TsIndustrialTestListenerInfo))

//! Object structure declaration
typedef struct
{
  //! State variable
  TeIndustrialTestState eState;
#ifdef INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
  TsIndustrialTestListenerInfo sListenerInfo;
#endif // INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
} TsIndustrialTest;


// Private functions declaration
//---------------------------------

static TeIndustrialTestError IndustrialTest_ACTION_OnCmdReceived(void);

static TeIndustrialTestError IndustrialTest_ACTION_OnNewFrameReceived(void);

static TeIndustrialTestError IndustrialTest_ACTION_OnStartEmbeddedTest(void);


// Private variables
//------------------

//! Object instanciation
TsIndustrialTest oIndustrialTest;

/*LDRA_HEADER_END*/

// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \fn         void IndustrialTest_Init(TsIndustrialTestListenerInfo* pListenerArg)
* \param[in]  pListenerArg
* \return     void 
***************************************************************************************************/
void IndustrialTest_Init(TsIndustrialTestListenerInfo* pListenerArg)
{  
  // Implementation
  //---------------
  {
    /*## BEGIN[GUID ca784890-099c-4598-a464-9f18414ad2c0] ##*/
    #ifdef INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
    IndustrialTest_SetupListener(*pListenerArg);
    #endif // INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
    
    
    TestProtocol_Init();
    
    //Test command definition
    TestProtocol_GetFrameHdl(&ReceiveFrameHdl, &TransmitFrameHdl);
    /*## END[GUID ca784890-099c-4598-a464-9f18414ad2c0] ##*/
  }
  // Changing state
  //------------------
  me->eState = INDUSTRIALTEST_STATE_IDLE;
}


/*!************************************************************************************************
* \fn         void IndustrialTest_Cleanup(void)
* \return     void 
***************************************************************************************************/
void IndustrialTest_Cleanup(void)
{  
  // Object state destruction
  //---------------------------------
  me->eState = INDUSTRIALTEST_STATE_ROOT;
  
  // Implementation
  //---------------
  {
    /*## BEGIN[GUID 578e15f7-8098-4ad2-a0d2-2f72dbb752f1] ##*/
    TestProtocol_Cleanup();
    /*## END[GUID 578e15f7-8098-4ad2-a0d2-2f72dbb752f1] ##*/
  }
}


/*!************************************************************************************************
* \fn         TeIndustrialTestError IndustrialTest_CmdProcessed(void)
* \return     TeIndustrialTestError 
***************************************************************************************************/
TeIndustrialTestError IndustrialTest_CmdProcessed(void)
{  
  // Local variables
  //------------------
  TeIndustrialTestError ric_reply;
  ric_reply = INDUSTRIAL_TEST_ERROR__INDUS_CMD_ERROR;
  
  // State test
  //---------------
  switch (me->eState)
  {
    case INDUSTRIALTEST_STATE_COMMAND_PROCESSING :
      // Transition action
      ric_reply = IndustrialTest_ACTION_OnCmdReceived();
      
      // Changing state
      me->eState = INDUSTRIALTEST_STATE_WAIT_FOR_COMMAND;
      break;
  
    default:
      break;
  }
  
  // Return result
  //------------------
  return ric_reply;
}


/*!************************************************************************************************
* \fn         TeIndustrialTestError IndustrialTest_NewFrameIsReceived(void)
* \return     TeIndustrialTestError 
***************************************************************************************************/
TeIndustrialTestError IndustrialTest_NewFrameIsReceived(void)
{  
  // Local variables
  //------------------
  TeIndustrialTestError ric_reply;
  TBool bFrameIsStartFrame;
  ric_reply = INDUSTRIAL_TEST_ERROR__INDUS_CMD_ERROR;
  
  
  // State test
  //---------------
  switch (me->eState)
  {
    case INDUSTRIALTEST_STATE_WAIT_FOR_START_COMMAND :
      // Transition action
      bFrameIsStartFrame = IndustrialTest_CheckFrameIsStartFrame();
      
      if (TRUE == bFrameIsStartFrame)
      {
        // Transition action
        ric_reply = INDUSTRIAL_TEST_ERROR__NO_ERROR;
      
        // Changing state
        me->eState = INDUSTRIALTEST_STATE_WAIT_FOR_COMMAND;
      }
      else
      {
        // Transition action
        ric_reply = INDUSTRIAL_TEST_ERROR__INDUS_CMD_ERROR;
        
        // Changing state
        me->eState = INDUSTRIALTEST_STATE_WAIT_FOR_START_COMMAND;
      }
      break;
  
    case INDUSTRIALTEST_STATE_WAIT_FOR_COMMAND :
      // Transition action
      ric_reply = IndustrialTest_ACTION_OnNewFrameReceived();
      
      // Changing state
      me->eState = INDUSTRIALTEST_STATE_COMMAND_PROCESSING;
      break;
  
    default:
      break;
  }
  
  // Return result
  //------------------
  return ric_reply;
}


/*!************************************************************************************************
* \fn         void IndustrialTest_StartEmbeddedTest(void)
* \return     void 
***************************************************************************************************/
void IndustrialTest_StartEmbeddedTest(void)
{  
  // State test
  //---------------
  switch (me->eState)
  {
    case INDUSTRIALTEST_STATE_IDLE :
      // Transition action
      IndustrialTest_ACTION_OnStartEmbeddedTest();
      
      // Changing state
      me->eState = INDUSTRIALTEST_STATE_WAIT_FOR_START_COMMAND;
      break;
  
    default:
      break;
  }
}


/*!************************************************************************************************
* \fn         void IndustrialTest_StopEmbeddedTest(void)
* \return     void 
***************************************************************************************************/
void IndustrialTest_StopEmbeddedTest(void)
{  
  
}


/*!************************************************************************************************
* \fn         void IndustrialTest_UnknownCmd(void)
* \return     void 
***************************************************************************************************/
void IndustrialTest_UnknownCmd(void)
{  
  // State test
  //---------------
  switch (me->eState)
  {
    case INDUSTRIALTEST_STATE_COMMAND_PROCESSING :
      // Changing state
      me->eState = INDUSTRIALTEST_STATE_WAIT_FOR_COMMAND;
      break;
  
    default:
      break;
  }
}


/*!************************************************************************************************
* \brief      Method to setup listener info dynamically.
* \fn         void IndustrialTest_SetupListener(TsIndustrialTestListenerInfo psListenerInfoArg)
* \param[in]  psListenerInfoArg : Listener information.
* \return     void 
***************************************************************************************************/
#ifdef INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES
void IndustrialTest_SetupListener(TsIndustrialTestListenerInfo psListenerInfoArg)
{  
  /*## BEGIN[GUID 20d879c9-614a-4af8-b924-06f7c0e835ef] ##*/
  me->sListenerInfo.oListenerId = psListenerInfoArg.oListenerId;
  me->sListenerInfo.psInterface = psListenerInfoArg.psInterface;
  /*## END[GUID 20d879c9-614a-4af8-b924-06f7c0e835ef] ##*/
}
#endif // INDUSTRIAL_TEST_LISTENER_DYNAMIC_INTERFACES


/*!************************************************************************************************
* \fn         TeIndustrialTestError IndustrialTest_AbortIndustrialTest(void)
* \return     TeIndustrialTestError 
***************************************************************************************************/
TeIndustrialTestError IndustrialTest_AbortIndustrialTest(void)
{  
  /*## BEGIN[GUID 11a19348-8412-46c0-ae13-7ab68dfc48e7] ##*/
    TeIndustrialTestError               loc_eReturValue            = INDUSTRIAL_TEST_ERROR__NO_ERROR;
    TeTestProtocolError                 loc_eTestProtocolReturValue    = TEST_PROTOCOL_ERROR__NO_ERROR;
    TeIndustrialTestCmdError            loc_eIndusTestCmdReturnVal = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
    TsIndustrialTestCommandListenerInfo loc_sIndustrialTestCommandListenerInfo;
    
    loc_sIndustrialTestCommandListenerInfo.oListenerId = 0U;
    loc_sIndustrialTestCommandListenerInfo.psInterface = &sIndustrialTestAsIndustrialTestCommandListener;
    loc_eIndusTestCmdReturnVal = IndustrialTestCommand_Execute(&loc_sIndustrialTestCommandListenerInfo, ET_ABORT_TST_IND, TransmitFrameHdl, ReceiveFrameHdl, 0, NULL);
    
    if(INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR == loc_eIndusTestCmdReturnVal)
    {
      loc_eTestProtocolReturValue = TestProtocol_SendFrame();
      if(TEST_PROTOCOL_ERROR__NO_ERROR != loc_eTestProtocolReturValue)
      {
        loc_eReturValue = INDUSTRIAL_TEST_ERROR__TEST_PROTOCOL_ERROR;
      }
      else
      {
        // test protocol command succeed
      }
    }
    else
    {
      loc_eReturValue = INDUSTRIAL_TEST_ERROR__INDUS_CMD_ERROR;
    }
    
    return loc_eReturValue;
  /*## END[GUID 11a19348-8412-46c0-ae13-7ab68dfc48e7] ##*/
}


/*!************************************************************************************************
* \fn         TBool IndustrialTest_CheckFrameIsStartFrame(void)
* \return     TBool 
***************************************************************************************************/
TBool IndustrialTest_CheckFrameIsStartFrame(void)
{  
  /*## BEGIN[GUID f5cfde4a-7d87-45d1-8c47-8dcbaaf6585d] ##*/
  TsIndustrialTestCommandListenerInfo loc_sIndustrialTestCommandListenerInfo;
  TeIndustrialTestCmdError            loc_eIndTestCmdError = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
  TBool                               loc_bFrameIsStartFrame = FALSE;
  
  //Initialise listener structure with local functions
  loc_sIndustrialTestCommandListenerInfo.oListenerId = 0U;
  loc_sIndustrialTestCommandListenerInfo.psInterface = &sIndustrialTestAsIndustrialTestCommandListener;
  loc_eIndTestCmdError = IndustrialTestCommand_ValidateAndFillAck(&loc_sIndustrialTestCommandListenerInfo, ET_START, TransmitFrameHdl, ReceiveFrameHdl);
  if(INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR == loc_eIndTestCmdError)
  {
    loc_bFrameIsStartFrame = TRUE;
  }
  else
  {
    //Receive frame is not start frame
  }
  
  return loc_bFrameIsStartFrame;
  /*## END[GUID f5cfde4a-7d87-45d1-8c47-8dcbaaf6585d] ##*/
}


/*!************************************************************************************************
* \fn         TeIndustrialTestCommandListenerError IndustrialTest_GetInfo(TIndex _idxDummyArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Parameters, TU8* pu8DataSize, TU8* pu8data)
* \param[in]  _idxDummyArg
* \param[in]  eInfoIdArg
* \param[in]  pu8Parameters
* \param[in]  pu8DataSize
* \param[in]  pu8data
* \return     TeIndustrialTestCommandListenerError 
***************************************************************************************************/
TeIndustrialTestCommandListenerError IndustrialTest_GetInfo(TIndex _idxDummyArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Parameters, TU8* pu8DataSize, TU8* pu8data)
{  
  /*## BEGIN[GUID 6069e39e-0bf1-4e9f-b24f-6a263fa0fb37] ##*/
  TeIndustrialTestCommandListenerError loc_eReturnValue = INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_ERROR;
  TeIndustrialTestListenerError        loc_eIndusTestListError  = INDUSTRIAL_TEST_LISTENER_ERROR__NO_ERROR;
  
  loc_eIndusTestListError = IndustrialTestListener_GetInfo(&(me->sListenerInfo), eInfoIdArg, pu8Parameters, pu8DataSize, pu8data);
  if(INDUSTRIAL_TEST_LISTENER_ERROR__NO_ERROR != loc_eIndusTestListError)
  {
    loc_eReturnValue = INDUSTRIAL_TEST_CMD_LISTENER_ERROR__LISTENER_ERROR;
  }
  
  return loc_eReturnValue;
  /*## END[GUID 6069e39e-0bf1-4e9f-b24f-6a263fa0fb37] ##*/
}


/*!************************************************************************************************
* \fn         TeIndustrialTestCommandListenerError IndustrialTest_Notify(TIndex _idxDummyArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Data)
* \param[in]  _idxDummyArg
* \param[in]  eInfoIdArg
* \param[in]  pu8Data
* \return     TeIndustrialTestCommandListenerError 
***************************************************************************************************/
TeIndustrialTestCommandListenerError IndustrialTest_Notify(TIndex _idxDummyArg, TeIndustrialTestCmd eInfoIdArg, TU8* pu8Data)
{  
  /*## BEGIN[GUID 04daf032-331c-4cf2-b62d-7ad75ebcc613] ##*/
  TeIndustrialTestCommandListenerError loc_eReturnValue = INDUSTRIAL_TEST_CMD_LISTENER_ERROR__NO_ERROR;
  TeIndustrialTestListenerError        loc_eIndusTestListError  = INDUSTRIAL_TEST_LISTENER_ERROR__NO_ERROR;
  
  loc_eIndusTestListError = IndustrialTestListener_Notify(&(me->sListenerInfo), eInfoIdArg, pu8Data);
  if(INDUSTRIAL_TEST_LISTENER_ERROR__NO_ERROR != loc_eIndusTestListError)
  {
    loc_eReturnValue = INDUSTRIAL_TEST_CMD_LISTENER_ERROR__LISTENER_ERROR;
  }
  
  return loc_eReturnValue;
  /*## END[GUID 04daf032-331c-4cf2-b62d-7ad75ebcc613] ##*/
}


/*!************************************************************************************************
* \fn         TeIndustrialTestCmdError IndustrialTest_SendKeyBoardPressedAnswer(TU8 u8ButtonPressedId)
* \param[in]  u8ButtonPressedId
* \return     TeIndustrialTestCmdError 
***************************************************************************************************/
TeIndustrialTestCmdError IndustrialTest_SendKeyBoardPressedAnswer(TU8 u8ButtonPressedId)
{  
  /*## BEGIN[GUID e8e88330-2a5a-4fcd-8c16-973030f06a97] ##*/
  TsIndustrialTestCommandListenerInfo loc_sIndustrialTestCommandListenerInfo;
  TeIndustrialTestCmdError            loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
  
  loc_sIndustrialTestCommandListenerInfo.oListenerId = 0U;
  loc_sIndustrialTestCommandListenerInfo.psInterface = &sIndustrialTestAsIndustrialTestCommandListener;
  loc_eReturnValue = IndustrialTestCommand_Execute(&loc_sIndustrialTestCommandListenerInfo, ET_KEY_PRESSED, TransmitFrameHdl, ReceiveFrameHdl, sizeof(u8ButtonPressedId), &u8ButtonPressedId);  
  
  return loc_eReturnValue;
  /*## END[GUID e8e88330-2a5a-4fcd-8c16-973030f06a97] ##*/
}


/*!************************************************************************************************
* \fn         TeIndustrialTestCmdError IndustrialTest_SendKeyBoardReleasedAnswer(TU8 u8ButtonPressedId)
* \param[in]  u8ButtonPressedId
* \return     TeIndustrialTestCmdError 
***************************************************************************************************/
TeIndustrialTestCmdError IndustrialTest_SendKeyBoardReleasedAnswer(TU8 u8ButtonPressedId)
{  
  /*## BEGIN[GUID b7afbaa3-82bd-4b05-a80d-6571112c5374] ##*/
  TsIndustrialTestCommandListenerInfo loc_sIndustrialTestCommandListenerInfo;
  TeIndustrialTestCmdError            loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
  
  loc_sIndustrialTestCommandListenerInfo.oListenerId = 0U;
  loc_sIndustrialTestCommandListenerInfo.psInterface = &sIndustrialTestAsIndustrialTestCommandListener;
  loc_eReturnValue = IndustrialTestCommand_Execute(&loc_sIndustrialTestCommandListenerInfo, ET_KEY_RELEASED, TransmitFrameHdl, ReceiveFrameHdl, sizeof(u8ButtonPressedId), &u8ButtonPressedId); 
  
  return loc_eReturnValue;
  /*## END[GUID b7afbaa3-82bd-4b05-a80d-6571112c5374] ##*/
}


/*!************************************************************************************************
* \fn         TeIndustrialTestCmdError IndustrialTest_SendSensitiveTestAnswer(TU8* pu8SensitiveTestValuesArg)
* \param[out] pu8SensitiveTestValuesArg
* \return     TeIndustrialTestCmdError 
***************************************************************************************************/
#ifdef MANAGE_SENSITIVE_TEST_ANS
TeIndustrialTestCmdError IndustrialTest_SendSensitiveTestAnswer(TU8* pu8SensitiveTestValuesArg)
{  
  /*## BEGIN[GUID d63be486-1c42-47e1-838c-f9f1dd94691a] ##*/
  TsIndustrialTestCommandListenerInfo loc_sIndustrialTestCommandListenerInfo;
  TeIndustrialTestCmdError            loc_eReturnValue = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
  
  loc_sIndustrialTestCommandListenerInfo.oListenerId = 0U;
  loc_sIndustrialTestCommandListenerInfo.psInterface = &sIndustrialTestAsIndustrialTestCommandListener;
  loc_eReturnValue = IndustrialTestCommand_Execute(&loc_sIndustrialTestCommandListenerInfo, ET_SENSITIVE_TEST_ANS, TransmitFrameHdl, ReceiveFrameHdl, SIZE_OF_SENSITIVE_TEST_ANS_DATA, pu8SensitiveTestValuesArg); 
  
  return loc_eReturnValue;
  /*## END[GUID d63be486-1c42-47e1-838c-f9f1dd94691a] ##*/
}
#endif //MANAGE_SENSITIVE_TEST_ANS


// Private functions implementation
//------------------------------------


/*!************************************************************************************************
* \fn         static TeIndustrialTestError IndustrialTest_ACTION_OnCmdReceived(void)
* \return     TeIndustrialTestError 
***************************************************************************************************/
static TeIndustrialTestError IndustrialTest_ACTION_OnCmdReceived(void)
{  
  /*## BEGIN[GUID 5a2aa6f6-0603-4ea5-aba2-529b498f3b9f] ##*/
  TsIndustrialTestCommandListenerInfo loc_sIndustrialTestCommandListenerInfo;
  TeIndustrialTestCmdError            loc_eIndTestCmdError  = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
  TeIndustrialTestError               loc_eReturnValue      = INDUSTRIAL_TEST_ERROR__NO_ERROR;
  TU8                                 loc_u8CmdId           = 0U;
  
  TestFrame_GetFrameId(ReceiveFrameHdl, &loc_u8CmdId);
  //Initialize listener info
  loc_sIndustrialTestCommandListenerInfo.oListenerId = 0U;
  loc_sIndustrialTestCommandListenerInfo.psInterface = &sIndustrialTestAsIndustrialTestCommandListener;
  loc_eIndTestCmdError = IndustrialTestCommand_ValidateAndFillAck(&loc_sIndustrialTestCommandListenerInfo, loc_u8CmdId, TransmitFrameHdl, ReceiveFrameHdl);
  if(INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR != loc_eIndTestCmdError)
  {
    loc_eReturnValue = INDUSTRIAL_TEST_ERROR__TEST_FRAME_ERROR;
  }
  else
  {
    loc_eIndTestCmdError = IndustrialTestCommand_Execute(&loc_sIndustrialTestCommandListenerInfo, loc_u8CmdId, TransmitFrameHdl, ReceiveFrameHdl,0, NULL);
    if(INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR != loc_eIndTestCmdError)
    {
      loc_eReturnValue = INDUSTRIAL_TEST_ERROR__TEST_FRAME_ERROR;
    }
    else
    {
      //Industrial test command succeed
    }
  }
  
  return loc_eReturnValue;
  /*## END[GUID 5a2aa6f6-0603-4ea5-aba2-529b498f3b9f] ##*/
}


/*!************************************************************************************************
* \fn         static TeIndustrialTestError IndustrialTest_ACTION_OnNewFrameReceived(void)
* \return     TeIndustrialTestError 
***************************************************************************************************/
static TeIndustrialTestError IndustrialTest_ACTION_OnNewFrameReceived(void)
{  
  /*## BEGIN[GUID 3040ca67-cf55-4b3c-b6a9-a3f72d3063ac] ##*/
  TeIndustrialTestError loc_eReturnValue      = INDUSTRIAL_TEST_ERROR__NO_ERROR;
  TU8                   loc_u8CmdId           = 0U;
  TBool                 loc_bFrameIsManaged = FALSE;
  
  TestFrame_GetFrameId(ReceiveFrameHdl, &loc_u8CmdId);
  //Initialize listener info
  loc_bFrameIsManaged = IndustrialTestCommand_CommandIsManaged(loc_u8CmdId);
  if(TRUE != loc_bFrameIsManaged)
  {
    loc_eReturnValue = INDUSTRIAL_TEST_ERROR__UNKNOWN_CMD;
  }
  else
  {
    //Industrial test command succeed
  }
  
  return loc_eReturnValue;
  /*## END[GUID 3040ca67-cf55-4b3c-b6a9-a3f72d3063ac] ##*/
}


/*!************************************************************************************************
* \fn         static TeIndustrialTestError IndustrialTest_ACTION_OnStartEmbeddedTest(void)
* \return     TeIndustrialTestError 
***************************************************************************************************/
static TeIndustrialTestError IndustrialTest_ACTION_OnStartEmbeddedTest(void)
{  
  /*## BEGIN[GUID 91abde25-36d9-481e-b1fb-e5e5624cce46] ##*/
  TeIndustrialTestError               loc_eReturValue            = INDUSTRIAL_TEST_ERROR__NO_ERROR;
  TeTestProtocolError                 loc_eTestProtocolReturValue    = TEST_PROTOCOL_ERROR__NO_ERROR;
  TeIndustrialTestCmdError            loc_eIndusTestCmdReturnVal = INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR;
  TsIndustrialTestCommandListenerInfo loc_sIndustrialTestCommandListenerInfo;
  
  loc_sIndustrialTestCommandListenerInfo.oListenerId = 0U;
  loc_sIndustrialTestCommandListenerInfo.psInterface = &sIndustrialTestAsIndustrialTestCommandListener;
  loc_eIndusTestCmdReturnVal = IndustrialTestCommand_Execute(&loc_sIndustrialTestCommandListenerInfo, ET_READY_FOR_TST, TransmitFrameHdl, ReceiveFrameHdl, 0, NULL);
  
  if(INDUSTRIAL_TEST_CMD_ERROR__NO_ERROR == loc_eIndusTestCmdReturnVal)
  {
    loc_eTestProtocolReturValue = TestProtocol_SendFrame();
    if(TEST_PROTOCOL_ERROR__NO_ERROR != loc_eTestProtocolReturValue)
    {
      loc_eReturValue = INDUSTRIAL_TEST_ERROR__TEST_PROTOCOL_ERROR;
    }
    else
    {
      // test protocol command succeed
    }
  }
  else
  {
    loc_eReturValue = INDUSTRIAL_TEST_ERROR__INDUS_CMD_ERROR;
  }
  
  return loc_eReturValue;
  /*## END[GUID 91abde25-36d9-481e-b1fb-e5e5624cce46] ##*/
}

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
