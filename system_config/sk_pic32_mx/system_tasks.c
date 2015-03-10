/*
 * File:   system_tasks.c
 * Author: patrick
 *
 * Created on 2014-09-13
 */

#include "../../app.h"
#include "system_definitions.h"
#include "system_config.h"

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
