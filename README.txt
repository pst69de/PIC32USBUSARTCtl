****************************************
  License 
****************************************
This project is open source. 
The license claimed on my (Patrick O. Ehrmann, pst69@pst69.de) parts of this code is creative commons zero (https://creativecommons.org/publicdomain/zero/1.0/).

There are parts within this project which claim another form of license, not disrupting the open source claim on my files. Respect the claims of other contributed within their files.

Parts within this project that provide other license claims:
----------------------------------------
TinyXML-2 project:
----------------------------------------
tinyxml2.h
tinyxml2.cpp

License
-------

TinyXML-2 is released under the zlib license:

This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must 
not claim that you wrote the original software. If you use this 
software in a product, an acknowledgment in the product documentation 
would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and 
must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source 
distribution.

Contributors
------------

Thanks very much to everyone who sends suggestions, bugs, ideas, and 
encouragement. It all helps, and makes this project fun.

The original TinyXML-1 has many contributors, who all deserve thanks
in shaping what is a very successful library. Extra thanks to Yves
Berquin and Andrew Ellerton who were key contributors.

TinyXML-2 grew from that effort. Lee Thomason is the original author
of TinyXML-2 (and TinyXML-1) but TinyXML-2 has been and is being improved
by many contributors.

Thanks to John Mackay at http://john.mackay.rosalilastudio.com for the TinyXML-2 logo!
----------------------------------------
----------------------------------------
These are patched files derived from the original to provide CPP compiler compatibility.
Microchip development platfrom MPLAB Harmony:
----------------------------------------
usb_TokenPID_Default.h
int_CPUCurrentPriorityLevel_Default.h
int_EnableControl_PIC32.h
int_INTCPUPriority_Default.h
int_INTCPUVector_Default.h
int_ShadowRegisterAssign_Unsupported.h
int_VariableOffset_Unsupported.h
int_VectorPriority_Default.h
osal_impl_basic.h
usart_LineControlMode_RXandTXCombined.h
usb_ALL_Interrupt_Default.h
usb_BDTFunctions_PIC32.h
usb_ERR_InterruptStatus_Default.h
usb_GEN_InterruptStatus_Default.h
usb_LastDirection_Default.h
usb_LastPingPong_Default.h
usb_LastTransactionDetails_Default.h
usb_PingPongMode_Unsupported.h

License:
----------------------------------------

Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
----------------------------------------
----------------------------------------

****************************************
Installation hints
****************************************

Source directory of a typical MPLABX project using MPLAB Harmony libraries.

Install MPLAB Harmony as proposed in C:\microchip\harmony\vX_YZ.
Make up projects folder C:\microchip\harmony.
Add the project folder PIC32USBUSARTCtl to the projects folder (like C:\microchip\harmony\projects\PIC32USBUSARTCtl).
Add an framework folder to your project (like C:\microchip\harmony\projects\PIC32USBUSARTCtl\framework).
Create an empty MPLABX project at your framework folder. This creates a MPLABX folder (like C:\microchip\harmony\projects\PIC32USBUSARTCtl\framework\PIC32USBUSARTCtl.X).
Create the repository folder src (like C:\microchip\harmony\projects\PIC32USBUSARTCtl\framework\src).
Clone the repository to the src folder.

The configuration.xml should be copied to C:\microchip\harmony\projects\PIC32USBUSARTCtl\framework\PIC32USBUSARTCtl.X\nbproject. Or set up the MPLABX project in equivalent way.

The files contained in harmony_mods should be copied to their respective places in the MPLAB Harmony library. In my actual installation these are:
C:\microchip\harmony\v1_03\framework\peripheral\int\templates
C:\microchip\harmony\v1_03\framework\peripheral\usart\templates
C:\microchip\harmony\v1_03\framework\peripheral\usb\templates
C:\microchip\harmony\v1_03\framework\osal\src

****************************************
Casual files
****************************************
There are files contained only for casual working purposes, don't mind the content for your project.
These are:
compiler_errors.txt, for resolving compiler issues later on


****************************************
Further Infomation
****************************************

see http://wiki69.pst69.homeip.net/index.php/PIC32_USB_USART_Controller
