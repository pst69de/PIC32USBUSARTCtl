/*
 * File:    system_init.h
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-11
 * Description:
 *   Header file for system_init.c to integrate to a C++ environment
 * Major Changes:
 *   Version 0: alpha development
 */

#ifndef SYSTEM_INIT_H
#define	SYSTEM_INIT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "system_config.h"

void SYS_Initialize ( void *data );

void SYS_USB_DEVICE_ISR(void);

//SYSTEM_OBJECTS sysObjects;

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_INIT_H */

