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
const char POEnet_empty[] = "       ";
const char POEnet_error[] = "!error!";
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
int  *node_id;
char *node_name;
int  *time_hours;
int  *time_minutes;
int  *time_seconds;

void POEnet_Node_Init(int *id, char *name, int *hours, int *minutes, int *seconds) {
    node_id = id;
    node_name = name;
    time_hours = hours;
    time_minutes = minutes;
    time_seconds = seconds;
    POEnetNode.Clear();
    tinyxml2::XMLElement *elemNode = POEnetNode.NewElement( &POEnet_node[0]);
    elemNode->SetAttribute("id",*node_id);
    elemNode->SetAttribute("name",node_name);
    POEnetNode.InsertFirstChild( elemNode);
}


void POEnet_AddAnalog(int id, float *Value, float *numerator, float *denominator, char *unit) {
    tinyxml2::XMLElement *elemAnalog = POEnetNode.NewElement( &POEnet_analog[0]); 
    elemAnalog->SetAttribute("id", id);
    // vv Replace by XMLRefAttribute
    elemAnalog->SetAttribute("value", Value);
    // vv Replace by XMLRefAttribute
    elemAnalog->SetAttribute("numerator", numerator);
    // vv Replace by XMLRefAttribute
    elemAnalog->SetAttribute("denominator", denominator);
    // vv Replace by XMLRefAttribute
    elemAnalog->SetAttribute("unit", unit, APP_STRING_SIZE);
    POEnetNode.RootElement()->InsertEndChild( elemAnalog);
}

void POEnet_AddDigital(int id, int *Value, char *loVal, char *hiVal) {
    tinyxml2::XMLElement *elemDigital = POEnetNode.NewElement( &POEnet_digital[0]); 
    elemDigital->SetAttribute("id", id);
    // vv Replace by XMLRefAttribute
    elemDigital->SetAttribute("value", Value);
    // vv Replace by XMLRefAttribute
    elemDigital->SetAttribute("lovalue", loVal, APP_STRING_SIZE);
    // vv Replace by XMLRefAttribute
    elemDigital->SetAttribute("hivalue", hiVal, APP_STRING_SIZE);
    POEnetNode.RootElement()->InsertEndChild( elemDigital);
}

void POEnet_AddSwitch(int id, int *Value, char *loVal, char *hiVal) {
    tinyxml2::XMLElement *elemSwitch = POEnetNode.NewElement( &POEnet_switch[0]); 
    elemSwitch->SetAttribute("id", id);
    // vv Replace by XMLRefAttribute
    elemSwitch->SetAttribute("value", Value);
    // vv Replace by XMLRefAttribute
    elemSwitch->SetAttribute("lovalue", loVal, APP_STRING_SIZE);
    // vv Replace by XMLRefAttribute
    elemSwitch->SetAttribute("hivalue", hiVal, APP_STRING_SIZE);
    POEnetNode.RootElement()->InsertEndChild( elemSwitch);
}

void POEnet_NodeDump(char *buffer) {
    tinyxml2::XMLPrinter myOut(true);
    POEnetNode.Print(&myOut);
    strncpy(buffer,myOut.CStr(),APP_BUFFER_SIZE);
}


void POEnet_Interpret(const char *buffer) {
    POEnetCommand.Parse(buffer);
    if (!POEnetCommand.Error()) {
        if (!strcmp(POEnetCommand.RootElement()->Name(), &POEnet_reset[0])) {
            // no thing to do, just pass on
        } else if (!strcmp(POEnetCommand.RootElement()->Name(), &POEnet_net[0])) {
            POEnet_GetNewNodeId();
        } else if (!strcmp(POEnetCommand.RootElement()->Name(), &POEnet_node[0])) {
            POEnet_DescribeNode();
        } else if (!strcmp(POEnetCommand.RootElement()->Name(), &POEnet_time[0])) {
            POEnet_SetTime();
        }
    }
}

bool POEnet_GetError(char *tostring) {
    if (POEnetCommand.Error()) {
        strncpy(tostring, POEnetCommand.ErrorName(), APP_STRING_SIZE);
        return true;
    }
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
    strncpy(myCommand, POEnetCommand.RootElement()->Name(), APP_STRING_SIZE);
    if (!strcmp(&myCommand[0], &POEnet_reset[0])) {
        strcpy(tostring, &POEnet_noop[0]);
    } else {
        strcpy(tostring, &myCommand[0]);
    }
}

void POEnet_GetNewNodeId() {
    tinyxml2::XMLElement *aNode;
    tinyxml2::XMLElement *myNode;
    aNode = POEnetCommand.RootElement()->FirstChildElement("node");
    if (!aNode) {
        // no node yet, we start with 1
        *node_id = 1;
    } else {
        while (aNode->NextSiblingElement("node")) { aNode = aNode->NextSiblingElement("node");}
        aNode->QueryIntAttribute("id",node_id);
        *node_id++;
    }
    myNode = POEnetCommand.NewElement("node");
    myNode->SetAttribute("id",*node_id);
    myNode->SetAttribute("name",node_name);
    POEnetCommand.RootElement()->InsertEndChild(myNode);
}

void POEnet_DescribeNode() {
    // copy all elements of POEnetNode.RootElement() to POEnetCommand.RootElement()
    tinyxml2::XMLElement *anElement;
    anElement = POEnetNode.RootElement()->FirstChildElement();
    while (anElement) {
        POEnetCommand.RootElement()->InsertEndChild( anElement->ShallowClone( &POEnetCommand));
        anElement = anElement->NextSiblingElement();
    }
}

void POEnet_SetTime() {
    // time command should be <time>08:57:32</time>
    char newTime[APP_STRING_SIZE];
    strncpy( &newTime[0], POEnetCommand.RootElement()->GetText(), APP_STRING_SIZE);
    newTime[2] = '\0';
    newTime[5] = '\0';
    newTime[8] = '\0';
    // Can ask XMLUtil::ToInt if conversion was successful
    tinyxml2::XMLUtil::ToInt( &newTime[0], time_hours);
    tinyxml2::XMLUtil::ToInt( &newTime[3], time_minutes);
    tinyxml2::XMLUtil::ToInt( &newTime[6], time_seconds);
}
