// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#include "SFY_OS.h"
#include "MovementManager.h"
#include "Encoder_Driver.h"
#include "stm8s_iwdg.h"
#include "stdlib.h"
#include "math.h" //ceil
#include "Task_Motor.h"


//_________________________________________________________________________________________________
/*!
* \fn      void Task1( void )
* \brief   Task 1 with the higher priority. 
*          电机任务 优先级高
*/
void Task_Motor(void)
{
//操作系统事件标志等待
  TU8 btEvents = 0U; 
  btEvents = SFY_OS_EventWait( TASK_Motor_MOVEMENTMANAGER_NEWMOVE   |
                               TASK_Motor_MOVEMENTMANAGER_NEWSEG    |
                               TASK_Motor_MOVEMENTMANAGER_NEWACTION |
                               TASK_Motor_MOVEMENTMANAGER_LOOP      |
                               TASK_Motor_SECU_TIMER,
                                      OS_INFINITE_TIME,
                                      OS_DO_NOT_CLEAR_EVENT_FLAGS);  //不清事件标志
  

  if((btEvents & TASK_Motor_SECU_TIMER) != 0u)
  {
    SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_SECU_TIMER);
    if(MotorControl_IsSecuLock() == FALSE)
    {
      MotorControl_SecuLock();     //电机安全控制 锁住
    }else
    {
      MotorControl_ReleaseLock();  //电机安全控制 解锁
    }
  }
  
  if((btEvents & TASK_Motor_MOVEMENTMANAGER_NEWACTION) != 0u)
  {
    SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWACTION);
    MovementManager_SetNextAction();       //任务中事件处理 执行下一个动作   
  }
  
  if((btEvents & TASK_Motor_MOVEMENTMANAGER_NEWMOVE) != 0u)
  {
    SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWMOVE);
    MovementManager_PlanAndSetNextMove();  //任务中事件处理 计划设置新运动   
  }
  
  if((btEvents & TASK_Motor_MOVEMENTMANAGER_NEWSEG) != 0u)
  {
    SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWSEG);    
    MovementManager_SetNextSegment();      //设置下一个参数                  
  } 
  
  if((btEvents & TASK_Motor_MOVEMENTMANAGER_LOOP) != 0u)
  {
    SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_LOOP);   
    MovementManagerLoop();                //运动管理检测  25ms 触发
  }
}



