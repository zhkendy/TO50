// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#pragma once



// Malloc is forbid by coding rule, you have to manually add Observer here
typedef enum PositionChangeObserver_e
{
  PositionChangeObserver_Limits,
  PositionChangeObserver_LastEnum,
}PositionChangeObserver_e;


typedef struct PositionObserver_s
{
  void (*Fptr_PositionChangeEvent)(void);
}PositionObserver_s;



void Position_RegisterObserver(PositionChangeObserver_e ObserverName,PositionObserver_s Observer);
void Position_NotifyObserver();