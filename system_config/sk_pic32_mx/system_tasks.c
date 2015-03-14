/*
 * File:    system_tasks.c
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-02-26
 * Description:
 *   not yet supplied
 * Major Changes:
 *   Version 0: alpha development
 */

#include "system_definitions.h"

void SYS_Tasks ( void )
{
    /* TODO:  Call the state machines of all modules and the application. */
#ifdef APP_USE_USB
    // Device layer tasks routine. CDC tasks gets called from device layer tasks 
    USB_DEVICE_Tasks(sysObjects.usbDevObject);
#endif
    /* Maintain the application's state machine. */
    APP_Tasks();
}
