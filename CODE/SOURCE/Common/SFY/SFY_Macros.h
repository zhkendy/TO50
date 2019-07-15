// **********************************************************************
// Copyright 2016 Somfy SAS  - IDDN.FR.001.110020.000.S.P.2016.000.31500  - All rights reserved.
// All reproduction, use, distribution of this software, in whole or in part, by any means, without SOMFY SAS's prior written approval, is strictly forbidden.
// ***********************************************************************

/*
/ AUTHOR : F.PELLARIN
/ DATE :08/07/02 07:13
/ DESCRIPTION : Common macros definitions
/ ********************************************************************* */
#pragma once



  //--------------------------------------------------------------------------------------------------
  // BIT, BYTE and WORD HANDLING
  //--------------------------------------------------------------------------------------------------

  #define GET_LSBYTE(word)                        (TU8)((word) & 0x00FFu)  /* Return Low Significant Byte of a word */
  #define GET_MSBYTE(word)                        (TU8)(((word) & 0xFF00u)>> 8u) /* Return Most Significant Byte of a word */
  #define GET_BIT(word, index)                    (TBool)(((word) &(1u<<(index)))>>(index)) /* Return indexed bit */
  #define SET_BIT(word, index)                    ((word) |= ( 1u << (index)))  /* Set indexed bit */
  #define CLEAR_BIT(word, index)                  ((word) &= (~( 1u << (index)))) /* Clear indexed bit */
  #define TOGGLE_BIT(word, index)                 ((word) ^= ( 1u << (index)))  /* Toggle indexed bit */
  #define TEST_BIT(word, index, value)            ( (((word) & (1u<<(index)))==(value)) ? TRUE : FALSE ) /* Return TRUE if indexed bit is equal to value */
  #define GET_BIT_BY_ADDRESS(address, index)      ( (TBool) (((*(address) & (1<<(index))) >> (index))) )  /* Get indexed bit from the pointed var */
  #define SET_BIT_BY_ADDRESS(address, index)      ( *(address) |= ((1u)<<(index)) )  /* Set indexed bit from the pointed var */
  #define CLEAR_BIT_BY_ADDRESS(address, index)    ( *(address) &= ~((1u)<<(index)) ) /* Clear indexed bit from the pointed var */
  #define TOGGLE_BIT_BY_ADDRESS(address, index)   ( *(address) ^= ((1u)<<(index)) )  /* Toggle indexed bit from the pointed var */
  #define TEST_BIT_BY_ADDRESS(word, index, value) ((*(address) & (1u<<(index)))==(value) ? TRUE : FALSE ) /* Return TRUE if indexed bit of the pointed var is equal to value */

  #define MERGE_NIBBLES_TO_BYTE(nib_H, nib_L)     ( (((TU8)(nib_H)) << 4u)  |  ((TU8)(nib_L)) )

  #define MERGE_BYTES_TO_WORD(byte_H, byte_L)     ( (((TU16)(byte_H)) << 8u)  |  ((TU16)(byte_L)) )

  #define MERGE_WORDS_TO_LONG(word_H, word_L)     ( (((TU32)(word_H))<<16u) | ((TU32)(word_L)) )

  #define MERGE_BYTES_TO_LONG(byte_3, byte_2, byte_1, byte_0)     ( (((TU32)(byte_3))<<24u) | (((TU32)(byte_2))<<16) | (((TU32)(byte_1))<<8) | ((TU32)(byte_0)) )

  #define GET_LOW_BYTE_FROM_WORD(word)            ( (TU8)( ((TU16)(word)>> 0u) & 0x00FFu )  )      /* Return Low  Significant Byte of a word */
  #define GET_HIGH_BYTE_FROM_WORD(word)           ( (TU8)( ((TU16)(word)>> 8u) & 0x00FFu )  )      /* Return High Significant Byte of a word */

  #define GET_LOW_WORD_FROM_LONG(Long)            ( (TU16)( ((Long)>> 0u) & 0x0000FFFFul ) )   /* Return Low  Significant word of a long */
  #define GET_HIGH_WORD_FROM_LONG(Long)           ( (TU16)( ((Long)>>16u) & 0x0000FFFFul ) )   /* Return High Significant word of a long */

  #define GET_LOW_BYTE_FROM_LOW_LONG(Long)        ( GET_LOW_BYTE_FROM_WORD(  GET_LOW_WORD_FROM_LONG(Long)  ) )
  #define GET_HIGH_BYTE_FROM_LOW_LONG(Long)       ( GET_HIGH_BYTE_FROM_WORD( GET_LOW_WORD_FROM_LONG(Long)  ) )
  #define GET_LOW_BYTE_FROM_HIGH_LONG(Long)       ( GET_LOW_BYTE_FROM_WORD(  GET_HIGH_WORD_FROM_LONG(Long) ) )
  #define GET_HIGH_BYTE_FROM_HIGH_LONG(Long)      ( GET_HIGH_BYTE_FROM_WORD( GET_HIGH_WORD_FROM_LONG(Long) ) )

  #define MAX(a, b)                               ((a) < (b) ? (b) : (a))



    /* Set to TRUE or FALSE the indexed bit */
  #define SET_BIT_VALUE(word, index, value)\
              {\
                if ((TBool)(value))\
                {\
                  CLEAR_BIT(word, index);\
                }\
                else\
                {\
                  SET_BIT(word, index);\
                }\
              }

   /* Set to TRUE or FALSE the indexed bit of the pointed var */
  #define SET_BIT_VALUE_BY_ADDRESS(address, index, value)\
              {\
                 if ((TBool)(value))\
                  {\
                    CLEAR_BIT_BY_ADDRESS(address, index);\
                  }\
                  else\
                  {\
                    SET_BIT_BY_ADDRESS(address, index);\
                  }\
              }


  #define CONVERT_BYTE_TO_BCD(Byte)                   MERGE_NIBBLES_TO_BYTE( (Byte)/10, (Byte)%10 )


  //--------------------------------------------
  // build 1 word from 2 bytes
  //--------------------------------------------
  #define MAKE_WORD_FROM_BYTES(btMsbArg, btLsbArg)    MERGE_BYTES_TO_WORD( (btMsbArg) , (btLsbArg) )

  #define MAKE_LONG_FROM_WORDS(wMsbArg, wLsbArg)      MERGE_WORDS_TO_LONG( (wMsbArg) , (wLsbArg) )

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)


