/*
 * File:   system_init.c
 * Author: patrick
 *
 * Created on 2014-09-13
 */

#include <xc.h>


// PIC32MX250F128B Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

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

#include "system_definitions.h"
#include "system_config.h"
#include "usb/usb_device.h"
#include "../../app.h"

/* Global allocation of structure to hold system objects handles */
SYSTEM_OBJECTS sysObjects;

/****************************************************
 * Endpoint Table needed by the Device Layer.
 ****************************************************/

uint8_t __attribute__((aligned(512))) endpointTable[USB_DEVICE_ENDPOINT_TABLE_SIZE];

/****************************************************
 * USB Device Layer Initialization Data
 ****************************************************/

USB_DEVICE_INIT usbDevInitData =
{
    /* System module initialization */
    .moduleInit = {SYS_MODULE_POWER_RUN_FULL},

    /* Identifies peripheral (PLIB-level) ID */
    .usbID = USB_ID_1,

    /* Stop in idle */
    .stopInIdle = false,

    /* Suspend in sleep */
    .suspendInSleep = false,

    /* Endpoint table */
    .endpointTable = endpointTable,

    /* Interrupt Source for USB module */
    .interruptSource = INT_SOURCE_USB_1,

    /* Number of function drivers registered to this instance of the
       USB device layer */
    .registeredFuncCount = 1,

    /* Function driver table registered to this instance of the USB device layer*/
    .registeredFunctions = (USB_DEVICE_FUNCTION_REGISTRATION_TABLE*)funcRegistrationTable,

    /* Pointer to USB Descriptor structure */
    .usbMasterDescriptor = (USB_DEVICE_MASTER_DESCRIPTOR*)&usbMasterDescriptor,

    /* USB Device Speed */
    .deviceSpeed = USB_SPEED_FULL

};

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

void PORTS_Initialize(void) {
    /* Set outputting Ports to Output (default ist Input), clear Analog Input */
    /* red LED */
    PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
    PLIB_PORTS_PinDirectionOutputSet(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
    PLIB_PORTS_PinModeSelect(APP_LED_PORTS_ID, APP_LEDR_AIPIN, PORTS_PIN_MODE_DIGITAL);
    /* green LED */
    PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDG_PIN);
    PLIB_PORTS_PinDirectionOutputSet(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDG_PIN);
    PLIB_PORTS_PinModeSelect(APP_LED_PORTS_ID, APP_LEDG_AIPIN, PORTS_PIN_MODE_DIGITAL);
}

void I2C_Initialize(void) {
    PLIB_I2C_Disable(APP_LCD_I2C_ID);
    // Prepare Ports I2C 1  
    PLIB_PORTS_PinClear(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SCL_PIN);
    PLIB_PORTS_PinClear(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SDA_PIN);
    // open drain configuration I2C 1 Pins 5V tolerant, pull up with 2k2
    PLIB_PORTS_PinOpenDrainEnable(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SCL_PIN);
    PLIB_PORTS_PinOpenDrainEnable(APP_LCD_PORTS_ID, APP_LCD_PORT_CHANNEL, APP_LCD_SDA_PIN);
    // Configure General I2C Options
    PLIB_I2C_SlaveClockStretchingEnable(APP_LCD_I2C_ID);
    // future option SMB compatibility
    PLIB_I2C_SMBDisable(APP_LCD_I2C_ID);
    // future option high baud rates (>100kHz)
    PLIB_I2C_HighFrequencyDisable(APP_LCD_I2C_ID);
    // only 7-Bit addresses
    PLIB_I2C_ReservedAddressProtectEnable(APP_LCD_I2C_ID);
    // Idle when proc idle (no operation in idle mode)
    PLIB_I2C_StopInIdleEnable(APP_LCD_I2C_ID);
    // Set Desired baud rate
    PLIB_I2C_BaudRateSet( APP_LCD_I2C_ID, APP_SYSCLK_FREQ, APP_LCD_I2C_BAUD);
    // Set the appropriate slave address (slave only)
    //PLIB_I2C_SlaveAddress7BitSet(APP_LCD_I2C_ID, MY_SLAVE_ADDRESS);
    // Optional:  Clear and enable interrupts before enabling the I2C module.
    // Enable the module -> in SYS_Startup
}

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
    // Enable the asynchronous mode
    PLIB_USART_SyncModeSelect(APP_USART_RX_ID, USART_ASYNC_MODE);
    /* Select 8 data bits, No parity and one stop bit */
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
    // Enable the asynchronous mode
    PLIB_USART_SyncModeSelect(APP_USART_TX_ID, USART_ASYNC_MODE);
    /* Select 8 data bits, No parity and one stop bit */
    PLIB_USART_LineControlModeSelect(APP_USART_TX_ID, APP_USART_TX_MODE);
    
    // enabling USARTs in SYS_Startup
}

void INT_Initialize(void) {
    /* enable the multi vector */
    PLIB_INT_MultiVectorSelect( INT_ID_0 );
    /* set priority and enable USB1 */
    PLIB_INT_VectorPrioritySet(APP_INT_ID, INT_VECTOR_USB1, INT_PRIORITY_LEVEL5);
    PLIB_INT_VectorSubPrioritySet(APP_INT_ID, INT_VECTOR_USB1, INT_SUBPRIORITY_LEVEL0);
    /* set priority and enable I2C */
    PLIB_INT_VectorPrioritySet(APP_INT_ID, INT_VECTOR_I2C1, INT_PRIORITY_LEVEL4);
    PLIB_INT_VectorSubPrioritySet(APP_INT_ID, INT_VECTOR_I2C1, INT_SUBPRIORITY_LEVEL0);
    /* set priority and enable Timer1 */
    PLIB_INT_VectorPrioritySet(APP_INT_ID, INT_VECTOR_T1, INT_PRIORITY_LEVEL3);
    PLIB_INT_VectorSubPrioritySet(APP_INT_ID, INT_VECTOR_T1, INT_SUBPRIORITY_LEVEL0);
    /* set priority and enable USART RX */
    PLIB_INT_VectorPrioritySet(APP_INT_ID, INT_SOURCE_USART_2_RECEIVE, INT_PRIORITY_LEVEL2);
    PLIB_INT_VectorSubPrioritySet(APP_INT_ID, INT_SOURCE_USART_2_RECEIVE, INT_SUBPRIORITY_LEVEL0);
    /* set priority and enable USART TX */
    PLIB_INT_VectorPrioritySet(APP_INT_ID, INT_SOURCE_USART_1_TRANSMIT, INT_PRIORITY_LEVEL1);
    PLIB_INT_VectorSubPrioritySet(APP_INT_ID, INT_SOURCE_USART_1_TRANSMIT, INT_SUBPRIORITY_LEVEL0);
    /* Enable the global interrupts */
    PLIB_INT_Enable(APP_INT_ID);
    PLIB_INT_SourceEnable(APP_INT_ID, INT_SOURCE_USB_1);
    PLIB_INT_SourceEnable(APP_INT_ID, INT_SOURCE_I2C_1_MASTER);
    PLIB_INT_SourceEnable(APP_INT_ID, INT_SOURCE_TIMER_1);
    //Not yet
    //PLIB_INT_SourceEnable(APP_INT_ID, INT_SOURCE_USART_2_RECEIVE);
    //PLIB_INT_SourceEnable(APP_INT_ID, INT_SOURCE_USART_1_TRANSMIT);
}

void SYS_Startup(void) {
    /* LCD I2C com */
    PLIB_I2C_Enable(I2C_ID_2);
    /* system clock */
    PLIB_INT_SourceFlagClear(APP_INT_ID, INT_SOURCE_TIMER_1);
    PLIB_TMR_Start(APP_TMR_CLOCK);
    // enable USARTs
    //Not yet
    //PLIB_USART_Enable(APP_USART_RX_ID);
    //PLIB_USART_Enable(APP_USART_TX_ID);    
}

/* Initialize the System */
void SYS_Initialize ( void *data )
{
    /* Initialize all modules and the application */
    /* Timer initializations */
    TMR_Initialize();
    /* Set outputting Ports to Output (default is Input), clear analog Input */
    PORTS_Initialize();
    /* Initialize the USB device layer */
    sysObjects.usbDevObject = USB_DEVICE_Initialize (USB_DEVICE_INDEX_0 ,
                                                    ( SYS_MODULE_INIT* ) & usbDevInitData);
    /* Check if the object returned by the device layer is valid */
    SYS_ASSERT((SYS_MODULE_OBJ_INVALID != usbDevObject), "Invalid USB DEVICE object");
    /* init I2C */
    I2C_Initialize();
    /* init USART */
    USART_Initialize();
    /* TODO:  Initialize all modules and the application. */

    /* Initialize the Application */
    APP_Initialize();

    /* Initialize the interrupt system (after all other init's) */
    INT_Initialize();
    /* startup system */
    SYS_Startup();
}
