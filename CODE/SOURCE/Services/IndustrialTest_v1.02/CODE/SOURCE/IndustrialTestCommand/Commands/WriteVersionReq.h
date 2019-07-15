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
// File "WriteVersionReq.h"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef __WRITEVERSIONREQ_H__
#define __WRITEVERSIONREQ_H__


// External inclusions
//--------------------
#include "TestFrame.h"
#include "IndustrialTestCommandListener.h"
#include "IndustrialTestCommand.h"


// Public types
//--------------

#define ET_WRITE_VERSION_REQ_LENGTH        0x13
#define ET_WRITE_VERSION_REQ_DATA_LENGTH   0x11
#define ET_WRITE_VERSION_ID_POSITION       0x02
#define ET_WRITE_VERSION_ADDRESS_AND_SIZE  0x03
#define ET_WRITE_VERSION_GIVEN_ADDRESS     0x04
#define ET_WRITE_VERSION_VERSION_POSITION  0x07
#define SIZE_MASK                          0x7F



// Public functions declaration
//-----------------------------------

TeIndustrialTestCmdError WriteVersionReq_Execute(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg, TU8 _u8DataSizeArg, TU8* _pu8DataArg);

TeIndustrialTestCmdError WriteVersionReq_ValidateAndFillAck(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg);


// Header end
//--------------
#endif // __WRITEVERSIONREQ_H__

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
