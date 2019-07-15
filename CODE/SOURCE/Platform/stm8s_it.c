/**
  ******************************************************************************
  * @file     stm8s_it.c
  * @author   MCD Application Team
  * @version  V2.0.1
  * @date     18-November-2011
  * @brief    Main Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "Config.h"
#include "stm8s_uart1.h"
#include "stm8s_uart3.h"
#include "stm8s_it.h"
#include "stm8s_itc.h"
#include "stm8s_awu.h"
#include "stm8s_tim1.h"
#include "stm8s_wwdg.h"

#include "UARTComDriver.h"
#include "Encoder_Driver.h"
#include "TEST_Uart.h"
#include "IndustrialTest.h"
#include "RTS_InputCapture.h"
#include "OSTick.h"

extern ToTestFrameId ReceiveFrame;
TU8 Button_flage=0;
TU8 TxCounter = 0;
extern TU8 NbrOfDataToTransfer;
extern TU8 TxBuffer[20];
extern TU8 Uart1_mode;



/* Private typedef -----------------------------------------------------------*/
#ifdef ENABLE_BOOTLOADER  
typedef struct
{
  void (*pIT_Handler0)(void);
  void (*pIT_Handler1)(void);
  void (*pIT_Handler2)(void);
  void (*pIT_Handler3)(void);
  void (*pIT_Handler4)(void);
  void (*pIT_Handler5)(void);
  void (*pIT_Handler6)(void);
  void (*pIT_Handler7)(void);
  void (*pIT_Handler8)(void);
  void (*pIT_Handler9)(void);
  void (*pIT_Handler10)(void);
  void (*pIT_Handler11)(void);
  void (*pIT_Handler12)(void);
  void (*pIT_Handler13)(void);
  void (*pIT_Handler14)(void);
  void (*pIT_Handler15)(void);
  void (*pIT_Handler16)(void);
  void (*pIT_Handler17)(void);
  void (*pIT_Handler18)(void);
  void (*pIT_Handler19)(void);
  void (*pIT_Handler20)(void);
  void (*pIT_Handler21)(void);
  void (*pIT_Handler22)(void);
  void (*pIT_Handler23)(void);
  void (*pIT_Handler24)(void);
  void (*pIT_HandlerTrap)(void);
}Ts_ItHandlerArray;

_Pragma ("section=\"APPITHandlerFonctionArrayRegion\"")
_Pragma("location=\"APPITHandlerFonctionArraySection\"")

CONST __root Ts_ItHandlerArray sItHandlerArray =
{
  APP_TLI_IRQHandler,                   // pIT_Handler0
  APP_AWU_IRQHandler,                   // pIT_Handler1
  APP_CLK_IRQHandler,                   // pIT_Handler2
  APP_EXTI_PORTA_IRQHandler,            // pIT_Handler3
  APP_EXTI_PORTB_IRQHandler,            // pIT_Handler4
  APP_EXTI_PORTC_IRQHandler,            // pIT_Handler5
  APP_EXTI_PORTD_IRQHandler,            // pIT_Handler6
  APP_EXTI_PORTE_IRQHandler,            // pIT_Handler7
  APP_TRAP_IRQHandler,                  // pIT_Handler8
  APP_TRAP_IRQHandler,                  // pIT_Handler9
  APP_SPI_IRQHandler,                   // pIT_Handler10
  APP_TIM1_UPD_OVF_TRG_BRK_IRQHandler,  // pIT_Handler11
  APP_TIM1_CAP_COM_IRQHandler,          // pIT_Handler12
  APP_TIM2_UPD_OVF_BRK_IRQHandler,      // pIT_Handler13
  APP_TIM2_CAP_COM_IRQHandler,          // pIT_Handler14
  APP_TIM3_UPD_OVF_BRK_IRQHandler,      // pIT_Handler15
  APP_TIM3_CAP_COM_IRQHandler,          // pIT_Handler16
  APP_UART1_TX_IRQHandler,              // pIT_Handler17
  APP_UART1_RX_IRQHandler,              // pIT_Handler18
  APP_I2C_IRQHandler,                   // pIT_Handler19
  APP_UART3_TX_IRQHandler,              // pIT_Handler20
  APP_UART3_RX_IRQHandler,              // pIT_Handler21
  APP_ADC2_IRQHandler,                  // pIT_Handler22
  APP_TIM4_UPD_OVF_IRQHandler,          // pIT_Handler23
  APP_EEPROM_EEC_IRQHandler,            // pIT_Handler24
  APP_TRAP_IRQHandler,                  // pIT_HandlerTrap
};
#endif // ENABLE_BOOTLOADER  

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

/** @addtogroup TIM2_OC_ActiveMode
  * @{
  */
#ifdef _COSMIC_
/**
  * @brief  Dummy interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(NonHandledInterrupt, 25)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif /*_COSMIC_*/

/**
  * @brief  TRAP interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_TRAP_IRQHandler(void)
#else  
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  WWDG_SWReset();
}
/**
  * @brief  Top Level Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_TLI_IRQHandler(void)
#else  
INTERRUPT_HANDLER(TLI_IRQHandler, 0)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

/**
  * @brief  Auto Wake Up Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_AWU_IRQHandler(void)
#else // ENABLE_BOOTLOADER
INTERRUPT_HANDLER(AWU_IRQHandler, 1)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  AWU_GetFlagStatus();
}

/**
  * @brief  Clock Controller Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER
__root void APP_CLK_IRQHandler(void)
#else // ENABLE_BOOTLOADER
INTERRUPT_HANDLER(CLK_IRQHandler, 2)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

/**
  * @brief  External Interrupt PORTA Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_EXTI_PORTA_IRQHandler(void)
#else  
INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

/**
  * @brief  External Interrupt PORTB Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_EXTI_PORTB_IRQHandler(void)
#else  
INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

/**
  * @brief  External Interrupt PORTC Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_EXTI_PORTC_IRQHandler(void)
#else  
INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

/**
  * @brief  External Interrupt PORTD Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_EXTI_PORTD_IRQHandler(void)
#else  
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

/**
  * @brief  External Interrupt PORTE Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_EXTI_PORTE_IRQHandler(void)
#else  
INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

#ifdef STM8S903
/**
  * @brief  External Interrupt PORTF Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_EXTI_PORTF_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(EXTI_PORTF_IRQHandler, 8)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}
#endif /*STM8S903*/

#if defined (STM8S208) || defined (STM8AF52Ax)
/**
  * @brief CAN RX Interrupt routine.
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_CAN_RX_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(CAN_RX_IRQHandler, 8)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

/**
  * @brief  CAN TX Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_CAN_TX_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(CAN_TX_IRQHandler, 9)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}
#endif /*STM8S208 || STM8AF52Ax */

/**
  * @brief  SPI Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_SPI_IRQHandler(void)
#else  
INTERRUPT_HANDLER(SPI_IRQHandler, 10)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

/**
  * @brief  Timer1 Update/Overflow/Trigger/Break Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_TIM1_UPD_OVF_TRG_BRK_IRQHandler(void)
#else  
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
  EncoderDriver_IT_Handler_Timer1_Update();
}

/**
  * @brief  Timer1 Capture/Compare Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_TIM1_CAP_COM_IRQHandler(void)
#else  
INTERRUPT_HANDLER(TIM1_CAP_COM_IRQHandler, 12)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

#ifdef STM8S903
/**
  * @brief  Timer5 Update/Overflow/Break/Trigger Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_TIM5_UPD_OVF_BRK_TRG_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(TIM5_UPD_OVF_BRK_TRG_IRQHandler, 13)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}
/**
  * @brief  Timer5 Capture/Compare Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_TIM5_CAP_COM_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(TIM5_CAP_COM_IRQHandler, 14)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

#else /*STM8S208, STM8S207, STM8S105 or STM8S103 or STM8AF62Ax or STM8AF52Ax or STM8AF626x */
/**
  * @brief  Timer2 Update/Overflow/Break Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_TIM2_UPD_OVF_BRK_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
  EncoderDriver_IT_Handler_Timer2_Update();
}

/**
  * @brief  Timer2 Capture/Compare Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_TIM2_CAP_COM_IRQHandler(void)
#else  
INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  if (TIM2_GetITStatus(TIM2_IT_CC2) == SET)
  {
    TIM2_ClearITPendingBit(TIM2_IT_CC2);
    InputCapture_Edge_IRQHandler();
  }
  
  if (TIM2_GetITStatus(TIM2_IT_CC1) == SET)
  {
    TIM2_ClearITPendingBit(TIM2_IT_CC1);
    InputCapture_Timeout_IRQHandler();  
  }
  
  if(TIM2_GetITStatus(TIM2_IT_CC3) == SET)//¼ì²â²¶×½3±êÖ¾Î»ÖÃ×´Ì¬
  {
    TIM2_ClearITPendingBit(TIM2_IT_CC3); //Çå³ý²¶×½3ÖÐ¶Ï£¬
    EncoderDriver_IT_MotorRotationTimeTimerTimeout();
  }
}
#endif /*STM8S903*/

#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S105) || \
    defined(STM8S005) ||  defined (STM8AF62Ax) || defined (STM8AF52Ax) || defined (STM8AF626x)
/**
  * @brief Timer3 Update/Overflow/Break Interrupt routine.
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_TIM3_UPD_OVF_BRK_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

/**
  * @brief  Timer3 Capture/Compare Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_TIM3_CAP_COM_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(TIM3_CAP_COM_IRQHandler, 16)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}
#endif /*STM8S208, STM8S207 or STM8S105 or STM8AF62Ax or STM8AF52Ax or STM8AF626x */

#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S103) || \
    defined(STM8S003) ||  defined (STM8AF62Ax) || defined (STM8AF52Ax) || defined (STM8S903)
/**
  * @brief  UART1 TX Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_UART1_TX_IRQHandler(void)
#else  
INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
//  UARTComDriver_Disable,
//  UARTComDriver_Enable_RxTxToIndus,
//  UARTComDriver_Enable_RxTxToDebug,

  if(Uart1_mode!=UARTComDriver_Enable_TxSQC)
  {
        /* Write one byte to the transmit data register */
      UART1_SendData8(TxBuffer[TxCounter++]);

      if (TxCounter == NbrOfDataToTransfer)
      {
        /* Disable the UART1 Transmit interrupt */
        UART1_ITConfig(UART1_IT_TXE, DISABLE);
        NbrOfDataToTransfer=0;
        TxCounter=0;
      }
  }else   //UARTComDriver_Enable_TxSQC
  {
    TU8 data;
    if(UART1_GetITStatus( UART1_IT_TXE))
    {
      UART1_ClearITPendingBit(UART1_IT_TXE);
      UART1_ClearFlag(UART1_FLAG_TXE);
      if(UARTComDriver_TX_GetNextByte(&data) == TRUE)
      {
        UART1_ClearITPendingBit(UART1_IT_TC);
        UART1_ClearFlag(UART1_FLAG_TC);
        UART1_SendData8(data);
      }
      else
      {
        UART1_ITConfig(UART1_IT_TXE, DISABLE);
      }
    }
    
    if(UART1_GetITStatus( UART1_IT_TC))
    {
      UART1_ClearITPendingBit(UART1_IT_TC);
      UART1_ITConfig(UART1_IT_TC, DISABLE);
    }
  } 
}

/**
  * @brief  UART1 RX Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_UART1_RX_IRQHandler(void)
#else  
INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  
   if(UART1_GetITStatus(UART1_IT_RXNE))
    {
      UART1_ClearITPendingBit(UART1_IT_RXNE);
      UART1_ClearFlag(UART1_FLAG_RXNE);
      TU8 data = UART1_ReceiveData8();
      
      TestFrame_GetData(data);
      TEST_Uart_ManageReceiveFrame();
    }   
}
#endif /*STM8S105*/

/**
  * @brief  I2C Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_I2C_IRQHandler(void)
#else  
INTERRUPT_HANDLER(I2C_IRQHandler, 19)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

#if defined(STM8S105) || defined(STM8S005) ||  defined (STM8AF626x)
/**
  * @brief  UART2 TX interrupt routine.
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_UART2_TX_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(UART2_TX_IRQHandler, 20)
#endif // ENABLE_BOOTLOADER  
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
  trap();
}

/**
  * @brief  UART2 RX interrupt routine.
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_UART2_RX_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(UART2_RX_IRQHandler, 21)
#endif // ENABLE_BOOTLOADER  
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
  trap();
}
#endif /* STM8S105*/

#if defined(STM8S207) || defined(STM8S007) || defined(STM8S208) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
/**
  * @brief  UART3 TX interrupt routine.
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_UART3_TX_IRQHandler(void)
#else  
INTERRUPT_HANDLER(UART3_TX_IRQHandler, 20)
#endif // ENABLE_BOOTLOADER  
{
  TU8 data;
  if(UART3_GetITStatus( UART3_IT_TXE))
  {
    UART3_ClearITPendingBit(UART3_IT_TXE);
    UART3_ClearFlag(UART3_FLAG_TXE);
    if(UARTComDriver_TX_GetNextByte(&data) == TRUE)
    {
      UART3_ClearITPendingBit(UART3_IT_TC);
      UART3_ClearFlag(UART3_FLAG_TC);
      UART3_SendData8(data);
    }
    else
    {
      UART3_ITConfig(UART3_IT_TXE, DISABLE);
    }
  }
  
  if(UART3_GetITStatus( UART3_IT_TC))
  {
    UART3_ClearITPendingBit(UART3_IT_TC);
    UART3_ITConfig(UART3_IT_TC, DISABLE);
  }
}

/**
  * @brief  UART3 RX interrupt routine.
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_UART3_RX_IRQHandler(void)
#else  
INTERRUPT_HANDLER(UART3_RX_IRQHandler, 21)
#endif // ENABLE_BOOTLOADER  
{
//  if(UART3_GetITStatus(UART3_IT_RXNE))
//  {
////    UART3_ClearITPendingBit(UART3_IT_RXNE);
////    UART3_ClearFlag(UART3_FLAG_RXNE);
//    TU8 data = UART3_ReceiveData8();
////    
////    if(UARTComDriver_GetDriverMode() == UARTComDriver_Enable_RxTxToIndus)
////    {
////      TestFrame_AddData(ReceiveFrame,data);
////      TEST_Uart_ManageReceiveFrame();
////    }
//  }
  trap();
}
  
#endif /*STM8S208 or STM8S207 or STM8AF52Ax or STM8AF62Ax */

#if defined(STM8S207) || defined(STM8S007) || defined(STM8S208) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
/**
  * @brief  ADC2 interrupt routine.
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_ADC2_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(ADC2_IRQHandler, 22)
#endif // ENABLE_BOOTLOADER  
{

    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    trap();

}
#else /*STM8S105, STM8S103 or STM8S903 or STM8AF626x */
/**
  * @brief  ADC1 interrupt routine.
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_ADC1_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(ADC1_IRQHandler, 22)
#endif // ENABLE_BOOTLOADER  
{

    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    return;

}
#endif /*STM8S208 or STM8S207 or STM8AF52Ax or STM8AF62Ax */

#ifdef STM8S903
/**
  * @brief  Timer6 Update/Overflow/Trigger Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_TIM6_UPD_OVF_TRG_IRQHandler(void)
#else  
INTERRUPT_HANDLER(TIM6_UPD_OVF_TRG_IRQHandler, 23)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#else /*STM8S208, STM8S207, STM8S105 or STM8S103 or STM8AF62Ax or STM8AF52Ax or STM8AF626x */
/**
  * @brief  Timer4 Update/Overflow Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER
__root void APP_TIM4_UPD_OVF_IRQHandler(void)
#else  
 INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
#endif // ENABLE_BOOTLOADER
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  OSTick_IRQHandler();
}
#endif /*STM8S903*/

/**
  * @brief  Eeprom EEC Interrupt routine
  * @param  None
  * @retval None
  */
#ifdef ENABLE_BOOTLOADER  
__root void APP_EEPROM_EEC_IRQHandler(void)
#else  
INTERRUPT_HANDLER(EEPROM_EEC_IRQHandler, 24)
#endif // ENABLE_BOOTLOADER  
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  trap();
}

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
