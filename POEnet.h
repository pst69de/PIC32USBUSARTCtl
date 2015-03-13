/* 
 * File:   POEnet.h
 * Author: patrick
 *
 * Created on 2015-03-13
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

