/* 
 * File:    app.h
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-02-26
 * Description:
 *   not yet supplied
 * Major Changes:
 *   Version 0: alpha development
 */

#ifndef APP_H
#define	APP_H

#ifdef	__cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config/sk_pic32_mx/system_config.h"
#ifdef APP_USE_USB
#include "usb/usb_device_cdc.h"
#endif
    
// Application States
typedef enum {
    // Application?s initial state
    APP_STATE_INIT,
    // Init LCD
    APP_STATE_LCD_INIT,
#ifdef APP_USE_USART
    // Init USART
    APP_STATE_USART_INIT,
#endif // of ifdef APP_USE_USART
#ifdef APP_USE_USB
    // Init USB
    APP_STATE_USB_INIT,
    // USB Configuration (wait on host to activate this device)
    APP_STATE_USB_CONFIGURATION,
    // Start a USB TX transaction
    APP_STATE_USB_WRITE,
    // Wait for the write to complete
    APP_STATE_USB_WAIT_WRITE_COMPLETE,
#endif // of ifdef APP_USE_USB
    // POE.net input phase
    APP_STATE_POENET_INPUT,
    // POE.net input interpretation
    APP_STATE_POENET_INPUT_READY,
    // POE.net command interpretation
    APP_STATE_POENET_COMMAND,
    // POE.net output phase
    APP_STATE_POENET_OUTPUT_PREPARE,
    // POE.net output phase finished
    APP_STATE_POENET_OUTPUT_READY,
#ifdef APP_POEnet_SECONDARY
    // POE.net pass secondary
    APP_STATE_POENET_PASS,
#endif // ifdef APP_POEnet_SECONDARY
    // LCD update
    APP_LCD_UPDATE,
    // (not used) Register timer callback
    APP_STATE_REGISTER_TMR,
    // (not used) Check Time
    APP_STATE_OUTPUT_TIME,
    // Exit code for something unexpected happened (give alarm signal)
    APP_STATE_ERROR,
    // Application hold after error
    APP_STATE_HOLD
} APP_STATES;

#ifdef APP_USE_USB
#define USB_DEVICE_CDC_INDEX_0 0
#endif

// App Timing Structure
typedef struct {
    int milliSeconds;
    int Seconds;
    int Minutes;
    int Hours;
    int Days;
    int Wait;
} APP_TIMING;

// I2C definitions
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

typedef enum {
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

// Application Data
typedef struct
{
    // The application's current state
    APP_STATES        state;
    // system timer handling
    int               timerCount;
    int               timerRepeat;
    bool              timerExpired;
    // Timing structure
    APP_TIMING        time;
    int               lastSecond;
    bool              pollSecond;
    // LCD I2C data
    LCD_Init_Sequence LCD_Init;
    I2C_States        I2C_State;
    I2C_Data_State    I2C_Transfer;
    uint8_t           LCD_Write[APP_LCD_I2C_WRITE_BUFFER_SIZE];
    int               LCD_WriteIx;
    uint8_t           LCD_Read[APP_LCD_I2C_READ_BUFFER_SIZE];
    int               LCD_ReadIx;
    char              LCD_Line[LCD_LINEBUFFERS][LCD_LINEBUFFER_SIZE];
    bool              LCD_Backlight;
    APP_STATES        LCD_Return_AppState;
    char              POEnetPrimInputBuf[APP_BUFFER_SIZE];
    int               POEnetPrimInputSize;
    int               POEnetPrimInputIdx;
    char              POEnetPrimOutputBuf[APP_BUFFER_SIZE];
    int               POEnetPrimOutputSize;
    int               POEnetPrimOutputIdx;
#ifdef APP_POEnet_SECONDARY
    char              POEnetSecInputBuf[APP_BUFFER_SIZE];
    int               POEnetSecInputSize;
    int               POEnetSecInputIdx;
    char              POEnetSecOutputBuf[APP_BUFFER_SIZE];
    int               POEnetSecOutputSize;
    int               POEnetSecOutputIdx;
#endif // of ifdef APP_POEnet_SECONDARY
#ifdef APP_USE_USB
    // USB: Device layer handle returned by device layer open function
    USB_DEVICE_HANDLE    deviceHandle;
    USB_DEVICE_CDC_INDEX cdcInstance;
    // USB: Device configured state
    bool                 isConfigured;
    // USB: Read Data Buffer
    //char                 readBuffer[USB_BUFFER_SIZE];
    // -> one of the Input Buffers defined by USB_INPUT_BUF
    // USB: Set Line Coding Data
    USB_CDC_LINE_CODING  setLineCodingData;
    // USB: Get Line Coding Data
    USB_CDC_LINE_CODING  getLineCodingData;
    // USB: Control Line State
    USB_CDC_CONTROL_LINE_STATE controlLineStateData;
    // USB: Break data
    uint16_t             breakData;
    // USB: Read transfer handle
    USB_DEVICE_CDC_TRANSFER_HANDLE readTransferHandle;
    // USB: Write transfer handle
    USB_DEVICE_CDC_TRANSFER_HANDLE writeTransferHandle;
    // USB: True if a character was read
    bool                 isReadComplete;
    // USB: True if a character was written
    bool                 isWriteComplete;
    // extend USB: output buffer
    //char                 writeBuffer[USB_BUFFER_SIZE];
    // -> one of the Output Buffers defined by USB_OUTPUT_BUF
    //int                  writeCount;
    // -> one of the Output Sizes defined by USB_OUTPUT_SIZE
#endif // of ifdef APP_USE_USB
#ifdef APP_USE_USART
    // USART 
    // defined usage of Buffers by USART_INPUT_BUF, USART_INPUT_SIZE, USART_INPUT_IDX, USART_OUTPUT_BUF, USART_OUTPUT_SIZE, USART_OUTPUT_IDX
    //char                 USARTreadBuffer[APP_USART_RX_BUFFER_SIZE];
    //int                  USARTreadIdx;
    //char                 USARTwriteBuffer[APP_USART_TX_BUFFER_SIZE];
    //int                  USARTwriteIdx;
#endif // of ifdef APP_USE_USART
    // POE.net handling
    char                 POEnetCommand[APP_STRING_SIZE];
    int                  POEnet_NodeId;
} APP_DATA;

#ifdef APP_USE_USB
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// These routines are called by drivers when certain events occur.
/*******************************************************************************
  Function:
    void APP_UsbDeviceEventCallBack(USB_DEVICE_EVENTS events)

  Summary:
    Device layer event notification callback.

  Description:
    This routine defines the device layer event notification callback.

  Precondition:
    The device layer should be opened by the application and the callback should
    be registered with the device layer.

  Parameters:
    events  - specific device event

  Returns:
    None.

  Remarks:
    None.
*/

bool APP_USBStateReset ( void );

void APP_USBDeviceEventCallBack(USB_DEVICE_EVENT events,
        void * eventData, uintptr_t context);

void APP_USBDeviceCDCEventHandler
(
    USB_DEVICE_CDC_INDEX index ,
    USB_DEVICE_CDC_EVENT event ,
    void * pData,
    uintptr_t userData
);

void APP_USBDeviceEventHandler ( 
    USB_DEVICE_EVENT event
,   void * eventData
,   uintptr_t context
);

USB_DEVICE_CDC_EVENT_RESPONSE APP_USBDeviceCDCEventHandler (
    USB_DEVICE_CDC_INDEX index
,   USB_DEVICE_CDC_EVENT event
,   void * pData
,   uintptr_t userData
);
#endif // of ifdef APP_USE_USB

// helper routines
void ClearBuffer(char *buffer);

void ClearString(char *str);

// APP routines
void APP_Initialize ( void );

void APP_TimingCallback ( void );

void APP_CheckTimedLED ( void );

void APP_I2C_AddWrite( uint8_t WriteIn);

void APP_I2C_M_Write(void);

void APP_I2C_Process(void);

bool APP_I2C_Ready(void);

void APP_LCD_AddCharWrite( char aChar);

void APP_LCD_Update(void);

void APP_LCD_ClearLine( uint8_t line);

void APP_LCD_Print(uint8_t line, uint8_t pos, char* string);

bool APP_LCD_Init(void);

bool APP_LCD_Ready(void);

#ifdef APP_USE_USART
void APP_USART_Read(void);

void APP_USART_Write(void);
#endif // of ifdef APP_USE_USART

void APP_Tasks ( void );


// *****************************************************************************
// *****************************************************************************
// Section: extern declarations
// *****************************************************************************
// *****************************************************************************

extern APP_DATA appData;

#ifdef APP_USE_USB
extern const USB_DEVICE_FUNCTION_REGISTRATION_TABLE
    funcRegistrationTable[USB_DEVICE_CDC_INSTANCES_NUMBER];

extern const USB_DEVICE_MASTER_DESCRIPTOR usbMasterDescriptor;
#endif // of ifdef APP_USE_USB

#ifdef	__cplusplus
}
#endif

#endif	/* APP_H */

