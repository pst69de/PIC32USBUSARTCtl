/* 
 * File:    user.cpp
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
/* Files to Include                                                           */
/******************************************************************************/

#ifdef __XC32
    #include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif

// only include distinct peripherals
//#include <peripheral/peripheral.h>            /* Include to use PIC32 peripheral libraries     */
#include <stdint.h>          /* For UINT32 definition                         */
#include <stdbool.h>         /* For true/false definition                     */
#include <exception>         /* Includes C++ try/catch functions              */

#include "user.hpp"          /* variables/params used by user.cpp             */

#include "system_init.h"
#include "app.h"

/* All the files in the C++ standard library declare its entities
 * within the std namespace. */
using namespace std;        /* use the standard namespace                     */

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* Initialize User Ports/Peripherals/Project here.  Users may want to
 * initialize peripheral classes here.  */

void InitApp(void)
{
    /* Setup analog functionality and port direction */
    /* Initialize peripherals */
    
    SYS_Initialize( NULL );
}

void RunApp(void)
{
    // centralized APP state machine (C code)
    APP_Tasks();
}
