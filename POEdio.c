/* 
 * File:    POEdio.c
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-26
 * Description:
 *   digital input output library
 *   This library uses polling.
 * Major Changes:
 *   Version 0: alpha development
 */

#include "POEdiopins.h"
#include "POEdio.h"

#ifdef APP_USE_DIO

#include "peripheral/adc/plib_ports.h"

PORTS_CHANNEL  pin2channel[DIO_PIN_COUNT] = {
    PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_B
,   PORT_CHANNEL_A
,   PORT_CHANNEL_A
,   PORT_CHANNEL_A
,   PORT_CHANNEL_A
,   PORT_CHANNEL_A
};

PORTS_BIT_POS  pin2portpos[DIO_PIN_COUNT] = {
    PORTS_BIT_POS_0
,   PORTS_BIT_POS_1
,   PORTS_BIT_POS_2
,   PORTS_BIT_POS_3
,   PORTS_BIT_POS_4
,   PORTS_BIT_POS_5
,   PORTS_BIT_POS_6
,   PORTS_BIT_POS_7
,   PORTS_BIT_POS_8
,   PORTS_BIT_POS_9
,   PORTS_BIT_POS_10
,   PORTS_BIT_POS_11
,   PORTS_BIT_POS_12
,   PORTS_BIT_POS_13
,   PORTS_BIT_POS_14
,   PORTS_BIT_POS_15
,   PORTS_BIT_POS_0
,   PORTS_BIT_POS_1
,   PORTS_BIT_POS_2
,   PORTS_BIT_POS_3
,   PORTS_BIT_POS_4
};

int DIO_ReadDI(int num) {
    return 0;
}

void DIO_SetDOto(int num, int val) {
    switch (num) {
#ifdef APP_DO_2
        2:
            break;
#endif
#ifdef APP_DO_3
        3:
            break;
#endif
#ifdef APP_DO_4
        4:
            if (val) {
                PLIB_PORTS_PinSet(PORTS_ID_0, pin2channel[APP_DO_4], pin2portpos[APP_DO_4]);
            } else {
                PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DO_4], pin2portpos[APP_DO_4]);
            }
            break;
#endif
        default: 
            if (val) {
                PLIB_PORTS_PinSet(PORTS_ID_0, pin2channel[APP_DO_1], pin2portpos[APP_DO_1]);
            } else {
                PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DO_1], pin2portpos[APP_DO_1]);
            }
            break;
    }
}

void DIO_SetDO(int num) {
    
}

void DIO_ClearDO(int num) {
    
}

void DIO_ToggleDO(int num) {
    
}


#endif // ifdef APP_USE_DIO
