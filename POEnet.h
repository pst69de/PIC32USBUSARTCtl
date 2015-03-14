/* 
 * File:    POEnet.h
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-13
 * Description:
 *   Call interface tinyxml for app loop core
 * Major Changes:
 *   Version 0: alpha development
 */

#ifndef POENET_H
#define	POENET_H

#include "tinyxml2.h"

#ifdef	__cplusplus
extern "C" {
#endif

void POEnet_Interpret(const char *buffer);

char *POEnet_Command(void);

#ifdef	__cplusplus
}
#endif

tinyxml2::XMLDocument POEnetCommand;

#endif	/* POENET_H */

