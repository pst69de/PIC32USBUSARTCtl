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
#define APP_USE_UART
#ifdef __32MX250F128B__
#define APP_USE_USB
#else
#undef APP_USE_USB
#endif
#define APP_USE_ADC
#define APP_USE_DIO

// imports:
#include "peripheral/ports/plib_ports.h"
#include "peripheral/int/plib_int.h"
#include "peripheral/tmr/plib_tmr.h"
#include "peripheral/i2c/plib_i2c.h"
#ifdef APP_USE_UART
#include "peripheral/usart/plib_usart.h"
#endif

// APP definitions 
#define APP_BUFFER_SIZE             1024
#define APP_STRING_SIZE             20

// compiler macros handling POE.net buffers
// POE.net defines a primary and secondary channel 
// either if the device is a simple node (has only the primary UART channel)
// or the device is a bridging node (has an primary USB connect and a secondary UART channel)
// other possibilities are not yet defined (e.g. primary and secondary UART channel)
#ifdef APP_USE_USB
#ifdef APP_USE_UART
// USB UART Bridging device
#define APP_POEnet_SECONDARY
#define USB_INPUT_BUF               POEnetPrimInputBuf
#define USB_INPUT_SIZE              POEnetPrimInputSize
#define USB_INPUT_IDX               POEnetPrimInputIdx
#define USB_OUTPUT_BUF              POEnetSecOutputBuf
#define USB_OUTPUT_SIZE             POEnetSecOutputSize
#define USB_OUTPUT_IDX              POEnetSecOutputIdx
#define UART_INPUT_BUF              POEnetSecInputBuf
#define UART_INPUT_SIZE             POEnetSecInputSize
#define UART_INPUT_IDX              POEnetSecInputIdx
#define UART_OUTPUT_BUF             POEnetPrimOutputBuf
#define UART_OUTPUT_SIZE            POEnetPrimOutputSize
#define UART_OUTPUT_IDX             POEnetPrimOutputIdx
#else // ifdef APP_USE_UART
// USB Test device
#undef APP_POEnet_SECONDARY
#define USB_INPUT_BUF               POEnetPrimInputBuf
#define USB_INPUT_SIZE              POEnetPrimInputSize
#define USB_INPUT_IDX               POEnetPrimInputIdx
#define USB_OUTPUT_BUF              POEnetPrimOutputBuf
#define USB_OUTPUT_SIZE             POEnetPrimOutputSize
#define USB_OUTPUT_IDX              POEnetPrimOutputIdx
#endif // else APP_USE_UART
#else // ifdef APP_USE_USB
// simple device with single UART channel
#undef APP_POEnet_SECONDARY
#define UART_INPUT_BUF              POEnetPrimInputBuf
#define UART_INPUT_SIZE             POEnetPrimInputSize
#define UART_INPUT_IDX              POEnetPrimInputIdx
#define UART_OUTPUT_BUF             POEnetPrimOutputBuf
#define UART_OUTPUT_SIZE            POEnetPrimOutputSize
#define UART_OUTPUT_IDX             POEnetPrimOutputIdx
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
#ifndef APP_USE_DIO
#define LEDR_Direction  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define LEDR_Mode       PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_4, PORTS_PIN_MODE_DIGITAL)
#define LEDR_OD
#define LEDR_Remap
#define LEDR_Clear      PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define LEDR_Set        PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#define LEDR_Toggle     PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_2)
#else // ifndef APP_USE_DIO
#define LEDR_Direction  
#define LEDR_Mode       
#define LEDR_OD         
#define LEDR_Remap      
#define LEDR_Clear      DIO_ClearDO(1)
#define LEDR_Set        DIO_SetDO(1)
#define LEDR_Toggle     DIO_ToggleDO(1)
#endif // else ifndef APP_USE_DIO
// LED yellow
// Used for I2C transmit / error signaling 
#ifndef APP_USE_DIO
#define LEDY_Direction  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define LEDY_Mode
#define LEDY_OD
#define LEDY_Remap
#define LEDY_Clear      PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define LEDY_Set        PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#define LEDY_Toggle     PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_4)
#else // ifndef APP_USE_DIO
#define LEDY_Direction  
#define LEDY_Mode       
#define LEDY_OD         
#define LEDY_Remap      
#define LEDY_Clear      DIO_ClearDO(3)
#define LEDY_Set        DIO_SetDO(3)
#define LEDY_Toggle     DIO_ToggleDO(3)
#endif // else ifndef APP_USE_DIO
// LED green
// Used for data acceptance
#ifndef APP_USE_DIO
#define LEDG_Direction  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
#define LEDG_Mode       PLIB_PORTS_PinModeSelect(PORTS_ID_0, PORTS_ANALOG_PIN_5, PORTS_PIN_MODE_DIGITAL)
#define LEDG_OD
#define LEDG_Remap
#define LEDG_Clear      PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
#define LEDG_Set        PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
#define LEDG_Toggle     PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_3)
#else // ifndef APP_USE_DIO
#define LEDG_Direction  
#define LEDG_Mode       
#define LEDG_OD         
#define LEDG_Remap      
#define LEDG_Clear      DIO_ClearDO(2)
#define LEDG_Set        DIO_SetDO(2)
#define LEDG_Toggle     DIO_ToggleDO(2)
#endif // else ifndef APP_USE_DIO
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
#define APP_LCD_I2C_ID            I2C_ID_1
#define APP_LCD_INT_PRIORITY      INT_PRIORITY_LEVEL4

#ifdef APP_LCD_I2C_ID
#define APP_LCD_InterruptPriority(iid)  LCD_InterruptPriority((iid))
#define APP_LCD_InterruptEnable(iid)    LCD_InterruptEnable((iid))
#define APP_LCD_SYS_Startup(iid)        LCD_SYS_Startup((iid))
#define APP_LCD_Initialize(pbf)         LCD_Initialize((pbf))
#define APP_LCD_Init(ms)          LCD_Init((ms))
#define APP_LCD_Ready             LCD_Ready()
#define APP_LCD_ClearLine(l)      LCD_ClearLine((l))
#define APP_LCD_PrintChar(l,p,c)  LCD_PrintChar((l),(p),(c))
#define APP_LCD_Print(l,p,s)      LCD_Print((l),(p),(s))
#define APP_LCD_Backlight         LCD_Backlight
#define APP_LCD_I2C_Ready         LCD_I2C_Ready()
#define APP_LCD_Update            LCD_Update()
#else
#define APP_LCD_InterruptPriority(iid)  
#define APP_LCD_InterruptEnable(iid)    
#define APP_LCD_SYS_Startup(iid)        
#define APP_LCD_Initialize(pbf)         
#define APP_LCD_Init(ms)          true
#define APP_LCD_Ready             false
#define APP_LCD_ClearLine(l)      
#define APP_LCD_PrintChar(l,p,c)  
#define APP_LCD_Print(l,p,s)      
#define APP_LCD_Backlight         appData.LCD_Dummy_switch
#define APP_LCD_I2C_Ready         true
#define APP_LCD_Update            
#endif

// defs UART 1/2 for COM
#ifdef APP_USE_UART
// Defs UART RX (is unit 2)
#define APP_UART_RX_ID            USART_ID_2
// UART RX Port
#define APP_UART_RX_PORTS_ID      PORTS_ID_0
#define APP_UART_RX_PORT_CHANNEL  PORT_CHANNEL_B
#define APP_UART_RX_PORT_PIN      PORTS_BIT_POS_5
#define APP_UART_RX_REMAP_PIN     INPUT_PIN_RPB5
#define APP_UART_RX_REMAP_FUNC    INPUT_FUNC_U2RX
// UART RX Module (keep the same if using the same module for RX & TX)
#define APP_UART_RX_BAUD          9600
#define APP_UART_RX_MODE          USART_8N1
#define APP_UART_RX_OPER          USART_ENABLE_TX_RX_USED // no flow control
#define APP_UART_RX_HAND          USART_HANDSHAKE_MODE_SIMPLEX // no flow control
// general RX config
#define APP_UART_RX_BUFFER_SIZE   APP_BUFFER_SIZE

//// Defs UART TX (is unit 1)
//#define APP_UART_TX_ID            UART_ID_1
//// UART TX Port
//#define APP_UART_TX_PORTS_ID      PORTS_ID_0
//#define APP_UART_TX_PORT_CHANNEL  PORT_CHANNEL_B
//#define APP_UART_TX_PORT_PIN      PORTS_BIT_POS_7
//#define APP_UART_TX_REMAP_PIN     OUTPUT_PIN_RPB7
////#define APP_UART_TX_PORT_PIN      PORTS_BIT_POS_15
////#define APP_UART_TX_REMAP_PIN     OUTPUT_PIN_RPB15
//#define APP_UART_TX_REMAP_FUNC    OUTPUT_FUNC_U1TX
// Defs UART TX (is unit 2)
#define APP_UART_TX_ID            APP_UART_RX_ID
#define APP_UART_TX_PORTS_ID      PORTS_ID_0
#define APP_UART_TX_PORT_CHANNEL  PORT_CHANNEL_B
#define APP_UART_TX_PORT_PIN      PORTS_BIT_POS_14
#define APP_UART_TX_REMAP_PIN     OUTPUT_PIN_RPB14
#define APP_UART_TX_REMAP_FUNC    OUTPUT_FUNC_U2TX
// UART TX Module (keep the same if using the same module for RX & TX)
#define APP_UART_TX_BAUD          APP_UART_RX_BAUD
#define APP_UART_TX_MODE          APP_UART_RX_MODE
#define APP_UART_TX_OPER          APP_UART_RX_OPER // no flow control
#define APP_UART_TX_HAND          APP_UART_RX_HAND // no flow control
// general TX config
#define APP_UART_TX_BUFFER_SIZE   APP_BUFFER_SIZE
#endif // of ifdef APP_USE_UART

#ifdef APP_USE_USB
// some of the USB driver definitions have to placed in system_config.h
// as this is refered by some of the USB driver code files
// *****************************************************************************
// Section: USB controller Driver Configuration compiler macros
// *****************************************************************************
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
// EP0 size in bytes (must be defined before inclusion of USB headers)
#define USB_DEVICE_EP0_BUFFER_SIZE           64
// *****************************************************************************
// Section: CDC Function Driver Configuration
// *****************************************************************************
// Maximum instances of CDC function driver
#define USB_DEVICE_CDC_INSTANCES_NUMBER      1
// CDC Transfer Queue Size for both read and
// write. Applicable to all instances of the
// function driver
#define USB_DEVICE_CDC_QUEUE_DEPTH_COMBINED  3
// index of CDC device driver
#define USB_DEVICE_CDC_INDEX_0               0

#endif // ifdef APP_USE_USB

#ifdef APP_USE_ADC
#define APP_ADC_NUM_PINS       2
#define APP_ADC_ID             ADC_ID_1
#define APP_ADC_CCLK_BASE      APP_PBCLK_FREQ
#define APP_ADC_CCLK_RATE      4000000L
// 1st analog input pin (should allways be defined)
#define APP_ADC1_INPUT_POS     ADC_INPUT_POSITIVE_AN9
#define APP_ADC1_PORTS_ID      PORTS_ID_0
#define APP_ADC1_PORT_CHANNEL  PORT_CHANNEL_B
#define APP_ADC1_PIN           PORTS_BIT_POS_15
#define APP_ADC1_AIPIN         PORTS_ANALOG_PIN_9
// 2nd analog input pin (maybe defined)
#define APP_ADC2_INPUT_POS     ADC_INPUT_POSITIVE_AN11
#define APP_ADC2_PORTS_ID      PORTS_ID_0
#define APP_ADC2_PORT_CHANNEL  PORT_CHANNEL_B
#define APP_ADC2_PIN           PORTS_BIT_POS_13
#define APP_ADC2_AIPIN         PORTS_ANALOG_PIN_11
// 3rd analog input pin (maybe defined)
#undef  APP_ADC3_INPUT_POS
// 4th analog input pin (maybe defined)
#undef  APP_ADC4_INPUT_POS
#else
// used as indicator for data polling
#define APP_ADC_NUM_PINS       0
#endif // ifdef APP_USE_ADC

#ifdef APP_USE_DIO
// Define dio Options
#include "../../POEdiopins.h"
#define APP_DI_1         DIO_PIN_B0
#define APP_DI_2         DIO_PIN_B1
#undef  APP_DI_3
#undef  APP_DI_4
#define APP_DI_COUNT     2
#define APP_DO_1         DIO_PIN_B2
#define APP_DO_1_OD
#define APP_DO_2         DIO_PIN_B3
#define APP_DO_2_OD
#define APP_DO_3         DIO_PIN_B4
#define APP_DO_3_OD
#undef  APP_DO_4
#define APP_DO_4_OD
#define APP_DO_COUNT     3
#endif // ifdef APP_USE_DIO

#ifdef	__cplusplus
}
#endif

#endif	// SYSTEM_CONFIG_H

