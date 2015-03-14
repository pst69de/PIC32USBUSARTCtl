/* 
 * File:    system.cpp
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-11
 * Description:
 *   maybe obsolete
 *   not yet supplied
 * Major Changes:
 *   Version 0: alpha development
 */
/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifdef __XC32
    #include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif

// only include distinct peripherals
//#include <peripheral/peripheral.h>            /* Include to use PIC32 peripheral libraries     */
#include <stdint.h>          /* For UINT32 definition                         */
#include <stdbool.h>         /* For true/false definition                     */
#include <exception>         /* Includes try/catch functions                  */

#include "system.hpp"        /* variables/params used by system.cpp           */

/******************************************************************************/
/* System Level Functions                                                     */
/*                                                                            */
/* Custom oscillator configuration funtions, reset source evaluation          */
/* functions, and other non-peripheral microcontroller initialization         */
/* functions get placed in system.cpp                                         */
/*                                                                            */
/******************************************************************************/

using namespace _system;

/* <Initialize variables in system.hpp and put code for system functs here.>  */
