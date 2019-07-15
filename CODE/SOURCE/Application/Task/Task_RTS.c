// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#include "SFY_OS.h"
#include "RTS_Ergonomic_Manager.h"
#include "Task_RTS.h"


void Task_RTS(void)
{
//操作系统事件标志等待
  TU8 btEvents = 0U; 
  btEvents = SFY_OS_EventWait( TASK_RTS_FRAME_RX_EVENT | TASK_RTS_TIMER_EVENT | TASK_RTS_DATA_CHANGE | TASK_RTS_ERGO_DEAD_MAN_TIMER ,
                               OS_INFINITE_TIME,
                               OS_CLEAR_EVENT_FLAGS);  //事件标志  任务时间  清除事件标志
  
  if((btEvents & TASK_RTS_FRAME_RX_EVENT) != 0u)
  {
    RTSProtocol_EventProcessFrame();
  }
  
  if( (btEvents & TASK_RTS_ERGO_DEAD_MAN_TIMER) != 0u)
  {
    ErgonomicManager_DeadManTimer_Timeout();
  }
   
  if(( btEvents & TASK_RTS_TIMER_EVENT) != 0u)
  { //系统开始 马上就触发 RTS时钟时序
    RTSProtocol_TimeElapsed();
  }
    
  if((btEvents & TASK_RTS_DATA_CHANGE) != 0u)
  {
    //Not Used. 
  }
}
