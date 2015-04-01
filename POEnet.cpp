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

tinyxml2::XMLDocument POEnetNode( true, tinyxml2::COLLAPSE_WHITESPACE);
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
int * node_id; 

void POEnet_Node_Init(int * id, char * name) {
    node_id = id;
    POEnetNode.Clear();
    tinyxml2::XMLElement *elemNode = POEnetNode.NewElement( &POEnet_node[0]);
    elemNode->SetAttribute("id",*id);
    elemNode->SetAttribute("name",name);
    POEnetNode.InsertFirstChild( elemNode);
}


void POEnet_AddAnalog(int id, float * Value, float * numerator, float * denominator, char * unit) {
    tinyxml2::XMLElement *elemNode = POEnetNode.NewElement( &POEnet_analog[0]); 
    elemNode->SetAttribute("id", id);
    // vv Replace by XMLRefAttribute
    elemNode->SetAttribute("value", *Value);
    // vv Replace by XMLRefAttribute
    elemNode->SetAttribute("numerator", *numerator);
    // vv Replace by XMLRefAttribute
    elemNode->SetAttribute("denominator", *denominator);
    // vv Replace by XMLRefAttribute
    elemNode->SetAttribute("unit", *denominator);
    POEnetNode.RootElement->InsertEndChild( elemNode);
}

void POEnet_AddDigital(int id, int * Value, char * loVal, char * hiVal) {
    tinyxml2::XMLElement *elemNode = POEnetNode.NewElement( &POEnet_digital[0]); 
    elemNode->SetAttribute("id", id);
    // vv Replace by XMLRefAttribute
    elemNode->SetAttribute("value", *Value);
    // vv Replace by XMLRefAttribute
    elemNode->SetAttribute("numerator", *numerator);
    // vv Replace by XMLRefAttribute
    elemNode->SetAttribute("denominator", *denominator);
    // vv Replace by XMLRefAttribute
    elemNode->SetAttribute("unit", *denominator);
    POEnetNode.RootElement->InsertEndChild( elemNode);
}

void POEnet_AddSwitch(int id, int * Value, char * loVal, char * hiVal) {
    tinyxml2::XMLElement *elemNode = POEnetNode.NewElement( &POEnet_switch[0]); 
    elemNode->SetAttribute("id", id);
    // vv Replace by XMLRefAttribute
    elemNode->SetAttribute("value", *Value);
    // vv Replace by XMLRefAttribute
    elemNode->SetAttribute("numerator", *numerator);
    // vv Replace by XMLRefAttribute
    elemNode->SetAttribute("denominator", *denominator);
    // vv Replace by XMLRefAttribute
    elemNode->SetAttribute("unit", *denominator);
    POEnetNode.RootElement->InsertEndChild( elemNode);
}


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
    char myCommand[APP_STRING_SIZE];
    strcpy(myCommand, POEnetCommand.RootElement()->Name());
    if (!strcmp(&myCommand[0], &POEnet_reset[0])) {
        strcpy(tostring, &POEnet_noop[0]);
    } else {
        strcpy(tostring, &myCommand[0]);
    }
}

void POEnet_GetNewNodeId(int *nodeid) {
    tinyxml2::XMLElement *aNode;
    tinyxml2::XMLElement *myNode;
    if (!strcmp("net", POEnetCommand.RootElement()->Name())) {
        aNode = POEnetCommand.RootElement()->FirstChildElement("node");
        if (!aNode) {
            // no node yet, we start with 1
            *nodeid = 1;
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

