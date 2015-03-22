/* 
 * File:    POEusb.c
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-20
 * Description:
 *   USB CDC library
 * Major Changes:
 *   Version 0: alpha development
 */

#include "POEi2clcd.h"

#ifdef APP_USE_USB
#include "system/common/sys_module.h"
#include "usb/usb_device.h"


// *****************************************************************************
// global variable declarations (possibly extern in Header)
// *****************************************************************************
SYS_MODULE_OBJ usbDevObject;

// *****************************************************************************
// Initialization routine
// *****************************************************************************

void USB_SYS_Init(void) {
    // Initialize the USB device layer
    usbDevObject = USB_DEVICE_Initialize (USB_DEVICE_INDEX_0 , ( SYS_MODULE_INIT* ) & usbDevInitData);
    // Check if the object returned by the device layer is valid
    SYS_ASSERT((SYS_MODULE_OBJ_INVALID != sysObjects.usbDevObject), "Invalid USB DEVICE object");
    
}

// *****************************************************************************
// ISR
// *****************************************************************************

void USB_SYS_ISR(void);

// *****************************************************************************
// permanent system task
// *****************************************************************************

void USB_SYS_Loop(void) {
    // Device layer tasks routine. CDC tasks gets called from device layer tasks 
    USB_DEVICE_Tasks(usbDevObject);
}


#endif //ifdef APP_USE_USB
