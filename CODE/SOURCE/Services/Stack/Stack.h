// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once
#include "SFY_Types.h"

typedef struct Stack_s* StackObjRef;

typedef enum StaticAllocatedStack_e
{
  //add yours here:
  UART_WriteStack,
  UART_ReadStack,
  MoveReqStack,
  MoveStack,
  SegmentStack,
  MoveStackTmp,
  SegmentStackTmp,
  Last_StaticAllocatedStack,
}StaticAllocatedStack_e;

typedef struct Stack_s
{
  TU8* pt_Buffer; 
  TU16 HeadRoomID;
  TU16 TailRoomID;
  TU16 StackDepht;
  TU16 ObjectCount; 
  TU16 FreeRoomleft; 
  TU16 Objectsize;
}Stack_s;


 StackObjRef Stack_Create_StaticAllocation(TU16 ObjectSize, TU16 StackDepth, TU8* Buffer,StaticAllocatedStack_e Mystack);


TBool Stack_PushTail(StackObjRef Stack,const void* Object);
TBool Stack_PushHead(StackObjRef Stack,const void* Object);
TBool Stack_PopHead(StackObjRef Stack,void* Object);
TBool Stack_PopTail(StackObjRef Stack,void* Object);
void  Stack_Purge(StackObjRef Stack);
TBool Stack_ForcedPushTail(StackObjRef Stack,const void* Object);
TBool Stack_ForcedPushHead(StackObjRef Stack,const void* Object);

TBool Stack_ReadObject(StackObjRef Stack,void* Object,TU16 RelativeToHeadObjectID);

TU16  Stack_GetObjectCount(StackObjRef Stack);
TU16  Stack_GetFreeRoomLeft(StackObjRef Stack);
TU16  Stack_GetObjectSize(StackObjRef Stack);

void  Stack_DBG_GetHeadAndTailRoomID(StackObjRef Stack,TU8 *Head,TU8 *Tail);


// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************