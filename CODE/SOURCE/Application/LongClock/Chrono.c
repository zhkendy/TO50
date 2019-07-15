// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


/**
* @file Chrono.c
* @addtogroup chrono
* @ingroup Application
* @{
* @author Gabriel Mula
* @date 2016
* @brief Chronometer Object, used to count time.
* @details This object manage and store in eeprom (using Archiver) a collection of staticaly allocated chronometre.
* To add/Delete chronometer, edit ChronoName_e enum.
* Object need to be init (create interface with archiver and SQC) and create (Load data from eeprom) before use.
* It's under user responsability to call Chrono_TimeAdd(TU16 Timems) function.
* Each time Chrono_TimeAdd(TU16 Timems) function is called, every running chrono are update.

*/

#include "Config.h"
#include "Chrono.h"
#include "STM8_Archiver.h"
#include "SQC_DataProvider.h"

extern TU8  Motor_HotProtect;
extern TS32 Motor_Protect_Count;

/**
* @brief SQC_DataProvider interface implementation function
*/
static TU8* Get_SQC_data(TU8* DataSize);

static void Chrono_SetToFactory();

/**
* @brief Initialisation Function.
* @details This function must be call during initialisation process. It register chronometer array in Archiver, and implement SQC interface.
*/
void Chrono_Init()
{
  Archiver_RegisterObject(ChronoArray,
                          Chrono_SetToFactory,
                          sizeof(ChronoArray),
                          FALSE);
  
    //SQC interface
  SQC_DataProviderNameInterface_s intf;
  intf.Fptr_Get = Get_SQC_data;
  SQC_DataProvider_RegisterDataSource(SQC_DataProvider_Chrono ,&intf);
  
}

/**
* @brief Creation Function..
* @details This function must be call during Object creation process. It simply load persistent data from eeprom.
*/
void Chrono_Create()
{
  Archiver_LoadMe(ChronoArray);
}

/**
* @brief Set To 0 all chrono of ChronoArray
* @details This function is called by archiver during first reistration or when there is an eeprom corruption.
*/
static void Chrono_SetToFactory()
{
  for(TU8 i = 0U; i<Chrono_LASTENUM;++i)
  {
    ChronoArray[i].DayTime_ms = 100U;//sage
    ChronoArray[i].Days = 0U;
    ChronoArray[i].Running = FALSE;
  }
}


/**
* @brief Provide SQC data.
* @details Implementation of SQC_DataProvider interface.
*/
static TU8* Get_SQC_data(TU8* DataSize)
{
  *DataSize = sizeof(ChronoArray);
  return (TU8*) ChronoArray;
}


/**
* @brief Count Time. Add Timems to all running chrono. Must be call from OS context. 
*/
void Chrono_TimeAdd(TU16 Timems)
{
  for(TU8 i = 0U; i<Chrono_LASTENUM;++i)
  {
    if(ChronoArray[i].Running == TRUE)
    {
      ChronoArray[i].DayTime_ms = Timems + ChronoArray[i].DayTime_ms;
      
      if(ChronoArray[i].DayTime_ms > MS_IN_A_DAY)
      {
        ChronoArray[i].DayTime_ms -= MS_IN_A_DAY;
        ChronoArray[i].Days += 1;
      }
    }
  }
}

/**
* @brief Count Time. dele Timems to all running chrono. Must be call from OS context. 
*/
void MotorProtTimeDele(TU16 Timems)
{
    if(Motor_Protect_Count > 1000u)
       Motor_Protect_Count -= Timems;
   
    if(Motor_Protect_Count <= MOTOR_HOT_RELEASE_COUNT) //0min*60
      Motor_HotProtect = false;
    
    if(Motor_Protect_Count < 1000u)
      Motor_Protect_Count =1000u;
}

void MotorProtTimeAdd(TU16 Timems)
{
   Motor_Protect_Count += Timems;
   if(Motor_Protect_Count > MOTOR_HOT_PROTECT_COUNT)//4min*60
   {
     Motor_HotProtect = true;
     Motor_Protect_Count = MOTOR_HOT_PROTECT_COUNT;
   }
   
   if(Motor_Protect_Count <= MOTOR_HOT_RELEASE_COUNT) //0min*60
     Motor_HotProtect = false;
  
   if(Motor_Protect_Count < 1000u)
     Motor_Protect_Count =1000u;
}

/**
* @brief Set Chrono value.
*/
void Chrono_SetValue(ChronoName_e Chrononame,Chrono_s ChronoToSet)
{
  ChronoArray[Chrononame] = ChronoToSet;
  ChronoArray[Chrononame].Running = FALSE;
}

/**
* @brief Get Chrono copy.
*/
Chrono_s Chrono_Get(ChronoName_e Chrono)
{
  return  ChronoArray[Chrono];
}


/**
* @brief Compare two chronometer
* @return        0 when equal
* @return       -1 when ChronoRef > Chrono
* @return        1 when ChronoRef < Chrono
*/
TS8 Chrono_Compare(const Chrono_s* ChronoRef,const Chrono_s* Chrono)
{
  TS8 Rtn = 0;
  if(ChronoRef->Days == Chrono->Days)
  {
     Rtn = ChronoRef->DayTime_ms > Chrono->DayTime_ms ? (-1) : (1);
  }
  else
  {
    Rtn = ChronoRef->Days > Chrono->Days ? (-1) : (1);
  }
  
  return Rtn;
}

/**
* @brief Start a chrono. 
*/
void Chrono_Start(ChronoName_e Chrono)
{
  ChronoArray[Chrono].Running = TRUE;
}

/**
* @brief Stop (Pause) a chrono.
*/
void Chrono_Stop(ChronoName_e Chrono)
{
  ChronoArray[Chrono].Running = FALSE;
}
/**
* @brief Restart a chrono. Set chrono to 0 and start.
*/
void Chrono_Restart(ChronoName_e Chrono)
{
  ChronoArray[Chrono].Running = TRUE;
  ChronoArray[Chrono].DayTime_ms = 0U;
  ChronoArray[Chrono].Days = 0U;
}

/** @}*/