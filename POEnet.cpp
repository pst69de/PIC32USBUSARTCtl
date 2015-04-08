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
const char POEnet_id[] = "id";
const char POEnet_name[] = "name";
const char POEnet_time[] = "time";
const char POEnet_analog[] = "analog";
const char POEnet_value[] = "value";
const char POEnet_numerator[] = "numerator";
const char POEnet_denominator[] = "denominator";
const char POEnet_unit[] = "unit";
const char POEnet_digital[] = "digital";
const char POEnet_lovalue[] = "lovalue";
const char POEnet_hivalue[] = "hivalue";
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
    elemNode->SetAttribute(&POEnet_id[0],*node_id);
    elemNode->SetAttribute(&POEnet_name[0],node_name);
    POEnetNode.InsertFirstChild( elemNode);
}


void POEnet_AddAnalog(int id, float *Value, float *numerator, float *denominator, char *unit) {
    tinyxml2::XMLElement *elemAnalog = POEnetNode.NewElement( &POEnet_analog[0]); 
    elemAnalog->SetAttribute(&POEnet_id[0], id);
    // vv Replace by XMLRefAttribute
    elemAnalog->SetAttribute(&POEnet_value[0], Value);
    // vv Replace by XMLRefAttribute
    elemAnalog->SetAttribute(&POEnet_numerator[0], numerator);
    // vv Replace by XMLRefAttribute
    elemAnalog->SetAttribute(&POEnet_denominator[0], denominator);
    // vv Replace by XMLRefAttribute
    elemAnalog->SetAttribute(&POEnet_unit[0], unit, APP_STRING_SIZE);
    POEnetNode.RootElement()->InsertEndChild( elemAnalog);
}

void POEnet_AddDigital(int id, int *Value, char *loVal, char *hiVal) {
    tinyxml2::XMLElement *elemDigital = POEnetNode.NewElement( &POEnet_digital[0]); 
    elemDigital->SetAttribute(&POEnet_id[0], id);
    // vv Replace by XMLRefAttribute
    elemDigital->SetAttribute(&POEnet_value[0], Value);
    // vv Replace by XMLRefAttribute
    elemDigital->SetAttribute(&POEnet_lovalue[0], loVal, APP_STRING_SIZE);
    // vv Replace by XMLRefAttribute
    elemDigital->SetAttribute(&POEnet_hivalue[0], hiVal, APP_STRING_SIZE);
    POEnetNode.RootElement()->InsertEndChild( elemDigital);
}

void POEnet_AddSwitch(int id, int *Value, char *loVal, char *hiVal) {
    tinyxml2::XMLElement *elemSwitch = POEnetNode.NewElement( &POEnet_switch[0]); 
    elemSwitch->SetAttribute(&POEnet_id[0], id);
    // vv Replace by XMLRefAttribute
    elemSwitch->SetAttribute(&POEnet_value[0], Value);
    // vv Replace by XMLRefAttribute
    elemSwitch->SetAttribute(&POEnet_lovalue[0], loVal, APP_STRING_SIZE);
    // vv Replace by XMLRefAttribute
    elemSwitch->SetAttribute(&POEnet_hivalue[0], hiVal, APP_STRING_SIZE);
    POEnetNode.RootElement()->InsertEndChild( elemSwitch);
}

void POEnet_NodeDump(char *buffer) {
    tinyxml2::XMLPrinter myOut(true);
    POEnetNode.Print(&myOut);
    strncpy(buffer,myOut.CStr(),APP_BUFFER_SIZE);
}

void POEnet_GetNewNodeId() {
    tinyxml2::XMLElement *aNode;
    tinyxml2::XMLElement *myNode;
    aNode = POEnetCommand.RootElement()->FirstChildElement(&POEnet_node[0]);
    if (!aNode) {
        // no node yet, we start with 1
        *node_id = 1;
    } else {
        while (aNode->NextSiblingElement(&POEnet_node[0])) { aNode = aNode->NextSiblingElement(&POEnet_node[0]);}
        aNode->QueryIntAttribute(&POEnet_id[0],node_id);
        *node_id++;
    }
    myNode = POEnetCommand.NewElement(&POEnet_node[0]);
    myNode->SetAttribute(&POEnet_id[0],*node_id);
    myNode->SetAttribute(&POEnet_name[0],node_name);
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

void POEnet_GetAnalog(tinyxml2::XMLElement *eleAnalog) {
    // POETODO: checking for numerator, denominator, unit settings
    int myId = eleAnalog->IntAttribute( &POEnet_id[0]);
    if (!myId) { myId = 1; } // if not (or invalid) specified take 1st 
    tinyxml2::XMLElement *myAnalog;
    myAnalog = POEnetNode.RootElement()->FirstChildElement(&POEnet_analog[0]);
    while (myAnalog) {
        if (myAnalog->IntAttribute( &POEnet_id[0]) == myId) {
            eleAnalog->InsertFirstChild( eleAnalog->GetDocument()->NewText(myAnalog->Attribute(&POEnet_value[0])));
            myAnalog = 0;
        } else {
            myAnalog = myAnalog->NextSiblingElement(&POEnet_analog[0]);
        }
    }
}

void POEnet_GetDigital(tinyxml2::XMLElement *eleDigital) {
    // POETODO: checking for lovalue, hivalue settings; translation with loValue & hiValue 
    int myId = eleDigital->IntAttribute( &POEnet_id[0]);
    if (!myId) { myId = 1; } // if not (or invalid) specified take 1st 
    tinyxml2::XMLElement *myDigital;
    myDigital = POEnetNode.RootElement()->FirstChildElement(&POEnet_digital[0]);
    while (myDigital) {
        if (myDigital->IntAttribute( &POEnet_id[0]) == myId) {
            eleDigital->InsertFirstChild( eleDigital->GetDocument()->NewText(myDigital->Attribute(&POEnet_value[0])));
            myDigital = 0;
        } else {
            myDigital = myDigital->NextSiblingElement(&POEnet_digital[0]);
        }
    }
}

void POEnet_SetSwitch(tinyxml2::XMLElement *eleSwitch) {
    // POETODO: checking for lovalue, hivalue settings; translation with loValue & hiValue 
    int myId = eleSwitch->IntAttribute( &POEnet_id[0]);
    int newValue;
    if (!myId) { myId = 1; } // if not (or invalid) specified take 1st 
    tinyxml2::XMLElement *mySwitch;
    mySwitch = POEnetNode.RootElement()->FirstChildElement(&POEnet_switch[0]);
    while (mySwitch) {
        if (mySwitch->IntAttribute( &POEnet_id[0]) == myId) {
            if (!eleSwitch->QueryIntText(&newValue)) {
                // Only if resolvable value; translation with loValue & hiValue not yet implemented
                mySwitch->SetAttribute( &POEnet_value[0], newValue);
            }
            mySwitch = 0;
        } else {
            mySwitch = mySwitch->NextSiblingElement(&POEnet_switch[0]);
        }
    }
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
        } else if (!strcmp(POEnetCommand.RootElement()->Name(), &POEnet_analog[0])) {
            if (POEnetCommand.RootElement()->IntAttribute(&POEnet_node[0]) == *node_id) {
                POEnet_GetAnalog(POEnetCommand.RootElement());
            }
        } else if (!strcmp(POEnetCommand.RootElement()->Name(), &POEnet_digital[0])) {
            if (POEnetCommand.RootElement()->IntAttribute(&POEnet_node[0]) == *node_id) {
                POEnet_GetDigital(POEnetCommand.RootElement());
            }
        } else if (!strcmp(POEnetCommand.RootElement()->Name(), &POEnet_switch[0])) {
            if (POEnetCommand.RootElement()->IntAttribute(&POEnet_node[0]) == *node_id) {
                POEnet_SetSwitch(POEnetCommand.RootElement());
                // POETODO: Clear element and signal to state machine
            }
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

