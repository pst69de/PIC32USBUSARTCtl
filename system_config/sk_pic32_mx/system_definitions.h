/* 
 * File:    system_definitions.h
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-02-26
 * Description:
 *   not yet supplied
 * Major Changes:
 *   Version 0: alpha development
 */

#ifndef SYSTEM_DEFINITIONS_H
#define	SYSTEM_DEFINITIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "system/common/sys_module.h"
#include "system_config.h"
#include "usb/usb_device.h"

/* System Object Handles Structure */
typedef struct {
    /* USB Device Layer System Module Object*/
    SYS_MODULE_OBJ usbDevObject;
} SYSTEM_OBJECTS;

/* External forward reference for system objects structure */
extern SYSTEM_OBJECTS sysObjects;

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_DEFINITIONS_H */

