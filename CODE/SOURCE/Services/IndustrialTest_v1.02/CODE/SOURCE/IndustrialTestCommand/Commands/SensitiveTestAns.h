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
// File "SensitiveTestAns.h"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef __SENSITIVETESTANS_H__
#define __SENSITIVETESTANS_H__


// External inclusions
//--------------------
#include "TestFrame.h"
#include "IndustrialTestCommandListener.h"
#include "IndustrialTestCommand.h"


// Public types
//--------------

#define ET_SENSITIVE_TEST_ANS_LENGTH		  0x40
#define ET_SENSITIVE_TEST_ANS_DATA_LENGTH	0x3D


// Public functions declaration
//-----------------------------------

TeIndustrialTestCmdError SensitiveTestAns_Execute(TsIndustrialTestCommandListenerInfo* _poListenerArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg, TU8 _u8DataSizeArg, TU8* _pu8DataArg);

TeIndustrialTestCmdError SensitiveTestAns_ValidateAndFillAck(TsIndustrialTestCommandListenerInfo* _poListenerArgArg, ToTestFrameId _oTransmitFrameArg, ToTestFrameId _oReceiveFrameArg);


// Header end
//--------------
#endif // __SENSITIVETESTANS_H__

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
