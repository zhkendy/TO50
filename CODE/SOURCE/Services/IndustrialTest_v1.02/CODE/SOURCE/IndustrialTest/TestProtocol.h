/**********************************************************************
This document and/or file is SOMFY’s property. All information it
contains is strictly confidential. This document and/or file shall
not be used, reproduced or passed on in any way, in full or in part
without SOMFY’s prior written approval. All rights reserved.
Ce document et/ou fichier est la propriété de SOMFY. Les informations
qu’il contient sont strictement confidentielles. Toute reproduction,
utilisation, transmission de ce document et/ou fichier, partielle ou
intégrale, non autorisée préalablement par SOMFY par écrit est
interdite. Tous droits réservés.
***********************************************************************/


//---------------------------------------------------------------------
// File "TestProtocol.h"   Generated with SOMFY code generator v[4.6.11.0]
//---------------------------------------------------------------------


// Header safety
//---------------------
#ifndef __TESTPROTOCOL_H__
#define __TESTPROTOCOL_H__


// External inclusions
//--------------------
#include "TestFrame.h"


// Public types
//--------------

typedef enum
{
  TEST_PROTOCOL_ERROR__NO_ERROR,
  TEST_PROTOCOL_ERROR__UNABLE_TO_TRANSMIT,
  TEST_PROTOCOL_ERROR__BAD_FRAME
} TeTestProtocolError;

typedef enum
{
	FRAME_IS_UNCOMPLETE = 0,
	FRAME_IS_COMPLETE
}TeFrameState;

typedef enum
{
  ET_READY_FOR_TST = 0x01,
  ET_ABORT_TST_IND = 0x02,
  ET_START = 0x03,
  ET_END = 0x04,
  ET_ACK = 0x05,
  ET_WRITE_RAM_REQ = 0x06,
  ET_WRITE_RAM_ANS = 0x07,
  ET_READ_RAM_REQ = 0x08,
  ET_READ_RAM_ANS = 0x09,
  ET_WRITE_NVOL_REQ = 0x0A,
  ET_WRITE_NVOL_ANS = 0x0B,
  ET_READ_NVOL_REQ = 0x0C,
  ET_READ_NVOL_ANS = 0x0D,
  ET_WRITE_IO_REQ = 0x0E,
  ET_WRITE_IO_ANS = 0x0F,
  ET_READ_IO_REQ = 0x10,
  ET_READ_IO_ANS = 0x11,
  ET_WRITE_PERIPH_REQ = 0x12,
  ET_WRITE_PERIPH_ANS = 0x13,
  ET_READ_PERIPH_REQ = 0x14,
  ET_READ_PERIPH_ANS = 0x15,
  ET_SIGN_REQ = 0x16,
  ET_SIGN_ANS = 0x17,
  ET_WRITE_TSTPARAM_REQ = 0x18,
  ET_WRITE_TSTPARAM_ANS = 0x19,
  ET_READ_TSTPARAM_REQ = 0x1A,
  ET_READ_TSTPARAM_ANS = 0x1B,
  ET_INC_TSTCNT_REQ = 0x1C,
  ET_INC_TSTCNT_ANS = 0x1D,
  ET_READ_PROGBITS_REQ = 0x1E,
  ET_READ_PROGBITS_ANS = 0x1F,
  ET_WRITE_VERSION_REQ = 0x20,
  ET_WRITE_VERSION_ANS = 0x21,
  ET_UNLOCK_NVOL_REQ = 0x22,
  ET_ECHO_REQ = 0x23,
  ET_CODE_INTEGRITY_REQ = 0x24,
  ET_CODE_INTEGRITY_ANS = 0x25,
  ET_START_SENSITIVE_TEST = 0x30,
  ET_SENSITIVE_TEST_ANS = 0x31,
  ET_STOP_SENSITIVE_TEST = 0x32,
  ET_START_KEYPAD_TEST = 0x40,
  ET_KEY_PRESSED = 0x41,
  ET_KEY_RELEASED = 0x42,
  ET_STOP_KEYPAD_TEST = 0x43,
  ET_RF_SEND = 0x44,
  ET_RF_MEASURED_FREQ = 0x45,
  ET_RF_RECEIVED_REQUEST = 0x46,
  ET_LED_ON = 0x47,
  ET_LED_OFF = 0x48,
  ET_LCD_DISPLAY = 0x49,
  ET_LCD_OFF = 0x4A,
  ET_POSITION_REQ = 0x4B,
  ET_POSITION_ANS = 0x4C,
  ET_RTC_MEASURED_FREQ = 0x4D,
  ET_DO_RF_CALC = 0x50,
  ET_DO_ALGO_REQ = 0x51,
  ET_DO_ALGO_ANS = 0x52,
  ET_GET_PLINE_REQ = 0x53,
  ET_GET_PLINE_ANS = 0x54,
  ET_GET_UCAP_REQ = 0x55,
  ET_GET_UCAP_ANS = 0x56,
  ET_RF_TXCARRIER_REQ = 0x57,
  ET_GET_CLK_REQ = 0x58,
  ET_GET_VOLT_REQ = 0x59,
  ET_GET_VOLT_ANS = 0x5A,
  ET_GET_DBS_REQ = 0x5B,
  ET_GET_DBS_ANS = 0x5C,
  ET_SET_WATCHDOG_REFRESH_FREQ = 0x5D,
  ET_GET_WATCHDOG_REFRESH_FREQ = 0x5E,
  ET_GET_WATCHDOG_REFRESH_FREQ_ANS = 0x5F,
  ET_GET_SIGNAL_PERIOD_REQ = 0x60,
  ET_GET_SIGNAL_PERIOD_ANS = 0x61,
  ET_HEAR_RF_TELCMD_REQ = 0x62,
  ET_HEAR_RF_TELCMD_ANS = 0x63,
  ET_LIGHT_ON = 0x64,
  ET_LIGHT_OFF = 0x65,
  ET_WARNING_LIGHT_ON = 0x66,
  ET_WARNING_LIGHT_OFF = 0x67,
  ET_WARNING_LIGHT_IN_REQ = 0x68,
  ET_WARNING_LIGHT_IN_ANS = 0x69,
  ET_WRITE_CALRF_REQ = 0x6A,
  ET_READ_CALRF_REQ = 0x6B,
  ET_READ_CALRF_ANS = 0x6C,
  ET_GET_SUPPLY_CURRENT_REQ = 0x6D,
  ET_GET_SUPPLY_CURRENT_ANS = 0x6E,
  ET_SET_PHOTOCELL_OUT_PWR = 0x70,
  ET_READ_PHOTOCELL_IN_REQ = 0x71,
  ET_READ_PHOTOCELL_IN_ANS = 0x72,
  ET_READ_KEYSWITCH_IN_REQ = 0x73,
  ET_READ_KEYSWITCH_IN_ANS = 0x74,
  ET_READ_PASS_DOOR_IN_REQ = 0x75,
  ET_READ_PASS_DOOR_IN_ANS = 0x76,
  ET_GET_MTR_CURRENT_IN_REQ = 0x77,
  ET_GET_MTR_CURRENT_IN_ANS = 0x78,
  ET_SET_MTR_SPEED = 0x79,
  ET_SET_EXTPERIPH_POWERMODE = 0x7A,
  ET_GET_EXTPERIPH_POWERMODE_REQ = 0x7B,
  ET_GET_EXTPERIPH_POWERMODE_ANS = 0x7C,
  ET_STOP = 0x80,
  ET_GO_UPPER_LIMIT = 0x81,
  ET_GO_LOWER_LIMIT = 0x82,
  ET_SET_SENS1 = 0x83,
  ET_SET_SENS2 = 0x84,
  ET_FEEDBACK_SHORT = 0x85,
  ET_FEEDBACK_LONG = 0x86,
  ET_SET_FDCH = 0x87,
  ET_SET_FDCB = 0x88,
  ET_RESET_PROG = 0x89,
  ET_SET_PI = 0x8A,
  ET_VIRGIN_CFG = 0x8B,
  ET_VIRGIN_STATUS = 0x8C,
  ET_RESET_CNT_REQ = 0x8D,
  ET_RESET_CNT_ANS = 0x8E,
  ET_REMOTE_CNT_REQ = 0x8F,
  ET_REMOTE_CNT_ANS = 0x90,
  ET_READ_RC_REQ = 0x91,
  ET_READ_RC_ANS = 0x92,
  ET_RX_CNT_REQ = 0x93,
  ET_RX_CNT_ANS = 0x94,
  ET_READ_RX_REQ = 0x95,
  ET_READ_RX_ANS = 0x96,
  ET_READ_CUSTOM_REQ = 0x97,
  ET_READ_CUSTOM_ANS = 0x98,
  ET_FREE_RUN1 = 0x99,
  ET_FREE_RUN2 = 0x9A,
  ET_DIAG_DATA_REQ = 0x9B,
  ET_DIAG_DATA_ANS = 0x9C,
  ET_GET_TORQ_REQ = 0x9D,
  ET_GET_TORQ_ANS = 0x9E,
  ET_GET_THERM_REQ = 0x9F,
  ET_GET_THERM_ANS = 0xA0,
  ET_WRITE_NODEADDRESS_REQ = 0xA1,
  ET_READ_NODEADDRESS_REQ = 0xA2,
  ET_READ_NODEADDRESS_ANS = 0xA3,
  ET_WRITE_CUSTOM_REQ = 0xA4,
  ET_WRITE_CUSTOM_ANS = 0xA5,
  ET_LAUNCH_AUTOTEST = 0xA6,
  ET_WRITE_PRODUCTKEY_REQ = 0xA7,
  ET_READ_PRODUCTKEY_REQ = 0xA8,
  ET_READ_PRODUCTKEY_ANS = 0xA9,
  ET_WRITE_MULTI_NODEADDR_REQ = 0xAA,
  ET_READ_MULTI_NODEADDR_REQ = 0xAB,
  ET_READ_MULTI_NODEADDR_ANS = 0xAC,
  ET_READ_PRODUCT_PARAM_REQ = 0xAD,
  ET_READ_PRODUCT_PARAM_ANS = 0xAE,
  ET_WRITE_PRODUCT_PARAM_REQ = 0xAF,
  ET_WRITE_PRODUCT_PARAM_ANS = 0xB0,
  ET_TRANSMIT_PRODUCT_PARAM = 0xB1,
  ET_START_DAUGHTER_BOARD_TEST = 0xB2,
  ET_DAUGHTER_BOARD_TEST_RESULT = 0xB3,
  ET_STOP_DAUGHTER_BOARD_TEST = 0xB4,
  ET_BUZZER_ON = 0xB5,
  ET_BUZZER_OFF = 0xB6,
  ET_SET_SAFETY_EDGE_OUT_PWR = 0xB7,
  ET_GET_EDGE_IMPEDANCE_REQ = 0xB8,
  ET_GET_EDGE_IMPEDANCE_ANS = 0xB9,
  ET_READ_2D_CODE_REQ = 0xC2,
  ET_READ_2D_CODE_ANS = 0xC3,
  ET_ESCAPE_CMD_SET_NB1_REQ = 0xF1,
  ET_ESCAPE_CMD_SET_NB1_ANS = 0xF2,
  ET_ESCAPE_CMD_SET_NB2_REQ = 0xF3,
  ET_ESCAPE_CMD_SET_NB2_ANS = 0xF4,
  ET_ESCAPE_CMD_SET_NB3_REQ = 0xF5,
  ET_ESCAPE_CMD_SET_NB3_ANS = 0xF6,
  ET_ESCAPE_CMD_SET_NB4_REQ = 0xF7,
  ET_ESCAPE_CMD_SET_NB4_ANS = 0xF8,
  ET_ESCAPE_CMD_SET_NB5_ANS = 0xF9,
  ET_NO_CMD = 0xFF
} TeIndustrialTestCmd;


// Public functions declaration
//-----------------------------------

void TestProtocol_Init(void);

void TestProtocol_Cleanup(void);

void TestProtocol_ReceiveData(void);

TeTestProtocolError TestProtocol_SendFrame(void);

TeTestProtocolError TestProtocol_FrameIsComplete(TeFrameState* peFrameState);

void TestProtocol_GetFrameHdl(ToTestFrameId* piReceiveFrameId, ToTestFrameId* piTransmitFrameId);


// Header end
//--------------
#endif // __TESTPROTOCOL_H__

/**********************************************************************
Copyright © (2013), Somfy SAS. All rights reserved.
All reproduction, use or distribution of this software, in whole or
in part, by any means, without Somfy SAS prior written approval, is
strictly forbidden.
***********************************************************************/
