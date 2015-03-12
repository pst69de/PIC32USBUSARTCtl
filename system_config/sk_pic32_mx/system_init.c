/*
 * File:   system_init.c
 * Author: patrick
 *
 * Created on 2014-09-13
 */

// PIC32MX250F128B Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

/* see configuration_bits.cpp
#ifndef	__cplusplus
// DEVCFG3
// USERID = No Setting
#pragma config PMDL1WAY = OFF           // Peripheral Module Disable Configuration (Allow multiple reconfigurations)
#pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow multiple reconfigurations)
#pragma config FUSBIDIO = OFF           // USB USID Selection (Controlled by Port Function)
#pragma config FVBUSONIO = OFF          // USB VBUS ON Selection (Controlled by Port Function)

// DEVCFG2
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_24         // PLL Multiplier (24x Multiplier)
#pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider (2x Divider)
#pragma config UPLLEN = ON              // USB PLL Enable (Enabled)
#pragma config FPLLODIV = DIV_4         // System PLL Output Clock Divider (PLL Divide by 4)

// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc (XT,HS,EC))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = ON                // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Enabled)
#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window Size is 25%)

// DEVCFG0
#pragma config JTAGEN = OFF             // JTAG Enable (JTAG Disabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)
#endif // ifndef __cplusplus
*/

#include "system_definitions.h"
#include "system_config.h"
#ifdef APP_USE_USB
#include "usb/usb_device.h"
#endif
// Global allocation of structure to hold system objects handles
SYSTEM_OBJECTS sysObjects;

#ifdef APP_USE_USB
#include "usb/usb_chapter_9.h"
#include "usb/usb_cdc.h"
#include "usb/usb_device_cdc.h"
//***************************************************
// Endpoint Table needed by the Device Layer.
//***************************************************

uint8_t __attribute__((aligned(512))) endpointTable[USB_DEVICE_ENDPOINT_TABLE_SIZE];

//******************************************
//  USB Device Desciptor for this demo
//******************************************
const USB_DEVICE_DESCRIPTOR fullSpeedDeviceDescriptor=
{
    0x12,                       // Size of this descriptor in bytes
    USB_DESCRIPTOR_DEVICE,      // DEVICE descriptor type
    0x0200,                     // USB Spec Release Number in BCD format
    USB_CDC_CLASS_CODE,         // Class Code
    USB_CDC_SUBCLASS_CODE,      // Subclass code
    0x00,                       // Protocol code
    USB_DEVICE_EP0_BUFFER_SIZE, // Max packet size for EP0, see system_config.h
    0x04D8,                     // Vendor ID
    0x000A,                     // Product ID: CDC RS-232 Emulation Demo
    0x0100,                     // Device release number in BCD format
    0x01,                       // Manufacturer string index
    0x02,                       // Product string index
    0x00,                       // Device serial number string index
    0x01                        // Number of possible configurations
};

//******************************************
//  Device Configuration Decriptor
//******************************************
const uint8_t fullSpeedConfigurationDescriptor1[]=
{
    // > Configuration Descriptor Header
    0x09,                                                   // Size of this descriptor
    USB_DESCRIPTOR_CONFIGURATION,                           // CONFIGURATION descriptor type
    66,0,                                                   // Total length of data for this configuration
    2,                                                      // Number of interfaces in this configuration
    1,                                                      // Index value of this configuration
    0,                                                      // Configuration string index
    USB_ATTRIBUTE_DEFAULT | USB_ATTRIBUTE_SELF_POWERED,     // Attributes, see usb_device.h
    //USB_ATTRIBUTE_DEFAULT,     // Attributes, see usb_device.h
    50,                                                     // Max power consumption (2X mA)
    // > Interface Descriptor 1
    9,                                              // Size of the descriptor
    USB_DESCRIPTOR_INTERFACE,                       // INTERFACE descriptor type
    0,                                              // Interface Number
    0,                                              // Alternate Setting Number
    1,                                              // Number of endpoints in this intf
    USB_CDC_COMMUNICATIONS_INTERFACE_CLASS_CODE,    // Class code
    USB_CDC_SUBCLASS_ABSTRACT_CONTROL_MODEL,        // Subclass code
    USB_CDC_PROTOCOL_AT_V250,                       // Protocol code
    0,                                              // Interface string index
    // > CDC Class-Specific Descriptors
    sizeof(USB_CDC_HEADER_FUNCTIONAL_DESCRIPTOR),                   // Size of the descriptor
    USB_CDC_DESC_CS_INTERFACE,                                      // CS_INTERFACE
    USB_CDC_FUNCTIONAL_HEADER,                                      // Type of functional descriptor
    0x20,0x01,                                                      // CDC spec version

    sizeof(USB_CDC_ACM_FUNCTIONAL_DESCRIPTOR),                      // Size of the descriptor
    USB_CDC_DESC_CS_INTERFACE,                                      // CS_INTERFACE
    USB_CDC_FUNCTIONAL_ABSTRACT_CONTROL_MANAGEMENT,                 // Type of functional descriptor
    USB_CDC_ACM_SUPPORT_LINE_CODING_LINE_STATE_AND_NOTIFICATION,    // bmCapabilities of ACM

    sizeof(USB_CDC_UNION_FUNCTIONAL_DESCRIPTOR_HEADER),             // Size of the descriptor
    USB_CDC_DESC_CS_INTERFACE,                                      // CS_INTERFACE
    USB_CDC_FUNCTIONAL_UNION,                                       // Type of functional descriptor
    0,                                                              // com interface number

    sizeof(USB_CDC_CALL_MANAGEMENT_DESCRIPTOR),                     // Size of the descriptor
    USB_CDC_DESC_CS_INTERFACE,                                      // CS_INTERFACE
    USB_CDC_FUNCTIONAL_CALL_MANAGEMENT,                             // Type of functional descriptor
    0x00,                                                           // bmCapabilities of CallManagement
    1,                                                              // Data interface number
    // Interrupt Endpoint (IN) Descriptor
    0x07,                           // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,        // Endpoint Descriptor
    0x81,                           // EndpointAddress ( EP1 IN INTERRUPT)
    USB_TRANSFER_TYPE_INTERRUPT,    // Attributes type of EP (INTERRUPT)
    0x0A,0x00,                      // Max packet size of this EP
    0x02,                           // Interval (in ms)
    // Interface Descriptor
    9,                                  // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,           // INTERFACE descriptor type
    1,                                  // Interface Number
    0,                                  // Alternate Setting Number
    2,                                  // Number of endpoints in this interface
    USB_CDC_DATA_INTERFACE_CLASS_CODE,  // Class code
    0,                                  // Subclass code
    USB_CDC_PROTOCOL_NO_CLASS_SPECIFIC, // Protocol code
    0,                                  // Interface string index
    // Bulk Endpoint (OUT) Descriptor
    0x07,                       // Sizeof of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor
    0x02,                       // Endpoint Address BULK OUT
    USB_TRANSFER_TYPE_BULK,     // Attributes BULK EP
    0x40,0x00,                  // MaxPacket Size of EP (BULK OUT)
    0x00,                       // Interval Can be ignored for BULK EPs.
    // Bulk Endpoint (IN)Descriptor
    0x07,                       // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor
    0x82,                       // EndpointAddress BULK IN
    USB_TRANSFER_TYPE_BULK,     // Attributes BULK EP
    0x40,0x00,                  // MaxPacket Size of EP (BULK IN)
    0x00,                       // Interval Can be ignored for BULK EPs.
};

//*************************************
//  String descriptors.
//************************************

// Language code string descriptor 0
const struct
{
    uint8_t bLength;
    uint8_t bDscType;
    uint16_t string[1];
}
sd000 =
{
    sizeof(sd000),          // Size of this descriptor in bytes
    USB_DESCRIPTOR_STRING,  // STRING descriptor type
    {0x0409}                // Language ID
};

// Manufacturer string descriptor 1
const struct
{
    uint8_t bLength;        // Size of this descriptor in bytes
    uint8_t bDscType;       // STRING descriptor type
    uint16_t string[25];    // String
}
sd001 =
{
    sizeof(sd001),
    USB_DESCRIPTOR_STRING,
    {'M','i','c','r','o','c','h','i','p',' ',
     'T','e','c','h','n','o','l','o','g','y',' ','I','n','c','.'}
};

// Product string descriptor 2
const struct
{
    uint8_t bLength;        // Size of this descriptor in bytes
    uint8_t bDscType;       // STRING descriptor type
    uint16_t string[22];    // String
}
sd002 =
{
    sizeof(sd002),
    USB_DESCRIPTOR_STRING,
    {'S','i','m','p','l','e',' ','C','D','C',' ',
     'D','e','v','i','c','e',' ','D','e','m','o' }
};

//**************************************
// Array of string descriptors
//**************************************
USB_DEVICE_STRING_DESCRIPTORS_TABLE stringDescriptors[3]=
{
    (const uint8_t *const)&sd000,
    (const uint8_t *const)&sd001,
    (const uint8_t *const)&sd002
};

//******************************************
// Array of full speed config descriptors
//******************************************
USB_DEVICE_CONFIGURATION_DESCRIPTORS_TABLE fullSpeedConfigDescSet[1] =
{
    fullSpeedConfigurationDescriptor1
};

//************************************************
// USB CDC Device Function Driver Initialization
// Data Structure
//************************************************
const USB_DEVICE_CDC_INIT  cdcInit =
{
    .queueSizeRead = 1,                     // Read Queue Size
    .queueSizeWrite = 1,                    // Write Queue Size
    .queueSizeSerialStateNotification = 1   // Serial State Notification Queue Size
};

//*************************************************
// USB Device Layer Function Driver Registration
// Table
//*************************************************
const USB_DEVICE_FUNCTION_REGISTRATION_TABLE funcRegistrationTable[1] =
{
    {
         .configurationValue = 1 ,                  // Configuration descriptor index
         .driver = (void *)USB_DEVICE_CDC_FUNCTION_DRIVER,  // CDC APIs exposed to the device layer
         .funcDriverIndex = 0 ,                     // Instance index of CDC function driver
         .funcDriverInit = (void *)&cdcInit,        // CDC init data
         .interfaceNumber = 0 ,                     // Start interface number of this instance
         .numberOfInterfaces = 2 ,                  // Total number of interfaces contained in this instance
         .speed = USB_SPEED_FULL|USB_SPEED_HIGH     // USB Speed
    }
};

//*************************************************
// USB Device Layer Master Descriptor Table
//*************************************************
const USB_DEVICE_MASTER_DESCRIPTOR usbMasterDescriptor =
{
    &fullSpeedDeviceDescriptor,    // Full Speed Device Descriptor.
    1,                            // Total number of full speed configurations available.
    &fullSpeedConfigDescSet[0],   // Pointer to array of full speed configurations descriptors.
    NULL,                         // High speed device desc is not supported.
    0,                            // Total number of high speed configurations available.
    NULL,                         // Pointer to array of high speed configurations descriptors.
    3,                            // Total number of string descriptors available.
    stringDescriptors,            // Pointer to array of string descriptors
    NULL,                         // Pointer to full speed dev qualifier.
    NULL,                         // Pointer to high speed dev qualifier.
};

//***************************************************
// USB Device Layer Initialization Data
//***************************************************

USB_DEVICE_INIT usbDevInitData =
{
    // System module initialization
    .moduleInit = {SYS_MODULE_POWER_RUN_FULL},

    // Identifies peripheral (PLIB-level) ID
    .usbID = USB_ID_1,

    // Stop in idle 
    .stopInIdle = false,

    // Suspend in sleep
    .suspendInSleep = false,

    // Endpoint table
    .endpointTable = endpointTable,

    // Interrupt Source for USB module
    .interruptSource = INT_SOURCE_USB_1,

    // Number of function drivers registered to this instance of the
    // USB device layer
    .registeredFuncCount = 1,

    // Function driver table registered to this instance of the USB device layer
    .registeredFunctions = (USB_DEVICE_FUNCTION_REGISTRATION_TABLE*)funcRegistrationTable,

    // Pointer to USB Descriptor structure
    .usbMasterDescriptor = (USB_DEVICE_MASTER_DESCRIPTOR*)&usbMasterDescriptor,

    // USB Device Speed
    .deviceSpeed = USB_SPEED_FULL

};
#endif // of ifdef APP_USE_USB

void TMR_Initialize(void) {
    // Timer1 System Clock
    // Stop the timer
    PLIB_TMR_Stop(APP_TMR_CLOCK);
    // Set the prescaler, and set the clock source as internal
    PLIB_TMR_PrescaleSelect(APP_TMR_CLOCK, APP_TMR_CLKPRESCALE);
    PLIB_TMR_ClockSourceSelect(APP_TMR_CLOCK, TMR_CLOCK_SOURCE_PERIPHERAL_CLOCK);
    // Clear the timer
    PLIB_TMR_Counter16BitClear(APP_TMR_CLOCK);
    // and preset interval
    PLIB_TMR_Period16BitSet(APP_TMR_CLOCK, APP_TMR_CLKINTERVAL);
}

void LEDS_Initialize(void) {
    // red LED
    LEDR_Clear;
    LEDR_Direction;
    LEDR_Mode;
    LEDR_Remap;
    LEDR_OD;
    // yellow LED
    LEDY_Clear;
    LEDY_Direction;
    LEDY_Mode;
    LEDY_Remap;
    LEDY_OD;
    // green LED
    LEDG_Clear;
    LEDG_Direction;
    LEDG_Mode;
    LEDG_Remap;
    LEDG_OD;
    // blue LED
    LEDB_Clear;
    LEDB_Direction;
    LEDB_Mode;
    LEDB_Remap;
    LEDB_OD;
    // white LED
    LEDW_Clear;
    LEDW_Direction;
    LEDW_Mode;
    LEDW_Remap;
    LEDW_OD;
}

void I2C_Initialize(void) {
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

#ifdef APP_USE_USART
void USART_Initialize(void) {
    // USART RX
    PLIB_USART_Disable(APP_USART_RX_ID);
    // Prepare Port
    PLIB_PORTS_PinClear(APP_USART_RX_PORTS_ID, APP_USART_RX_PORT_CHANNEL, APP_USART_RX_PORT_PIN);
    PLIB_PORTS_PinDirectionInputSet(APP_USART_RX_PORTS_ID, APP_USART_RX_PORT_CHANNEL, APP_USART_RX_PORT_PIN);
    PLIB_PORTS_RemapInput(APP_USART_RX_PORTS_ID, APP_USART_RX_REMAP_FUNC, APP_USART_RX_REMAP_PIN);
    // USART (RX) Config
    PLIB_USART_OperationModeSelect(APP_USART_RX_ID, APP_USART_RX_OPER);
    PLIB_USART_HandshakeModeSelect(APP_USART_RX_ID, APP_USART_RX_HAND);
    PLIB_USART_BaudRateSet(APP_USART_RX_ID, APP_PBCLK_FREQ, APP_USART_RX_BAUD);
    // Select 8 data bits, No parity and one stop bit
    PLIB_USART_LineControlModeSelect(APP_USART_RX_ID, APP_USART_RX_MODE);
    
    // USART 1 TX
    PLIB_USART_Disable(APP_USART_TX_ID);
    // Prepare Port
    PLIB_PORTS_PinClear(APP_USART_TX_PORTS_ID, APP_USART_TX_PORT_CHANNEL, APP_USART_TX_PORT_PIN);
    PLIB_PORTS_PinDirectionOutputSet(APP_USART_TX_PORTS_ID, APP_USART_TX_PORT_CHANNEL, APP_USART_TX_PORT_PIN);
    PLIB_PORTS_RemapOutput(APP_USART_TX_PORTS_ID, APP_USART_TX_REMAP_FUNC, APP_USART_TX_REMAP_PIN);
    // USART (TX) Config
    PLIB_USART_OperationModeSelect(APP_USART_TX_ID, APP_USART_TX_OPER);
    PLIB_USART_HandshakeModeSelect(APP_USART_TX_ID, APP_USART_TX_HAND);
    PLIB_USART_BaudRateSet(APP_USART_TX_ID, APP_PBCLK_FREQ, APP_USART_TX_BAUD);
    // Select 8 data bits, No parity and one stop bit
    PLIB_USART_LineControlModeSelect(APP_USART_TX_ID, APP_USART_TX_MODE);
    
    // enabling USARTs in SYS_Startup
}
#endif // of ifdef APP_USE_USART

void INT_Initialize(void) {
    // enable the multi vector
    PLIB_INT_MultiVectorSelect( APP_INT_ID );
#ifdef APP_USE_USB
    // set priority and enable USB1
    PLIB_INT_VectorPrioritySet(APP_INT_ID, INT_VECTOR_USB1, INT_PRIORITY_LEVEL5);
    PLIB_INT_VectorSubPrioritySet(APP_INT_ID, INT_VECTOR_USB1, INT_SUBPRIORITY_LEVEL0);
#endif // of ifdef APP_USE_USB
    // set priority and enable I2C
    PLIB_INT_VectorPrioritySet(APP_INT_ID, INT_VECTOR_I2C1, INT_PRIORITY_LEVEL4);
    PLIB_INT_VectorSubPrioritySet(APP_INT_ID, INT_VECTOR_I2C1, INT_SUBPRIORITY_LEVEL0);    
    // set priority and enable Timer1
    PLIB_INT_VectorPrioritySet(APP_INT_ID, INT_VECTOR_T1, INT_PRIORITY_LEVEL3);
    PLIB_INT_VectorSubPrioritySet(APP_INT_ID, INT_VECTOR_T1, INT_SUBPRIORITY_LEVEL0);
#ifdef APP_USE_USART
    // set priority and enable USART RX
    PLIB_INT_VectorPrioritySet(APP_INT_ID, INT_SOURCE_USART_2_RECEIVE, INT_PRIORITY_LEVEL2);
    PLIB_INT_VectorSubPrioritySet(APP_INT_ID, INT_SOURCE_USART_2_RECEIVE, INT_SUBPRIORITY_LEVEL0);
    // set priority and enable USART TX
    PLIB_INT_VectorPrioritySet(APP_INT_ID, INT_SOURCE_USART_1_TRANSMIT, INT_PRIORITY_LEVEL1);
    PLIB_INT_VectorSubPrioritySet(APP_INT_ID, INT_SOURCE_USART_1_TRANSMIT, INT_SUBPRIORITY_LEVEL0);
#endif // of ifdef APP_USE_USART
    // Enable the global interrupts
    PLIB_INT_Enable(APP_INT_ID);
#ifdef APP_USE_USB
    PLIB_INT_SourceEnable(APP_INT_ID, INT_SOURCE_USB_1);
#endif
    PLIB_INT_SourceEnable(APP_INT_ID, INT_SOURCE_I2C_1_ERROR);
    PLIB_INT_SourceEnable(APP_INT_ID, INT_SOURCE_I2C_1_SLAVE);
    PLIB_INT_SourceEnable(APP_INT_ID, INT_SOURCE_I2C_1_MASTER);
    PLIB_INT_SourceEnable(APP_INT_ID, INT_SOURCE_TIMER_1);
#ifdef APP_USE_USART
    PLIB_INT_SourceEnable(APP_INT_ID, INT_SOURCE_USART_2_RECEIVE);
    PLIB_INT_SourceEnable(APP_INT_ID, INT_SOURCE_USART_1_TRANSMIT);
#endif
}

void SYS_Startup(void) {
    // LCD I2C com
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_SOURCE_I2C_1_ERROR);
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_SOURCE_I2C_1_SLAVE);
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_SOURCE_I2C_1_MASTER);
    // see http://ww1.microchip.com/downloads/en/DeviceDoc/80000531F.pdf Errata
    // Enabling I2C1 makes Ports A0 / A1 unusable
    // Enabling I2C2 makes Ports B5 / B6 unusable
    PLIB_I2C_Enable(APP_LCD_I2C_ID);
    // system clock 
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_SOURCE_TIMER_1);
    PLIB_TMR_Start(APP_TMR_CLOCK);
#ifdef APP_USE_USART
    // enable USARTs
    PLIB_USART_Enable(APP_USART_RX_ID);
    PLIB_USART_Enable(APP_USART_TX_ID);    
#endif
}

/* Initialize the System */
void SYS_Initialize ( void *data )
{
    // Initialize all modules and the application
    // Timer initializations
    TMR_Initialize();
    // Set outputting Ports to Output (default is Input), clear analog Input
    LEDS_Initialize();
#ifdef APP_USE_USB
    // Initialize the USB device layer
    sysObjects.usbDevObject = USB_DEVICE_Initialize (USB_DEVICE_INDEX_0 ,
                                                    ( SYS_MODULE_INIT* ) & usbDevInitData);
    // Check if the object returned by the device layer is valid
    SYS_ASSERT((SYS_MODULE_OBJ_INVALID != usbDevObject), "Invalid USB DEVICE object");
#endif
    // init I2C
    I2C_Initialize();
#ifdef APP_USE_USART
    // init USART
    USART_Initialize();
#endif
    /* TODO:  Initialize all modules and the application. */

    // Initialize the Application
    APP_Initialize();

    // Initialize the interrupt system (after all other init's)
    INT_Initialize();
    // startup system
    SYS_Startup();
}
