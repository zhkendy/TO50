// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************



/**
*  @file Chrono.h
* @addtogroup chrono
* @{
*/

/**
* @brief Millisecond in a day 1000*60*60*24
*/
#define MS_IN_A_DAY (86400000)

/**
* @enum ChronoName_e
* @brief  Enumeration of staticaly allocated chrono
* @details All Chrono are stored into an array of size Chrono_LASTENUM.
* To add a new chrono, simply add a new enum here.
* Chrono use two counter, a 16 bits days counter and a 32 bits couter to count millisecond of the current day. Then each chrono can count up to 65536 days.
*/
typedef enum ChronoName_e
{
  Chrono_TotalTime,             /**< Used By SQC, representent Total time since first boot*/
  Chrono_LowPowerTime,          /**< Used By SQC, representent Total time in low power*/
  Chrono_HighPowerTime,         /**< Used By SQC, representent Total time high power*/
  Chrono_MoveTime,              /**< Used By SQC, representent Total move time*/
  Chrono_LASTENUM,
}ChronoName_e;



/**
* @struct Chrono_s
* @brief  Sturct that represent chrono
*/
typedef struct Chrono_s
{
  TBool Running;                /**< True if chrono is running, False else.*/
  TU32  DayTime_ms;              /**< Counter, ms value.*/
  TU16  Days;                    /**< Counter, day value.*/
}Chrono_s;

/**
* @var ChronoArray
* @brief Array of all chrono declared in ChronoName_e. This Array is store in eeprom.
*/
static Chrono_s ChronoArray[Chrono_LASTENUM];

void Chrono_Init();
void Chrono_Create();

void Chrono_TimeAdd(TU16 Timems);
void MotorProtTimeDele(TU16 Timems);
void MotorProtTimeAdd(TU16 Timems);
void Chrono_SetValue(ChronoName_e Chrononame,Chrono_s ChronoToSet);
Chrono_s Chrono_Get(ChronoName_e Chrono);
TS8 Chrono_Compare(const Chrono_s* ChronoRef,const Chrono_s* Chrono);
void Chrono_Start(ChronoName_e Chrono);
void Chrono_Stop(ChronoName_e Chrono);
void Chrono_Restart(ChronoName_e Chrono);

/** @}*/