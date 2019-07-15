// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************


#ifndef   APPLICATION_MAIN_H
  #define APPLICATION_MAIN_H

  //------------------------------------------------------------------------------------------
  // list all application tasks so that they are known in main.c
  //------------------------------------------------------------------------------------------

  //------------------------------------------------------------------------------------------
  // init / starts all OS resources required at the start of software for the application part
  //------------------------------------------------------------------------------------------
  void Application_OS_Init (void);

  //------------------------------------------------------------------------------------------
  // init of instance pools
  //------------------------------------------------------------------------------------------
  void Application_InitClasses(void);

  //------------------------------------------------------------------------------------------
  // specific low power management
  //------------------------------------------------------------------------------------------
  // void Application_LowPowerProcess(void);

  TU8 Application_Check_Heap_SpaceLeft();
  TU8 Application_Check_Stack_SpaceLeft();
#endif

/**********************************************************************
Copyright © (2008), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
