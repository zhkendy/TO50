// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#include <stdlib.h>
#include <string.h>
#include "SFY_Types.h"

#include "Stack.h"

#define TAIL_DIRECTION (1)
#define HEAD_DIRECTION (-1)




Stack_s StaticAllocatedStackArray[Last_StaticAllocatedStack];

static TU16 ComputeRoomID_RollingBuffer(TU16 Stack_Depht,TU16 RoomID, TS16 Shift);
static TU8* GetRoomAddress(StackObjRef Stack,TU16 RoomID);




StackObjRef Stack_Create_StaticAllocation(TU16 ObjectSize, TU16 StackDepth, TU8* Buffer,StaticAllocatedStack_e Mystack)
{
// LDRA want only one return by function... and doesn't want a pointer in return... so...here is my workaroud... ugly...
  TBool ValidStack = FALSE;
  if(Mystack < Last_StaticAllocatedStack)
  {
    //Initialise Object
    ValidStack = TRUE;
    StaticAllocatedStackArray[Mystack].StackDepht = StackDepth;
    StaticAllocatedStackArray[Mystack].Objectsize = ObjectSize;
    StaticAllocatedStackArray[Mystack].pt_Buffer = Buffer;
    StaticAllocatedStackArray[Mystack].HeadRoomID = 0U;
    StaticAllocatedStackArray[Mystack].TailRoomID = 0U;
    StaticAllocatedStackArray[Mystack].ObjectCount  = 0U;
    StaticAllocatedStackArray[Mystack].FreeRoomleft = StackDepth;
  } 
  return (ValidStack == TRUE) ? (&StaticAllocatedStackArray[Mystack]) : ( (StackObjRef) NULL);
}



static TU8* GetRoomAddress(StackObjRef Stack,TU16 RoomID)
{
  return (RoomID < Stack->StackDepht) ? (Stack->pt_Buffer + (RoomID * Stack->Objectsize) ) : ( (TU8*) NULL) ;
}


static TU16 ComputeRoomID_RollingBuffer(TU16 Stack_Depht,TU16 RoomID, TS16 Shift)
{
  TS32 tmp = RoomID + Shift;
  TU16 rtn = (  (tmp < 0 ? (Stack_Depht + tmp) : (tmp))  % Stack_Depht );
  return rtn;
}


TBool Stack_PushTail(StackObjRef Stack,const void* Object)
{
  TBool Rtn = FALSE;
  if(Stack != NULL && Stack->FreeRoomleft > 0u)
  {
    //Tail point to the tail object, if any; So, on first object insertion, tail ID musn't move.
    TS16 ShiftTail = (Stack->ObjectCount == 0u) ? (0u):(TAIL_DIRECTION);
    //Compute next free roomID.
    TU16 FreeRoomID = ComputeRoomID_RollingBuffer(Stack->StackDepht,Stack->TailRoomID,ShiftTail);
    //Get the Room start address
    TU8* RoomPtr = GetRoomAddress(Stack,FreeRoomID);
    if(RoomPtr != NULL)
    {
      //Store Object
      memcpy(RoomPtr,Object,Stack->Objectsize);
      Stack->TailRoomID = FreeRoomID;
      Stack->ObjectCount++;
      Stack->FreeRoomleft--;
      Rtn = TRUE;
    }
  }
  return Rtn;
}

TBool Stack_PushHead(StackObjRef Stack,const void* Object)
{
    TBool Rtn = FALSE;
  if(Stack != NULL && Stack->FreeRoomleft > 0u)
  {
    //Tail point to the Head object, if any
    TS16 ShiftHead = (Stack->ObjectCount == 0u) ? (0u):(HEAD_DIRECTION);
    //Compute the new Tail roomID.
    TU16 FreeRoomID  = ComputeRoomID_RollingBuffer(Stack->StackDepht,Stack->HeadRoomID,ShiftHead);
    //Get the Room start address
    TU8* RoomPtr = GetRoomAddress(Stack,FreeRoomID);
    if(RoomPtr != NULL)
    {
      //Store Object
      memcpy(RoomPtr,Object,Stack->Objectsize);
      Stack->HeadRoomID = FreeRoomID;
      Stack->ObjectCount++;
      Stack->FreeRoomleft--;
      Rtn= TRUE;
    }
  }
  return Rtn;
}

TBool Stack_PopHead(StackObjRef Stack,void* Object)
{
    TBool Rtn = FALSE;
  if(Stack != NULL && Stack->ObjectCount > 0u)
  {
    //Get Room ptr
    TU8* RoomPtr = GetRoomAddress(Stack,Stack->HeadRoomID);
    if(RoomPtr != NULL)
    {
      //Cpy object
      memcpy(Object,RoomPtr,Stack->Objectsize);
      //Is shift tail needed ?
      TS16 ShiftHead = (Stack->ObjectCount == 1u) ? (0u):(TAIL_DIRECTION);
      //Compute new Tail value.
      Stack->HeadRoomID = ComputeRoomID_RollingBuffer(Stack->StackDepht,Stack->HeadRoomID,ShiftHead);
      Stack->ObjectCount--;
      Stack->FreeRoomleft++;
      Rtn= TRUE;
    }
  }
  return Rtn;
}


void  Stack_DBG_GetHeadAndTailRoomID(StackObjRef Stack,TU8 *Head,TU8 *Tail)
{
  *Head = Stack->HeadRoomID;
  *Tail = Stack->TailRoomID;
}

TBool Stack_PopTail(StackObjRef Stack,void* Object)
{
    TBool Rtn = FALSE;
  if(Stack != NULL && Stack->ObjectCount > 0u)
  {
    //Get Room ptr
    TU8* RoomPtr = GetRoomAddress(Stack,Stack->TailRoomID);
    if(RoomPtr != NULL)
    {
      //Cpy object
      memcpy(Object,RoomPtr,Stack->Objectsize);
      //Is shift tail needed ?
      TS16 ShiftTail = (Stack->ObjectCount == 1u) ? (0u):(HEAD_DIRECTION);
      //Compute new Tail value.
      Stack->TailRoomID = ComputeRoomID_RollingBuffer(Stack->StackDepht,Stack->TailRoomID,ShiftTail);
      Stack->ObjectCount--;
      Stack->FreeRoomleft++;
      Rtn= TRUE;
    }
  }
  return Rtn;
}


void Stack_Purge(StackObjRef Stack)
{
  if(Stack != NULL)
  {
    Stack->ObjectCount = 0U;
    Stack->FreeRoomleft = Stack->StackDepht;
    Stack->HeadRoomID = 0U;
    Stack->TailRoomID = 0U;
  }
}

TU16  Stack_GetObjectCount(StackObjRef Stack)
{
  return (Stack != NULL) ? (Stack->ObjectCount) : (0u);
}

TU16  Stack_GetFreeRoomLeft(StackObjRef Stack)
{
  return (Stack != NULL) ? (Stack->FreeRoomleft) : (0u);
}

TU16  Stack_GetObjectSize(StackObjRef Stack)
{
  return (Stack != NULL) ? (Stack->Objectsize) : (0u);
}


TBool Stack_ForcedPushTail(StackObjRef Stack,const void* Object)
{
    TBool Rtn = FALSE;
  if( Stack != NULL )
  {
    //Tail point to the tail object, if any; So, on first object insertion, tail ID musn't move.
    TS16 ShiftTail = (Stack->ObjectCount == 0u) ? (0u):(TAIL_DIRECTION);
    //If enough space, it's a Normal Push tail. Else, head object is overwrite
    TU8 OverWrite = (Stack->FreeRoomleft == 0u) ? (0u):(1u);
    //Compute the new Tail roomID.
    TU16 WriteToRoomID = ComputeRoomID_RollingBuffer(Stack->StackDepht,Stack->TailRoomID,ShiftTail);
    //Get the Room start address
    TU8* RoomPtr = GetRoomAddress(Stack,WriteToRoomID);
    if(RoomPtr != NULL)
    {
      Stack->TailRoomID = WriteToRoomID;
      //Store Object
      memcpy(RoomPtr,Object,Stack->Objectsize);
      //When overwrite, don't increment counter.
      Stack->ObjectCount += OverWrite;
      Stack->FreeRoomleft -= OverWrite;
      Rtn= TRUE;
    }
  }
  return Rtn;
}

TBool Stack_ForcedPushHead(StackObjRef Stack,const void* Object)
{
    TBool Rtn = FALSE;
  if(Stack != NULL )
  {
    //Tail point to the Head object, if any; So, on first object insertion, Head ID musn't move.
    TS16 ShiftHead = (Stack->ObjectCount == 0u) ? (0u):(HEAD_DIRECTION);
    //If enough space, it's a Normal Push tail. Else, head object is overwrite
    TU8 OverWrite = (Stack->FreeRoomleft == 0u) ? (0u):(1u);
    
    TU16 WriteToRoomID = ComputeRoomID_RollingBuffer(Stack->StackDepht,Stack->HeadRoomID,ShiftHead);
    //Compute the new Tail roomID.
    TU8* RoomPtr = GetRoomAddress(Stack,WriteToRoomID);
    if(RoomPtr != NULL)
    {
      Stack->HeadRoomID = WriteToRoomID;
      //Store Object
      memcpy(RoomPtr,Object,Stack->Objectsize);
      //When overwrite, don't increment counter.
      Stack->ObjectCount += OverWrite;
      Stack->FreeRoomleft -= OverWrite;
      Rtn= TRUE;
    }
  }
  return Rtn;
  
}


TBool Stack_ReadObject(StackObjRef Stack,void* Object,TU16 RelativeToHeadObjectID)
{
    TBool Rtn = FALSE;
  if(Stack != NULL &&  RelativeToHeadObjectID < (Stack->ObjectCount))
  {
    //Compute the room to read. counting from head.
    TU16 RoomToRead = ComputeRoomID_RollingBuffer(Stack->StackDepht,Stack->HeadRoomID,RelativeToHeadObjectID);
    //Get Room ptr
    TU8* RoomPtr = GetRoomAddress(Stack,RoomToRead);
    if(RoomPtr != NULL)
    {
      memcpy(Object,RoomPtr,Stack->Objectsize);
      Rtn= TRUE;
    }
  }
  return Rtn;
  
}



