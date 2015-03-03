/*
 * File:   system_interrupt.c
 * Author: patrick
 *
 * Created on 2014-09-13
 */

#include <xc.h>
#include <sys/attribs.h>
#include "../../app.h"
#include "system_definitions.h"
#include "system_config.h"

/* 
 * priority in interrupts (7)6 = high 0 = low  
*/

void __ISR ( _USB_1_VECTOR,ipl4soft ) _InterruptHandler_USB ( void )
{
    USB_DEVICE_Tasks_ISR(sysObjects.usbDevObject);
}

/* Vector for I2C 1, priority 3, software controlled register switching (on MX1xx/MX2xx MCUs) */
void __ISR( _I2C_1_VECTOR, ipl3soft ) _InterruptHandler_I2C_1(void) {
	APP_LCD_Process();
    /* Clear the interrupt flag */
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_VECTOR_I2C1);
}

/* Vector for timer 1, priority 2, software controlled register switching (on MX1xx/MX2xx MCUs) */
void __ISR( _TIMER_1_VECTOR, ipl2soft ) _InterruptHandler_TMR_1(void) {
    /* App Timing */
    APP_TimingCallback();
    /* Clear the interrupt flag */
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_SOURCE_TIMER_1);
}
