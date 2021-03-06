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

extern const char POEnet_noop[];
extern const char POEnet_reset[];
extern const char POEnet_net[];
extern const char POEnet_node[];
extern const char POEnet_time[];
extern const char POEnet_digital[];
extern const char POEnet_analog[];
extern const char POEnet_switch[];
extern const char POEnet_pwm[];
extern const char POEnet_text[];
extern const char POEnet_action[];


void POEnet_Interpret(const char *buffer);

bool POEnet_GetError(char *tostring);

void POEnet_Output(char *buffer);

void POEnet_GetCommand(char *tostring);

void POEnet_GetNewNodeId(int *nodeid);

void POEnet_SetNodeProperty(char *name, int propId, char *attributeList);

#ifdef	__cplusplus
}
#endif

#endif	/* POENET_H */

