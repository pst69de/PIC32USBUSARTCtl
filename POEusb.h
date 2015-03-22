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

// *****************************************************************************
// Section: USB controller Driver Configuration compiler macros
// *****************************************************************************
#ifdef APP_USE_USB
// Enables Device Support 
#define DRV_USB_DEVICE_SUPPORT               true
// Disables host support 
#define DRV_USB_HOST_SUPPORT                 false
// Provides 3 endpoints 
#define DRV_USB_ENDPOINTS_NUMBER             3
// Only one instance of the USB Peripheral
#define DRV_USB_INSTANCES_NUMBER             1
// Enables interrupt mode 
#define DRV_USB_INTERRUPT_MODE               true
// *****************************************************************************
// Section: USB Device Layer Configuration
// *****************************************************************************
// Maximum device layer instances 
#define USB_DEVICE_INSTANCES_NUMBER          1
// Maximum clients for Device Layer 
#define USB_DEVICE_CLIENTS_NUMBER            1
// EP0 size in bytes 
#define USB_DEVICE_EP0_BUFFER_SIZE           64
// *****************************************************************************
// Section: CDC Function Driver Configuration
// *****************************************************************************
// Maximum instances of CDC function driver 
#define USB_DEVICE_CDC_INSTANCES_NUMBER      1
// 
// CDC Transfer Queue Size for both read and
// write. Applicable to all instances of the
// function driver 
//
#define USB_DEVICE_CDC_QUEUE_DEPTH_COMBINED  3
// Application USB Device CDC Read Buffer Size. This should be a multiple of
// the CDC Bulk Endpoint size 
#define USB_BUFFER_SIZE                      APP_BUFFER_SIZE





#endif //ifdef APP_USE_USB

#ifdef	__cplusplus
}
#endif

#endif	/* POEUSB_H */
