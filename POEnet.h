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

#ifdef	__cplusplus
extern "C" {
#endif

void POEnet_Interpret(const char *buffer);

void POEnet_Output(char *buffer);

void POEnet_GetCommand(char *tostring);

void POEnet_GetNewNodeId(int *nodeid);

#ifdef	__cplusplus
}
#endif

#endif	/* POENET_H */

