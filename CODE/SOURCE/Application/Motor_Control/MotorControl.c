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

//获取当前运行方向
MotorDirection_e MotorControl_GetCurrentSetMoveDirection()
{
  return  me.Segment.SetPoint.Direction;
}

//电机控制 初始化
void MotorControl_Init(  void (*Fprt_MotorControlIsLockBySecuManager)(void),void (*Fprt_MotorControlIsUnLock)(void))
{
  me.Fprt_MotorControlIsLockBySecuManager = Fprt_MotorControlIsLockBySecuManager; //判断是否阻塞函数
  me.Fprt_MotorControlIsUnLock            = Fprt_MotorControlIsUnLock;            //电机锁住标志
  me.AccMaxPerLoop = 0U;
  me.DccMaxPerLoop = 0U; 
  me.CurrentState = MotorState_Stopped;         //used by security monitor 初始化为 stop
  me.SecuTimerTriggerState = MotorState_Stopped;//初始化为 stop
  me.GenrerateIntermediateTarget_ForAcc = FALSE;//速度不逐渐增加
  me.GenrerateIntermediateTarget_ForDcc = FALSE;//速度不逐渐减少
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
  {//开启一个定时器用于判断电机是否安全锁住
    trap();
  } 
}

//电机控制创建
void MotorControl_Create(TBool WatchdogReset)
{
  MotorDriver_Create();
  EncoderDriver_Create(WatchdogReset);
}

//电机控制安全锁住
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

//电机控制 安全解锁
void MotorControl_ReleaseLock()
{
  me.SecuLock = FALSE;
  me.Fprt_MotorControlIsUnLock();
}


//获取安全锁状态
MotorState_e MotorControl_GetSecuLockState()
{
  return me.SecuLockState;
}


TBool MotorControl_IsSecuLock()
{
  return me.SecuLock;
}

//获取电机控制当前状态
MotorState_e MotorControl_GetState()
{
  return me.CurrentState;
}



static void ComputeAccPerLoop()
{
  if(me.Segment.SetPoint.AccMax == 0)                                           //Acc递增数值为0  速度不递增
  {
    me.AccMaxPerLoop = 0U;
    me.GenrerateIntermediateTarget_ForAcc = FALSE;                              //不产生中间增量目标
  }else
  {
    me.AccMaxPerLoop = (me.Segment.SetPoint.AccMax/MOVECONTROLLOOP_PERSECOND)+1;//计算单位增量值  1S完成共40次
    me.GenrerateIntermediateTarget_ForAcc = TRUE;
  }
  
  if(me.Segment.SetPoint.DccMax == 0)                                           //Dcc递减数值为0 
  {
    me.DccMaxPerLoop = 0U;
    me.GenrerateIntermediateTarget_ForDcc = FALSE;                              //不产生中间减量目标 速度不递减
  }else
  {
    me.DccMaxPerLoop = (me.Segment.SetPoint.DccMax/MOVECONTROLLOOP_PERSECOND)+1;//计算单位减量值  1S完成共40次
    me.GenrerateIntermediateTarget_ForDcc = TRUE;                               //速度不逐渐减少
  }
}


static IntermediateTargetRequieredAction_e GetIntermediateTargetRequieredAction()
{
  IntermediateTargetRequieredAction_e Rtn = IntermediateTargetRequieredAction_None;
  
  if(me.Segment.SetPoint.LoopMode == LoopMode_PWM || me.Segment.SetPoint.LoopMode == LoopMode_Speed)
  {    // 在 LoopMode_PWM 或LoopMode_Speed 模式
      if(me.Segment.SetPoint.AbsTarget == 0)
      {//目标值等于0
        Rtn = IntermediateTargetRequieredAction_Stop;  //停止
      }else if(me.IntermediateTarget != 0 && (me.Segment.SetPoint.Direction) == sign(me.IntermediateTarget))
      {//目标不为0 且中间值不等于0 且方向相同
          if(abs(me.Segment.SetPoint.AbsTarget) > abs(me.IntermediateTarget))
            {//目标值大于中间值
              Rtn = IntermediateTargetRequieredAction_Acc;  //中间值 增加请求
            }
          else if(abs(me.Segment.SetPoint.AbsTarget) < abs(me.IntermediateTarget))
            {//目标值小于中间值
              Rtn = IntermediateTargetRequieredAction_Dcc;  //中间值 减少请求
            }
          else
            {//目标值等于中间值
              Rtn = IntermediateTargetRequieredAction_None; //中间值 没有改变请求
            }
      }else if(me.IntermediateTarget == 0)                //目标不等于0 中间速度为0  方向同向 
      {
          Rtn = IntermediateTargetRequieredAction_Acc;   //中间值 增加
      }else //Need to stop before change rotation direction//方向不一致
      {
          Rtn = IntermediateTargetRequieredAction_Stop; //停止
      }
  }else
  {//LoopMode_Brake 刹车模式
    Rtn = IntermediateTargetRequieredAction_Brake;   //刹车
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


//计算中间目标
static void ComputeIntermediateTarget()
{
  me.CurrentAction = GetIntermediateTargetRequieredAction();// me.CurrentAction  模式判断
  
  if(me.CurrentAction == IntermediateTargetRequieredAction_Acc)                 //速度增加
  {
    if(me.GenrerateIntermediateTarget_ForAcc == TRUE)
    {  //产生中间目标 ForAcc
      TU16 AbsIntermediateTarget = (TU16) abs( (int) me.IntermediateTarget);//中间速度
      TU16 AbsTarget = (TU16) abs( (int) me.Segment.SetPoint.AbsTarget);
      if(AbsIntermediateTarget < me.Segment.SetPoint.AbsStartUp)//中间参数 小于启动速度
      {//开始状态  等于开始速度
        AbsIntermediateTarget = me.Segment.SetPoint.AbsStartUp;
      }else{//中间状态 速度增加
        AbsIntermediateTarget += me.AccMaxPerLoop;              //中间参数 增加
      }
      
      if(AbsIntermediateTarget > AbsTarget)                     //大于目标值
      {//速度保持
        AbsIntermediateTarget = AbsTarget;
      }
      me.IntermediateTarget = (TS16) (AbsIntermediateTarget * me.Segment.SetPoint.Direction);
    }else
    {
      me.IntermediateTarget = me.Segment.SetPoint.AbsTarget * me.Segment.SetPoint.Direction;//原来的数值
    }
  }
  else if(me.CurrentAction == IntermediateTargetRequieredAction_Dcc)            //速度减小
  {
      if(me.GenrerateIntermediateTarget_ForDcc == TRUE)  //速度逐渐减小
      {
        TS16 AbsIntermediateTarget = abs(me.IntermediateTarget);
        TS16 AbsTarget = abs(me.Segment.SetPoint.AbsTarget);
        AbsIntermediateTarget -= me.DccMaxPerLoop;
        if(AbsIntermediateTarget < AbsTarget)
          {
            AbsIntermediateTarget = AbsTarget;
          }
        me.IntermediateTarget = AbsIntermediateTarget * me.Segment.SetPoint.Direction;
      }else                                               //速度立即减小                                        
      {
        me.IntermediateTarget = me.Segment.SetPoint.AbsTarget * me.Segment.SetPoint.Direction;
      }
  }
  else if(me.CurrentAction == IntermediateTargetRequieredAction_Stop)           //stop需要停止
  {
      if(me.GenrerateIntermediateTarget_ForDcc == TRUE)  //速度逐渐减小
      {
        TS16 AbsIntermediateTarget = abs(me.IntermediateTarget);
        AbsIntermediateTarget -= me.DccMaxPerLoop;
        if(AbsIntermediateTarget < 0)
          {
            AbsIntermediateTarget = 0U;
            me.I_ErrorSum = 0U;
          }
        me.IntermediateTarget = AbsIntermediateTarget * sign(me.IntermediateTarget);
      }else                                              //速度立即减小
      {
        me.IntermediateTarget = 0U;
        me.I_ErrorSum = 0U;
      }
  }
  else if(me.CurrentAction == IntermediateTargetRequieredAction_Brake)          //brake刹车方向不一致
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
        if( me.IntermediateTarget > 0)  //中间速度大于0 当前速度为0
        {
          if(Low_Speed_Start > 160u) //4s
             me.CurrentState = MotorState_BlockedUnRoll; //卷起阻塞
        }else//me.IntermediateTarget <0
        {
          if(Low_Speed_Start > 160u) //4s
             me.CurrentState = MotorState_BlockedRoll;   //展开阻塞
        }
      }else if(me.IntermediateTarget != 0)
      {
        me.CurrentState = MotorState_Stopped;         //停止
      }else//me.IntermediateTarget == 0
      {
        me.CurrentState = MotorState_Stopped;         //停止
        me.I_ErrorSum = 0U;
      }
  }else if(me.CurrentSpeed > 0)   //当前速度为>0
  {
    if(me.IntermediateTarget > 0)//中间速度大于0 
    {
      if(abs(me.CurrentSpeed) <= LOWSPEED_RPM && abs(me.CurrentPWM) > MOTORLOWSPEED_PWM_MINIMUMVALUE && Low_Speed_Start > 160u && Low_Speed_enable==true) //4s //25ms*50=1250ms之后开始检测低速异常
      {
        me.CurrentState = MotorState_UnRolling_LowSpeed;//低速卷起  带载能力不行
        DEBUG_PRINT("[DBG] low protect\n");
      }else
      {
        me.CurrentState = MotorState_UnRolling;        //正常卷起
      }
    }
    else //me.IntermediateTarget <= 0  中间速度 与当前速度相反 快速减速
    {
       me.CurrentState = MotorState_ForcedUnRoll;      //减速   
    }
  }
  
  else//(me.CurrentSpeed < 0) //当前速度为<0
  {
    if(me.IntermediateTarget < 0)
    {
      if(abs(me.CurrentSpeed) <= LOWSPEED_RPM && abs(me.CurrentPWM) > MOTORLOWSPEED_PWM_MINIMUMVALUE && Low_Speed_Start > 160u && Low_Speed_enable==true) //4s //25ms*50=1250ms之后开始检测低速异常
      {
        me.CurrentState = MotorState_Rolling_LowSpeed;  //低速展开 带载能力不行
        DEBUG_PRINT("[DBG] low protect\n");
      }else
      {
        me.CurrentState = MotorState_Rolling;           //展开
      }
    }
    else //me.IntermediateTarget >= 0   中间速度 与当前速度相反 快速减速
    {
      me.CurrentState = MotorState_ForcedRoll;          //减速 
    }
  }
}




static void SetPWM()
{
  switch(me.Segment.SetPoint.LoopMode)
  {
    case LoopMode_Speed :
    case LoopMode_PWM :
          GPIO_WriteHigh(BRAKECONTROL_PORT,BRAKECONTROL_PIN);  //刹车控制 电机运行

          Low_Speed_Start++;
          if(Low_Speed_Start > 200u)
          Low_Speed_Start = 200u;

          MotorDriver_Move(me.CurrentPWM,me.CurrentSpeed);
          //设置当前PWM 速度  必须同向
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
      
    case LoopMode_Brake ://刹车
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
//             GPIO_WriteLow(BRAKECONTROL_PORT,BRAKECONTROL_PIN);   //刹车
//           }
//           if(Brake_Count<-100)
//             Brake_Count=-100;
//       }
         
          if(Brake_Count == 0)
           {
             MotorDriver_Brake(0);                                
             GPIO_WriteLow(BRAKECONTROL_PORT,BRAKECONTROL_PIN);   //刹车
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
  ComputeSpeedError();//速度差值
  TS16 Pv = (TS16)  (P*me.CurrentSpeedError);  //P=(0.03) 速度差值
  TS16 Iv = (TS16)  (me.I_ErrorSum * I);       //I=(0.018)
  TS16 NewPWM = Pv + Iv;
  Saturate16(&NewPWM,-ABS_MAX_PWM,ABS_MAX_PWM);//PWM值范围判断
  
  if((((PreviousSpeed != me.CurrentSpeed) && (me.CurrentSpeed != 0)) || (me.CurrentSpeed == 0)))                                    
  {//速度在改变 或者 当前速度为0
    me.I_ErrorSum += me.CurrentSpeedError;//速度差值 求和
    Saturate32(&me.I_ErrorSum ,-IntegerSumSatValue,IntegerSumSatValue);//速度差值和
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
      me.CurrentPWM = me.IntermediateTarget;//中间PWM值 
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

//电机安全检测
static void SecurityCheck()
{
  if(me.SecurityMonitorEnable == TRUE)
  { //电机安全检测使能
    //Error timer not start and error detected => start the appopriate timer
    if(ErrorState(me.SecuTimerTriggerState ) == FALSE &&
       ErrorState(me.CurrentState) == TRUE  &&
       me.SecuLock == FALSE)
    {//错误状态出现处理   之前正常,现在故障
      me.SecuTimerTriggerState = me.CurrentState;
     //DEBUG_PRINT1("[DBG][MotorControl] %s\n",MotorState_TXT[me.SecuTimerTriggerState])

      switch(me.CurrentState)
      {
          case MotorState_BlockedRoll   :
          case MotorState_BlockedUnRoll :
            SFY_OS_TimerStart(MOTORCONTROL_SECU_TIMER_ID,BLOCKINGTIME_MS,OS_ONE_SHOT_TIMER);
            //阻塞 计时开始  500MS
            break;
            
          case MotorState_ForcedRoll   :
          case MotorState_ForcedUnRoll :
            SFY_OS_TimerStart(MOTORCONTROL_SECU_TIMER_ID,MOVEFORCED_MS,OS_ONE_SHOT_TIMER);
            //强制运行  计时开始 1s 
            break;
            
          case MotorState_UnRolling_LowSpeed :
          case MotorState_Rolling_LowSpeed   :
            SFY_OS_TimerStart(MOTORCONTROL_SECU_TIMER_ID,LOWSPEEDTIME_MS,OS_ONE_SHOT_TIMER);
            //低速运行  计时开始 1s 超过时间就停止
            break;
            
          case MotorState_Rolling   :
          case MotorState_UnRolling :
          case MotorState_Stopped   :
            SFY_OS_TimerStop(MOTORCONTROL_SECU_TIMER_ID);
            //电机运行  停止安全计时
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
    {//停止计时 之前故障,现在正常
      me.SecuTimerTriggerState = me.CurrentState;
      SFY_OS_TimerStop(MOTORCONTROL_SECU_TIMER_ID);
    }
    // Timeout => Stay lock until move stop + timeout
    // 超时 低速或者堵转超时之后 执行
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

//速度检测 事件触发
static void CheckSpeedTrigger()
{
  if(me.Segment.SpeedTrigger.TriggerEnable == TRUE)
  {
    if( (me.Segment.SpeedTrigger.SpeedTriggerValue == 0) &&
        (me.CurrentSpeed == me.Segment.SpeedTrigger.SpeedTriggerValue))//当前速度已为0
    {
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWSEG);
    }
    else if(me.Segment.SpeedTrigger.SpeedTriggerValue != 0 &&         //速度达到目标值
            abs(me.CurrentSpeed - me.Segment.SpeedTrigger.SpeedTriggerValue) < SPEED_TOLERANCE)
    {
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWSEG);
    }
  }
}

//位置检测 事件触发
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
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWSEG);//向上达到目的地址
    }
    else if((me.Segment.SetPoint.Direction == MotorDirection_UnRoll) && (me.CurrentPosition >= me.Segment.PositionTrigger.PosTriggerValue))
    {     
      SFY_OS_EventSignal(OS_SIGNAL_TO_SPECIFIC_TASK,OS_TASK_Motor,TASK_Motor_MOVEMENTMANAGER_NEWSEG);//向下达到目的地址
    }
  }
}

//更新检测 事件触发
static void UpdateAndCheckTimeTrigger()
{
  if(me.Segment.TimeTrigger.TriggerEnable == TRUE)
  {
    me.Segment.TimeTrigger.TimeTriggerValue -= MOVECONTROLLOOP_TIME_MS;
    //每次运动循环检测 运行时间都需要减去25ms 
    if(me.Segment.TimeTrigger.TimeTriggerValue <= 0) //时间用完了
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


//电机控制循环
void MotorControl_ControlLoop(const TS16 *Speed,const TU32 *Position)
{
  me.CurrentSpeed    = *Speed;    //当前速度
  me.CurrentPosition = *Position; //当前位置
  ComputeMotorState();            //计算电机工作状态
  SecurityCheck();                //电机安全检测

  ComputeIntermediateTarget();    //计算中间目标
  ComputePWM();                   //计算PWM值
  SetPWM();                       //设置PWM
  
  CheckSpeedTrigger();            //速度检测
  CheckPosTrigger();              //位置检测
  UpdateAndCheckTimeTrigger();    //更新检测事件触发
  
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
  if(me.Segment.SetPoint.LoopMode == LoopMode_PWM && LoopMode == LoopMode_Speed )      //循环模式LoopMode_Speed 速度  
  {
    me.I_ErrorSum = (TS32) (me.CurrentPWM/I);// I (0.018)
    me.IntermediateTarget = me.CurrentSpeed;
  }else if(me.Segment.SetPoint.LoopMode == LoopMode_Speed && LoopMode == LoopMode_PWM  )//循环模式LoopMode_PWM  PWM
  {
    me.I_ErrorSum = 0U;
    me.IntermediateTarget = me.CurrentPWM;
  }else if(me.Segment.SetPoint.LoopMode == LoopMode_Brake)  //刹车
  {
    me.I_ErrorSum = 0U;
  }
}


void MotorControl_SetSegment(const MotorControl_Segment_s *Segment)
{
  if(me.SecuLock == FALSE)                           //没有安全锁住
  {
    MotorControl_HoldCurrentSetSegment();            //参数复位
    me.CurrentSpeed    = EncoderDriver_GetSpeed();   //获取当前速度
    me.CurrentPosition = EncoderDriver_GetPosition();//获取当前位置
    SetLoopMode(Segment->SetPoint.LoopMode);         //设置循环模式LoopMode模式设置
    me.Segment = *Segment;                           //参数导入
    ComputeAccPerLoop();                             //计算单位增减量值
  }
}


//电机控制 安全检测
void MotorControl_SecurityMonitorCMD(TBool Enable)
{
      me.SecurityMonitorEnable = Enable;
  if( me.SecurityMonitorEnable == FALSE)  //安全检测 关闭
  {
    SFY_OS_TimerStop(MOTORCONTROL_SECU_TIMER_ID);
    SFY_OS_EventReset(OS_TASK_Motor,TASK_Motor_SECU_TIMER);
    if(me.SecuLock == TRUE)
    {
      MotorControl_ReleaseLock();        //解锁
    }
  }
}

/** @}*/