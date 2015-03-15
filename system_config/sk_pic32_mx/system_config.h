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

// global usage defines
#define APP_USE_USART
#define APP_USE_USB


// imports:
#include "peripheral/ports/plib_ports.h"
#include "peripheral/int/plib_int.h"
#include "peripheral/tmr/plib_tmr.h"
#include "peripheral/i2c/plib_i2c.h"
#ifdef APP_USE_USART
#include "peripheral/usart/plib_usart.h"
#endif
#ifdef APP_USE_USB
#endif

// APP definitions 
#define APP_BUFFER_SIZE             1024
#define APP_STRING_SIZE             20

// compiler macros handling POE.net buffers
// POE.net defines a primary and secondary channel 
// either if the device is a simple node (has only the primary USART channel)
// or the device is a bridging node (has an primary USB connect and a secondary USART channel)
// other possibilities are not yet defined (e.g. primary and secondary USART channel)
#ifdef APP_USE_USB
#ifdef APP_USE_USART
// USB USART Bridging device
#define APP_POEnet_SECONDARY
#define USB_INPUT_BUF               POEnetPrimInputBuf
#define USB_INPUT_SIZE              POEnetPrimInputSize
#define USB_INPUT_IDX               POEnetPrimInputIdx
#define USB_OUTPUT_BUF              POEnetSecOutputBuf
#define USB_OUTPUT_SIZE             POEnetSecOutputSize
#define USB_OUTPUT_IDX              POEnetSecOutputIdx
#define USART_INPUT_BUF             POEnetSecInputBuf
#define USART_INPUT_SIZE            POEnetSecInputSize
#define USART_INPUT_IDX             POEnetSecInputIdx
#define USART_OUTPUT_BUF            POEnetPrimOutputBuf
#define USART_OUTPUT_SIZE           POEnetPrimOutputSize
#define USART_OUTPUT_IDX            POEnetPrimOutputIdx
#else // ifdef APP_USE_USART
// USB Test device
#undef APP_POEnet_SECONDARY
#define USB_INPUT_BUF               POEnetPrimInputBuf
#define USB_INPUT_SIZE              POEnetPrimInputSize
#define USB_INPUT_IDX               POEnetPrimInputIdx
#define USB_OUTPUT_BUF              POEnetPrimOutputBuf
#define USB_OUTPUT_SIZE             POEnetPrimOutputSize
#define USB_OUTPUT_IDX              POEnetPrimOutputIdx
#endif // else APP_USE_USART
#else // ifdef APP_USE_USB
// simple device with single USART channel
#undef APP_POEnet_SECONDARY
#define USART_INPUT_BUF             POEnetPrimInputBuf
#define USART_INPUT_SIZE            POEnetPrimInputSize
#define USART_INPUT_IDX             POEnetPrimInputIdx
#define USART_OUTPUT_BUF            POEnetPrimOutputBuf
#define USART_OUTPUT_SIZE           POEnetPrimOutputSize
#define USART_OUTPUT_IDX            POEnetPrimOutputIdx
#endif // else APP_USE_USB

// System constants for APP usage
#define APP_SYSCLK_FREQ             24000000L
#define APP_PBCLK_FREQ              24000000L

// APP's interrupt handling
#define APP_INT_ID                  INT_ID_0

// APP's standard timing
// timer 1 for system clock
#define APP_TMR_CLOCK               TMR_ID_1
// Interrupt source for Timer ... should be choosen timer 
#define APP_TMR_INT_SOURCE          INT_SOURCE_TIMER_1
// Prescaling of Timer (depends on choosen systemclock, see pragma's of system_init.c; with 24MHz 1:8 results in 3000kHz; 3000 > 1kHz / 1ms period 
#define APP_TMR_CLKPRESCALE         TMR_PRESCALE_VALUE_8
#define APP_TMR_CLKINTERVAL         3000
    
// Application LEDs Definitions 
// compiler macro consts & funcs for LED Handling
// LED red 
// Used for general error signaling 
#define LEDR_Direction  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define LEDR_Mode       PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_4, PORTS_PIN_MODE_DIGITAL)
#define LEDR_OD
#define LEDR_Remap
#define LEDR_Clear      PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define LEDR_Set        PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define LEDR_Toggle     PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
// LED yellow
// Used for I2C transmit / error signaling 
#define LEDY_Direction  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define LEDY_Mode
#define LEDY_OD
#define LEDY_Remap
#define LEDY_Clear      PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define LEDY_Set        PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define LEDY_Toggle     PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
// LED green
// Used for data acceptance
#define LEDG_Direction  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
#define LEDG_Mode       PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_5, PORTS_PIN_MODE_DIGITAL)
#define LEDG_OD
#define LEDG_Remap
#define LEDG_Clear      PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
#define LEDG_Set        PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
#define LEDG_Toggle     PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
// LED blue
// Not Used
#define LEDB_Direction
#define LEDB_Mode
#define LEDB_OD
#define LEDB_Remap
#define LEDB_Clear
#define LEDB_Set
#define LEDB_Toggle
// LED white
// Not Used
#define LEDW_Direction
#define LEDW_Mode
#define LEDW_OD
#define LEDW_Remap
#define LEDW_Clear
#define LEDW_Set
#define LEDW_Toggle

// defs I2C 1 for LCD 
// see http://ww1.microchip.com/downloads/en/DeviceDoc/80000531F.pdf Errata
// Enabling I2C1 makes Ports A0 / A1 unusable
// Enabling I2C2 makes Ports B5 / B6 unusable
#define APP_LCD_PORTS_ID               PORTS_ID_0
// PORTS mappings
#define APP_LCD_PORT_CHANNEL           PORT_CHANNEL_B
#define APP_LCD_SCL_PIN                PORTS_BIT_POS_8
#define APP_LCD_SDA_PIN                PORTS_BIT_POS_9
// LCD Reset later by Power Control (define Port Pin for Output H = display on, L = display off, reinit display after power cycle)
// I2C1 
#define APP_LCD_I2C_ID                 I2C_ID_1
#define APP_LCD_I2C_BAUD               100000L
// I2C data handling
#define APP_LCD_I2C_READ_BUFFER_SIZE   8
#define APP_LCD_I2C_WRITE_BUFFER_SIZE  336

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
#define LCD_ADDRESS                (0x27 << 1) //
// PCF8574A (A0 = H, A1 = H, A2 = H)
//#define LCD_ADDRESS                (0x3F << 1) 
#define I2C_WRITE                  0
#define I2C_READ                   1
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


// defs USART 1/2 for COM
#ifdef APP_USE_USART
// Defs USART RX (is unit 2)
#define APP_USART_RX_ID            USART_ID_2
// USART RX Port
#define APP_USART_RX_PORTS_ID      PORTS_ID_0
#define APP_USART_RX_PORT_CHANNEL  PORT_CHANNEL_B
#define APP_USART_RX_PORT_PIN      PORTS_BIT_POS_5
#define APP_USART_RX_REMAP_PIN     INPUT_PIN_RPB5
#define APP_USART_RX_REMAP_FUNC    INPUT_FUNC_U2RX
// USART RX Module (keep the same if using the same module for RX & TX)
#define APP_USART_RX_BAUD          9600
#define APP_USART_RX_MODE          USART_8N1
#define APP_USART_RX_OPER          USART_ENABLE_TX_RX_USED // no flow control
#define APP_USART_RX_HAND          USART_HANDSHAKE_MODE_SIMPLEX // no flow control
// general RX config
#define APP_USART_RX_BUFFER_SIZE   APP_BUFFER_SIZE

// Defs USART TX (is unit 1)
#define APP_USART_TX_ID            USART_ID_1
// USART TX Port
#define APP_USART_TX_PORTS_ID      PORTS_ID_0
#define APP_USART_TX_PORT_CHANNEL  PORT_CHANNEL_B
#define APP_USART_TX_PORT_PIN      PORTS_BIT_POS_7
#define APP_USART_TX_REMAP_PIN     OUTPUT_PIN_RPB7
//#define APP_USART_TX_PORT_PIN      PORTS_BIT_POS_15
//#define APP_USART_TX_REMAP_PIN     OUTPUT_PIN_RPB15
#define APP_USART_TX_REMAP_FUNC    OUTPUT_FUNC_U1TX
// USART TX Module (keep the same if using the same module for RX & TX)
#define APP_USART_TX_BAUD          9600
#define APP_USART_TX_MODE          USART_8N1
#define APP_USART_TX_OPER          USART_ENABLE_TX_RX_USED // no flow control
#define APP_USART_TX_HAND          USART_HANDSHAKE_MODE_SIMPLEX // no flow control
// general TX config
#define APP_USART_TX_BUFFER_SIZE   1024
#endif // of ifdef APP_USE_USART

// *****************************************************************************
// Section: USB controller Driver Configuration
// *****************************************************************************
#ifdef APP_USE_USB
// Enables Device Support 
#define DRV_USB_DEVICE_SUPPORT               true
// Disables host support 
#define DRV_USB_HOST_SUPPORT                 false
// Provides 3 endpoints 
#define DRV_USB_ENDPOINTS_NUMBER             3
// Only one instance of the USB Peripheral
#define DRV_USB_INSTANCES_NUMBER             1
// Enables interrupt mode 
#define DRV_USB_INTERRUPT_MODE               true
// *****************************************************************************
// Section: USB Device Layer Configuration
// *****************************************************************************
// Maximum device layer instances 
#define USB_DEVICE_INSTANCES_NUMBER          1
// Maximum clients for Device Layer 
#define USB_DEVICE_CLIENTS_NUMBER            1
// EP0 size in bytes 
#define USB_DEVICE_EP0_BUFFER_SIZE           64
// *****************************************************************************
// Section: CDC Function Driver Configuration
// *****************************************************************************
// Maximum instances of CDC function driver 
#define USB_DEVICE_CDC_INSTANCES_NUMBER      1
// 
// CDC Transfer Queue Size for both read and
// write. Applicable to all instances of the
// function driver 
//
#define USB_DEVICE_CDC_QUEUE_DEPTH_COMBINED  3
// Application USB Device CDC Read Buffer Size. This should be a multiple of
// the CDC Bulk Endpoint size 
#define USB_BUFFER_SIZE                      APP_BUFFER_SIZE

//extern const USB_DEVICE_FUNCTION_REGISTRATION_TABLE funcRegistrationTable[USB_DEVICE_CDC_INSTANCES_NUMBER];

//extern const USB_DEVICE_MASTER_DESCRIPTOR usbMasterDescriptor;
#endif // of ifdef APP_USE_USB

/* TODO:  Define build-time Configuration Options. */

#ifdef	__cplusplus
}
#endif

#endif	// SYSTEM_CONFIG_H

