// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


//---------------------------------------------------------------------
// File "CRC.c"   Generated with SOMFY code generator v[3.10.0.0]
// 16 bits CRC computing
// Features summary
// CRC 16bits, polynomial= 0x1021, initial value= 0x0000, inversion IN and OUT algorithm, no XOR at the end. Calculation method= "fragmented message", verification method= "direct method". Then for optimization issue, we have decided to invert algo instead of in and out values. That means polynomial= 0x8408 and shift is right shift.
//
// Further, we have 2 available algorithms depending on issue on flash size. A classic algorithm (size efficient) and a fast algo based on a 512 bytes table (speed efficient).
// In DOC directory is an abstract of io-homecontrol CRC specification.
// Below is a test pattern :F8 00 00 01 80 16 04 AE 00 01 67 00 00 80 00 00 00 25 2A 6E DA 00 58 30 0D  so far CRC = 0C19 (MSB / LSB)
// Regarding speed measurements, we have run on GT60 2 tests per algo. Classic algo, first test is 1 block of 509 bytes = 20ms. Second test is 509 writes of 1 bytes = 42ms. Table algo, first test is 1 block of 509 bytes = 10ms. Second test is 509 writes of 1 bytes = 32ms.
//---------------------------------------------------------------------


// External inclusions
//--------------------



// Internal inclusion
//------------------
#include "CRC.h"


// Private types
//-------------

#define CRC_INIT_VALUE 0x0000

//! To use fast CRC computation : faster but use needs 512 byte of flash memory
#ifdef  __IAR_SYSTEMS_ICC__
	#define FAST_CRC
#else
	//#define FAST_CRC
#endif


#ifdef FAST_CRC
	// Table for CRC table driven computation : table contains pre-computed values for poly = 0x8408
	static const TU16 abtCrcLookupTab[] = {
	                                   0x0000,0x1189,0x2312,0x329B,0x4624,0x57AD,0x6536,0x74BF,
	                                   0x8C48,0x9DC1,0xAF5A,0xBED3,0xCA6C,0xDBE5,0xE97E,0xF8F7,
	                                   0x1081,0x0108,0x3393,0x221A,0x56A5,0x472C,0x75B7,0x643E,
	                                   0x9CC9,0x8D40,0xBFDB,0xAE52,0xDAED,0xCB64,0xF9FF,0xE876,
	                                   0x2102,0x308B,0x0210,0x1399,0x6726,0x76AF,0x4434,0x55BD,
	                                   0xAD4A,0xBCC3,0x8E58,0x9FD1,0xEB6E,0xFAE7,0xC87C,0xD9F5,
	                                   0x3183,0x200A,0x1291,0x0318,0x77A7,0x662E,0x54B5,0x453C,
	                                   0xBDCB,0xAC42,0x9ED9,0x8F50,0xFBEF,0xEA66,0xD8FD,0xC974,
	                                   0x4204,0x538D,0x6116,0x709F,0x0420,0x15A9,0x2732,0x36BB,
	                                   0xCE4C,0xDFC5,0xED5E,0xFCD7,0x8868,0x99E1,0xAB7A,0xBAF3,
	                                   0x5285,0x430C,0x7197,0x601E,0x14A1,0x0528,0x37B3,0x263A,
	                                   0xDECD,0xCF44,0xFDDF,0xEC56,0x98E9,0x8960,0xBBFB,0xAA72,
	                                   0x6306,0x728F,0x4014,0x519D,0x2522,0x34AB,0x0630,0x17B9,
	                                   0xEF4E,0xFEC7,0xCC5C,0xDDD5,0xA96A,0xB8E3,0x8A78,0x9BF1,
	                                   0x7387,0x620E,0x5095,0x411C,0x35A3,0x242A,0x16B1,0x0738,
	                                   0xFFCF,0xEE46,0xDCDD,0xCD54,0xB9EB,0xA862,0x9AF9,0x8B70,
	                                   0x8408,0x9581,0xA71A,0xB693,0xC22C,0xD3A5,0xE13E,0xF0B7,
	                                   0x0840,0x19C9,0x2B52,0x3ADB,0x4E64,0x5FED,0x6D76,0x7CFF,
	                                   0x9489,0x8500,0xB79B,0xA612,0xD2AD,0xC324,0xF1BF,0xE036,
	                                   0x18C1,0x0948,0x3BD3,0x2A5A,0x5EE5,0x4F6C,0x7DF7,0x6C7E,
	                                   0xA50A,0xB483,0x8618,0x9791,0xE32E,0xF2A7,0xC03C,0xD1B5,
	                                   0x2942,0x38CB,0x0A50,0x1BD9,0x6F66,0x7EEF,0x4C74,0x5DFD,
	                                   0xB58B,0xA402,0x9699,0x8710,0xF3AF,0xE226,0xD0BD,0xC134,
	                                   0x39C3,0x284A,0x1AD1,0x0B58,0x7FE7,0x6E6E,0x5CF5,0x4D7C,
	                                   0xC60C,0xD785,0xE51E,0xF497,0x8028,0x91A1,0xA33A,0xB2B3,
	                                   0x4A44,0x5BCD,0x6956,0x78DF,0x0C60,0x1DE9,0x2F72,0x3EFB,
	                                   0xD68D,0xC704,0xF59F,0xE416,0x90A9,0x8120,0xB3BB,0xA232,
	                                   0x5AC5,0x4B4C,0x79D7,0x685E,0x1CE1,0x0D68,0x3FF3,0x2E7A,
	                                   0xE70E,0xF687,0xC41C,0xD595,0xA12A,0xB0A3,0x8238,0x93B1,
	                                   0x6B46,0x7ACF,0x4854,0x59DD,0x2D62,0x3CEB,0x0E70,0x1FF9,
	                                   0xF78F,0xE606,0xD49D,0xC514,0xB1AB,0xA022,0x92B9,0x8330,
	                                   0x7BC7,0x6A4E,0x58D5,0x495C,0x3DE3,0x2C6A,0x1EF1,0x0F78
	                                 };
#else
	#define CRC_POLY               0x8408
#endif


// Private functions declaration
//---------------------------------

static void CRC_PRV_Compute(TCRC * me, TU8 btValueArg);


// Public functions implementation
//--------------------------------------


/*!************************************************************************************************
* \fn         void CRC_Init(TCRC * me)
* \param[in]  me : Pointer on concerned instance
* \return     void
***************************************************************************************************/
void CRC_Init(TCRC * me)
{
  // Implementation
  //---------------
  {
    /*## BEGIN[GUID d23da847-7050-4d5b-ab0b-7e6e24a20f9a] ##*/
    *me = CRC_INIT_VALUE;
    /*## END[GUID d23da847-7050-4d5b-ab0b-7e6e24a20f9a] ##*/
  }
}


/*!************************************************************************************************
* \fn         void CRC_ComputeArray(TCRC * me, TU8 abtValueArg[], TU16 wSizeArg)
* \param[in]  me : Pointer on concerned instance
* \param[in]  abtValueArg[]
* \param[in]  wSizeArg
* \return     void
***************************************************************************************************/
void CRC_ComputeArray(TCRC * me, TU8 abtValueArg[], TU16 wSizeArg)
{
  {
    /*## BEGIN[GUID e01101fb-2577-4bda-b669-ef47f6699f87] ##*/
    // Local variables
    TU16 wDataIdx = 0U;

    do
    {
    	CRC_PRV_Compute( me, abtValueArg[wDataIdx] );
    }
    while(++wDataIdx < wSizeArg);
    /*## END[GUID e01101fb-2577-4bda-b669-ef47f6699f87] ##*/
  }
}


/*!************************************************************************************************
* \fn         void CRC_ComputeByte(TCRC * me, TU8 btValueArg)
* \param[in]  me : Pointer on concerned instance
* \param[in]  btValueArg
* \return     void
***************************************************************************************************/
void CRC_ComputeByte(TCRC * me, TU8 btValueArg)
{
  {
    /*## BEGIN[GUID 0476c619-147f-45fd-a0ef-c860876ec004] ##*/
    CRC_PRV_Compute( me, btValueArg );
    /*## END[GUID 0476c619-147f-45fd-a0ef-c860876ec004] ##*/
  }
}


/*!************************************************************************************************
* \fn         void CRC_ComputeFixedValue(TCRC * me, TU8 btValueArg, TU16 wSizeArg)
* \param[in]  me : Pointer on concerned instance
* \param[in]  btValueArg
* \param[in]  wSizeArg
* \return     void
***************************************************************************************************/
void CRC_ComputeFixedValue(TCRC * me, TU8 btValueArg, TU16 wSizeArg)
{
  {
    /*## BEGIN[GUID 15ff94a5-be81-4738-85d8-b725ff74079c] ##*/
    // Local variables
    TU16 wDataIdx = 0U;

    do
    {
    	CRC_PRV_Compute( me, btValueArg );
    }
    while(++wDataIdx < wSizeArg);
    /*## END[GUID 15ff94a5-be81-4738-85d8-b725ff74079c] ##*/
  }
}


// Private functions implementation
//------------------------------------


/*!************************************************************************************************
* \fn         static void CRC_PRV_Compute(TCRC * me, TU8 btValueArg)
* \param[in]  me : Pointer on concerned instance
* \param[in]  btValueArg
* \return     void
***************************************************************************************************/
static void CRC_PRV_Compute(TCRC * me, TU8 btValueArg)
{
  {
    /*## BEGIN[GUID 2fdf00cb-5552-486d-adaf-3ba2c9a99997] ##*/
    #ifdef FAST_CRC
    	*me = (*me >> 8) ^ abtCrcLookupTab[(TU8)*me ^ btValueArg];
    #else
    	TU8 btIdx = 0U;

    	*me ^= (TU16)btValueArg;

    	do
    	{
    		if (*me & 0x0001)
    		{
    			*me = (*me >> 1) ^ CRC_POLY;
    		}
    		else
    		{
    			*me >>= 1;
    		}
    	}
    	while(++btIdx < 8);
    #endif
    /*## END[GUID 2fdf00cb-5552-486d-adaf-3ba2c9a99997] ##*/
  }
}

/**********************************************************************
Copyright © (2009), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
