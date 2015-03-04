/* 
 * File:   system_config.h
 * Author: patrick
 *
 * Created on 2014-09-18
 */

#ifndef SYSTEM_CONFIG_H
#define	SYSTEM_CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

/* imports:
 * ports
 * int
 * tmr
 */
#include "peripheral/ports/plib_ports.h"
#include "peripheral/int/plib_int.h"
#include "peripheral/tmr/plib_tmr.h"
#include "peripheral/i2c/plib_i2c.h"
    
/* APP definitions */
#define APP_SYSCLK_FREQ             24000000L
#define APP_PBCLK_FREQ              24000000L


/* timer 1 for system clock */
#define APP_TMR_CLOCK               TMR_ID_1
/* Interrupt source for Timer ... should be choosen timer  */
#define APP_TMR_INT_SOURCE          INT_SOURCE_TIMER_1
/* Prescaling of Timer (depends on choosen systemclock, see pragma's of system_init.c; with 15MHz 1:8 results in 1875kHz; 1875 > 1kHz / 1ms period */
#define APP_TMR_CLKPRESCALE         TMR_PRESCALE_VALUE_8
#define APP_TMR_CLKINTERVAL         3000
    
/* Application Ports Definitions */
/* Ports ID = 0 (future extension may take higher IDs) */
#define APP_LED_PORTS_ID            PORTS_ID_0
/* PORTS mappings */
#define APP_LED_PORT_CHANNEL        PORT_CHANNEL_A
#define APP_LEDR_PIN                PORTS_BIT_POS_0
#define APP_LEDR_AIPIN              PORTS_ANALOG_PIN_0
#define APP_LEDG_PIN                PORTS_BIT_POS_1
#define APP_LEDG_AIPIN              PORTS_ANALOG_PIN_1

/* APP's interrupt handling */
#define APP_INT_ID                  INT_ID_0

/* defs I2C 1 for LCD */
#define APP_LCD_PORTS_ID               PORTS_ID_0
/* PORTS mappings */
#define APP_LCD_PORT_CHANNEL           PORT_CHANNEL_B
#define APP_LCD_SCL_PIN                PORTS_BIT_POS_8
#define APP_LCD_SDA_PIN                PORTS_BIT_POS_9
/* TODO: defs CS/RST if needed: CS tied to GND (always select), RST tied to MCLR (reset on chip reset) */
/* LCD Reset later by Power Control (define Port Pin for Output H = display on, L = display off, reinit display after power cycle) */
/* I2C1 */
#define APP_LCD_I2C_ID                 I2C_ID_1
#define APP_LCD_I2C_BAUD               100000L
/* I2C data handling */
#define APP_LCD_I2C_READ_BUFFER_SIZE   8
#define APP_LCD_I2C_WRITE_BUFFER_SIZE  336

/* consts for I2C LCD Handling  */
/*
  configuration I2C Chip PCF8574 on HD44780 Interface by sainsmart (most propably others)
  address of PCF8574 (without an A) is , PCF8574 is 0x3F, LSB 0-2 are configurable, normally set H by pullup
  PCF8574   HD44780
  Bit 0   = RS
  Bit 1   = R/W
  Bit 2   = E
  Bit 3   = N.C. (possible patching use)
  Bit 4   = D4
  Bit 5   = D5
  Bit 6   = D6
  Bit 7   = D7
  so the 4bit data is always the high nibble of the transfer byte and control is done in the low nibble
*/
/* address shifted by 1 as LSB is R/W */
#define LCD_ADDRESS                (0x27 << 1)
#define I2C_WRITE                  0
#define I2C_READ                   1
#define LCD_LINEBUFFER_SIZE        20
#define LCD_LINEBUFFERS            4
/* Command/Data -> RS Bit 0 */
#define LCD_COMMAND                0x00
#define LCD_DATA                   0x01
/* Read/Write -> RW Bit 1 (always 0 = W) */
#define LCD_RW_WRITE               0x00
/* Enable (Clock Strobe) -> E Bit 2 (1 -> 0) */
#define LCD_E_PREPARE              0x02
#define LCD_E_WRITE                0x00
/* commands differentiated by (H)igh (1st) and (L)ow (2nd) nibble */
/* LCD_4BIT_H and LCD_LINEFONT_L compare in the init sequence and cannot be changed on Runtime */
#define LCD_4BIT_H                 0x20  // 0x30 would be 8bit mode
//#define LCD_LINEFONT_L             0x80  // 2line display (1line bit7 off) with 5x7 font (5x10 font = bit6 on)
#define LCD_LINEFONT_L             0x00 // try 1line mode (5x7 font) 1st    
/* display OFF/ON */
#define LCD_DISPLAY_OFF_H          0x00
#define LCD_DISPLAY_OFF_L          0x80 // display off (no cursor, cursor underline)
#define LCD_DISPLAY_ON_H           0x00
#define LCD_DISPLAY_ON_L           0xc0 // display on, no cursor, cursor underline
/* clear display, cursor to 0 */
#define LCD_CLEAR_H                0x00
#define LCD_CLEAR_L                0x10
/* set cursor move (0 = right), shift on/off (0 = off)  */
#define LCD_CURSORSHIFT_H          0x10
#define LCD_CURSORSHIFT_L          0x00
/* set cursor to home */
#define LCD_SET_HOME_H             0x10
#define LCD_SET_HOME_L             0x00

/* TODO: defs UART 1/2 for COM */

// *****************************************************************************
// Section: USB controller Driver Configuration
// *****************************************************************************
/* Enables Device Support */
#define DRV_USB_DEVICE_SUPPORT      true
/* Disables host support */
#define DRV_USB_HOST_SUPPORT        false
/* Provides 3 endpoints */
#define DRV_USB_ENDPOINTS_NUMBER    3
/* Only one instance of the USB Peripheral*/
#define DRV_USB_INSTANCES_NUMBER    1
/* Enables interrupt mode */
#define DRV_USB_INTERRUPT_MODE      true
// *****************************************************************************
// Section: USB Device Layer Configuration
// *****************************************************************************
/* Maximum device layer instances */
#define USB_DEVICE_INSTANCES_NUMBER        1
/* Maximum clients for Device Layer */
#define USB_DEVICE_CLIENTS_NUMBER          1
/* EP0 size in bytes */
#define USB_DEVICE_EP0_BUFFER_SIZE          64
// *****************************************************************************
// Section: CDC Function Driver Configuration
// *****************************************************************************
/* Maximum instances of CDC function driver */
#define USB_DEVICE_CDC_INSTANCES_NUMBER   1
/* CDC Transfer Queue Size for both read and
   write. Applicable to all instances of the
   function driver */
#define USB_DEVICE_CDC_QUEUE_DEPTH_COMBINED      3
/* Application USB Device CDC Read Buffer Size. This should be a multiple of
 * the CDC Bulk Endpoint size */
#define APP_READ_BUFFER_SIZE 64

/* TODO:  Define build-time Configuration Options. */

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_CONFIG_H */

