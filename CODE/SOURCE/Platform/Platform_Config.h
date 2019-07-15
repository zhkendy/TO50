/**********************************************************************
This document and/or file is SOMFY�s property. All information it
contains is strictly confidential. This document and/or file shall
not be used, reproduced or passed on in any way, in full or in part
without SOMFY�s prior written approval. All rights reserved.
Ce document et/ou fichier est la propri�t� de SOMFY. Les informations
qu�il contient sont strictement confidentielles. Toute reproduction,
utilisation, transmission de ce document et/ou fichier, partielle ou
int�grale, non autoris�e pr�alablement par SOMFY par �crit est
interdite. Tous droits r�serv�s.
***********************************************************************/


//---------------------------------------------------------------------
// File "ApplicationCpu.h"   Generated with my own two hands
//---------------------------------------------------------------------


// Header safety
//---------------------
#pragma once





// External inclusions
//--------------------
  

void CLK_Configuration();
void PlatformConfig_DisableAllPeriph();
void PlatformConfig_SetAllGPIOToLow();
void PlatformConfig_SetGPIOToLowPower();
void Setup_And_Start_Watchdog();
void PlatformConfig_SetUpHardWareForHighPower();
void PlatformConfig_SetUpHardWareForLowPower();