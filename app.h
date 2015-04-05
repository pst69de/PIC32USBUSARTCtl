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
//#include "usb/usb_device_cdc.h"
#endif
    
// Application States
typedef enum {
    // Application?s initial state
    APP_STATE_INIT,
    // POE.net node initialization
    APP_STATE_POENET_INIT,
    // Init LCD
    APP_STATE_LCD_INIT,
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
#ifdef APP_USE_UART
    // Init UART
    APP_STATE_UART_INIT,
#endif // of ifdef APP_USE_UART
    // after init's send reset
    APP_STATE_POENET_RESET,
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
    // poll data
    APP_STATE_POLL_DATA,
#ifdef APP_USE_ADC
    // Start a ADC read
    APP_STATE_START_ADC,
    // Convert initiated ADC read
    APP_STATE_CONVERT_ADC,
    // wait on ADC conversion
    APP_STATE_READ_ADC,
#endif // ifdef APP_USE_ADC
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

// App Timing Structure
typedef struct {
    int milliSeconds;
    int Seconds;
    int Minutes;
    int Hours;
    int Days;
    int Wait;
} APP_TIMING;

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
    // LCD (Re-)Init function callback
    APP_STATES        LCD_Init_Return;
    // LCD Refresh function callback
    APP_STATES        LCD_Return_AppState;
    // LCD Dummy switch for Backlight
    bool              LCD_Dummy_switch;
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
#endif // ifdef APP_POEnet_SECONDARY
// -> USB Handling migrated to POEusb; UART needs only buffer handling
    // POE.net handling
    char              POEnetUID[APP_STRING_SIZE];
    char              POEnetCommand[APP_STRING_SIZE];
    char              POEnetXMLError[2 * APP_STRING_SIZE];
    int               POEnet_NodeId;
    bool              pollValues;
    int               pollGranularity;
    APP_STATES        poll_Return_AppState;
// ADC_PinIdx is used as polling state for data polling
    int               ADC_PinIdx;
#ifdef APP_USE_ADC
    APP_STATES        ADC_Return_AppState;
    int               ADC_PinValue[APP_ADC_NUM_PINS];
    float             ADC_Numerator[APP_ADC_NUM_PINS];
    float             ADC_Denominator[APP_ADC_NUM_PINS];
    float             ADC_Value[APP_ADC_NUM_PINS];
    char              ADC_Unit[APP_ADC_NUM_PINS][APP_STRING_SIZE];
    char              ADC_Representation[APP_ADC_NUM_PINS][APP_STRING_SIZE];
#endif // ifdef APP_USE_ADC
#ifdef APP_USE_DIO
    int               DI_Value[APP_DI_COUNT];
    char              DI_LoValue[APP_DI_COUNT][APP_STRING_SIZE];
    char              DI_HiValue[APP_DI_COUNT][APP_STRING_SIZE];
    int               DO_Value[APP_DO_COUNT];
    char              DO_LoValue[APP_DO_COUNT][APP_STRING_SIZE];
    char              DO_HiValue[APP_DO_COUNT][APP_STRING_SIZE];
#endif
} APP_DATA;

// -> USB Handling migrated to POEusb

// helper routines
void ClearBuffer(char *buffer);

void ClearString(char *str);

// APP routines
void APP_Initialize ( void );

void APP_TimingCallback ( void );

void APP_CheckTimedLED ( void );

#ifdef APP_USE_UART
void APP_UART_Read(void);

void APP_UART_Write(void);
#endif // of ifdef APP_USE_UART

void APP_Tasks ( void );

// *****************************************************************************
// Section: extern declarations
// *****************************************************************************

extern APP_DATA appData;

#ifdef	__cplusplus
}
#endif

#endif	/* APP_H */

