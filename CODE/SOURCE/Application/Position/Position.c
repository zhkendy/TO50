// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#include "Position.h"
#include "PositionChangeObserver.h"
#include "STM8_Archiver.h"
#include "SQC_DataProvider.h"


#ifdef POSITION_PRINT_DEBUG
#include "Position_DebugTXT.h"
#include "DebugPrint.h"
#endif

#ifdef POSITION_PRINT_INFO
#include "Position_DebugTXT.h"
#include "DebugPrint_info.h"
#endif

#ifdef POSITION_PRINT_LOG	
#include "Position_DebugTXT.h"
#include "DebugPrint_Log.h"
#endif




typedef struct Position_SQC_s
{
  TU32 UEL;
  TU32 DEL;
  TU32 MY;
  TU8  PositionSet;
}Position_SQC_s;

typedef struct Position_s
{
  TU32 AbsPositionValueMotorTurn;
  struct 
  {
    TU8 PositionSet : 1;
    TU8 PositionCanBeReset : 1;
    TU8 PositionCanBeDisable : 1;
  };
}Position_s;


static void Position_SetToFactory();
static TU8* Get_SQC_data(TU8* DataSize);

static Position_s PositionArray[PositionName_LastEnum];
static Position_SQC_s SQC;
static void Test_ThatMyIsBeetweenUL();
static TU32 Compute_EndCourseDistance();

PositionObserver_s ObserverArray[PositionChangeObserver_LastEnum];


//位置信息初始化
void Position_Init()
{
   Archiver_RegisterObject(PositionArray,
                           Position_SetToFactory,
                           sizeof(PositionArray),
                           TRUE); 
   //SQC 
   SQC_DataProviderNameInterface_s intf;
   intf.Fptr_Get = Get_SQC_data;
   SQC_DataProvider_RegisterDataSource(SQC_DataProvider_Position ,&intf); 
}


void Position_Create()
{
    Archiver_LoadMe(PositionArray);
}

static TU8* Get_SQC_data(TU8* DataSize)
{
  //Take and return snapshot
  *DataSize = sizeof(SQC);
  SQC.UEL = PositionArray[PositionName_UEL].AbsPositionValueMotorTurn;
  SQC.DEL = PositionArray[PositionName_DEL].AbsPositionValueMotorTurn;
  SQC.MY  = PositionArray[PositionName_My].AbsPositionValueMotorTurn;
  SQC.PositionSet = PositionArray[PositionName_My].PositionSet;
  return (TU8*) &SQC;
}

void Position_RegisterObserver(PositionChangeObserver_e ObserverName,PositionObserver_s Observer)
{
  if(ObserverName < PositionChangeObserver_LastEnum)
  {
    ObserverArray[ObserverName] = Observer;
  }
}

void Position_NotifyObserver()
{
  for(TU8 i= 0U; i<PositionChangeObserver_LastEnum; ++i)
  {
    ObserverArray[i].Fptr_PositionChangeEvent();
  } 
}


const TU32* Position_GetValue(PositionName_e PosName)
{
  return (&PositionArray[PosName].AbsPositionValueMotorTurn);
  //return (PositionArray[PosName].PositionSet == 1u) ? (&PositionArray[PosName].AbsPositionValueMotorTurn) : ((TU32*)(NULL));
}


//位置信息复位
void Position_Reset()
{
  Position_SetToFactory();
  Position_NotifyObserver();
}

static void Position_SetToFactory()
{
  //初始化上限位
  PositionArray[PositionName_UEL].AbsPositionValueMotorTurn = FACTORY_ABS_POS_UP;
  PositionArray[PositionName_UEL].PositionSet = 0;
  PositionArray[PositionName_UEL].PositionCanBeReset = 1;
  PositionArray[PositionName_UEL].PositionCanBeDisable = 0U;
  DEBUG_PRINT5("[DBG] Position[%s], Value=%ld, Set(%d), CanBeReset(%d), CanBeDisable(%d)\n",
               PositionName_TXT[PositionName_UEL],
               PositionArray[PositionName_UEL].AbsPositionValueMotorTurn,
               PositionArray[PositionName_UEL].PositionSet,
               PositionArray[PositionName_UEL].PositionCanBeReset,
               PositionArray[PositionName_UEL].PositionCanBeDisable);
  
  //初始化下限位
  PositionArray[PositionName_DEL].AbsPositionValueMotorTurn = FACTORY_ABS_POS_DOWN;
  PositionArray[PositionName_DEL].PositionSet = 0;
  PositionArray[PositionName_DEL].PositionCanBeReset = 1; 
  PositionArray[PositionName_DEL].PositionCanBeDisable = 0U;
  DEBUG_PRINT5("[DBG] Position[%s], Value=%ld, Set(%d), CanBeReset(%d), CanBeDisable(%d)\n",
               PositionName_TXT[PositionName_DEL],
               PositionArray[PositionName_DEL].AbsPositionValueMotorTurn,
               PositionArray[PositionName_DEL].PositionSet,
               PositionArray[PositionName_DEL].PositionCanBeReset,
               PositionArray[PositionName_DEL].PositionCanBeDisable);
  
  //初始化MY中间位置
  PositionArray[PositionName_My].AbsPositionValueMotorTurn = 0U;
  PositionArray[PositionName_My].PositionSet = 0U;
  PositionArray[PositionName_My].PositionCanBeReset = 1;
  PositionArray[PositionName_My].PositionCanBeDisable = 1;
  DEBUG_PRINT5("[DBG] Position[%s], Value=%ld, Set(%d), CanBeReset(%d), CanBeDisable(%d)\n",
               PositionName_TXT[PositionName_My],
               PositionArray[PositionName_My].AbsPositionValueMotorTurn,
               PositionArray[PositionName_My].PositionSet,
               PositionArray[PositionName_My].PositionCanBeReset,
               PositionArray[PositionName_My].PositionCanBeDisable);
  
  //初始化起始位置
  PositionArray[PositionName_Origin].AbsPositionValueMotorTurn = ONE_PRODUCT_TURN;
  PositionArray[PositionName_Origin].PositionSet = 1;
  PositionArray[PositionName_Origin].PositionCanBeReset = 0U;
  PositionArray[PositionName_Origin].PositionCanBeDisable = 0U;
  DEBUG_PRINT5("[DBG] Position[%s], Value=%ld, Set(%d), CanBeReset(%d), CanBeDisable(%d)\n",
               PositionName_TXT[PositionName_Origin],
               PositionArray[PositionName_Origin].AbsPositionValueMotorTurn,
               PositionArray[PositionName_Origin].PositionSet,
               PositionArray[PositionName_Origin].PositionCanBeReset,
               PositionArray[PositionName_Origin].PositionCanBeDisable);
  
  //初始化终止位置
  PositionArray[PositionName_EndOfWorld].AbsPositionValueMotorTurn = (TU32) ((TU32) 0xFFFFFFFFu - (TU32) ONE_PRODUCT_TURN) ;
  PositionArray[PositionName_EndOfWorld].PositionSet = 1;
  PositionArray[PositionName_EndOfWorld].PositionCanBeReset = 0U;
  PositionArray[PositionName_EndOfWorld].PositionCanBeDisable = 0U;
  DEBUG_PRINT5("[DBG] Position[%s], Value=%ld, Set(%d), CanBeReset(%d), CanBeDisable(%d)\n",
               PositionName_TXT[PositionName_EndOfWorld],
               PositionArray[PositionName_EndOfWorld].AbsPositionValueMotorTurn,
               PositionArray[PositionName_EndOfWorld].PositionSet,
               PositionArray[PositionName_EndOfWorld].PositionCanBeReset,
               PositionArray[PositionName_EndOfWorld].PositionCanBeDisable);
}




TBool IsOnPosition(PositionName_e PosNameRef,const TU32* CurrentPosition)
{
  return (Position_Compare(PosNameRef,CurrentPosition,POSITION_TOLERANCE) == CurrentPositionIsOnRef) ? (TRUE):(FALSE);
}

//检测MY是否在上下限位之间
static void Test_ThatMyIsBeetweenUL()
{
  if(PositionArray[PositionName_My].PositionSet == 1)
  {
    PositionCompare_e UL = Position_Compare(PositionName_UEL,&PositionArray[PositionName_My].AbsPositionValueMotorTurn,POSITION_TOLERANCE);
    PositionCompare_e DL = Position_Compare(PositionName_DEL,&PositionArray[PositionName_My].AbsPositionValueMotorTurn,POSITION_TOLERANCE);
    if(UL == CurrentPositionIsAboveRef ||
       UL == CurrentPositionIsOnRef ||
       DL == CurrentPositionIsBelowRef ||
       DL == CurrentPositionIsOnRef)
    {
      Position_Disable(PositionName_My);
    }
  }
}


static TU32 Compute_EndCourseDistance()
{
  return PositionArray[PositionName_DEL].AbsPositionValueMotorTurn - PositionArray[PositionName_UEL].AbsPositionValueMotorTurn;
}

//percentage value [0-200] => 200 => 100%
TU32 Position_ConvertPercentageCloseToAbsolutePosition(TU8 PercentageClose)
{
  TU32 Delta = Compute_EndCourseDistance();
  TU32 ToAdd = (Delta * PercentageClose)/CLOSE_PERCENTAGE_VALUE;
  return PositionArray[PositionName_UEL].AbsPositionValueMotorTurn + ToAdd;
}

TBool Position_Set(PositionName_e PosName, TU32 PosValue)
{
  TBool Rtn = FALSE;
  if(PositionArray[PosName].PositionCanBeReset == 1)
  {
    PositionArray[PosName].AbsPositionValueMotorTurn = PosValue;
    PositionArray[PosName].PositionSet = 1;

    DEBUG_PRINT2("[DBG] Position[%s] New Value=%ld\n",
                 PositionName_TXT[PosName],
                 PositionArray[PosName].AbsPositionValueMotorTurn);
    Test_ThatMyIsBeetweenUL();//检测MY是否在上下限位之间
    Position_NotifyObserver();
    Rtn =  TRUE;
  }
  
  return Rtn;
}

TBool PositionMY_Set(PositionName_e PosName, TU32 PosValue)
{
  TBool Rtn = FALSE;
  if(PositionArray[PosName].PositionCanBeReset == 1 && PositionArray[PosName].PositionSet == 1)
  {
    PositionArray[PosName].AbsPositionValueMotorTurn = PosValue;
    PositionArray[PosName].PositionSet = 1;
    
    DEBUG_PRINT2("[DBG] Position[%s] New Value=%ld\n",
                 PositionName_TXT[PosName],
                 PositionArray[PosName].AbsPositionValueMotorTurn);
    Test_ThatMyIsBeetweenUL();//检测MY是否在上下限位之间
    Position_NotifyObserver();
    Rtn =  TRUE;
  }
  
  return Rtn;
}




//检测 限位是否已经设置
TBool Position_IsPositionSet(PositionName_e PosName)
{
  return (PositionArray[PosName].PositionSet == 1 ? (TRUE) : (FALSE));
}

//取消 位置或者限位
TBool Position_Disable(PositionName_e PosName)
{
    TBool Rtn = FALSE;
  if(PositionArray[PosName].PositionCanBeDisable == 1)
  {
    PositionArray[PosName].PositionSet = 0U;
    DEBUG_PRINT1("[DBG] Position[%s] Disable\n",PositionName_TXT[PosName]);
    Rtn = TRUE;
  }
return Rtn;
}




PositionCompare_e Position_Compare(PositionName_e PosNameRef,const TU32* CurrentPosition,TU8 Tolerance)
{
  PositionCompare_e Rtn = RefPositionIsNotSet;
  
  if(PositionArray[PosNameRef].PositionSet == 1)
  {
    if(*CurrentPosition > (PositionArray[PosNameRef].AbsPositionValueMotorTurn + Tolerance))
    {
      Rtn = CurrentPositionIsBelowRef;
    }
    else if(*CurrentPosition < (PositionArray[PosNameRef].AbsPositionValueMotorTurn - Tolerance))
    {
      Rtn = CurrentPositionIsAboveRef;
    }
    else
    {
      Rtn = CurrentPositionIsOnRef;
    }
  }
  else
  {
    Rtn = RefPositionIsNotSet;
  }
  
  return Rtn;
}

//对比两个参数
PositionCompare_e Position_CompareTwoAbsolutePosition(const TU32* PosRef,const TU32* CurrentPosition,TU8 Tolerance)
{
    PositionCompare_e Rtn = RefPositionIsNotSet;
    
  if(*CurrentPosition > (*PosRef + Tolerance))
  {
    Rtn = CurrentPositionIsBelowRef;   //在目标位置下面
  }
  else if(*CurrentPosition < (*PosRef - Tolerance))
  {
    Rtn = CurrentPositionIsAboveRef;   //在目标位置上面
  }
  else
  {
    Rtn = CurrentPositionIsOnRef;      //在目标位置
  }
  
  return Rtn;
}
  
TU32 Position_UEL()
{
  return PositionArray[PositionName_UEL].AbsPositionValueMotorTurn;
}  

TU32 Position_DEL()
{
  return PositionArray[PositionName_DEL].AbsPositionValueMotorTurn;
}

TU32 Position_MyPlace()
{
  return PositionArray[PositionName_My].AbsPositionValueMotorTurn;
}
