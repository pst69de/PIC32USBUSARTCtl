/* 
 * File:    POEusb.h
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-22
 * Description:
 *   USB CDC library
 * Major Changes:
 *   Version 0: alpha development
 */

#ifndef POEUSB_H
#define	POEUSB_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "system_config.h"

#ifdef APP_USE_USB


// these USB driver definitions have to placed in system_config.h
// as this is refered by some of the USB driver code files
// *****************************************************************************
// Section: USB controller Driver Configuration compiler macros
// *****************************************************************************
// Enables Device Support
//#define DRV_USB_DEVICE_SUPPORT               true
// Disables host support
//#define DRV_USB_HOST_SUPPORT                 false
// Provides 3 endpoints
//#define DRV_USB_ENDPOINTS_NUMBER             3
// Only one instance of the USB Peripheral
//#define DRV_USB_INSTANCES_NUMBER             1
// Enables interrupt mode
//#define DRV_USB_INTERRUPT_MODE               true
// *****************************************************************************
// Section: USB Device Layer Configuration
// *****************************************************************************
// Maximum device layer instances 
//#define USB_DEVICE_INSTANCES_NUMBER          1
// Maximum clients for Device Layer 
//#define USB_DEVICE_CLIENTS_NUMBER            1
// EP0 size in bytes (must be defined before inclusion of USB headers)
//#define USB_DEVICE_EP0_BUFFER_SIZE           64
// *****************************************************************************
// Section: CDC Function Driver Configuration
// *****************************************************************************
// Maximum instances of CDC function driver 
//#define USB_DEVICE_CDC_INSTANCES_NUMBER      1
// CDC Transfer Queue Size for both read and
// write. Applicable to all instances of the
// function driver 
//#define USB_DEVICE_CDC_QUEUE_DEPTH_COMBINED  3
// index of CDC device driver
//#define USB_DEVICE_CDC_INDEX_0               0

// Application USB Device CDC Read Buffer Size. This should be a multiple of
// the CDC Bulk Endpoint size 
#define USB_BUFFER_SIZE                      APP_BUFFER_SIZE

// *****************************************************************************
// includes
// *****************************************************************************

#include "system/common/sys_module.h"

#include "usb/usb_device.h"
#include "usb/usb_device_cdc.h"

// *****************************************************************************
// possible external Variables
// *****************************************************************************

extern SYS_MODULE_OBJ usbDevObject;

// vvvv from app.h vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
extern const USB_DEVICE_FUNCTION_REGISTRATION_TABLE
    funcRegistrationTable[USB_DEVICE_CDC_INSTANCES_NUMBER];

extern const USB_DEVICE_MASTER_DESCRIPTOR usbMasterDescriptor;
// ^^^^ from app.h ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// *****************************************************************************
// Initialization routine
// *****************************************************************************

void USB_SYS_Init(void);

// *****************************************************************************
// ISR
// *****************************************************************************
// Interrupt framing routine and interrupt configuration are done in the APP
// As of high importance the ISR should have one of the highest priorities

void USB_SYS_ISR(void);

// *****************************************************************************
// permanent system task
// *****************************************************************************

void USB_SYS_Loop(void);

// vvvv from app.h vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// *****************************************************************************
// routines and declarations for APP (Application Callback Routines)
// *****************************************************************************
/* (obsolete)
  Function:
    void APP_UsbDeviceEventCallBack(USB_DEVICE_EVENTS events)

  Summary:
    Device layer event notification callback.

  Description:
    This routine defines the device layer event notification callback.

  Precondition:
    The device layer should be opened by the application and the callback should
    be registered with the device layer.

  Parameters:
    events  - specific device event

  Returns:
    None.

  Remarks:
    None.
*/

bool USB_StateReset ( void );

void USB_DeviceEventCallBack(
    USB_DEVICE_EVENT events
,   void * eventData, uintptr_t context
);

void USB_DeviceCDCEventHandler (
    USB_DEVICE_CDC_INDEX index 
,   USB_DEVICE_CDC_EVENT event 
,   void * pData
,   uintptr_t userData
);

void USB_DeviceEventHandler ( 
    USB_DEVICE_EVENT event
,   void * eventData
,   uintptr_t context
);

USB_DEVICE_CDC_EVENT_RESPONSE USB_DeviceCDCEventHandler (
    USB_DEVICE_CDC_INDEX index
,   USB_DEVICE_CDC_EVENT event
,   void * pData
,   uintptr_t userData
);
// ^^^^ from app.h ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// *****************************************************************************
// APP Interface routines 
// *****************************************************************************

// Open the USB Device (answer true if successful, else repeat trying) 
bool USB_Open(void);

// Gets true, if the CDC was successfully registered
bool USB_isConfigured(void);
// Gets true, if the device is attached to a host (powered)
bool USB_isAttached(void);

// prepare to write
void USB_PrepareWrite(void * buffer, int size);
// is data written to host
bool USB_isWriteComplete(void);

// is data available
bool USB_isReadComplete(void);
// read Data to buffer, true if data was read
bool USB_ReadToBuffer(void * buffer, int maxSize);

#endif //ifdef APP_USE_USB

#ifdef	__cplusplus
}
#endif

#endif	/* POEUSB_H */
