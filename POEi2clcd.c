/* 
 * File:    POEi2clcd.c
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-20
 * Description:
 *   I2C LCD library
 * Major Changes:
 *   Version 0: alpha development
 */

#include "POEi2clcd.h"

// system_init calls

void LCD_Initialize(void) {
    PLIB_I2C_Disable(APP_LCD_I2C_ID);
    // Prepare Ports I2C 1  
    PLIB_PORTS_PinClear(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SCL_PIN);
    PLIB_PORTS_PinClear(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SDA_PIN);
    PLIB_PORTS_RemapOutput(APP_LCD_PORTS_ID, OUTPUT_FUNC_NO_CONNECT, OUTPUT_PIN_RPB8);
    // open drain configuration I2C 1 Pins 5V tolerant, pull up with 2k2
    PLIB_PORTS_PinOpenDrainEnable(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SCL_PIN);
    PLIB_PORTS_PinOpenDrainEnable(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SDA_PIN);
    //PLIB_PORTS_PinDirectionOutputSet(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SCL_PIN);
    //PLIB_PORTS_PinDirectionOutputSet(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SDA_PIN);
    // set the ports Input
    PLIB_PORTS_PinDirectionInputSet(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SCL_PIN);
    PLIB_PORTS_PinDirectionInputSet(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SDA_PIN);
    // Configure General I2C Options
    PLIB_I2C_SlaveClockStretchingEnable(APP_LCD_I2C_ID);
    // Disable to be called 
    PLIB_I2C_GeneralCallDisable(APP_LCD_I2C_ID);
    // future option SMB compatibility
    PLIB_I2C_SMBDisable(APP_LCD_I2C_ID);
    // future option high baud rates (>100kHz)
    PLIB_I2C_HighFrequencyEnable(APP_LCD_I2C_ID);
    // only 7-Bit addresses
    PLIB_I2C_ReservedAddressProtectDisable(APP_LCD_I2C_ID);
    // Idle when proc idle (no operation in idle mode)
    PLIB_I2C_StopInIdleDisable(APP_LCD_I2C_ID);
    // Set Desired baud rate
    PLIB_I2C_BaudRateSet( APP_LCD_I2C_ID, APP_SYSCLK_FREQ, APP_LCD_I2C_BAUD);
    // Set the appropriate slave address (slave only)
    //PLIB_I2C_SlaveAddress7BitSet(APP_LCD_I2C_ID, 0x7f);
    // Optional:  Clear and enable interrupts before enabling the I2C module.
    // Enable the module -> in SYS_Startup
}

// interrupt calls
 
// app calls


