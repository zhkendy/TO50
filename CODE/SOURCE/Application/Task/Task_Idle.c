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
//����ϵͳ�¼���־�ȴ�
TU8 btEvents = SFY_OS_EventWait(  TASK_IDLE_EVENT ,
                                  OS_INFINITE_TIME,
                                  OS_DO_NOT_CLEAR_EVENT_FLAGS); //�����¼���־
  if(btEvents != 0u)
  { 
    if((btEvents & TASK_IDLE_EVENT) != 0u)
    {
      Controller_Idle();         // TASK_IDLE_EVENT ���ݱ��� ��ѹ��� ���Ե͹���
    } 
  }
}


