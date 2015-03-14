/* 
 * File:    main.cpp
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-11
 * Description:
 *   not yet supplied
 * Major Changes:
 *   Version 0: alpha development
 */
/******************************************************************************/
/*  Files to Include                                                          */
/******************************************************************************/

#ifdef __XC32
    #include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
#include <GenericTypeDefs.h>

// only include distinct peripherals
//#include <peripheral/peripheral.h>           /* Include to use PIC32 peripheral libraries      */
#include <stdint.h>         /* For UINT32 definition                          */
#include <stdbool.h>        /* For true/false definition                      */
#include <exception>        /* Includes C++ try/catch functions               */

#include "system.hpp"       /* System funct/params, like osc/periph config    */
#include "user.hpp"         /* User funct/params, such as InitApp             */

/* All the files in the C++ standard library declare its entities
 * within the std namespace. */
using namespace std;        /* use the standard namespace                     */

/******************************************************************************/
/* Global Variable/Classes Declaration                                        */
/******************************************************************************/

/* TODO add variables and classes. These can be organized many ways.  */

/* i.e. INT32 <variable_name>; */

/* Simple C++ class example */
/*
 * class c_rectangle
 * {
 *     UINT32 width, height;
 *   public:
 *     // The class constructor, note no return parameter
 *     c_rectangle(int,int); 
 *     // area() may be called with a c_rectangle type variable var.area();
 *     UINT32 area() {return (width*height);}
 * };
 *
 * // class constructor, :: is the scope operator
 * c_rectangle::c_rectangle(int w,int h) 
 * {
 *    width=w;
 *    height=h;
 * } */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

INT main(void)
{

#ifndef PIC32_STARTER_KIT
    /*The JTAG is on by default on POR.  A PIC32 Starter Kit uses the JTAG, but
    for other debug tool use, like ICD 3 and Real ICE, the JTAG should be off
    to free up the JTAG I/O */
    DDPCONbits.JTAGEN = 0;
#endif

    /*Refer to the C32/XC32 peripheral library documentation for more
    information on the SYTEMConfig function.

    This function sets the PB divider, the Flash Wait States, and the DRM
    /wait states to the optimum value.  It also enables the cacheability for
    the K0 segment.  It could has side effects of possibly alter the pre-fetch
    buffer and cache.  It sets the RAM wait states to 0.  Other than
    the SYS_FREQ, this takes these parameters.  The top 3 may be '|'ed
    together:

    SYS_CFG_WAIT_STATES (configures flash wait states from system clock)
    SYS_CFG_PB_BUS (configures the PB bus from the system clock)
    SYS_CFG_PCACHE (configures the pCache if used)
    SYS_CFG_ALL (configures the flash wait states, PB bus, and pCache)*/
    // Not needed in particular, should be done by SYS_Initialize() 
    //SYSTEMConfig(SYS_FREQ, SYS_CFG_ALL);
    /* TODO Add user clock/system configuration code if appropriate.  */

    /* Initialize I/O and Peripherals for application */
    // defined in user.hpp/user.cpp redirects to the SYS_Initialize
    InitApp();

    /*Configure Multivector Interrupt Mode.  Using Single Vector Mode
    is expensive from a timing perspective, so most applications
    should probably not use a Single Vector Mode*/
    // not needed in particular
    //INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);

    while(1)
    {
        /* RunApp of user.cpp */
        RunApp();
    }
}


