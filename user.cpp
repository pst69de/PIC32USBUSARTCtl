/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifdef __XC32
    #include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif

#include <peripheral/peripheral.h>            /* Include to use PIC32 peripheral libraries     */
#include <stdint.h>          /* For UINT32 definition                         */
#include <stdbool.h>         /* For true/false definition                     */
#include <exception>         /* Includes C++ try/catch functions              */

#include "user.hpp"          /* variables/params used by user.cpp             */

/* All the files in the C++ standard library declare its entities
 * within the std namespace. */
using namespace std;        /* use the standard namespace                     */

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* TODO Initialize User Ports/Peripherals/Project here.  Users may want to
 * initialize peripheral classes here.  */

void InitApp(void)
{
    /* Setup analog functionality and port direction */

    /* Initialize peripherals */
}
