// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


//---------------------------------------------------------------------
// File "CRC.h"   Generated with SOMFY code generator v[3.10.0.0]
// 16 bits CRC computing
// Features summary 
// CRC 16bits, polynomial= 0x1021, initial value= 0x0000, inversion IN and OUT algorithm, no XOR at the end. Calculation method= "fragmented message", verification method= "direct method". Then for optimization issue, we have decided to invert algo instead of in and out values. That means polynomial= 0x8408 and shift is right shift.
// 
// Further, we have 2 available algorithms depending on issue on flash size. A classic algorithm (size efficient) and a fast algo based on a 512 bytes table (speed efficient).
// In DOC directory is an abstract of io-homecontrol CRC specification.
// Below is a test pattern :F8 00 00 01 80 16 04 AE 00 01 67 00 00 80 00 00 00 25 2A 6E DA 00 58 30 0D  so far CRC = 0C19 (MSB / LSB)
// Regarding speed measurements, we have run on GT60 2 tests per algo. Classic algo, first test is 1 block of 509 bytes = 20ms. Second test is 509 writes of 1 bytes = 42ms. Table algo, first test is 1 block of 509 bytes = 10ms. Second test is 509 writes of 1 bytes = 32ms.
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef CRC_H
#define CRC_H
#include "SFY_Types.h"

// Public types
//--------------

typedef TU16 TCRC;

#define CRC_SIZE 2


// Public functions declaration
//-----------------------------------

void CRC_Init(TCRC * me);

void CRC_ComputeArray(TCRC * me, TU8 abtValueArg[], TU16 wSizeArg);

void CRC_ComputeByte(TCRC * me, TU8 btValueArg);

void CRC_ComputeFixedValue(TCRC * me, TU8 btValueArg, TU16 wSizeArg);


// Header end
//--------------
#endif // CRC_H

/**********************************************************************
Copyright © (2009), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
