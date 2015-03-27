/* 
 * File:    POEdio.h
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-26
 * Description:
 *   digital input output library
 *   This library uses polling.
 * Major Changes:
 *   Version 0: alpha development
 */

#ifndef POEDIO_H
#define	POEDIO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "system_config/sk_pic32_mx/system_config.h"

#ifdef APP_USE_DIO

// Initialize those by APP_DI_1..4 and APP_DO_1..4 defined pins
void DIO_Initialize(void);

int DIO_ReadDI(int num);

void DIO_SetDOto(int num, int val);
void DIO_SetDO(int num);
void DIO_ClearDO(int num);
void DIO_ToggleDO(int num);

#endif // ifdef APP_USE_DIO

#ifdef	__cplusplus
}
#endif

#endif	// ifndef POEDIO_H
