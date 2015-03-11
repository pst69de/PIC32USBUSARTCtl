/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifdef __XC32
    #include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif

#include <peripheral/peripheral.h>            /* Include to use PIC32 peripheral libraries     */
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
