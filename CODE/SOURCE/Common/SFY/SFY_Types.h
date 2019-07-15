// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#pragma once
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
//---------------------------------------------------------------------
// File "SFY_Types.h"   Writed By MG 04/2015
//---------------------------------------------------------------------

//If Io_Types are already defined, SFY_Types shall not be defined because it will redefine the same types: to be compliant with ioLib 2.4GHz
#ifndef __IAR_SYSTEMS_ICC__
#error "File writed for IAR : check it before use"
#endif

typedef signed char     TS8;
typedef signed short    TS16;
typedef signed long     TS32;
typedef unsigned char   TU8;
typedef unsigned short  TU16;
typedef unsigned long   TU32;
typedef float           TF32;

typedef bool TBool;
#define TRUE  true
#define FALSE false

typedef TU8	TByte;  /* TByte var is a 8 bits var */
#define TBYTE_MAX   255
                

typedef TU16 TIndex;
#define OBJECT_ASSOCIATION_EMPTY  (0xFFFFU)
#define OBJECT_ID_ERROR(objectId) (objectId > 0xFFFCU)

#define CONST const
#define CONST_ARG const
#define CONST_AREA


/**********************************************************************
Copyright © (2010), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
