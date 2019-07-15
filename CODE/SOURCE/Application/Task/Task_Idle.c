// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#include "Config.h"
#include "SFY_OS.h"
#include "Controller.h"
#include "Task_Controller.h"


void Task_Idle(void)
{
//操作系统事件标志等待
TU8 btEvents = SFY_OS_EventWait(  TASK_IDLE_EVENT ,
                                  OS_INFINITE_TIME,
                                  OS_DO_NOT_CLEAR_EVENT_FLAGS); //不清事件标志
  if(btEvents != 0u)
  { 
    if((btEvents & TASK_IDLE_EVENT) != 0u)
    {
      Controller_Idle();         // TASK_IDLE_EVENT 数据保存 电压检测 尝试低功耗
    } 
  }
}


