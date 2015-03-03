/* 
 * File:   system_definitions.h
 * Author: patrick
 *
 * Created on 2014-09-13
 */

#ifndef SYSTEM_DEFINITIONS_H
#define	SYSTEM_DEFINITIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "system/common/sys_module.h"

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

