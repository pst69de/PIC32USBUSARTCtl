/*
 * File:   system_tasks.c
 * Author: patrick
 *
 * Created on 2014-09-13
 */

#include "../../app.h"
#include "system_definitions.h"

void SYS_Tasks ( void )
{
    /* TODO:  Call the state machines of all modules and the application. */
    /* Device layer tasks routine. CDC tasks gets called from device layer tasks */
    USB_DEVICE_Tasks(sysObjects.usbDevObject);

    /* Maintain the application's state machine. */
    APP_Tasks();
}
