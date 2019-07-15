// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#include "SFY_OS.h"
#include "RTS_Ergonomic_Manager.h"
#include "Task_RTS.h"


void Task_RTS(void)
{
//����ϵͳ�¼���־�ȴ�
  TU8 btEvents = 0U; 
  btEvents = SFY_OS_EventWait( TASK_RTS_FRAME_RX_EVENT | TASK_RTS_TIMER_EVENT | TASK_RTS_DATA_CHANGE | TASK_RTS_ERGO_DEAD_MAN_TIMER ,
                               OS_INFINITE_TIME,
                               OS_CLEAR_EVENT_FLAGS);  //�¼���־  ����ʱ��  ����¼���־
  
  if((btEvents & TASK_RTS_FRAME_RX_EVENT) != 0u)
  {
    RTSProtocol_EventProcessFrame();
  }
  
  if( (btEvents & TASK_RTS_ERGO_DEAD_MAN_TIMER) != 0u)
  {
    ErgonomicManager_DeadManTimer_Timeout();
  }
   
  if(( btEvents & TASK_RTS_TIMER_EVENT) != 0u)
  { //ϵͳ��ʼ ���Ͼʹ��� RTSʱ��ʱ��
    RTSProtocol_TimeElapsed();
  }
    
  if((btEvents & TASK_RTS_DATA_CHANGE) != 0u)
  {
    //Not Used. 
  }
}
