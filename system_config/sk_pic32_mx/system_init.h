/* 
 * File:   system_init.h
 * Author: Patrick O. Ehrmann (pst69@pst69.de)
 *
 * Created on 2015-03-11
 *
 * Use:
 * Header file for system_init.c to integrate to a C++ environment
 */

#ifndef SYSTEM_INIT_H
#define	SYSTEM_INIT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "system_config.h"

void SYS_Initialize ( void *data );

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_INIT_H */

