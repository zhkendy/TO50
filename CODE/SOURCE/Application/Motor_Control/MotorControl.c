// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


/**
* @file MotorControl.c

* @addtogroup Motor Control
* @ingroup Application
* @{
* @author Gabriel Mula.
* @date 2016.
* @brief test
* @details todo\n
*/

#include "Config.h"
#include "Encoder_Driver.h"
#include "Motor_Driver.h"
#include "Position.h" 
#include "MotorControl.h"
#include "SFY_OS.h"
#include "Chrono.h"
#include "PowerMonitor.h"
#include "RTS_Ergonomic_Manager.h"

TU8   Low_Speed_Start=0;
TS8   Brake_Count=0;
TS16  CurrentPWM2=0;
TS16  BrakePWM=0;
TU32  Motor_Protect_Count=1000u;
bool  Low_Speed_enable=true;

extern bool  Motor_Protect_Enable;
extern TU16  Ledtime_outsecond;
extern TU8   User_LedMode;
extern TU16  Led_time_last;
extern TU8   LedMode;


typedef enum
{
  IntermediateTargetRequieredAction_Acc,    
  IntermediateTargetRequieredAction_Dcc,   
  IntermediateTargetRequieredAction_Stop,  
  IntermediateTargetRequieredAction_Brake,  
  IntermediateTargetRequieredAction_None,  
}IntermediateTargetRequieredAction_e;


typedef struct
{
  TU16 AccMaxPerLoop;
  TU16 DccMaxPerLoop;
  MotorState_e CurrentState;     //used by security monitor
  MotorControl_Segment_s Segment;
  TBool SecurityMonitorEnable;
  MotorState_e SecuTimerTriggerState;
  TBool SecuLock;
  MotorState_e SecuLockState;  
  TBool GenrerateIntermediateTarget_ForAcc;
  TBool GenrerateIntermediateTarget_ForDcc;
  IntermediateTargetRequieredAction_e CurrentAction;
  TU32 CurrentPosition;
  TS16 CurrentSpeed;
  TS16 CurrentSpeedError;
  TS32 I_ErrorSum;
  TS16 IntermediateTarget;
  TS16 CurrentPWM;
 
  void (*Fprt_MotorControlIsLockBySecuManager)(void);
  void (*Fprt_MotorControlIsUnLock)(void);          
}MotorControl_t;


MotorControl_Segment_s NullSegment =
{
  .UnInteruptible = FALSE,                 
  .SpeedTrigger.TriggerEnable = FALSE,
  .SpeedTrigger.SpeedTriggerValue  = 0u,
  .PositionTrigger.TriggerEnable   = FALSE,
  .PositionTrigger.PosTriggerValue = 0u,
  .SetPoint.Direction = MotorDirection_Stop,
  .SetPoint.LoopMode  = LoopMode_Brake,     
  .SetPoint.AbsTarget  = ABS_MAX_PWM,     
  .SetPoint.AbsStartUp = ABS_MAX_PWM,      
  .SetPoint.AccMax = 0u,
  .SetPoint.DccMax = 0u,
};



static MotorControl_t MotorControl;
#define me MotorControl


#ifdef MOTORCONTROL_PRINT_DEBUG	
#include "MotorControl_DebugTXT.h"
#include "DebugPrint.h"
#endif

#ifdef MOTORCONTROL_PRINT_INFO
#include "MotorControl_DebugTXT.h"
#include "DebugPrint_info.h"
#endif

#ifdef MOTORCONTROL_PRINT_LOG	
#include "MotorControl_DebugTXT.h"
#include "DebugPrint_Log.h"
#endif


static void SetLoopMode(MotorControl_LoopMode_e LoopMode);
static void ComputeAccPerLoop();
static IntermediateTargetRequieredAction_e GetIntermediateTargetRequieredAction();
static void ComputeIntermediateTarget();
static void ComputeMotorState();
static void SecurityCheck();
static void SetPWM();
static void ComputeSpeedError();
static void Saturate32(TS32 *ToSat,TS32 Min, TS32 Max);
static void Saturate16(TS16 *ToSat,TS16 Min, TS16 Max);
static void ComputePWM();
static void CheckSpeedTrigger();
static void CheckPosTrigger();
static void UpdateAndCheckTimeTrigger();
static void SpeedAsserv();
static TBool ErrorState(MotorState_e state);


const MotorControl_Segment_s* MotorControl_GetCurrentSegment()
{
  return &me.Segment;
}

//��ȡ��ǰ���з���
MotorDirection_e MotorControl_GetCurrentSetMoveDirection()
{
  return  me.Segment.SetPoint.Direction;
}

//������� ��ʼ��
void MotorControl_Init(  void (*Fprt_MotorControlIsLockBySecuManager)(void),void (*Fprt_MotorControlIsUnLock)(void))
{
  me.Fprt_MotorControlIsLockBySecuManager = Fprt_MotorControlIsLockBySecuManager; //�ж��Ƿ���������
  me.Fprt_MotorControlIsUnLock            = Fprt_MotorControlIsUnLock;            //�����ס��־
  me.AccMaxPerLoop = 0U;
  me.DccMaxPerLoop = 0U; 
  me.CurrentState = MotorState_Stopped;         //used by security monitor ��ʼ��Ϊ stop
  me.SecuTimerTriggerState = MotorState_Stopped;//��ʼ��Ϊ stop
  me.GenrerateIntermediateTarget_ForAcc = FALSE;//�ٶȲ�������
  me.GenrerateIntermediateTarget_ForDcc = FALSE;//�ٶȲ��𽥼���
  me.SecurityMonitorEnable = TRUE;
  me.CurrentSpeed  = 0U;
  me.CurrentAction = IntermediateTargetRequieredAction_None;
  me.IntermediateTarget = 0U;
  me.CurrentSpeedError  = 0U;
  me.CurrentPWM = 0U;
  me.I_ErrorSum = 0U;
  me.SecuLock = FALSE;
  me.Segment  = NullSegment;
  
  MotorDriver_Init();
  EncoderDriver_Init();
  
  if(SFY_OS_TimerCreate(MOTORCONTROL_SECU_TIMER_ID,OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Motor,TASK_Motor_SECU_TIMER) != OS_SUCCESS)
  {//����һ����ʱ�������жϵ���Ƿ�ȫ��ס
    trap();
  } 
}

//������ƴ���
void MotorControl_Create(TBool WatchdogReset)
{
  MotorDriver_Create();
  EncoderDriver_Create(WatchdogReset);
}

//������ư�ȫ��ס
void MotorControl_SecuLock()
{
  MotorDriver_CountMotorBloccked();
  me.SecuLockState = me.SecuTimerTriggerState;
  MotorControl_SetSegment(&NullSegment);
  me.CurrentSpeed = 0U;
  me.CurrentAction = IntermediateTargetRequieredAction_None;
  me.IntermediateTarget = 0U;
  me.CurrentSpeedError  = 0U;
  me.CurrentPWM = 0U;
  me.I_ErrorSum = 0U;
  MotorDriver_Brake(ABS_MAX_PWM);//driver ic brake max
  me.SecuLock = TRUE;
  me.Fprt_MotorControlIsLockBySecuManager();
}

//������� ��ȫ����
void MotorControl_ReleaseLock()
{
  me.SecuLock = FALSE;
  me.Fprt_MotorControlIsUnLock();
}


//��ȡ��ȫ��״̬
MotorState_e MotorControl_GetSecuLockState()
{
  return me.SecuLockState;
}


TBool MotorControl_IsSecuLock()
{
  return me.SecuLock;
}

//��ȡ������Ƶ�ǰ״̬
MotorState_e MotorControl_GetState()
{
  return me.CurrentState;
}



static void ComputeAccPerLoop()
{
  if(me.Segment.SetPoint.AccMax == 0)                                           //Acc������ֵΪ0  �ٶȲ�����
  {
    me.AccMaxPerLoop = 0U;
    me.GenrerateIntermediateTarget_ForAcc = FALSE;                              //�������м�����Ŀ��
  }else
  {
    me.AccMaxPerLoop = (me.Segment.SetPoint.AccMax/MOVECONTROLLOOP_PERSECOND)+1;//���㵥λ����ֵ  1S��ɹ�40��
    me.GenrerateIntermediateTarget_ForAcc = TRUE;
  }
  
  if(me.Segment.SetPoint.DccMax == 0)                                           //Dcc�ݼ���ֵΪ0 
  {
    me.DccMaxPerLoop = 0U;
    me.GenrerateIntermediateTarget_ForDcc = FALSE;                              //�������м����Ŀ�� �ٶȲ��ݼ�
  }else
  {
    me.DccMaxPerLoop = (me.Segment.SetPoint.DccMax/MOVECONTROLLOOP_PERSECOND)+1;//���㵥λ����ֵ  1S��ɹ�40��
    me.GenrerateIntermediateTarget_ForDcc = TRUE;                               //�ٶȲ��𽥼���
  }
}


static IntermediateTargetRequieredAction_e GetIntermediateTargetRequieredAction()
{
  IntermediateTargetRequieredAction_e Rtn = IntermediateTargetRequieredAction_None;
  
  if(me.Segment.SetPoint.LoopMode == LoopMode_PWM || me.Segment.SetPoint.LoopMode == LoopMode_Speed)
  {    // �� LoopMode_PWM ��LoopMode_Speed ģʽ
      if(me.Segment.SetPoint.AbsTarget == 0)
      {//Ŀ��ֵ����0
        Rtn = IntermediateTargetRequieredAction_Stop;  //ֹͣ
      }else if(me.IntermediateTarget != 0 && (me.Segment.SetPoint.Direction) == sign(me.IntermediateTarget))
      {//Ŀ�겻Ϊ0 ���м�ֵ������0 �ҷ�����ͬ
          if(abs(me.Segment.SetPoint.AbsTarget) > abs(me.IntermediateTarget))
            {//Ŀ��ֵ�����м�ֵ
              Rtn = IntermediateTargetRequieredAction_Acc;  //�м�ֵ ��������
            }
          else if(abs(me.Segment.SetPoint.AbsTarget) < abs(me.IntermediateTarget))
            {//Ŀ��ֵС���м�ֵ
              Rtn = IntermediateTargetRequieredAction_Dcc;  //�м�ֵ ��������
            }
          else
            {//Ŀ��ֵ�����м�ֵ
              Rtn = IntermediateTargetRequieredAction_None; //�м�ֵ û�иı�����
            }
      }else if(me.IntermediateTarget == 0)                //Ŀ�겻����0 �м��ٶ�Ϊ0  ����ͬ�� 
      {
          Rtn = IntermediateTargetRequieredAction_Acc;   //�м�ֵ ����
      }else //Need to stop before change rotation direction//����һ��
      {
          Rtn = IntermediateTargetRequieredAction_Stop; //ֹͣ
      }
  }else
  {//LoopMode_Brake ɲ��ģʽ
    Rtn = IntermediateTargetRequieredAction_Brake;   //ɲ��
  }
  
  return Rtn;
}



static void Saturate16(TS16 *ToSat,TS16 Min, TS16 Max)
{
  if(*ToSat > Max)
  {
    *ToSat = Max;
  }
  
  if(*ToSat < Min)
  {
    *ToSat = Min;
  }
}

static void Saturate32(TS32 *ToSat,TS32 Min, TS32 Max)
{
  if(*ToSat > Max)
  {
    *ToSat = Max;
  }
  
  if(*ToSat < Min)
  {
    *ToSat = Min;
  }
}

MotorDirection_e MotorControl_GetLastMoveDirection()
{
  return MotorDriver_GetLastMoveDirection();
}


//�����м�Ŀ��
static void ComputeIntermediateTarget()
{
  me.CurrentAction = GetIntermediateTargetRequieredAction();// me.CurrentAction  ģʽ�ж�
  
  if(me.CurrentAction == IntermediateTargetRequieredAction_Acc)                 //�ٶ�����
  {
    if(me.GenrerateIntermediateTarget_ForAcc == TRUE)
    {  //�����м�Ŀ�� ForAcc
      TU16 AbsIntermediateTarget = (TU16) abs( (int) me.IntermediateTarget);//�м��ٶ�
      TU16 AbsTarget = (TU16) abs( (int) me.Segment.SetPoint.AbsTarget);
      if(AbsIntermediateTarget < me.Segment.SetPoint.AbsStartUp)//�м���� С�������ٶ�
      {//��ʼ״̬  ���ڿ�ʼ�ٶ�
        AbsIntermediateTarget = me.Segment.SetPoint.AbsStartUp;
      }else{//�м�״̬ �ٶ�����
        AbsIntermediateTarget += me.AccMaxPerLoop;              //�м���� ����
      }
      
      if(AbsIntermediateTarget > AbsTarget)                     //����Ŀ��ֵ
      {//�ٶȱ���
        AbsIntermediateTarget = AbsTarget;
      }
      me.IntermediateTarget = (TS16) (AbsIntermediateTarget * me.Segment.SetPoint.Direction);
    }else
    {
      me.IntermediateTarget = me.Segment.SetPoint.AbsTarget * me.Segment.SetPoint.Direction;//ԭ������ֵ
    }
  }
  else if(me.CurrentAction == IntermediateTargetRequieredAction_Dcc)            //�ٶȼ�С
  {
      if(me.GenrerateIntermediateTarget_ForDcc == TRUE)  //�ٶ��𽥼�С
      {
        TS16 AbsIntermediateTarget = abs(me.IntermediateTarget);
        TS16 AbsTarget = abs(me.Segment.SetPoint.AbsTarget);
        AbsIntermediateTarget -= me.DccMaxPerLoop;
        if(AbsIntermediateTarget < AbsTarget)
          {
            AbsIntermediateTarget = AbsTarget;
          }
        me.IntermediateTarget = AbsIntermediateTarget * me.Segment.SetPoint.Direction;
      }else                                               //�ٶ�������С                                        
      {
        me.IntermediateTarget = me.Segment.SetPoint.AbsTarget * me.Segment.SetPoint.Direction;
      }
  }
  else if(me.CurrentAction == IntermediateTargetRequieredAction_Stop)           //stop��Ҫֹͣ
  {
      if(me.GenrerateIntermediateTarget_ForDcc == TRUE)  //�ٶ��𽥼�С
      {
        TS16 AbsIntermediateTarget = abs(me.IntermediateTarget);
        AbsIntermediateTarget -= me.DccMaxPerLoop;
        if(AbsIntermediateTarget < 0)
          {
            AbsIntermediateTarget = 0U;
            me.I_ErrorSum = 0U;
          }
        me.IntermediateTarget = AbsIntermediateTarget * sign(me.IntermediateTarget);
      }else                                              //�ٶ�������С
      {
        me.IntermediateTarget = 0U;
        me.I_ErrorSum = 0U;
      }
  }
  else if(me.CurrentAction == IntermediateTargetRequieredAction_Brake)          //brakeɲ������һ��
  {
      me.IntermediateTarget = me.Segment.SetPoint.AbsTarget;
  }
  else
  {
    //IntermediateTargetRequieredAction_None
  }
}


static void ComputeMotorState()
{
  if(me.CurrentSpeed == 0) 
  {
      if(me.IntermediateTarget != 0 && abs(me.CurrentPWM) > MOTORBLOCKED_PWM_MINIMUMVALUE && me.Segment.SetPoint.LoopMode != LoopMode_Brake)
      {
        if( me.IntermediateTarget > 0)  //�м��ٶȴ���0 ��ǰ�ٶ�Ϊ0
        {
          if(Low_Speed_Start > 160u) //4s
             me.CurrentState = MotorState_BlockedUnRoll; //��������
        }else//me.IntermediateTarget <0
        {
          if(Low_Speed_Start > 160u) //4s
             me.CurrentState = MotorState_BlockedRoll;   //չ������
        }
      }else if(me.IntermediateTarget != 0)
      {
        me.CurrentState = MotorState_Stopped;         //ֹͣ
      }else//me.IntermediateTarget == 0
      {
        me.CurrentState = MotorState_Stopped;         //ֹͣ
        me.I_ErrorSum = 0U;
      }
  }else if(me.CurrentSpeed > 0)   //��ǰ�ٶ�Ϊ>0
  {
    if(me.IntermediateTarget > 0)//�м��ٶȴ���0 
    {
      if(abs(me.CurrentSpeed) <= LOWSPEED_RPM && abs(me.CurrentPWM) > MOTORLOWSPEED_PWM_MINIMUMVALUE && Low_Speed_Start > 160u && Low_Speed_enable==true) //4s //25ms*50=1250ms֮��ʼ�������쳣
      {
        me.CurrentState = MotorState_UnRolling_LowSpeed;//���پ���  ������������
        DEBUG_PRINT("[DBG] low protect\n");
      }else
      {
        me.CurrentState = MotorState_UnRolling;        //��������
      }
    }
    else //me.IntermediateTarget <= 0  �м��ٶ� �뵱ǰ�ٶ��෴ ���ټ���
    {
       me.CurrentState = MotorState_ForcedUnRoll;      //����   
    }
  }
  
  else//(me.CurrentSpeed < 0) //��ǰ�ٶ�Ϊ<0
  {
    if(me.IntermediateTarget < 0)
    {
      if(abs(me.CurrentSpeed) <= LOWSPEED_RPM && abs(me.CurrentPWM) > MOTORLOWSPEED_PWM_MINIMUMVALUE && Low_Speed_Start > 160u && Low_Speed_enable==true) //4s //25ms*50=1250ms֮��ʼ�������쳣
      {
        me.CurrentState = MotorState_Rolling_LowSpeed;  //����չ�� ������������
        DEBUG_PRINT("[DBG] low protect\n");
      }else
      {
        me.CurrentState = MotorState_Rolling;           //չ��
      }
    }
    else //me.IntermediateTarget >= 0   �м��ٶ� �뵱ǰ�ٶ��෴ ���ټ���
    {
      me.CurrentState = MotorState_ForcedRoll;          //���� 
    }
  }
}




static void SetPWM()
{
  switch(me.Segment.SetPoint.LoopMode)
  {
    case LoopMode_Speed :
    case LoopMode_PWM :
          GPIO_WriteHigh(BRAKECONTROL_PORT,BRAKECONTROL_PIN);  //ɲ������ �������

          Low_Speed_Start++;
          if(Low_Speed_Start > 200u)
          Low_Speed_Start = 200u;

          MotorDriver_Move(me.CurrentPWM,me.CurrentSpeed);
          //���õ�ǰPWM �ٶ�  ����ͬ��
          // DEBUG_PRINT1("\n[DBG] PWM(%d)\n",me.CurrentPWM);
          if(Motor_Protect_Enable == true)
          {
              if(abs(me.CurrentPWM) >= 700) 
              MotorProtTimeAdd(25);
              else if(abs(me.CurrentPWM) <700 && abs(me.CurrentPWM) >= 600)
              MotorProtTimeAdd(22);
              else if(abs(me.CurrentPWM) <600 && abs(me.CurrentPWM) >= 550)
              MotorProtTimeAdd(12);	
          }

          Brake_Count = 0;

//      if(me.CurrentSpeed >= MIDDLE_20_SPEED)
//         Brake_Count=2u;
//      else if(me.CurrentSpeed >= MININUM_10_SPEED )
//         Brake_Count=1u;
//      else
//         Brake_Count=0u;
//      
//        BrakePWM=me.CurrentPWM;
//        if(me.CurrentSpeed > MININUM_10_SPEED )
//        {
//             if(BrakePWM>=780)
//               CurrentPWM2=-660;
//             else if(BrakePWM<=-780)
//               CurrentPWM2=660;
//             
//             else if(BrakePWM>=760)
//               CurrentPWM2=-640;
//             else if(BrakePWM<=-760)
//               CurrentPWM2=640;
//             
//             else if(BrakePWM>=730)
//               CurrentPWM2=-610;
//             else if(BrakePWM<=-730)
//               CurrentPWM2=610;
//             
//              else if(BrakePWM>=700)
//               CurrentPWM2=-580;
//             else if(BrakePWM<=-700)
//               CurrentPWM2=580; 
//             
//             else if(BrakePWM>=670)
//               CurrentPWM2=-550;
//             else if(BrakePWM<=-670)
//               CurrentPWM2=550;
//     
//             else if(BrakePWM>=630)
//               CurrentPWM2=-510;
//             else if(BrakePWM<=-630)
//               CurrentPWM2=510;
//             
//             else if(BrakePWM>=590)
//               CurrentPWM2=-470;
//             else if(BrakePWM<=-590)
//               CurrentPWM2=470;
//             
//             else if(BrakePWM>=540)
//               CurrentPWM2=-420;
//             else if(BrakePWM<=-540)
//               CurrentPWM2=420;
//             
//             else if(BrakePWM>=500)
//               CurrentPWM2=-380;
//             else if(BrakePWM<=-500)
//               CurrentPWM2=380;
//             
//             else if(BrakePWM>=450)
//               CurrentPWM2=-330;
//             else if(BrakePWM<=-450)
//               CurrentPWM2=330;
//             
//             else if(BrakePWM>=400)
//             { 
//              CurrentPWM2=-300;
//              Brake_Count=1u;
//             }
//             else if(BrakePWM<=-400)
//             {
//               CurrentPWM2=300; 
//               Brake_Count=1u;
//             }
//             
//             else if(BrakePWM>0)
//             {
//               CurrentPWM2=-260;
//               Brake_Count=1u;
//             }
//             else if(BrakePWM<0)
//             { 
//               CurrentPWM2=260;
//               Brake_Count=1u;
//             }
//        }else
//        {
//             if(BrakePWM>=600)
//               CurrentPWM2=-300;
//             else if(BrakePWM<=-600)
//               CurrentPWM2=300;
//             
//             else if(BrakePWM>=350)
//               CurrentPWM2=-260;
//             else if(BrakePWM<=-350)
//               CurrentPWM2=260;
//            
//             else if(BrakePWM>0)
//               CurrentPWM2=-260;
//             else if(BrakePWM<0)
//               CurrentPWM2=260;    
//        }
        
      break;
      
    case LoopMode_Brake ://ɲ��
         Low_Speed_enable=true;
         Low_Speed_Start=0u;
         BrakePWM=0;
         MotorProtTimeDele(25); 
              
              
//       if(Brake_Count > 0)
//       {	 		
//          MotorDriver_Move2(CurrentPWM2);
//          if((Brake_Count == 2) && (CurrentPWM2 >0))
//              CurrentPWM2 -= 200;
//          else if((Brake_Count == 2) && (CurrentPWM2 <0))
//              CurrentPWM2 += 200;
//       }else
//       { 
//           if(Brake_Count == 0)
//           {
//             MotorDriver_Brake(0);                                
//             GPIO_WriteLow(BRAKECONTROL_PORT,BRAKECONTROL_PIN);   //ɲ��
//           }
//           if(Brake_Count<-100)
//             Brake_Count=-100;
//       }
         
          if(Brake_Count == 0)
           {
             MotorDriver_Brake(0);                                
             GPIO_WriteLow(BRAKECONTROL_PORT,BRAKECONTROL_PIN);   //ɲ��
           }
          
           if(Brake_Count<-100)
             Brake_Count=-100;
          
           Brake_Count--;	
      break;
      
    default :
      trap();
      break;
  } 
}

static void ComputeSpeedError()
{
  me.CurrentSpeedError = me.IntermediateTarget - me.CurrentSpeed;
}

//good compromise beetween reactivity and robustness against soft blocking detection
static const TS32 IntegerSumSatValue =(TS32) ((ABS_MAX_PWM * 3) / I) ;

static void SpeedAsserv()
{
  static TS16 PreviousSpeed = 0U;
  ComputeSpeedError();//�ٶȲ�ֵ
  TS16 Pv = (TS16)  (P*me.CurrentSpeedError);  //P=(0.03) �ٶȲ�ֵ
  TS16 Iv = (TS16)  (me.I_ErrorSum * I);       //I=(0.018)
  TS16 NewPWM = Pv + Iv;
  Saturate16(&NewPWM,-ABS_MAX_PWM,ABS_MAX_PWM);//PWMֵ��Χ�ж�
  
  if((((PreviousSpeed != me.CurrentSpeed) && (me.CurrentSpeed != 0)) || (me.CurrentSpeed == 0)))                                    
  {//�ٶ��ڸı� ���� ��ǰ�ٶ�Ϊ0
    me.I_ErrorSum += me.CurrentSpeedError;//�ٶȲ�ֵ ���
    Saturate32(&me.I_ErrorSum ,-IntegerSumSatValue,IntegerSumSatValue);//�ٶȲ�ֵ��
  }
  
  PreviousSpeed = me.CurrentSpeed;
  me.CurrentPWM = NewPWM;
}



static void ComputePWM()
{
  switch(me.Segment.SetPoint.LoopMode)
  { 
    case LoopMode_Speed : 
       SpeedAsserv();   
      break;
      
    case LoopMode_PWM :  
    case LoopMode_Brake :
      me.CurrentPWM = me.IntermediateTarget;//�м�PWMֵ 
      break;
	  
    default :
      trap();
      break;
  }
}

static TBool ErrorState(MotorState_e state)
{
  TBool Rtn = FALSE;
  switch(state)
  {
    case MotorState_BlockedRoll :       
    case MotorState_BlockedUnRoll :
    case MotorState_ForcedRoll :        
    case MotorState_ForcedUnRoll :
    case MotorState_UnRolling_LowSpeed :
    case MotorState_Rolling_LowSpeed :
      Rtn= TRUE;  
      break;
      
    case MotorState_Rolling :
    case MotorState_UnRolling :
    case MotorState_Stopped :
      Rtn= FALSE;  
      break;
      
    default :
      trap();//unknow request
      break;
  }
  return Rtn;
}

//�����ȫ���
static void SecurityCheck()
{
  if(me.SecurityMonitorEnable == TRUE)
  { //�����ȫ���ʹ��
    //Error timer not start and error detected => start the appopriate timer
    if(ErrorState(me.SecuTimerTriggerState ) == FALSE &&
       ErrorState(me.CurrentState) == TRUE  &&
       me.SecuLock == FALSE)
    {//����״̬���ִ���   ֮ǰ����,���ڹ���
      me.SecuTimerTriggerState = me.CurrentState;
     //DEBUG_PRINT1("[DBG][MotorControl] %s\n",MotorState_TXT[me.SecuTimerTriggerState])

      switch(me.CurrentState)
      {
          case MotorState_BlockedRoll   :
          case MotorState_BlockedUnRoll :
            SFY_OS_TimerStart(MOTORCONTROL_SECU_TIMER_ID,BLOCKINGTIME_MS,OS_ONE_SHOT_TIMER);
            //���� ��ʱ��ʼ  500MS
            break;
            
          case MotorState_ForcedRoll   :
          case MotorState_ForcedUnRoll :
            SFY_OS_TimerStart(MOTORCONTROL_SECU_TIMER_ID,MOVEFORCED_MS,OS_ONE_SHOT_TIMER);
            //ǿ������  ��ʱ��ʼ 1s 
            break;
            
          case MotorState_UnRolling_LowSpeed :
          case MotorState_Rolling_LowSpeed   :
            SFY_OS_TimerStart(MOTORCONTROL_SECU_TIMER_ID,LOWSPEEDTIME_MS,OS_ONE_SHOT_TIMER);
            //��������  ��ʱ��ʼ 1s ����ʱ���ֹͣ
            break;
            
          case MotorState_Rolling   :
          case MotorState_UnRolling :
          case MotorState_Stopped   :
            SFY_OS_TimerStop(MOTORCONTROL_SECU_TIMER_ID);
            //�������  ֹͣ��ȫ��ʱ
            break;
            
          default :
              trap();//unknow request
            break;
      }
    }
    //Error timer  start and error disapear and no timeout => stop timer
    else if(ErrorState(me.SecuTimerTriggerState ) == TRUE &&
            ErrorState(me.CurrentState) == FALSE  &&
              me.SecuLock == FALSE)
    {//ֹͣ��ʱ ֮ǰ����,��������
      me.SecuTimerTriggerState = me.CurrentState;
      SFY_OS_TimerStop(MOTORCONTROL_SECU_TIMER_ID);
    }
    // Timeout => Stay lock until move stop + timeout
    // ��ʱ ���ٻ��߶�ת��ʱ֮�� ִ��
    else if(me.SecuLock == TRUE && me.CurrentState != MotorState_Stopped)
    {
      SFY_OS_TimerStart(MOTORCONTROL_SECU_TIMER_ID,SECULOCK_RELEASE_TIME_MS,OS_ONE_SHOT_TIMER);
    }
  }
  else
  {
    SFY_OS_TimerStop(MOTORCONTROL_SECU_TIMER_ID);
  }
}

//�ٶȼ�� �¼�����
static void CheckSpeedTrigger()
{
  if(me.Segment.SpeedTrigger.TriggerEnable == TRUE)
  {
    if( (me.Segment.SpeedTrigger.SpeedTriggerValue == 0) &&
        (me.CurrentSpeed == me.Segment.SpeedTrigger.SpeedTriggerValue))//��ǰ�ٶ���Ϊ0
    {
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWSEG);
    }
    else if(me.Segment.SpeedTrigger.SpeedTriggerValue != 0 &&         //�ٶȴﵽĿ��ֵ
            abs(me.CurrentSpeed - me.Segment.SpeedTrigger.SpeedTriggerValue) < SPEED_TOLERANCE)
    {
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWSEG);
    }
  }
}

//λ�ü�� �¼�����
static void CheckPosTrigger()
{
  if(me.Segment.PositionTrigger.TriggerEnable == TRUE)
  {
     if( (me.Segment.SetPoint.Direction == MotorDirection_Roll) && (me.CurrentPosition <= me.Segment.PositionTrigger.PosTriggerValue+(ONE_PRODUCT_TURN/2)))
     {
       //DEBUG_PRINT("[DBG] UEL low protect\n");
       Low_Speed_enable=false;
     }
     if((me.Segment.SetPoint.Direction == MotorDirection_UnRoll) && (me.CurrentPosition >= me.Segment.PositionTrigger.PosTriggerValue-(ONE_PRODUCT_TURN/2)))
     { 
       //DEBUG_PRINT("[DBG] DEL low protect\n");
       Low_Speed_enable=false;
     } 
       
       
    if( (me.Segment.SetPoint.Direction == MotorDirection_Roll) && (me.CurrentPosition <= me.Segment.PositionTrigger.PosTriggerValue))
    {
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWSEG);//���ϴﵽĿ�ĵ�ַ
    }
    else if((me.Segment.SetPoint.Direction == MotorDirection_UnRoll) && (me.CurrentPosition >= me.Segment.PositionTrigger.PosTriggerValue))
    {     
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWSEG);//���´ﵽĿ�ĵ�ַ
    }
  }
}

//���¼�� �¼�����
static void UpdateAndCheckTimeTrigger()
{
  if(me.Segment.TimeTrigger.TriggerEnable == TRUE)
  {
    me.Segment.TimeTrigger.TimeTriggerValue -= MOVECONTROLLOOP_TIME_MS;
    //ÿ���˶�ѭ����� ����ʱ�䶼��Ҫ��ȥ25ms 
    if(me.Segment.TimeTrigger.TimeTriggerValue <= 0) //ʱ��������
    {
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWSEG);
    }
  }
}

void MotorControl_HoldCurrentSetSegment()
{
  SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWSEG);
  me.Segment.PositionTrigger.TriggerEnable = FALSE;
  me.Segment.SpeedTrigger.TriggerEnable    = FALSE;
  me.Segment.TimeTrigger.TriggerEnable     = FALSE;
}


//�������ѭ��
void MotorControl_ControlLoop(const TS16 *Speed,const TU32 *Position)
{
  me.CurrentSpeed    = *Speed;    //��ǰ�ٶ�
  me.CurrentPosition = *Position; //��ǰλ��
  ComputeMotorState();            //����������״̬
  SecurityCheck();                //�����ȫ���

  ComputeIntermediateTarget();    //�����м�Ŀ��
  ComputePWM();                   //����PWMֵ
  SetPWM();                       //����PWM
  
  CheckSpeedTrigger();            //�ٶȼ��
  CheckPosTrigger();              //λ�ü��
  UpdateAndCheckTimeTrigger();    //���¼���¼�����
  
  if(MotorControl_IsSecuLock() == TRUE)
  {
    static TU8 ctr = 0U;
    ctr = (MotorControl_GetState() == MotorState_Stopped) ? (ctr + 1u):(0u);
    if(ctr > 50u)
    {
      ctr = 0U;
      MotorControl_ReleaseLock();
    }
  }

}

static void SetLoopMode(MotorControl_LoopMode_e LoopMode)
{
  if(me.Segment.SetPoint.LoopMode == LoopMode_PWM && LoopMode == LoopMode_Speed )      //ѭ��ģʽLoopMode_Speed �ٶ�  
  {
    me.I_ErrorSum = (TS32) (me.CurrentPWM/I);// I (0.018)
    me.IntermediateTarget = me.CurrentSpeed;
  }else if(me.Segment.SetPoint.LoopMode == LoopMode_Speed && LoopMode == LoopMode_PWM  )//ѭ��ģʽLoopMode_PWM  PWM
  {
    me.I_ErrorSum = 0U;
    me.IntermediateTarget = me.CurrentPWM;
  }else if(me.Segment.SetPoint.LoopMode == LoopMode_Brake)  //ɲ��
  {
    me.I_ErrorSum = 0U;
  }
}


void MotorControl_SetSegment(const MotorControl_Segment_s *Segment)
{
  if(me.SecuLock == FALSE)                           //û�а�ȫ��ס
  {
    MotorControl_HoldCurrentSetSegment();            //������λ
    me.CurrentSpeed    = EncoderDriver_GetSpeed();   //��ȡ��ǰ�ٶ�
    me.CurrentPosition = EncoderDriver_GetPosition();//��ȡ��ǰλ��
    SetLoopMode(Segment->SetPoint.LoopMode);         //����ѭ��ģʽLoopModeģʽ����
    me.Segment = *Segment;                           //��������
    ComputeAccPerLoop();                             //���㵥λ������ֵ
  }
}


//������� ��ȫ���
void MotorControl_SecurityMonitorCMD(TBool Enable)
{
      me.SecurityMonitorEnable = Enable;
  if( me.SecurityMonitorEnable == FALSE)  //��ȫ��� �ر�
  {
    SFY_OS_TimerStop(MOTORCONTROL_SECU_TIMER_ID);
    SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_SECU_TIMER);
    if(me.SecuLock == TRUE)
    {
      MotorControl_ReleaseLock();        //����
    }
  }
}

/** @}*/