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
#include "app.h"


tinyxml2::XMLDocument POEnetCommand;

void POEnet_Interpret(const char *buffer) {
    POEnetCommand.Parse(buffer);
}

void POEnet_Output(char *buffer) {
    tinyxml2::XMLPrinter myOut;
    POEnetCommand.Print(&myOut);
    strcpy(buffer,myOut.CStr());
    POEnetCommand.Clear();
}

void POEnet_GetCommand(char *tostring) {
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

