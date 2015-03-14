/* 
 * File:    interrupts.cpp
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-11
 * Description:
 *   not yet supplied
 * Major Changes:
 *   Version 0: alpha development
 */
/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifdef __XC32
    #include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif

// only include distinct peripherals
//#include <peripheral/peripheral.h>            /* Include to use PIC32 peripheral libraries     */
#include <sys/attribs.h>     /* For __ISR definition                          */
#include <stdint.h>          /* For uint32_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include <exception>         /* Includes try/catch functions                  */
#include "system_config.h"
#include "app.h"
#include "system_definitions.h"
#include "system_init.h"

/* All the files in the C++ standard library declare its entities
 * within the std namespace. */
using namespace std;        /* use the standard namespace                     */

/******************************************************************************/
/* Interrupt Vector Options                                                   */
/******************************************************************************/
/*                                                                            */
/* VECTOR NAMES:                                                              */
/*                                                                            */
/* _CORE_TIMER_VECTOR          _COMPARATOR_2_VECTOR                           */
/* _CORE_SOFTWARE_0_VECTOR     _UART_2A_VECTOR                                */
/* _CORE_SOFTWARE_1_VECTOR     _I2C_2A_VECTOR                                 */
/* _EXTERNAL_0_VECTOR          _SPI_2_VECTOR                                  */
/* _TIMER_1_VECTOR             _SPI_2A_VECTOR                                 */
/* _INPUT_CAPTURE_1_VECTOR     _I2C_4_VECTOR                                  */
/* _OUTPUT_COMPARE_1_VECTOR    _UART_3_VECTOR                                 */
/* _EXTERNAL_1_VECTOR          _UART_2_VECTOR                                 */
/* _TIMER_2_VECTOR             _SPI_3A_VECTOR                                 */
/* _INPUT_CAPTURE_2_VECTOR     _I2C_3A_VECTOR                                 */
/* _OUTPUT_COMPARE_2_VECTOR    _UART_3A_VECTOR                                */
/* _EXTERNAL_2_VECTOR          _SPI_4_VECTOR                                  */
/* _TIMER_3_VECTOR             _I2C_5_VECTOR                                  */
/* _INPUT_CAPTURE_3_VECTOR     _I2C_2_VECTOR                                  */
/* _OUTPUT_COMPARE_3_VECTOR    _FAIL_SAFE_MONITOR_VECTOR                      */
/* _EXTERNAL_3_VECTOR          _RTCC_VECTOR                                   */
/* _TIMER_4_VECTOR             _DMA_0_VECTOR                                  */
/* _INPUT_CAPTURE_4_VECTOR     _DMA_1_VECTOR                                  */
/* _OUTPUT_COMPARE_4_VECTOR    _DMA_2_VECTOR                                  */
/* _EXTERNAL_4_VECTOR          _DMA_3_VECTOR                                  */
/* _TIMER_5_VECTOR             _DMA_4_VECTOR                                  */
/* _INPUT_CAPTURE_5_VECTOR     _DMA_5_VECTOR                                  */
/* _OUTPUT_COMPARE_5_VECTOR    _DMA_6_VECTOR                                  */
/* _SPI_1_VECTOR               _DMA_7_VECTOR                                  */
/* _I2C_3_VECTOR               _FCE_VECTOR                                    */
/* _UART_1A_VECTOR             _USB_1_VECTOR                                  */
/* _UART_1_VECTOR              _CAN_1_VECTOR                                  */
/* _SPI_1A_VECTOR              _CAN_2_VECTOR                                  */
/* _I2C_1A_VECTOR              _ETH_VECTOR                                    */
/* _SPI_3_VECTOR               _UART_4_VECTOR                                 */
/* _I2C_1_VECTOR               _UART_1B_VECTOR                                */
/* _CHANGE_NOTICE_VECTOR       _UART_6_VECTOR                                 */
/* _ADC_VECTOR                 _UART_2B_VECTOR                                */
/* _PMP_VECTOR                 _UART_5_VECTOR                                 */
/* _COMPARATOR_1_VECTOR        _UART_3B_VECTOR                                */
/*                                                                            */
/* Refer to the device specific .h file in the C32/XC32 Compiler              */
/* pic32mx\include\proc directory for a complete Vector and IRQ mnemonic      */
/* listings for the PIC32 device.                                             */
/*                                                                            */
/* PRIORITY OPTIONS:                                                          */
/*                                                                            */
/* (default) IPL0AUTO, IPL1, IPL2, ... IPL7 (highest)                         */
/*                                                                            */
/* Example Shorthand Syntax                                                   */
/*                                                                            */
/* void __ISR(<Vector Name>,<PRIORITY>) user_interrupt_routine_name(void)     */
/* {                                                                          */
/*     <Clear Interrupt Flag>                                                 */
/* }                                                                          */
/*                                                                            */
/* For more interrupt macro examples refer to the C compiler User Guide in    */
/* the C compiler /doc directory.                                             */
/*                                                                            */
/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

#ifdef	__cplusplus
extern "C" {
#endif

// ***********************************************
// priority in interrupts (7)6 = high 0 = low  
// ***********************************************

#ifdef APP_USE_USB
// Vector for USB 1, priority 5, software controlled register switching (on MX1xx/MX2xx MCUs)
void __ISR ( _USB_1_VECTOR,ipl5soft ) _InterruptHandler_USB ( void )
{
    SYS_USB_DEVICE_ISR();
    //USB_DEVICE_Tasks_ISR(sysObjects.usbDevObject);
}
#endif

// Vector for I2C 1, priority 4, software controlled register switching (on MX1xx/MX2xx MCUs)
void __ISR( _I2C_1_VECTOR, ipl4soft ) _InterruptHandler_I2C_1(void) {
    if (PLIB_INT_SourceFlagGet(APP_INT_ID, INT_SOURCE_I2C_1_MASTER)) {
        APP_I2C_Process();
    }
    if (PLIB_INT_SourceFlagGet(APP_INT_ID, INT_SOURCE_I2C_1_ERROR)) {
        LEDR_Set;
    }
    // Clear the interrupt flag
    //PLIB_INT_SourceFlagClear(APP_INT_ID, INT_VECTOR_I2C1);
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_SOURCE_I2C_1_ERROR);
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_SOURCE_I2C_1_SLAVE);
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_SOURCE_I2C_1_MASTER);
}

// Vector for timer 1, priority 3, software controlled register switching (on MX1xx/MX2xx MCUs)
void __ISR( _TIMER_1_VECTOR, ipl3soft ) _InterruptHandler_TMR_1(void) {
    // App Timing
    APP_TimingCallback();
    // Clear the interrupt flag
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_SOURCE_TIMER_1);
}

#ifdef APP_USE_USART
// Vector for UART 2 RX , priority 2, software controlled register switching (on MX1xx/MX2xx MCUs)
void __ISR( _UART_2_VECTOR, ipl2soft ) _InterruptHandler_U2RX(void) {
    // App Timing
    APP_USART_Read();
    // Clear the interrupt flag
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_SOURCE_USART_2_RECEIVE);
}

// Vector for UART 1 TX , priority 1, software controlled register switching (on MX1xx/MX2xx MCUs)
void __ISR( _UART_1_VECTOR, ipl1soft ) _InterruptHandler_U1TX(void) {
    // App Timing
    APP_USART_Write();
    // Clear the interrupt flag
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_SOURCE_USART_1_TRANSMIT);
}
#endif

#ifdef	__cplusplus
}
#endif
