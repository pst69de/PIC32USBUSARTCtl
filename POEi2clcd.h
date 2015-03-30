/* 
 * File:    POEi2clcd.h
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-20
 * Description:
 *   I2C LCD library
 * Major Changes:
 *   Version 0: alpha development
 */

#ifndef POEI2CLCD_H
#define	POEI2CLCD_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "peripheral/int/plib_int.h"
#include "peripheral/ports/plib_ports.h"
#include "peripheral/i2c/plib_i2c.h"
#include "system_config.h"

#ifdef APP_LCD_I2C_ID
// see http://ww1.microchip.com/downloads/en/DeviceDoc/80000531F.pdf Errata
// Enabling I2C1 makes Ports A0 / A1 unusable
// Enabling I2C2 makes Ports B5 / B6 unusable

// compiler macros configuration
#if APP_LCD_I2C_ID == I2C_ID_1
#define LCD_I2C_ID                 I2C_ID_1
// PORTS mappings
#define LCD_PORTS_ID               PORTS_ID_0
#define LCD_PORT_CHANNEL           PORT_CHANNEL_B
#define LCD_SCL_PIN                PORTS_BIT_POS_8
#define LCD_SDA_PIN                PORTS_BIT_POS_9
// LCD Reset later by Power Control (define Port Pin for Output H = display on, L = display off, reinit display after power cycle)
// INT mappings
#define LCD_INT_VECTOR             INT_VECTOR_I2C1
#define LCD_INT_SOURCE_ERROR       INT_SOURCE_I2C_1_ERROR
#define LCD_INT_SOURCE_SLAVE       INT_SOURCE_I2C_1_SLAVE
#define LCD_INT_SOURCE_MASTER      INT_SOURCE_I2C_1_MASTER
#else // if APP_LCD_I2C_ID == I2C_ID_1
#define LCD_I2C_ID                 I2C_ID_2
#define LCD_PORTS_ID               PORTS_ID_0
// PORTS mappings
#define LCD_PORT_CHANNEL           PORT_CHANNEL_B
#define LCD_SCL_PIN                PORTS_BIT_POS_3
#define LCD_SDA_PIN                PORTS_BIT_POS_2
// INT mappings
#define LCD_INT_VECTOR             INT_VECTOR_I2C2
#define LCD_INT_SOURCE_ERROR       INT_SOURCE_I2C_2_ERROR
#define LCD_INT_SOURCE_SLAVE       INT_SOURCE_I2C_2_SLAVE
#define LCD_INT_SOURCE_MASTER      INT_SOURCE_I2C_2_MASTER
#endif // else APP_LCD_I2C_ID == I2C_ID_1
#define LCD_INT_PRIORITY           APP_LCD_INT_PRIORITY

#define LCD_I2C_BAUD               100000L
// I2C data handling
#define LCD_I2C_READ_BUFFER_SIZE   8
#define LCD_I2C_WRITE_BUFFER_SIZE  336

// compiler macro consts for I2C LCD Handling
//
// configuration I2C Chip PCF8574 on HD44780 Interface by sainsmart (most propably others)
// address of PCF8574 (without an A) is 0x27 , PCF8574A is 0x3F, LSB 0-2 are configurable, normally set H by pullup
// PCF8574   HD44780
// Bit 0   = RS
// Bit 1   = R/W
// Bit 2   = E
// Bit 3   = BL on H
// Bit 4   = D4
// Bit 5   = D5
// Bit 6   = D6
// Bit 7   = D7
// so the 4bit data is always the high nibble of the transfer byte and control is done in the low nibble
//
// address shifted by 1 as LSB is R/W */
// PCF8574 (A0 = H, A1 = H, A2 = H)
#define LCD_ADDRESS                (0x27 << 1)
// PCF8574A (A0 = H, A1 = H, A2 = H)
//#define LCD_ADDRESS                (0x3F << 1) 

#define LCD_WRITE                  0
#define LCD_READ                   1
#define LCD_LINEBUFFER_SIZE        20
#define LCD_LINEBUFFERS            4
// Command/Data -> RS Bit 0
#define LCD_COMMAND                0x00
#define LCD_DATA                   0x01
// Read/Write -> RW Bit 1 (always 0 = W)
#define LCD_RW_WRITE               0x00
// Enable (Clock Strobe) -> E Bit 2 (1 -> 0)
#define LCD_E_PREPARE              0x04
#define LCD_E_WRITE                0x00
// commands differentiated by (H)igh (1st) and (L)ow (2nd) nibble
// LCD_4BIT_H and LCD_LINEFONT_L compare in the init sequence and cannot be changed on Runtime
#define LCD_8BIT_H                 0x30  // 0x30 would be 8bit mode
#define LCD_4BIT_H                 0x20  // 0x30 would be 8bit mode
#define LCD_LINEFONT_L             0x80  // 2line display (1line bit7 off) with 5x7 font (5x10 font = bit6 on)
//#define LCD_LINEFONT_L             0x00 // try 1line mode (5x7 font) 1st
// display OFF/ON
#define LCD_DISPLAY_OFF_H          0x00
#define LCD_DISPLAY_OFF_L          0x80 // display off (no cursor, cursor underline)
#define LCD_DISPLAY_ON_H           0x00
#define LCD_DISPLAY_ON_L           0xc0 // display on, no cursor, cursor underline
// clear display, cursor to 0
#define LCD_CLEAR_H                0x00
#define LCD_CLEAR_L                0x10
// set cursor move (0 = right), shift on/off (0 = off)
#define LCD_CURSORSHIFT_H          0x00
#define LCD_CURSORSHIFT_L          0x60
// set cursor to home
#define LCD_SET_HOME1_H            0x80
#define LCD_SET_HOME1_L            0x00
#define LCD_SET_HOME2_H            0xc0
#define LCD_SET_HOME2_L            0x00
// set backlight bit if necessary
#define LCD_BACKLIGHT              0x08

// extern reference point
extern bool LCD_Backlight;

// system_init calls
void LCD_Initialize(uint32_t pb_clk_freq);

void LCD_InterruptPriority(INT_MODULE_ID int_mod_id);

void LCD_InterruptEnable(INT_MODULE_ID int_mod_id);

void LCD_SYS_Startup(INT_MODULE_ID int_mod_id);

// interrupt calls

void LCD_I2C_Process(void);

// app calls
// Initialization looping (recall until true)
bool LCD_Init(int mSecs);
// 
bool LCD_Ready(void);

// (private) void LCD_AddWrite( uint8_t WriteIn);
// (private) void LCD_AddAddress( uint8_t WriteIn);
// (private) void LCD_AddCharWrite( char aChar);

// is LCD ready for Updates
bool LCD_I2C_Ready(void);
// start writing command buffer to LCD
// (private) void LCD_StartWrite(void);
// Update the LCD of the line buffers
void LCD_Update(void);

// clear a line in the LCD line buffer
void LCD_ClearLine( uint8_t line);
// Update a char in the LCD line buffer
void LCD_PrintChar(uint8_t line, uint8_t pos, char lcdChar);
// put a string
void LCD_Print(uint8_t line, uint8_t pos, char* lcdString);

// typedefs
// I2C mode definitions (not completely used yet
typedef enum {
    // Uninitialized
    I2C_UNINITIALIZED,
    // Master modes
    I2C_MASTER_IDLE,
    I2C_MASTER_WRITE,
    I2C_MASTER_WRITE_READ,
    I2C_MASTER_READ,
    // Slave modes
    I2C_SLAVE_IDLE,
    I2C_SLAVE_READ,
    I2C_SLAVE_READ_WRITE,
    I2C_SLAVE_WRITE
} I2C_States;

// I2C transfer state definitions
typedef enum {
    // Bus Idle
    I2C_Idle,
    // Master Write Cycle
    I2C_MS_Start,
    I2C_MS_Address,
    I2C_MS_Transmit,
    I2C_MS_Repeat,
    I2C_MS_Stop,
    // Master Read Cycle
    I2C_MS_Receive,
    I2C_MS_Ack,
    // Slave Read Cycle
    I2C_SL_Receive,
    I2C_SL_Ack,
    // Slave Write Cycle
    I2C_SL_Transmit
} I2C_Data_State;

// LCD Init
typedef enum {
    LCD_uninitialized,
    LCD_wait_on,
    LCD_init_8bit,
    LCD_wait_8bit,
    LCD_switch_8bit,
    LCD_8bit_wait,
    LCD_switch_4bit,
    LCD_linefont_4bit,
    LCD_displayoff,
    LCD_displayclear,
    LCD_waitclear,
    LCD_cursor_shift,
    LCD_displayon,
    LCD_ready
} LCD_Init_Sequence;

#endif // ifdef APP_LCD_I2C_ID

#ifdef	__cplusplus
}
#endif

#endif	/* POEI2CLCD_H */

