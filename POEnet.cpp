/* 
 * File:    POEnet.cpp
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-13
 * Description:
 *   Call interface tinyxml for app loop core
 * Major Changes:
 *   Version 0: alpha development
 */

#include <string.h>
#include "POEnet.h"
#include "tinyxml2.h"
#include "system_config.h"
#include "app.h"


tinyxml2::XMLDocument POEnetCommand( true, tinyxml2::COLLAPSE_WHITESPACE);
// const char for non-relevant command
const char POEnet_noop[] = "noop";
const char POEnet_reset[] = "reset";
const char POEnet_net[] = "net";
const char POEnet_node[] = "node";
const char POEnet_time[] = "time";
const char POEnet_digital[] = "digital";
const char POEnet_analog[] = "analog";
const char POEnet_switch[] = "switch";
const char POEnet_pwm[] = "pwm";
const char POEnet_text[] = "text";
const char POEnet_action[] = "action";

void POEnet_Interpret(const char *buffer) {
    POEnetCommand.Parse(buffer);
}

bool POEnet_GetError(char *tostring) {
    return false; // no error
}

void POEnet_Output(char *buffer) {
    tinyxml2::XMLPrinter myOut(true);
    POEnetCommand.Print(&myOut);
    strcpy(buffer,myOut.CStr());
    POEnetCommand.Clear();
}

void POEnet_GetCommand(char *tostring) {
    char myCommand[16];
    strcpy(tostring, POEnetCommand.RootElement()->Name());
}

void POEnet_GetNewNodeId(int *nodeid) {
    tinyxml2::XMLElement *aNode;
    tinyxml2::XMLElement *myNode;
    if (!strcmp("net", POEnetCommand.RootElement()->Name())) {
        aNode = POEnetCommand.RootElement()->FirstChildElement("node");
        if (!aNode) {
            // no node yet, we start with 0
            *nodeid = 0;
        } else {
            while (aNode->NextSiblingElement("node")) { aNode = aNode->NextSiblingElement("node");}
            aNode->QueryIntAttribute("id",nodeid);
            *nodeid++;
        }
        myNode = POEnetCommand.NewElement("node");
        myNode->SetAttribute("id",*nodeid);
        POEnetCommand.RootElement()->InsertEndChild(myNode);
    } else {
        // something's wrong -> do not touch old id
    }
}

void POEnet_SetNodeProperty(char *name, int propId, char *attributeList) {
    
}
