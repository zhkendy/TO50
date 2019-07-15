// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#include "Config.h"
#include "SFY_OS.h"
#include "LongTimer.h"
#include "Chrono.h"
#include "Controller.h"
#include "IndustrialTest.h"
#include "RTS_Ergonomic_Manager.h"
#include "RtsLowPowerManager.h"
#include "Task_Controller.h"


void Task_Controller(void)
{
//����ϵͳ�¼���־�ȴ�
TU8 btEvents = SFY_OS_EventWait(  TASK_Controller_EMS_TIMEOUT_EVENT |
                                  TASK_Controller_IndusUARTRX_EVENT |
                                  TASK_Controller_Boot_EVENT |
                                  TASK_Controller_LOWPOWER_TIMEOUT_EVENT |
                                  TASK_Controller_LongTimerTick_EVENT |
                                  TASK_Controller_EMS_LowPowerTest_EVENT |
                                  TASK_Controller_STOP_TO ,
                                    OS_INFINITE_TIME,
                                    OS_DO_NOT_CLEAR_EVENT_FLAGS);//�����¼���־
  if(btEvents != 0u)
  { 

    if((btEvents & TASK_Controller_EMS_LowPowerTest_EVENT) != 0u)
    {
      SFY_OS_EventReset(OS_TASK_Controller,TASK_Controller_EMS_LowPowerTest_EVENT);
    }
    
    if((btEvents & TASK_Controller_EMS_TIMEOUT_EVENT) != 0u)
    {
      SFY_OS_EventReset(OS_TASK_Controller,TASK_Controller_EMS_TIMEOUT_EVENT);
      Controller_SetState(ActuatorState_StopEMS);    //step2
    }
    
    if((btEvents & TASK_Controller_IndusUARTRX_EVENT) != 0u)
    {
      SFY_OS_EventReset(OS_TASK_Controller,TASK_Controller_IndusUARTRX_EVENT);
      TestProtocol_ReceiveData();
    }
    
    if((btEvents & TASK_Controller_LOWPOWER_TIMEOUT_EVENT) != 0u)
    {
      SFY_OS_EventReset(OS_TASK_Controller,TASK_Controller_LOWPOWER_TIMEOUT_EVENT);
      ControllerSetNoRTSFlag();
    }
    
    if((btEvents & TASK_Controller_LongTimerTick_EVENT) != 0u)
    {
      SFY_OS_EventReset(OS_TASK_Controller,TASK_Controller_LongTimerTick_EVENT);
      LongTimer_TimeAdd(LongTimerTimebase_ms);   //����100ms
      Chrono_TimeAdd(LongTimerTimebase_ms);      //����100ms
      Controller_TestRTSTiming();
      RtsLowPowerManager_MeasureRSSIFromHighPower();
      LongTimer_HandleTimeout();  
        
//      if(ErgonomicManager_GetCurrentMode() == ErgoMode_EMS_Sensitivity_On)
//      {
//        RtsLowPowerManager_MeasureRSSIFromHighPower();
//      }
  
    }
    
    if((btEvents & TASK_Controller_Boot_EVENT) != 0u)
    {
      SFY_OS_EventReset(OS_TASK_Controller,TASK_Controller_Boot_EVENT);
      SFY_OS_TaskDelay(200);
      Controller_Create();        //����������  ss
      DEBUG_PRINT("[DBG] Controller_Create\n");
    }
    
    if((btEvents & TASK_Controller_STOP_TO) != 0u)
    {
      SFY_OS_EventReset(OS_TASK_Controller,TASK_Controller_STOP_TO);
      Controller_PowerOffSensor();//�رձ�������Դ ���ݱ���
    }
        
  }
}


