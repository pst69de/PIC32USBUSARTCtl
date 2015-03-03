/*
 * File:   main.c
 * Author: patrick
 *
 * Created on 2014-09-13
 */
#include <stdbool.h>
#include <stdlib.h>

int main ( void )
{
    /* Initialize all MPLAB Harmony modules, including application(s). */
    // disable temporarily, if you wish to precompile
    SYS_Initialize( NULL );

    while ( true )
    {
        /* Maintain the state machines of all MPLAB Harmony modules. */
        // disable temporarily, if you wish to precompile
        SYS_Tasks( );
    }

    /* Execution should not come here during normal operation */
    return ( EXIT_FAILURE );
}
