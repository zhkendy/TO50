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
*          ������� ���ȼ���
*/
void Task_Motor(void)
{
//����ϵͳ�¼���־�ȴ�
  TU8 btEvents = 0U; 
  btEvents = SFY_OS_EventWait( TASK_Motor_MOVEMENTMANAGER_NEWMOVE   |
                               TASK_Motor_MOVEMENTMANAGER_NEWSEG    |
                               TASK_Motor_MOVEMENTMANAGER_NEWACTION |
                               TASK_Motor_MOVEMENTMANAGER_LOOP      |
                               TASK_Motor_SECU_TIMER,
                                      OS_INFINITE_TIME,
                                      OS_DO_NOT_CLEAR_EVENT_FLAGS);  //�����¼���־
  

  if((btEvents & TASK_Motor_SECU_TIMER) != 0u)
  {
    SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_SECU_TIMER);
    if(MotorControl_IsSecuLock() == FALSE)
    {
      MotorControl_SecuLock();     //�����ȫ���� ��ס
    }else
    {
      MotorControl_ReleaseLock();  //�����ȫ���� ����
    }
  }
  
  if((btEvents & TASK_Motor_MOVEMENTMANAGER_NEWACTION) != 0u)
  {
    SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWACTION);
    MovementManager_SetNextAction();       //�������¼����� ִ����һ������   
  }
  
  if((btEvents & TASK_Motor_MOVEMENTMANAGER_NEWMOVE) != 0u)
  {
    SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWMOVE);
    MovementManager_PlanAndSetNextMove();  //�������¼����� �ƻ��������˶�   
  }
  
  if((btEvents & TASK_Motor_MOVEMENTMANAGER_NEWSEG) != 0u)
  {
    SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWSEG);    
    MovementManager_SetNextSegment();      //������һ������                  
  } 
  
  if((btEvents & TASK_Motor_MOVEMENTMANAGER_LOOP) != 0u)
  {
    SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_LOOP);   
    MovementManagerLoop();                //�˶�������  25ms ����
  }
}



