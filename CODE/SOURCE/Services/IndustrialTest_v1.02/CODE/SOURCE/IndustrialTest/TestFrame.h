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
// File "TestFrame.h"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef __TESTFRAME_H__
#define __TESTFRAME_H__



//! Typedef for external associations
typedef TIndex ToTestFrameId;

// Public types
//--------------

typedef enum
{
  TEST_FRAME_ERROR__NO_ERROR,
  TEST_FRAME_ERROR__BAD_FRAME_DATA_SIZE,
  TEST_FRAME_ERROR__BAD_CKSUM,
  TEST_FRAME_ERROR__BAD_CMD_PARAMETER,
  TEST_FRAME_ERROR__NULL_POINTER_PARAMETER
} TeTestFrameError;

typedef enum
{
  RECEIVE_FRAME = 1,
  TRANSMIT_FRAME
}TeFrameDirection;

#define LENGTH_HEADER_MASK		0x3FU
#define FRAME_HEADER_POSITION	0U
#define FRAME_CMD_POSITION		1U
#define FRAME_DATA_POSITION		2U
#define SIZE_OF_FRAME			64U

#define HEADER_SIZE			1U
#define CMD_SIZE			1U
#define CHK_SIZE			1U
#define MULTIFRAME_NEEDED	0x80U
#define NO_ACK_NEEDED		0x40U


typedef enum
{
  TESTFRAME_NOT_USED,
  TESTFRAME_USED,
  TESTFRAME_INVALID_STATE
} TeTestFrameAliveState;


// Public functions declaration
//-----------------------------------

TeTestFrameAliveState TestFrame_AreYouAlive(ToTestFrameId oIndexArg);

ToTestFrameId TestFrame_Init(void);

void TestFrame_Cleanup(ToTestFrameId oIndexArg);

void TestFrame_AddData(ToTestFrameId oIndexArg, TU8 u8Data);
void TestFrame_GetData(TU8 u8Data);

void TestFrame_GetFrameData(ToTestFrameId oIndexArg, TU8* pu8DataLength, TU8* pu8Data);

void TestFrame_GetFrameHeader(ToTestFrameId oIndexArg, TU8* pu8FrameHeader);

void TestFrame_GetFrameId(ToTestFrameId oIndexArg, TU8* pu8FrameId);

void TestFrame_GetFrameLength(ToTestFrameId oIndexArg, TU8* pu8FrameLength);

TeTestFrameError TestFrame_UpdateTestFrame(ToTestFrameId oIndexArg, TU8 u8CmdId, TU8 u8CmdDataSize, TU8* pu8CmdData, TBool bMultiFrame, TBool bAckNeeded);

TBool TestFrame_CksumIsValid(ToTestFrameId oIndexArg);

void TestFrame_CompleteFrameReceived(ToTestFrameId oIndexArg);

void TestFrame_ClearFrame(ToTestFrameId oIndexArg);

void TestFrame_FrameIsLock(ToTestFrameId oIndexArg);

void TestFrame_FrameIsUnlock(ToTestFrameId oIndexArg);

// Fonction d'initialisation du pool d'objets
void TestFrame_InitClass(void);

// Fonction de destruction du pool d'objets
void TestFrame_DestroyClass(void);


// Header end
//--------------
#endif // __TESTFRAME_H__

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
