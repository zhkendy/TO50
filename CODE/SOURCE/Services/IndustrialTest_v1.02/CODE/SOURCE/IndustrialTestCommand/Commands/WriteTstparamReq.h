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
// File "WriteTstparamReq.h"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef __WRITETSTPARAMREQ_H__
#define __WRITETSTPARAMREQ_H__


// External inclusions
//--------------------
#include "TestFrame.h"
#include "IndustrialTestCommandListener.h"
#include "IndustrialTestCommand.h"


// Public types
//--------------

#define ET_WRITE_TSTPARAM_REQ_LENGTH        0x16
#define ET_WRITE_TSTPARAM_REQ_DATA_LENGTH   0x03
#define ET_WRITE_TSTPARAM_DATA_POSITION     0x02
#define SIZE_MASK                           0x7F

#define WRITE_TSTPARAM_ANS_DATA_SIZE		0x01
#define NB_BYTE_POSITION					0x03



// Public functions declaration
//-----------------------------------

TeIndustrialTestCmdError WriteTstparamReq_Execute(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg, TU8 _u8DataSizeArg, TU8* _pu8DataArg);

TeIndustrialTestCmdError WriteTstparamReq_ValidateAndFillAck(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg);


// Header end
//--------------
#endif // __WRITETSTPARAMREQ_H__

/**********************************************************************
Copyright � (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
