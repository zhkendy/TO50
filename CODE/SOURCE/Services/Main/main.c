// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

/**
* @file main.c
* @author Gabriel Mula
* @date 2016
*/

#include "SFY_OS.h"
#include "Config.h"
#include "main.h"
#include "ApplicationMain.h"
#include "Platform_Config.h"
#include "stm8s_itc.h"
#include "stm8s_flash.h"
#include "stm8s_gpio.h"
#include "Pcb.h"

extern TBool bStayInBootProgram;


/**
* @var Copyright
* @details This Char array contain Somfy Copyright, and it's placed in compiled source code at adress 0x8080
*/
//#pragma location=0xAA00
__root const char Copyright[] = "Copyright 2016 Somfy SAS - IDDN.FR.001.110020.000.S.P.2016.000.31500 - SOFT " SOFTWARE_VERSION " " PRODUCT_TYPE;

#ifdef ENABLE_BOOTLOADER
#pragma location="SHARED_DATA_WITH_BOOTLOADER_NO_INIT_IN_RAM"
__no_init TBool bUseAppliITVector;
#endif



void __iar_data_init2();

#ifdef ENABLE_BOOTLOADER
_Pragma ("section=\"APPMainRegion\"") \
_Pragma ("location=\"APPMainSection\"")
#endif

/**
* @brief Main, Setup OS, Create Application, and start OS.
*/
void main(void)
{
    __iar_data_init2();
    __disable_interrupt();
    CLK_Configuration();  
    PlatformConfig_DisableAllPeriph();
    PlatformConfig_SetGPIOToLowPower();
    __enable_interrupt(); 
    
    //BRAKE¡¡  
    GPIO_Init(BRAKECONTROL_PORT,BRAKECONTROL_PIN,BRAKECONTROL_MODE); 
    GPIO_WriteLow(BRAKECONTROL_PORT,BRAKECONTROL_PIN);   //É²³µ

    // OS init func to be called before any other
    SFY_OS_Init();
    // Application operating system Initialisation
    // -------------------------------------------
    Application_OS_Init();         
    Application_InitClasses();    
  
#ifdef ENABLE_BOOTLOADER
    // Program always runs application  
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    FLASH_ProgramByte((uint32_t)&bStayInBootProgram, (uint8_t)FALSE);
    //if true, the data cannot save to eeprom
    bUseAppliITVector=TRUE;
#endif
    // ===============================================================================================
    // START MULTITASKING
    // =============================================================================================== 
    SFY_OS_Start();  /* NOTE: will never return to this point */
} 


// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************
