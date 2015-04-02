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

#ifdef APP_LCD_I2C_ID

#include <string.h>

// global control variables
I2C_States        I2C_State;
I2C_Data_State    I2C_Transfer;
LCD_Init_Sequence LCD_InitState;
int               Waits;
int               lastWait;
uint8_t           LCD_Write[LCD_I2C_WRITE_BUFFER_SIZE];
int               LCD_WriteIx;
uint8_t           LCD_Read[LCD_I2C_READ_BUFFER_SIZE];
int               LCD_ReadIx;
char              LCD_Line[LCD_LINEBUFFERS][LCD_LINEBUFFER_SIZE];
bool              LCD_Backlight;

// system_init calls

void LCD_Initialize(uint32_t pb_clk_freq) {
    // global inits
    int i;
    I2C_State = I2C_UNINITIALIZED;
    I2C_Transfer = I2C_Idle;
    Waits = 20; // Wait at least 20 ms after power on (and only after power on)
    LCD_InitState = LCD_wait_on;
    memset( &LCD_Write[0], '\0', LCD_I2C_WRITE_BUFFER_SIZE);
    LCD_WriteIx = 0;
    memset( &LCD_Read[0], '\0', LCD_I2C_READ_BUFFER_SIZE);
    LCD_ReadIx = 0;
    memset( &LCD_Line[0][0], ' ', LCD_LINEBUFFERS * LCD_LINEBUFFER_SIZE);
    LCD_Backlight = false;
    // device inits
    PLIB_I2C_Disable(LCD_I2C_ID);
    // Prepare Ports I2C 1  
    PLIB_PORTS_PinClear(LCD_PORTS_ID, LCD_PORT_CHANNEL, LCD_SCL_PIN);
    PLIB_PORTS_PinClear(LCD_PORTS_ID, LCD_PORT_CHANNEL, LCD_SDA_PIN);
    //PLIB_PORTS_RemapOutput(LCD_PORTS_ID, OUTPUT_FUNC_NO_CONNECT, OUTPUT_PIN_RPB8);
    // open drain configuration I2C 1 Pins 5V tolerant, pull up with 2k2
    PLIB_PORTS_PinOpenDrainEnable(LCD_PORTS_ID, LCD_PORT_CHANNEL, LCD_SCL_PIN);
    PLIB_PORTS_PinOpenDrainEnable(LCD_PORTS_ID, LCD_PORT_CHANNEL, LCD_SDA_PIN);
    //PLIB_PORTS_PinDirectionOutputSet(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SCL_PIN);
    //PLIB_PORTS_PinDirectionOutputSet(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SDA_PIN);
    // set the ports Input
    PLIB_PORTS_PinDirectionInputSet(LCD_PORTS_ID, LCD_PORT_CHANNEL, LCD_SCL_PIN);
    PLIB_PORTS_PinDirectionInputSet(LCD_PORTS_ID, LCD_PORT_CHANNEL, LCD_SDA_PIN);
    // Configure General I2C Options
    PLIB_I2C_SlaveClockStretchingEnable(APP_LCD_I2C_ID);
    // Disable to be called 
    PLIB_I2C_GeneralCallDisable(LCD_I2C_ID);
    // future option SMB compatibility
    PLIB_I2C_SMBDisable(LCD_I2C_ID);
    // future option high baud rates (>100kHz)
    PLIB_I2C_HighFrequencyEnable(LCD_I2C_ID);
    // only 7-Bit addresses
    PLIB_I2C_ReservedAddressProtectDisable(LCD_I2C_ID);
    // Idle when proc idle (no operation in idle mode)
    PLIB_I2C_StopInIdleDisable(LCD_I2C_ID);
    // Set Desired baud rate
    PLIB_I2C_BaudRateSet( LCD_I2C_ID, pb_clk_freq, LCD_I2C_BAUD);
    // Set the appropriate slave address (slave only)
    //PLIB_I2C_SlaveAddress7BitSet(APP_LCD_I2C_ID, 0x7f);
    // Optional:  Clear and enable interrupts before enabling the I2C module.
    // Enable the module -> in SYS_Startup
}

void LCD_InterruptPriority(INT_MODULE_ID int_mod_id) {
    PLIB_INT_VectorPrioritySet(int_mod_id, LCD_INT_VECTOR, LCD_INT_PRIORITY);
    PLIB_INT_VectorSubPrioritySet(int_mod_id, LCD_INT_VECTOR, INT_SUBPRIORITY_LEVEL0);
}

void LCD_InterruptEnable(INT_MODULE_ID int_mod_id) {
    PLIB_INT_SourceEnable(int_mod_id, LCD_INT_SOURCE_ERROR);
    PLIB_INT_SourceEnable(int_mod_id, LCD_INT_SOURCE_SLAVE);
    PLIB_INT_SourceEnable(int_mod_id, LCD_INT_SOURCE_MASTER);
}

void LCD_SYS_Startup(INT_MODULE_ID int_mod_id) {
    // Clear Interrupt flags
    PLIB_INT_SourceFlagClear(int_mod_id, LCD_INT_SOURCE_ERROR);
    PLIB_INT_SourceFlagClear(int_mod_id, LCD_INT_SOURCE_SLAVE);
    PLIB_INT_SourceFlagClear(int_mod_id, LCD_INT_SOURCE_MASTER);
    // see http://ww1.microchip.com/downloads/en/DeviceDoc/80000531F.pdf Errata
    // Enabling I2C1 makes Ports A0 / A1 unusable
    // Enabling I2C2 makes Ports B5 / B6 unusable
    PLIB_I2C_Enable(LCD_I2C_ID);
}

// interrupt calls

void LCD_I2C_Process(void) {
    uint8_t Xfer;
    switch ( I2C_State) {
        case I2C_UNINITIALIZED:
            break;
        case I2C_MASTER_IDLE:
            break;
        case I2C_MASTER_WRITE:
            switch (I2C_Transfer) {
                case I2C_MS_Start:
                    // for this purpose no collision checking by PLIB_I2C_ArbitrationLossHasOccurred
                    if (PLIB_I2C_TransmitterIsReady(LCD_I2C_ID)) {
                        // top byte in buffer should always be a formatted i2c slave address
                        I2C_Transfer = I2C_MS_Address;
                        LCD_WriteIx--;
                        Xfer = LCD_Write[LCD_WriteIx];
                        PLIB_I2C_StartClear(LCD_I2C_ID);
                        PLIB_I2C_TransmitterByteSend(LCD_I2C_ID, Xfer);
                    }
                    break;
                case I2C_MS_Address:
                    // for this purpose no collision checking by PLIB_I2C_ArbitrationLossHasOccurred
                    if (!PLIB_I2C_TransmitterByteWasAcknowledged(LCD_I2C_ID)) {
                        /* POETODO: Future use of a delegate */
                        //APP_Set_ErrorCondition("I2C NACK");
                        // Slave did not acknowledge its address -> its not there, reset ready state 
                        I2C_Transfer = I2C_Idle;
                        I2C_State = I2C_MASTER_IDLE;
                        LCD_InitState = LCD_uninitialized;
                    } else {
                        if ( PLIB_I2C_TransmitterIsReady(LCD_I2C_ID)) {
                            I2C_Transfer = I2C_MS_Transmit;
                            LCD_WriteIx--;
                            Xfer = LCD_Write[LCD_WriteIx];
                            PLIB_I2C_TransmitterByteSend(LCD_I2C_ID, Xfer);
                        }
                    }
                    break;
                case I2C_MS_Transmit:
                    // for this purpose no collision checking by PLIB_I2C_ArbitrationLossHasOccurred
                    if (!PLIB_I2C_TransmitterByteWasAcknowledged(LCD_I2C_ID)) {
                        //LEDR_Set;
                    } else {
                        if (PLIB_I2C_TransmitterIsReady(LCD_I2C_ID)) {
                            if (LCD_WriteIx > 0) {
                                LCD_WriteIx--;
                                Xfer = LCD_Write[LCD_WriteIx];
                                PLIB_I2C_TransmitterByteSend(LCD_I2C_ID, Xfer);
                                // keep appData.LCD_Transfer = I2C_MS_Transmit
                            } else {
                                // if (appData.LCD_WriteIx > 0) { PLIB_I2C_MasterStartRepeat(APP_LCD_I2C_ID); not used in this purpose
                                PLIB_I2C_MasterStop(LCD_I2C_ID);
                                I2C_Transfer = I2C_MS_Stop;
                            }
                        // ] else { -> if PLIB_I2C_TransmitterByteWasAcknowledged doesn't happen transfer should be restarted and after some tries given up
                        // i assume all goes well ;)
                        }
                    }
                    break;
                case I2C_MS_Repeat:
                    // only used for a write/read cycle or for subsequent writes to multiple slaves
                    break;
                case I2C_MS_Stop:
                    // when this occurs, all is done, go back to wait
                    //LEDY_Clear;
                    I2C_State = I2C_MASTER_IDLE;
                    I2C_Transfer = I2C_Idle;
                    break;
            }
            break;
        case I2C_MASTER_WRITE_READ:
            // needs a repeatable and modifiable address variable (switch of RW bit in address)
            // needs an acknowledge after read byte
            break;
        case I2C_MASTER_READ:
            // needs a repeatable and modifiable address variable (switch of RW bit in address)
            // needs an acknowledge after read byte
            break;
        case I2C_SLAVE_IDLE:
            // data coming in -> switch to appropriate mode
            break;
        case I2C_SLAVE_READ:
            break;
        case I2C_SLAVE_READ_WRITE:
            break;
        case I2C_SLAVE_WRITE:
            break;
        // The default state should never be executed.
        default:
            break;
    }
}

// app calls

void LCD_AddWrite( uint8_t WriteIn) {
    if (LCD_Backlight) {
        LCD_Write[LCD_WriteIx++] = WriteIn | LCD_BACKLIGHT;
    } else {
        LCD_Write[LCD_WriteIx++] = WriteIn;
    }
}

void LCD_AddAddress( uint8_t WriteIn) {
    LCD_Write[LCD_WriteIx++] = WriteIn;
}

void LCD_AddCharWrite( char aChar) {
    // as described, high nibble first and with LIFO ordering
    uint8_t intChar = (uint8_t)aChar;
    LCD_AddWrite( LCD_DATA | LCD_RW_WRITE | LCD_E_WRITE | ((intChar & 0x0f) << 4));
    LCD_AddWrite( LCD_DATA | LCD_RW_WRITE | LCD_E_PREPARE | ((intChar & 0x0f) << 4));
    LCD_AddWrite( LCD_DATA | LCD_RW_WRITE | LCD_E_WRITE | (intChar & 0xf0));
    LCD_AddWrite( LCD_DATA | LCD_RW_WRITE | LCD_E_PREPARE | (intChar & 0xf0));
}

void LCD_StartWrite(void) {
    I2C_State = I2C_MASTER_WRITE;
    I2C_Transfer = I2C_MS_Start;
    PLIB_I2C_MasterStart(LCD_I2C_ID);
}

bool LCD_I2C_Ready(void) {
    return (I2C_State == I2C_MASTER_IDLE);
}

void LCD_Update(void) {
    // I2C write buffer filling in LIFO mode
    uint8_t c;
    for (c = LCD_LINEBUFFER_SIZE; c > 0; c--) {
        LCD_AddCharWrite(LCD_Line[3][c-1]);
    }
    for (c = LCD_LINEBUFFER_SIZE; c > 0; c--) {
        LCD_AddCharWrite(LCD_Line[1][c-1]);
    }
    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_SET_HOME2_L);
    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_SET_HOME2_L);
    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_SET_HOME2_H);
    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_SET_HOME2_H);
    for (c = LCD_LINEBUFFER_SIZE; c > 0; c--) {
        LCD_AddCharWrite(LCD_Line[2][c-1]);
    }
    for (c = LCD_LINEBUFFER_SIZE; c > 0; c--) {
        LCD_AddCharWrite(LCD_Line[0][c-1]);
    }
    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_SET_HOME1_L);
    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_SET_HOME1_L);
    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_SET_HOME1_H);
    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_SET_HOME1_H);
    LCD_AddAddress( LCD_ADDRESS | I2C_WRITE);
    LCD_StartWrite();
}

void LCD_ClearLine( uint8_t line) {
    uint8_t i;
    for (i = 0; i < LCD_LINEBUFFER_SIZE; i++) { LCD_Line[line][i] = ' ';}
} 

void LCD_PrintChar(uint8_t line, uint8_t pos, char lcdChar) {
    LCD_Line[line][pos] = lcdChar;
}

void LCD_Print(uint8_t line, uint8_t pos, char* lcdString) {
    uint8_t i, len;
    len = strlen(lcdString);
    if (len + pos > LCD_LINEBUFFER_SIZE) { len = LCD_LINEBUFFER_SIZE - pos; }
    for (i = 0; i < len; i++) {
        LCD_PrintChar(line, pos+i, *(lcdString+i));
    }
}

bool LCD_Init(int mSecs) {
    if (I2C_State == I2C_UNINITIALIZED) {
        I2C_State = I2C_MASTER_IDLE;
        // Power on waiting by LCD_Initialize
        LCD_InitState = LCD_wait_on;
    } else if (Waits) {
        if (lastWait != mSecs) {
            lastWait = mSecs;
            Waits--;
        }
    } else {
        switch (LCD_InitState) {
            case LCD_uninitialized:
                // fault state I2C slave (LCD) didn't acknowledge its address 
                LCD_InitState = LCD_wait_on;
                return true;
                break;
            case LCD_wait_on:
                // data & address always LIFO
                LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_8BIT_H);
                LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_8BIT_H);
                LCD_AddAddress( LCD_ADDRESS | I2C_WRITE);
                LCD_StartWrite();
                LCD_InitState = LCD_init_8bit;
                break;
            case LCD_init_8bit:
                if (LCD_I2C_Ready()) {
                    Waits = 4; // Wait at least 4 ms after 8bit switch
                    LCD_InitState = LCD_wait_8bit;
                }
                break;
            case LCD_wait_8bit:
                // data & address always LIFO
                LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_8BIT_H);
                LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_8BIT_H);
                LCD_AddAddress( LCD_ADDRESS | I2C_WRITE);
                LCD_StartWrite();
                LCD_InitState = LCD_switch_8bit;
                break;
            case LCD_switch_8bit:
                if (LCD_I2C_Ready()) {
                    Waits = 1; // Wait at least 1 ms after 8bit switch
                    LCD_InitState = LCD_8bit_wait;
                }
                break;
            case LCD_8bit_wait:
                // data & address always LIFO
                LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_4BIT_H);
                LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_4BIT_H);
                LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_8BIT_H);
                LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_8BIT_H);
                LCD_AddAddress( LCD_ADDRESS | I2C_WRITE);
                LCD_StartWrite();
                LCD_InitState = LCD_switch_4bit;
                break;
            case LCD_switch_4bit:
                if (LCD_I2C_Ready()) {
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_LINEFONT_L);
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_LINEFONT_L);
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_4BIT_H);
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_4BIT_H);
                    LCD_AddAddress( LCD_ADDRESS | I2C_WRITE);
                    LCD_StartWrite();
                    LCD_InitState = LCD_linefont_4bit;
                }
                break;
            case LCD_linefont_4bit:
                if (LCD_I2C_Ready()) {
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_DISPLAY_OFF_L);
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_DISPLAY_OFF_L);
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_DISPLAY_OFF_H);
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_DISPLAY_OFF_H);
                    LCD_AddAddress( LCD_ADDRESS | I2C_WRITE);
                    LCD_StartWrite();
                    LCD_InitState = LCD_displayoff;
                }
                break;
            case LCD_displayoff:
                if (LCD_I2C_Ready()) {
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_CLEAR_L);
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_CLEAR_L);
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_CLEAR_H);
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_CLEAR_H);
                    LCD_AddAddress( LCD_ADDRESS | I2C_WRITE);
                    LCD_StartWrite();
                    LCD_InitState = LCD_displayclear;
                }
                break;
            case LCD_displayclear:
                if (LCD_I2C_Ready()) {
                    Waits = 2; // Wait at least 2 ms after display clear
                    LCD_InitState = LCD_waitclear;
                }
                break;
            case LCD_waitclear:
                LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_CURSORSHIFT_L);
                LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_CURSORSHIFT_L);
                LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_CURSORSHIFT_H);
                LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_CURSORSHIFT_H);
                LCD_AddAddress( LCD_ADDRESS | I2C_WRITE);
                LCD_StartWrite();
                LCD_InitState = LCD_cursor_shift;
                break;
            case LCD_cursor_shift:
                if (LCD_I2C_Ready()) {
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_DISPLAY_ON_L);
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_DISPLAY_ON_L);
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_DISPLAY_ON_H);
                    LCD_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_DISPLAY_ON_H);
                    LCD_AddAddress( LCD_ADDRESS | I2C_WRITE);
                    LCD_StartWrite();
                    LCD_InitState = LCD_displayon;
                }
                break;
            case LCD_displayon:
                if (LCD_I2C_Ready()) {
                    LCD_InitState = LCD_ready;
                }
                break;
            case LCD_ready:
                return true;
                break;
        }
    }
    return false;
}

bool LCD_Ready(void) {
    return (LCD_InitState == LCD_ready);
}

#endif //ifdef APP_LCD_I2C_ID

