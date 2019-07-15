// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

#ifndef SFY_ASSERT_H
  #define SFY_ASSERT_H

  #include "ApplicationDebug.h"
  #include "SFY_Types.h"
  
  #define RUNTIME_WARNING()           Breakpoint()

  #define RUNTIME_ASSERT(expression)    \
          if ( ! (expression) )         \
          {                             \
            RUNTIME_WARNING();          \
          }

  #define DEBUG_ASSERT(expression)      \
          if ( ! (expression) )         \
          {                             \
            DebugAbort();               \
          }

  #ifdef DEV_ASSERT_IS_ACTIVE
    #define DEV_ASSERT(expression)      \
          if ( ! (expression) )         \
          {                             \
            DebugAbort();               \
          }
  #else
    #define DEV_ASSERT(expression)
  #endif


  #define TRAP_LOG(TrapLogId)                     RUNTIME_WARNING()
  
  #define TRAP_SIGNAL(ObjectId, TrapSignalId)     RUNTIME_WARNING()
  
  enum
    { TRAP_LOG_ID_FLASH_RAM_POOL_OVERFLOW
    , TRAP_LOG_ID_RAM_SUBPOOL_OVERFLOW
    , TRAP_LOG_ID_RAM_POOL_OVERFLOW
    , TRAP_LOG_ID_LONG_TIMER_POOL_ALLOCATION_ERROR
    , TRAP_LOG_ID_LONG_TIMER_POOL_OVERFLOW
    , TRAP_LOG_ID_MESSAGE_POOL_OVERFLOW
    , TRAP_LOG_ID_INVALID_RAM_POINTER
    , TRAP_LOG_ID_UNUSED_QUERY
    , TRAP_LOG_ID_QUERY_POOL_OVERFLOW
    , TRAP_LOG_ID_QUERY_CRITERION_ADD_FAIL
    , TRAP_LOG_ID_QUERY_EXECUTION_FAIL
    , TRAP_LOG_ID_DATA_BASE_INVALID_VALUE
    , TRAP_LOG_ID_INVALID_PARAMETER_VALUE
    , TRAP_LOG_ID_PAGE_ERASE_ERROR
  };
  
  
  enum
    { TRAP_SIGNAL_ID__POOL_ALLOCATION_ERROR
    , TRAP_SIGNAL_ID__POOL_OVERFLOW
  };
  
  

  typedef TU32 TAddrPtr;

  #define DEBUG_CHECK_THIS_DATA_RAM_ADDRESS_IS_VALID(address)     RUNTIME_ASSERT( ((TAddrPtr)(address) >= RAM_MIN) && ((TAddrPtr)(address) <= RAM_MAX) )
  #define DEBUG_CHECK_THIS_DATA_FLASH_ADDRESS_IS_VALID(address)   RUNTIME_ASSERT( ((TAddrPtr)(address) >= DATA_FLASH_MIN) && ((TAddrPtr)(address) <= DATA_FLASH_MAX) )
  #define DEBUG_CHECK_THIS_CODE_FLASH_ADDRESS_IS_VALID(address)   RUNTIME_ASSERT( ((TAddrPtr)(address) >= CODE_FLASH_MIN) && ((TAddrPtr)(address) <= CODE_FLASH_MAX) )
  

#endif /* SFY_ASSERT_H */
