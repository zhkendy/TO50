// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

/**
* @file STM8_Archiver_Conf.h
* @brief Microcontroler target specific's configuration File.
* @author Gabriel Mula
* @version xx
* @date Septembre 2015
*/

#pragma once

static const TU16 Bloc_AStart_Adress_Table[] = {0x4000,0x4080,0x4100,0x4180,0x4200,0x4280,0x4300,0x4380};       ///<  Array of each absolute eeprom block's start address
static const TU16 Bloc_AStop_Adress_Table[]  = {0x407F,0x40FF,0x417F,0x41FF,0x427F,0x42FF,0x437F,0x43FF};       ///<  Array of each absolute eeprom block's end address


#define MAX_NBR_OBJECT                                  (25)                                                    ///< Total number of objects that can be register in the archiver 
#define CRC_LENGHT                                      (2)                                                     ///< CRC lenght in byte. 2 for CRC16
#define CRITICAL_BLOCK_CNT                              (1)                                                     ///< Number of needed critical block
#define CRITICAL_BLOCK_COPY                             (3)                                                     ///< Number of Copy of each citical block. Can't be null
#define FLASH_DATA_BLOCKS_COUNT                         (8)

#if CRITICAL_BLOCK_COPY < 1
#error "CRITICAL_BLOCK_COPY can't be < 1"
#endif

                                         
#define BLOCK_USED_BY_CRITICAL_AREA                     (CRITICAL_BLOCK_CNT + (CRITICAL_BLOCK_CNT*CRITICAL_BLOCK_COPY)) //1+1*3=4 < Number of physical block used by critical area
#define NORMAL_BLOCK_CNT                                (FLASH_DATA_BLOCKS_COUNT - BLOCK_USED_BY_CRITICAL_AREA)         //8-4=4   < Number of Normal block 
#define LOGIC_BLOCK_CNT                                 (NORMAL_BLOCK_CNT + CRITICAL_BLOCK_CNT)                         //4+1=5   < Number of logic block. A critical block count for one



  





