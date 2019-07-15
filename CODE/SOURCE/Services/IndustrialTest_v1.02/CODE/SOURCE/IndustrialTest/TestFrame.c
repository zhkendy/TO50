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
// File "TestFrame.c"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// External inclusions
//--------------------
#include "IndustrialTestConfig.h"
#include "SFY_Includes.h"
#include "RamPool.h"


// Internal inclusion
//------------------
#include "TestFrame.h"


// Private types
//-------------

/* me is a macro equivalent to this in C++ */
/* LDRA Rule Deactivation : 210 S Macro name is not upper case. */
/*LDRA_INSPECTED 210 S*/
#define me (&aoTestFrame[oIndexArg])

//! State structure declaration
typedef enum
{
  TESTFRAME_STATE_ROOT,
  TESTFRAME_STATE_IDLE,
  TESTFRAME_LAST_STATE = TESTFRAME_STATE_IDLE //!< Last state of TestFrame object used for state machine coherency
} TeTestFrameState;

//! Field sizes
#define TESTFRAME_STATE_SIZE  sizeof(TeTestFrameState)
#define TESTFRAME_U8_FRAME_SIZE  (sizeof(TU8)*SIZE_OF_FRAME)
#define TESTFRAME_U8_CURRENT_FRAME_INDEX_SIZE  (sizeof(TU8))
#define TESTFRAME_U8_FRAME_HEADER_SIZE  (sizeof(TU8))
#define TESTFRAME_U8_FRAME_CMDID_SIZE  (sizeof(TU8))
#define TESTFRAME_U8_FRAME_LENGTH_SIZE  (sizeof(TU8))
#define TESTFRAME_U8_CKSUM_SIZE  (sizeof(TU8))
#define TESTFRAME_B_NEW_FRAME_CAN_BE_RECEIVED_SIZE  (sizeof(TBool))

//! Object structure declaration
typedef struct
{
  //! State variable
  TeTestFrameState eState;
  TU8 u8Frame[SIZE_OF_FRAME];
  TU8 u8CurrentFrameIndex;
  TU8 u8FrameHeader;
  TU8 u8FrameCmdId;
  TU8 u8FrameLength;
  TU8 u8Cksum;
  //! Last frame finished to be managed. Frame information can be reset.
  TBool bNewFrameCanBeReceived;
} TsTestFrame;

TU8 Receive_Buffer[24];
TU8 current_index=0;



// Private functions declaration
//---------------------------------

// Function to set the State Machine State Value
static void TestFrame_PRV_SetStateForRP(ToTestFrameId oIndexArg, TeTestFrameState eStateArg);

// Function to get the State Machine State Value
static TeTestFrameState TestFrame_PRV_GetStateForRP(ToTestFrameId oIndexArg);


// Private variables
//------------------

//! Object instanciation
TsTestFrame aoTestFrame[NB_OF_TEST_FRAME];

//! DataManager information structure
CONST TsRamPoolInfo sTestFrame_PoolInfo =
{
  {
    (TSetStateCBack)TestFrame_PRV_SetStateForRP,
    (TGetStateCBack)TestFrame_PRV_GetStateForRP
  },
  NB_OF_TEST_FRAME,
  (void*)aoTestFrame,
  NULL
};

/*LDRA_HEADER_END*/

// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \fn         TeTestFrameAliveState TestFrame_AreYouAlive(ToTestFrameId oIndexArg)
* \param[in]  oIndexArg : Index concerning instance
* \return     TeTestFrameAliveState 
***************************************************************************************************/
TeTestFrameAliveState TestFrame_AreYouAlive(ToTestFrameId oIndexArg)
{  
  // Local variables
  //------------------
  TeTestFrameAliveState loc_eState = TESTFRAME_NOT_USED;
  
  // IF TestFrame Object state is ROOT state
  if(me->eState == TESTFRAME_STATE_ROOT)
  {
    // THEN TestFrame Object is NOT USED
    loc_eState = TESTFRAME_NOT_USED;
  }
  else
  {
    // ELSE IF TestFrame Object state is under or equal to maximum state
    if (me->eState <= TESTFRAME_LAST_STATE)
    {
      // THEN TestFrame Object is considered as USED
      loc_eState = TESTFRAME_USED;
    }
    else
    {
      // ELSE TestFrame Object is in a INVALID STATE
      loc_eState = TESTFRAME_INVALID_STATE;
    }
  }
  return loc_eState;
}


/*!************************************************************************************************
* \fn         ToTestFrameId TestFrame_Init(void)
* \return     ToTestFrameId 
***************************************************************************************************/
ToTestFrameId TestFrame_Init(void)
{  
  // Local variables
  //------------------
  ToTestFrameId oIndexArg;
  
  // Tell the RAMPOOL to create the instance
  oIndexArg = RamPool_CreateInstance(&sTestFrame_PoolInfo);
  if(RAMPOOL_ERROR_DETECTED(oIndexArg) == FALSE)
  {
    // Implementation
    //---------------
    {
      /*## BEGIN[GUID 894c07e3-c3cf-4515-a0c5-544d18c6c603] ##*/
        //Clean last frame
        MEMSET(me->u8Frame, 0, SIZE_OF_FRAME);
        
        //Clear current index
        me->u8CurrentFrameIndex = 0U;
      
        //Clear frame header
        me->u8FrameHeader = 0U;
        
        //Clear frame command identifier
        me->u8FrameCmdId = 0U;
        
        //Clear frame length
        me->u8FrameLength = 0U;
        
        //Clear frame check sum
        me->u8Cksum = 0U;
        
        me->bNewFrameCanBeReceived = TRUE;
      /*## END[GUID 894c07e3-c3cf-4515-a0c5-544d18c6c603] ##*/
    }
    // Changing state
    //------------------
    me->eState = TESTFRAME_STATE_IDLE;
  }
  
  // Return result
  //------------------
  return oIndexArg;
}


/*!************************************************************************************************
* \fn         void TestFrame_Cleanup(ToTestFrameId oIndexArg)
* \param[in]  oIndexArg : Index concerning instance
* \return     void 
***************************************************************************************************/
void TestFrame_Cleanup(ToTestFrameId oIndexArg)
{  
  // Implementation
  //---------------
  
  if(oIndexArg < NB_OF_TEST_FRAME)
  {
    /*## BEGIN[GUID 1fca0f10-63e0-4518-b271-beabba38a453] ##*/
    /*## END[GUID 1fca0f10-63e0-4518-b271-beabba38a453] ##*/
    // Call Rampool to destroy the instance
    RamPool_DestroyInstance(&sTestFrame_PoolInfo, oIndexArg);
  }
}


/*!************************************************************************************************
* \fn         void TestFrame_AddData(ToTestFrameId oIndexArg, TU8 u8Data)
* \param[in]  oIndexArg : Index concerning instance
* \param[in]  u8Data
* \return     void 
***************************************************************************************************/
void TestFrame_AddData(ToTestFrameId oIndexArg, TU8 u8Data)
{  
  /*## BEGIN[GUID 3a516496-4178-45de-b073-268f005615e0] ##*/
  TU8*  loc_pu8CurrentIndex = &(me->u8CurrentFrameIndex);
  TU8*  loc_pu8FrameHeader = &(me->u8FrameHeader);
  TU8*  loc_pu8FrameLength = &(me->u8FrameLength);
  TU8*  loc_pu8FrameCksum = &(me->u8Cksum);
  
  me->u8Frame[(*loc_pu8CurrentIndex)] = u8Data;
  
  if(FRAME_HEADER_POSITION == (*loc_pu8CurrentIndex))
  {
    (*loc_pu8FrameHeader) = u8Data;
    (*loc_pu8FrameLength) = 0U;
    (*loc_pu8FrameCksum) = (*loc_pu8FrameCksum) + u8Data;
  }
  else
  {    
    if(FRAME_CMD_POSITION == (*loc_pu8CurrentIndex))
    {
      me->u8FrameCmdId = u8Data;
      (*loc_pu8FrameCksum) = (*loc_pu8FrameCksum) + u8Data;
    }
    else
    {
      if((*loc_pu8FrameLength) == (0x3FU & (*loc_pu8FrameHeader)))
      {
        (*loc_pu8FrameCksum) = ~((*loc_pu8FrameCksum)) + 1;
      }
      else
      {
        (*loc_pu8FrameCksum) = (*loc_pu8FrameCksum) + u8Data;
      }
    }
  }
  
  (*loc_pu8CurrentIndex) = (*loc_pu8CurrentIndex) + 1;
  (*loc_pu8FrameLength) = (*loc_pu8FrameLength) + 1;
  /*## END[GUID 3a516496-4178-45de-b073-268f005615e0] ##*/
}


void TestFrame_GetData(TU8 u8Data)
{  
    //current_index++;
  if(current_index<20)
    {
       Receive_Buffer[current_index++]=u8Data;
    }
  else
   current_index=0; 
}


/*!************************************************************************************************
* \fn         void TestFrame_GetFrameData(ToTestFrameId oIndexArg, TU8* pu8DataLength, TU8* pu8Data)
* \param[in]  oIndexArg : Index concerning instance
* \param[in]  pu8DataLength
* \param[in]  pu8Data
* \return     void 
***************************************************************************************************/
void TestFrame_GetFrameData(ToTestFrameId oIndexArg, TU8* pu8DataLength, TU8* pu8Data)
{  
  /*## BEGIN[GUID 7d4c7221-343e-4f4f-b71f-a506998922db] ##*/
  //Check pointers are not NULL
  if((NULL != pu8DataLength)
    &&(NULL != pu8Data))
  {
    *pu8DataLength = me->u8FrameLength;
    
    MEMCPY(pu8Data, me->u8Frame, me->u8FrameLength);
  }
  else
  {
    //TODO Add an error to return
  }
  /*## END[GUID 7d4c7221-343e-4f4f-b71f-a506998922db] ##*/
}


/*!************************************************************************************************
* \fn         void TestFrame_GetFrameHeader(ToTestFrameId oIndexArg, TU8* pu8FrameHeader)
* \param[in]  oIndexArg : Index concerning instance
* \param[in]  pu8FrameHeader
* \return     void 
***************************************************************************************************/
void TestFrame_GetFrameHeader(ToTestFrameId oIndexArg, TU8* pu8FrameHeader)
{  
  /*## BEGIN[GUID 309e3106-f03d-4068-8eb3-8c60bf19ff94] ##*/
  //Check if pointer is not NULL
  if(NULL != pu8FrameHeader)
  {
    *pu8FrameHeader = me->u8FrameHeader;
  }
  else
  {
    //TODO Add an error to return
  }
  /*## END[GUID 309e3106-f03d-4068-8eb3-8c60bf19ff94] ##*/
}


/*!************************************************************************************************
* \fn         void TestFrame_GetFrameId(ToTestFrameId oIndexArg, TU8* pu8FrameId)
* \param[in]  oIndexArg : Index concerning instance
* \param[in]  pu8FrameId
* \return     void 
***************************************************************************************************/
void TestFrame_GetFrameId(ToTestFrameId oIndexArg, TU8* pu8FrameId)
{  
  /*## BEGIN[GUID eda977ca-d5d8-4b36-ab01-cf5c4ecfa962] ##*/
  //Check if pointer is not NULL
  if(NULL != pu8FrameId)
  {
  *pu8FrameId = me->u8FrameCmdId;
  }
  else
  {
    //TODO Add an error to return
  }
  /*## END[GUID eda977ca-d5d8-4b36-ab01-cf5c4ecfa962] ##*/
}


/*!************************************************************************************************
* \fn         void TestFrame_GetFrameLength(ToTestFrameId oIndexArg, TU8* pu8FrameLength)
* \param[in]  oIndexArg : Index concerning instance
* \param[in]  pu8FrameLength
* \return     void 
***************************************************************************************************/
void TestFrame_GetFrameLength(ToTestFrameId oIndexArg, TU8* pu8FrameLength)
{  
  /*## BEGIN[GUID f1116448-a7fe-43c7-8c4b-d2bd5dd188e5] ##*/
    //Check if pointer is not NULL
  if(NULL != pu8FrameLength)
  {
    *pu8FrameLength = me->u8FrameLength;
  }
  else
  {
    //TODO Add an error to return
  }
  /*## END[GUID f1116448-a7fe-43c7-8c4b-d2bd5dd188e5] ##*/
}


/*!************************************************************************************************
* \fn         TeTestFrameError TestFrame_UpdateTestFrame(ToTestFrameId oIndexArg, TU8 u8CmdId, TU8 u8CmdDataSize, TU8* pu8CmdData, TBool bMultiFrame, TBool bAckNeeded)
* \param[in]  oIndexArg : Index concerning instance
* \param[in]  u8CmdId
* \param[in]  u8CmdDataSize
* \param[in]  pu8CmdData
* \param[in]  bMultiFrame
* \param[in]  bAckNeeded
* \return     TeTestFrameError 
***************************************************************************************************/
TeTestFrameError TestFrame_UpdateTestFrame(ToTestFrameId oIndexArg, TU8 u8CmdId, TU8 u8CmdDataSize, TU8* pu8CmdData, TBool bMultiFrame, TBool bAckNeeded)
{  
  /*## BEGIN[GUID 03ff427b-37c3-468f-8f49-59758b909130] ##*/
  TeTestFrameError  loc_eReturnValue = TEST_FRAME_ERROR__NO_ERROR;
  TU8               loc_u8FrameHeader = 0U;
  TU8               loc_u8FrameIndex = 0U;
  TU8               loc_u8Cksum = 0U;
  
  if(SIZE_OF_FRAME > u8CmdDataSize)
  {
    //Clean last frame
    MEMSET(me->u8Frame, 0, SIZE_OF_FRAME);
  
    //Build frame header
    //Frame length
    //Length never includes the header, but always the checksum.
    //Length mini = 3 for HEADER + CMD + CHK, as for a message of type START or END
    //Length maxi = 63 for HEADER + CMD + 61 DATA + CHK
    me->u8FrameLength = HEADER_SIZE + CMD_SIZE + u8CmdDataSize + CHK_SIZE;
    loc_u8FrameHeader = me->u8FrameLength - 1;
  
    //|Bit Multi frame|NO ACK|Frame Length 5|Frame Length 4|Frame Length 3|Frame Length 2|Frame Length 1|Frame Length 0|
    //Check if multi frame answer is needed
    //bit 7 is reset (=0) indicate the answer message is contained in a unique frame.
    //bit 7 is set (=1), indicates the answer message needs to be contained in several frames.
    if(TRUE == bMultiFrame)
    {
      loc_u8FrameHeader = loc_u8FrameHeader | MULTIFRAME_NEEDED;
    }
  
    //bit 6 is set (=1), indicates the sender of the command does NOT request a ACK message
    //bit 6 is reset (=0), indicates the sender of the command REQUEST a ACK message
    if(FALSE == bAckNeeded)
    {
      loc_u8FrameHeader = loc_u8FrameHeader | NO_ACK_NEEDED;
    }
  
    //Build frame and calculate checksum
    //Set header at header frame place
    me->u8Frame[FRAME_HEADER_POSITION] = loc_u8FrameHeader;
    loc_u8Cksum = loc_u8Cksum + loc_u8FrameHeader;
  
    //Set comman id at command id frame place
    me->u8Frame[FRAME_CMD_POSITION] = u8CmdId;
    loc_u8Cksum = loc_u8Cksum + u8CmdId;
  
    //Set data in frame
    for(loc_u8FrameIndex = FRAME_DATA_POSITION; loc_u8FrameIndex<(u8CmdDataSize+FRAME_DATA_POSITION); loc_u8FrameIndex++)
    {
      me->u8Frame[loc_u8FrameIndex] = pu8CmdData[loc_u8FrameIndex - FRAME_DATA_POSITION];
      loc_u8Cksum = loc_u8Cksum + me->u8Frame[loc_u8FrameIndex];
    }
  
    //Calculate checksum
    //Checksum = 2’s complement sum of all bytes
    //for emitter CHK = NOT(header+cmd+param) +1
    loc_u8Cksum = ~(loc_u8Cksum) + 1;
    me->u8Frame[loc_u8FrameIndex] = loc_u8Cksum;
  }
  else
  {
    loc_eReturnValue = TEST_FRAME_ERROR__BAD_FRAME_DATA_SIZE;
  }
  
  return loc_eReturnValue;
  /*## END[GUID 03ff427b-37c3-468f-8f49-59758b909130] ##*/
}



/*!************************************************************************************************
* \fn         TBool TestFrame_CksumIsValid(ToTestFrameId oIndexArg)
* \param[in]  oIndexArg : Index concerning instance
* \return     TBool 
***************************************************************************************************/
TBool TestFrame_CksumIsValid(ToTestFrameId oIndexArg)
{  
  /*## BEGIN[GUID 13b6be7f-1d68-4cde-a8e2-a69b8aaf0ac2] ##*/
    TBool loc_bCksumIsValid = FALSE;
  
    //Check CKSUM
    if(me->u8Cksum == me->u8Frame[me->u8FrameLength - 1])
    {
      loc_bCksumIsValid = TRUE;
    }
  
    return loc_bCksumIsValid;
  /*## END[GUID 13b6be7f-1d68-4cde-a8e2-a69b8aaf0ac2] ##*/
}


/*!************************************************************************************************
* \fn         void TestFrame_CompleteFrameReceived(ToTestFrameId oIndexArg)
* \param[in]  oIndexArg : Index concerning instance
* \return     void 
***************************************************************************************************/
void TestFrame_CompleteFrameReceived(ToTestFrameId oIndexArg)
{  
  /*## BEGIN[GUID b91304dd-aec6-4b57-834d-30a7b712c895] ##*/
  me->u8CurrentFrameIndex = 0U;
  /*## END[GUID b91304dd-aec6-4b57-834d-30a7b712c895] ##*/
}


/*!************************************************************************************************
* \fn         void TestFrame_ClearFrame(ToTestFrameId oIndexArg)
* \param[in]  oIndexArg : Index concerning instance
* \return     void 
***************************************************************************************************/
void TestFrame_ClearFrame(ToTestFrameId oIndexArg)
{  
  /*## BEGIN[GUID b858cfc1-3cf5-41a8-835e-8e6be3ad054f] ##*/
  //Clean last frame
  MEMSET(me->u8Frame, 0, SIZE_OF_FRAME);
  
  //Clear current index
  me->u8CurrentFrameIndex = 0U;
  
  //Clear frame header
  me->u8FrameHeader = 0U;
  
  //Clear frame command identifier
  me->u8FrameCmdId = 0U;
  
  //Clear frame length
  me->u8FrameLength = 0U;
  
  //Clear frame check sum
  me->u8Cksum = 0U;
  /*## END[GUID b858cfc1-3cf5-41a8-835e-8e6be3ad054f] ##*/
}


/*!************************************************************************************************
* \fn         void TestFrame_FrameIsLock(ToTestFrameId oIndexArg)
* \param[in]  oIndexArg : Index concerning instance
* \return     void 
***************************************************************************************************/
void TestFrame_FrameIsLock(ToTestFrameId oIndexArg)
{  
  /*## BEGIN[GUID 5799c06c-ee67-4122-8385-261074487f83] ##*/
  me->bNewFrameCanBeReceived = FALSE;
  /*## END[GUID 5799c06c-ee67-4122-8385-261074487f83] ##*/
}


/*!************************************************************************************************
* \fn         void TestFrame_FrameIsUnlock(ToTestFrameId oIndexArg)
* \param[in]  oIndexArg : Index concerning instance
* \return     void 
***************************************************************************************************/
void TestFrame_FrameIsUnlock(ToTestFrameId oIndexArg)
{  
  /*## BEGIN[GUID ef0549b4-266c-4b21-9b0a-e8a29ff42979] ##*/
  me->bNewFrameCanBeReceived = TRUE;
  /*## END[GUID ef0549b4-266c-4b21-9b0a-e8a29ff42979] ##*/
}


/*!************************************************************************************************
* \brief      Fonction d'initialisation du pool d'objets
* \fn         void TestFrame_InitClass(void)
* \return     void 
***************************************************************************************************/
void TestFrame_InitClass(void)
{  
  // Call RamPool to initialise TestFrame objects pool
  RamPool_Init(&sTestFrame_PoolInfo);
}


/*!************************************************************************************************
* \brief      Fonction de destruction du pool d'objets
* \fn         void TestFrame_DestroyClass(void)
* \return     void 
***************************************************************************************************/
void TestFrame_DestroyClass(void)
{  
  // Call RamPool to initialise TestFrame objects pool
  RamPool_Init(&sTestFrame_PoolInfo);
}


// Private functions implementation
//------------------------------------


/*!************************************************************************************************
* \brief      Function to set the State Machine State Value
* \fn         static void TestFrame_PRV_SetStateForRP(ToTestFrameId oIndexArg, TeTestFrameState eStateArg)
* \param[in]  oIndexArg
* \param[in]  eStateArg
* \return     void 
***************************************************************************************************/
static void TestFrame_PRV_SetStateForRP(ToTestFrameId oIndexArg, TeTestFrameState eStateArg)
{  
  // Set TestFrame object statechart state
  me->eState = eStateArg;
}


/*!************************************************************************************************
* \brief      Function to get the State Machine State Value
* \fn         static TeTestFrameState TestFrame_PRV_GetStateForRP(ToTestFrameId oIndexArg)
* \param[in]  oIndexArg
* \return     TeTestFrameState 
***************************************************************************************************/
static TeTestFrameState TestFrame_PRV_GetStateForRP(ToTestFrameId oIndexArg)
{  
  // Local variables
  //------------------
  TeTestFrameState loc_eRetValue;
  
  // Init local variable with current TestFrame object statechart state
  loc_eRetValue = me->eState;
  
  // Return the value
  return loc_eRetValue;
}

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
