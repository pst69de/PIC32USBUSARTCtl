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

#include "peripheral/ports/plib_ports.h"

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

void DIO_PinModeSelectDigital(int num) {
    switch (num) {
        case DIO_PIN_B0:
            PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_2, PORTS_PIN_MODE_DIGITAL);
        case DIO_PIN_B1:
            PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_3, PORTS_PIN_MODE_DIGITAL);
        case DIO_PIN_B2:
            PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_4, PORTS_PIN_MODE_DIGITAL);
        case DIO_PIN_B3:
            PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_5, PORTS_PIN_MODE_DIGITAL);
#ifdef __32MX150F128B__
        case DIO_PIN_B12:
            PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_12, PORTS_PIN_MODE_DIGITAL);
#endif
        case DIO_PIN_B13:
            PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_11, PORTS_PIN_MODE_DIGITAL);
        case DIO_PIN_B14:
            PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_10, PORTS_PIN_MODE_DIGITAL);
        case DIO_PIN_B15:
            PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_9, PORTS_PIN_MODE_DIGITAL);
        default:
            break;
    }
}

void DIO_Initialize(void) {
#ifdef APP_DO_1
    PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DO_1], pin2portpos[APP_DO_1]);
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, pin2channel[APP_DO_1], pin2portpos[APP_DO_1]);
    DIO_PinModeSelectDigital(APP_DO_1);
    APP_DO_1_OD;
#endif
#ifdef APP_DO_2
    PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DO_2], pin2portpos[APP_DO_2]);
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, pin2channel[APP_DO_2], pin2portpos[APP_DO_2]);
    DIO_PinModeSelectDigital(APP_DO_2);
    APP_DO_2_OD;
#endif
#ifdef APP_DO_3
    PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DO_3], pin2portpos[APP_DO_3]);
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, pin2channel[APP_DO_3], pin2portpos[APP_DO_3]);
    DIO_PinModeSelectDigital(APP_DO_3);
    APP_DO_3_OD;
#endif
#ifdef APP_DO_4
    PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DO_4], pin2portpos[APP_DO_4]);
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, pin2channel[APP_DO_4], pin2portpos[APP_DO_4]);
    DIO_PinModeSelectDigital(APP_DO_4);
    APP_DO_4_OD;
#endif
#ifdef APP_DI_1
    PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DI_1], pin2portpos[APP_DI_1]);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, pin2channel[APP_DI_1], pin2portpos[APP_DI_1]);
    DIO_PinModeSelectDigital(APP_DI_1);
#endif
#ifdef APP_DI_2
    PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DI_2], pin2portpos[APP_DI_2]);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, pin2channel[APP_DI_2], pin2portpos[APP_DI_2]);
    DIO_PinModeSelectDigital(APP_DI_2);
#endif
#ifdef APP_DI_3
    PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DI_3], pin2portpos[APP_DI_3]);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, pin2channel[APP_DI_3], pin2portpos[APP_DI_3]);
    DIO_PinModeSelectDigital(APP_DI_3);
#endif
#ifdef APP_DI_4
    PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DI_4], pin2portpos[APP_DI_4]);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, pin2channel[APP_DI_4], pin2portpos[APP_DI_4]);
    DIO_PinModeSelectDigital(APP_DI_4);
#endif
}


int DIO_ReadDI(int num) {
    int retVal = 0;
    switch (num) {
#ifdef APP_DI_2
        case 2:
            if (PLIB_PORTS_PinGet(PORTS_ID_0,pin2channel[APP_DI_2], pin2portpos[APP_DI_2])) {
                retVal = 1;
            }
            break;
#endif
#ifdef APP_DI_3
        case 3:
            if (PLIB_PORTS_PinGet(PORTS_ID_0,pin2channel[APP_DI_3], pin2portpos[APP_DI_3])) {
                retVal = 1;
            }
            break;
#endif
#ifdef APP_DI_4
        case 4:
            if (PLIB_PORTS_PinGet(PORTS_ID_0,pin2channel[APP_DI_4], pin2portpos[APP_DI_4])) {
                retVal = 1;
            }
            break;
#endif
        default:
            if (PLIB_PORTS_PinGet(PORTS_ID_0,pin2channel[APP_DI_1], pin2portpos[APP_DI_1])) {
                retVal = 1;
            }
            break;
    }
    return retVal;
}

void DIO_SetDOto(int num, int val) {
    switch (num) {
#ifdef APP_DO_2
        case 2:
            if (val) {
                PLIB_PORTS_PinSet(PORTS_ID_0, pin2channel[APP_DO_2], pin2portpos[APP_DO_2]);
            } else {
                PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DO_2], pin2portpos[APP_DO_2]);
            }
            break;
#endif
#ifdef APP_DO_3
        case 3:
            if (val) {
                PLIB_PORTS_PinSet(PORTS_ID_0, pin2channel[APP_DO_3], pin2portpos[APP_DO_3]);
            } else {
                PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DO_3], pin2portpos[APP_DO_3]);
            }
            break;
#endif
#ifdef APP_DO_4
        case 4:
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
    switch (num) {
#ifdef APP_DO_2
        case 2:
            PLIB_PORTS_PinSet(PORTS_ID_0, pin2channel[APP_DO_2], pin2portpos[APP_DO_2]);
            break;
#endif
#ifdef APP_DO_3
        case 3:
            PLIB_PORTS_PinSet(PORTS_ID_0, pin2channel[APP_DO_3], pin2portpos[APP_DO_3]);
            break;
#endif
#ifdef APP_DO_4
        case 4:
            PLIB_PORTS_PinSet(PORTS_ID_0, pin2channel[APP_DO_4], pin2portpos[APP_DO_4]);
            break;
#endif
        default: 
            PLIB_PORTS_PinSet(PORTS_ID_0, pin2channel[APP_DO_1], pin2portpos[APP_DO_1]);
            break;
    }
}

void DIO_ClearDO(int num) {
    switch (num) {
#ifdef APP_DO_2
        case 2:
            PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DO_2], pin2portpos[APP_DO_2]);
            break;
#endif
#ifdef APP_DO_3
        case 3:
            PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DO_3], pin2portpos[APP_DO_3]);
            break;
#endif
#ifdef APP_DO_4
        case 4:
            PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DO_4], pin2portpos[APP_DO_4]);
            break;
#endif
        default: 
            PLIB_PORTS_PinClear(PORTS_ID_0, pin2channel[APP_DO_1], pin2portpos[APP_DO_1]);
            break;
    }
}

void DIO_ToggleDO(int num) {
    switch (num) {
#ifdef APP_DO_2
        case 2:
            PLIB_PORTS_PinToggle(PORTS_ID_0, pin2channel[APP_DO_2], pin2portpos[APP_DO_2]);
            break;
#endif
#ifdef APP_DO_3
        case 3:
            PLIB_PORTS_PinToggle(PORTS_ID_0, pin2channel[APP_DO_3], pin2portpos[APP_DO_3]);
            break;
#endif
#ifdef APP_DO_4
        case 4:
            PLIB_PORTS_PinToggle(PORTS_ID_0, pin2channel[APP_DO_4], pin2portpos[APP_DO_4]);
            break;
#endif
        default: 
            PLIB_PORTS_PinToggle(PORTS_ID_0, pin2channel[APP_DO_1], pin2portpos[APP_DO_1]);
            break;
    }
}


#endif // ifdef APP_USE_DIO
