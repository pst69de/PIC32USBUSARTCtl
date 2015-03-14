/*
 * File:    app.c
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-02-26
 * Description:
 *   not yet supplied
 * Major Changes:
 *   Version 0: alpha development
 */

#include <string.h>
#include "app.h"
#include "system_config.h"
#include "POEnet.h"

// helper routines
void ClearBuffer(char *buffer) {
    buffer = memset(buffer, '\0', APP_BUFFER_SIZE);
    //char *first = buffer;
    //while (*buffer != '\0') {
    //   *buffer = '\0';
    //    buffer++;
    //    if (buffer-first >= APP_BUFFER_SIZE) { break; }
    //}
}

void ClearString(char *str) {
    str = memset(str, '\0', APP_STRING_SIZE);
    //char *first = str;
    //while (*str != '\0') {
    //    *str = '\0';
    //    str++;
    //    if (str-first >= APP_STRING_SIZE) { break; }
    //}
}

// APP's data struct
APP_DATA appData;

// Initialize
void APP_Initialize ( void )
{
    int i;
    // Initialize App Timing to zero 
    appData.time.milliSeconds = 0;
    appData.time.Seconds = 0;
    appData.time.Minutes = 0;
    appData.time.Hours = 0;
    appData.time.Days = 0;
    appData.lastSecond = 59;
    appData.time.Wait = 0;
    // timing repetition 
    appData.timerCount = 0;
    appData.timerRepeat = 0;
    appData.timerExpired = false;
    // LCD I2C data 
    appData.I2C_State = I2C_UNINITIALIZED;
    appData.I2C_Transfer = I2C_Idle;
    for (i = 0; i < APP_LCD_I2C_WRITE_BUFFER_SIZE; i++) { appData.LCD_Write[i] = 0;};
    appData.LCD_WriteIx = 0;
    for (i = 0; i < APP_LCD_I2C_READ_BUFFER_SIZE; i++) { appData.LCD_Read[i] = 0;};
    appData.LCD_ReadIx = 0;
    for (i = 0; i < LCD_LINEBUFFERS; i++) { APP_LCD_ClearLine(i); }
#ifdef APP_USE_USB
    // USB Inits
    appData.deviceHandle = -1;
    appData.cdcInstance = 0;
    appData.isConfigured = false;
    appData.getLineCodingData.dwDTERate = 9600;
    appData.getLineCodingData.bCharFormat = 0;
    appData.getLineCodingData.bParityType = 0;
    appData.getLineCodingData.bDataBits = 8;
    appData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    appData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    appData.isReadComplete = true;
    appData.isWriteComplete = true;
#endif // of ifdef APP_USE_USB
#ifdef APP_USE_USART
    // USART init
    ClearBuffer(&appData.USARTreadBuffer[0]);
    //for (i = 0; i < APP_USART_RX_BUFFER_SIZE; i++) { appData.USARTreadBuffer[i] = 0;};
    appData.USARTreadIdx = 0;
    ClearBuffer(&appData.USARTwriteBuffer[0]);
    //for (i = 0; i < APP_USART_TX_BUFFER_SIZE; i++) { appData.USARTwriteBuffer[i] = 0;};
    appData.USARTwriteIdx = 0;
#endif // of ifdef APP_USE_USART
    // POE.net handling
    ClearString(&appData.POEnetCommand[0]);
    appData.POEnet_NodeId = -1;
    // Place the App state machine in its initial state.
    appData.state = APP_STATE_INIT;
}

const char numChar[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

// APP Time Base
void APP_TimingCallback ( void ) {
    // Adjust App Timing
    appData.time.milliSeconds++;
    if (appData.time.milliSeconds == 1000) {
        appData.time.milliSeconds = 0;
        appData.time.Seconds++;
        if (appData.time.Seconds == 60) {
            appData.time.Seconds = 0;
            appData.time.Minutes++;
            if (appData.time.Minutes == 60) {
                appData.time.Minutes = 0;
                appData.time.Hours++;
                if (appData.time.Hours == 24) {
                    appData.time.Hours = 0;
                    appData.time.Days++;
                }
                // Set Hours to Time field
                appData.LCD_Line[0][2] = numChar[appData.time.Hours % 10];
                appData.LCD_Line[0][1] = numChar[appData.time.Hours / 10];
            }
            // Set Minutes to Time field
            appData.LCD_Line[0][5] = numChar[appData.time.Minutes % 10];
            appData.LCD_Line[0][4] = numChar[appData.time.Minutes / 10];
        }
        // Set Seconds to Time field
        appData.LCD_Line[0][8] = numChar[appData.time.Seconds % 10];
        appData.LCD_Line[0][7] = numChar[appData.time.Seconds / 10];
        appData.LCD_Return_AppState = appData.state;
        appData.state = APP_LCD_UPDATE;
    }
    if (appData.timerCount > 0) {
        appData.timerCount--;
        if (appData.timerCount == 0) {
            appData.timerExpired = true;
            appData.timerCount = appData.timerRepeat;
        }
    }
    if (appData.time.Wait > 0) {
        appData.time.Wait--;
    }
}

// APP Timed LED callback registration routine
// boolean return for future extension e.g. with SYS_TMR-Registration
bool APP_RegisterTimedLED ( int thisTimeMs) {
    appData.timerRepeat = thisTimeMs;
    appData.timerCount = appData.timerRepeat;
    return true;
}

// APP Timed LED routine (blink on different operation states)
void APP_CheckTimedLED (void) {
    if (appData.timerExpired) {
        // Toggle LED & clear flag
        LEDR_Toggle;
        appData.timerExpired = false;
    }
}

#ifdef APP_USE_USB
// USB Routines
//****************************************************
// This function is called in every step of the
// application state machine.
//****************************************************
bool APP_USBStateReset(void) {
    // This function returns true if the device was reset
    bool retVal;
    if(!appData.isConfigured) {
        // Clear Display from USB Status
        appData.LCD_Line[0][11] = '-';
        appData.LCD_Line[0][12] = '-';
        appData.state = APP_LCD_UPDATE;
        appData.LCD_Return_AppState = APP_STATE_USB_CONFIGURATION;
        appData.state = APP_STATE_USB_CONFIGURATION;
        appData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        appData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        appData.isReadComplete = true;
        appData.isWriteComplete = true;
        retVal = true;
    }
    else {
        retVal = false;
    }
    return(retVal);
}

//******************************************************
// USB Device Layer Events - Application Event Handler
//******************************************************
void APP_USBDeviceEventHandler ( USB_DEVICE_EVENT event,
        void * eventData, uintptr_t context ) {
    uint8_t * configuredEventData;
    switch ( event ) {
        case USB_DEVICE_EVENT_RESET:
            appData.isConfigured = false;
            break;
        case USB_DEVICE_EVENT_CONFIGURED:
            // Check the configuration. We only support configuration 1 
            configuredEventData = (uint8_t *)eventData;
            if ( *configuredEventData == 1) {
                // Register the CDC Device application event handler here.
                // Note how the appData object pointer is passed as the
                // user data 
                USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0,
                        APP_USBDeviceCDCEventHandler, (uintptr_t)&appData);
                // Mark that the device is now configured 
                appData.isConfigured = true;
                appData.LCD_Line[0][11] = 'S';
            }
            break;
        case USB_DEVICE_EVENT_POWER_DETECTED:
            // VBUS was detected. We can attach the device 
            USB_DEVICE_Attach(appData.deviceHandle);
            appData.LCD_Line[0][12] = 'B';
            break;
        case USB_DEVICE_EVENT_POWER_REMOVED:
            // VBUS is not available any more. Detach the device. 
            USB_DEVICE_Detach(appData.deviceHandle);
            appData.LCD_Line[0][12] = '-';
            break;
        case USB_DEVICE_EVENT_SUSPENDED:
            break;
        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_ERROR:
        default:
            break;
    }
}

//******************************************************
// USB CDC Device Events - Application Event Handler
//******************************************************

USB_DEVICE_CDC_EVENT_RESPONSE APP_USBDeviceCDCEventHandler (
    USB_DEVICE_CDC_INDEX index ,
    USB_DEVICE_CDC_EVENT event ,
    void * pData,
    uintptr_t userData
) {
    APP_DATA * appDataObject;
    appDataObject = (APP_DATA *)userData;
    USB_CDC_CONTROL_LINE_STATE * controlLineStateData;
    uint16_t * breakData;
    switch ( event ) {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:
            // This means the host wants to know the current line
            // coding. This is a control transfer request. Use the
            // USB_DEVICE_ControlSend() function to send the data to
            // host.
            USB_DEVICE_ControlSend(appDataObject->deviceHandle, &appDataObject->getLineCodingData,
                    sizeof(USB_CDC_LINE_CODING));
            break;
        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:
            // This means the host wants to set the line coding.
            // This is a control transfer request. Use the
            // USB_DEVICE_ControlReceive() function to receive the
            // data from the host 
            USB_DEVICE_ControlReceive(appDataObject->deviceHandle, &appDataObject->setLineCodingData,
                    sizeof(USB_CDC_LINE_CODING));
            break;
        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:
            // This means the host is setting the control line state.
            // Read the control line state. We will accept this request
            // for now. 
            controlLineStateData = (USB_CDC_CONTROL_LINE_STATE *)pData;
            appDataObject->controlLineStateData.dtr = controlLineStateData->dtr;
            appDataObject->controlLineStateData.carrier =
                controlLineStateData->carrier;
            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_SEND_BREAK:
            // This means that the host is requesting that a break of the
            // specified duration be sent. Read the break duration
            breakData = (uint16_t *)pData;
            appDataObject->breakData = *breakData;
            break;
        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:
            // This means that the host has sent some data
            appDataObject->isReadComplete = true;
            break;
        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:
            // The data stage of the last control transfer is
            // complete. For now we accept all the data
            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:
            // This means the GET LINE CODING function data is valid. We dont
            // do much with this data in this demo.
            break;
        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:
            // This means that the data write got completed. We can schedule
            // the next read.
            appDataObject->isWriteComplete = true;
            break;
        default:
            break;
    }
    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}
#endif // of ifdef APP_USE_USB

// LCD I2C Routines
// I2C write buffer filling in LIFO mode
void APP_I2C_AddWrite( uint8_t WriteIn) {
    appData.LCD_Write[appData.LCD_WriteIx] = WriteIn;
    appData.LCD_WriteIx++;
}

void APP_I2C_M_Write(void) {
    appData.I2C_State = I2C_MASTER_WRITE;
    appData.I2C_Transfer = I2C_MS_Start;
    LEDY_Set;
    PLIB_I2C_MasterStart(APP_LCD_I2C_ID);
}

void APP_I2C_Process(void) {
    uint8_t Xfer;
    switch ( appData.I2C_State) {
        case I2C_UNINITIALIZED:
            break;
        case I2C_MASTER_IDLE:
            break;
        case I2C_MASTER_WRITE:
            switch (appData.I2C_Transfer) {
                case I2C_MS_Start:
                    // for this purpose no collision checking by PLIB_I2C_ArbitrationLossHasOccurred
                    if (PLIB_I2C_TransmitterIsReady(APP_LCD_I2C_ID)) {
                        // top byte in buffer should always be a formatted i2c slave address
                        appData.I2C_Transfer = I2C_MS_Address;
                        appData.LCD_WriteIx--;
                        Xfer = appData.LCD_Write[appData.LCD_WriteIx];
                        PLIB_I2C_StartClear(APP_LCD_I2C_ID);
                        PLIB_I2C_TransmitterByteSend(APP_LCD_I2C_ID, Xfer);
                    }
                    break;
                case I2C_MS_Address:
                    // for this purpose no collision checking by PLIB_I2C_ArbitrationLossHasOccurred
                    if (!PLIB_I2C_TransmitterByteWasAcknowledged(APP_LCD_I2C_ID)) {
                        LEDR_Set;
                    } else {
                        if ( PLIB_I2C_TransmitterIsReady(APP_LCD_I2C_ID)) {
                            appData.I2C_Transfer = I2C_MS_Transmit;
                            appData.LCD_WriteIx--;
                            Xfer = appData.LCD_Write[appData.LCD_WriteIx];
                            PLIB_I2C_TransmitterByteSend(APP_LCD_I2C_ID, Xfer);
                        }
                    }
                    break;
                case I2C_MS_Transmit:
                    // for this purpose no collision checking by PLIB_I2C_ArbitrationLossHasOccurred
                    if (!PLIB_I2C_TransmitterByteWasAcknowledged(APP_LCD_I2C_ID)) {
                        LEDR_Set;
                    } else {
                        if (PLIB_I2C_TransmitterIsReady(APP_LCD_I2C_ID)) {
                            if (appData.LCD_WriteIx > 0) {
                                appData.LCD_WriteIx--;
                                Xfer = appData.LCD_Write[appData.LCD_WriteIx];
                                PLIB_I2C_TransmitterByteSend(APP_LCD_I2C_ID, Xfer);
                                // keep appData.LCD_Transfer = I2C_MS_Transmit
                            } else {
                                // if (appData.LCD_WriteIx > 0) { PLIB_I2C_MasterStartRepeat(APP_LCD_I2C_ID); not used in this purpose
                                PLIB_I2C_MasterStop(APP_LCD_I2C_ID);
                                appData.I2C_Transfer = I2C_MS_Stop;
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
                    LEDY_Clear;
                    appData.I2C_State = I2C_MASTER_IDLE;
                    appData.I2C_Transfer = I2C_Idle;
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

bool APP_I2C_Ready(void) {
    return (appData.I2C_State == I2C_MASTER_IDLE);
}

// as described in system_config.h high nibble first and with LIFO ordering
void APP_LCD_AddCharWrite( char aChar) {
    uint8_t intChar = (uint8_t)aChar;
    APP_I2C_AddWrite( LCD_DATA | LCD_RW_WRITE | LCD_E_WRITE | ((intChar & 0x0f) << 4));
    APP_I2C_AddWrite( LCD_DATA | LCD_RW_WRITE | LCD_E_PREPARE | ((intChar & 0x0f) << 4));
    APP_I2C_AddWrite( LCD_DATA | LCD_RW_WRITE | LCD_E_WRITE | (intChar & 0xf0));
    APP_I2C_AddWrite( LCD_DATA | LCD_RW_WRITE | LCD_E_PREPARE | (intChar & 0xf0));
}

void APP_LCD_Update(void) {
    uint8_t c;
    for (c = LCD_LINEBUFFER_SIZE; c > 0; c--) {
        APP_LCD_AddCharWrite(appData.LCD_Line[3][c-1]);
    }
    for (c = LCD_LINEBUFFER_SIZE; c > 0; c--) {
        APP_LCD_AddCharWrite(appData.LCD_Line[1][c-1]);
    }
    APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_SET_HOME2_L);
    APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_SET_HOME2_L);
    APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_SET_HOME2_H);
    APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_SET_HOME2_H);
    for (c = LCD_LINEBUFFER_SIZE; c > 0; c--) {
        APP_LCD_AddCharWrite(appData.LCD_Line[2][c-1]);
    }
    for (c = LCD_LINEBUFFER_SIZE; c > 0; c--) {
        APP_LCD_AddCharWrite(appData.LCD_Line[0][c-1]);
    }
    APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_SET_HOME1_L);
    APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_SET_HOME1_L);
    APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_SET_HOME1_H);
    APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_SET_HOME1_H);
    APP_I2C_AddWrite( LCD_ADDRESS | I2C_WRITE);
    APP_I2C_M_Write();
}

void APP_LCD_ClearLine( uint8_t line) {
    uint8_t i;
    for (i = 0; i < LCD_LINEBUFFER_SIZE; i++) { appData.LCD_Line[line][i] = ' ';}
} 

void APP_LCD_Print(uint8_t line, uint8_t pos, char* string) {
    uint8_t i, len;
    len = strlen(string);
    if (len + pos > LCD_LINEBUFFER_SIZE) { len = LCD_LINEBUFFER_SIZE - pos; }
    for (i = 0; i < len; i++) {
        appData.LCD_Line[line][i + pos] = *(string+i);
    }
}

bool APP_LCD_Init(void) {
    if (appData.I2C_State == I2C_UNINITIALIZED) {
        appData.I2C_State = I2C_MASTER_IDLE;
        appData.time.Wait = 20; // Wait at least 20 ms after power on
        appData.LCD_Init = LCD_wait_on;
    }
    switch (appData.LCD_Init) {
        case LCD_wait_on:
            if (!appData.time.Wait) {
                // data & address always LIFO
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_8BIT_H);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_8BIT_H);
                APP_I2C_AddWrite( LCD_ADDRESS | I2C_WRITE);
                APP_I2C_M_Write();
                appData.LCD_Init = LCD_init_8bit;
            }
            break;
        case LCD_init_8bit:
            if (APP_I2C_Ready()) {
                appData.time.Wait = 4; // Wait at least 4 ms after 8bit switch
                appData.LCD_Init = LCD_wait_8bit;
            }
            break;
        case LCD_wait_8bit:
            if (!appData.time.Wait) {
                // data & address always LIFO
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_8BIT_H);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_8BIT_H);
                APP_I2C_AddWrite( LCD_ADDRESS | I2C_WRITE);
                APP_I2C_M_Write();
                appData.LCD_Init = LCD_switch_8bit;
            }
            break;
        case LCD_switch_8bit:
            if (APP_I2C_Ready()) {
                appData.time.Wait = 1; // Wait at least 1 ms after 8bit switch
                appData.LCD_Init = LCD_8bit_wait;
            }
            break;
        case LCD_8bit_wait:
            if (!appData.time.Wait) {
                // data & address always LIFO
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_4BIT_H);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_4BIT_H);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_8BIT_H);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_8BIT_H);
                APP_I2C_AddWrite( LCD_ADDRESS | I2C_WRITE);
                APP_I2C_M_Write();
                appData.LCD_Init = LCD_switch_4bit;
            }
            break;
        case LCD_switch_4bit:
            if (APP_I2C_Ready()) {
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_LINEFONT_L);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_LINEFONT_L);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_4BIT_H);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_4BIT_H);
                APP_I2C_AddWrite( LCD_ADDRESS | I2C_WRITE);
                APP_I2C_M_Write();
                appData.LCD_Init = LCD_linefont_4bit;
            }
            break;
        case LCD_linefont_4bit:
            if (APP_I2C_Ready()) {
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_DISPLAY_OFF_L);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_DISPLAY_OFF_L);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_DISPLAY_OFF_H);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_DISPLAY_OFF_H);
                APP_I2C_AddWrite( LCD_ADDRESS | I2C_WRITE);
                APP_I2C_M_Write();
                appData.LCD_Init = LCD_displayoff;
            }
            break;
        case LCD_displayoff:
            if (APP_I2C_Ready()) {
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_CLEAR_L);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_CLEAR_L);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_CLEAR_H);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_CLEAR_H);
                APP_I2C_AddWrite( LCD_ADDRESS | I2C_WRITE);
                APP_I2C_M_Write();
                appData.LCD_Init = LCD_displayclear;
            }
            break;
        case LCD_displayclear:
            if (APP_I2C_Ready()) {
                appData.time.Wait = 2; // Wait at least 2 ms after display clear
                appData.LCD_Init = LCD_waitclear;
            }
            break;
        case LCD_waitclear:
            if (!appData.time.Wait) {
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_CURSORSHIFT_L);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_CURSORSHIFT_L);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_CURSORSHIFT_H);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_CURSORSHIFT_H);
                APP_I2C_AddWrite( LCD_ADDRESS | I2C_WRITE);
                APP_I2C_M_Write();
                appData.LCD_Init = LCD_cursor_shift;
            }
            break;
        case LCD_cursor_shift:
            if (APP_I2C_Ready()) {
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_DISPLAY_ON_L);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_DISPLAY_ON_L);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_WRITE | LCD_DISPLAY_ON_H);
                APP_I2C_AddWrite( LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_DISPLAY_ON_H);
                APP_I2C_AddWrite( LCD_ADDRESS | I2C_WRITE);
                APP_I2C_M_Write();
                appData.LCD_Init = LCD_displayon;
            }
            break;
        case LCD_displayon:
            if (APP_I2C_Ready()) {
                appData.LCD_Init = LCD_ready;
            }
            break;
        case LCD_ready:
            return true;
            break;
    }
    return false;
}

bool APP_LCD_Ready(void) {
    return (appData.LCD_Init == LCD_ready);
}

#ifdef APP_USE_USART
// USART routines 
// INT Handling Read
void APP_USART_Read(void) {
    while (PLIB_USART_ReceiverDataIsAvailable(APP_USART_RX_ID)) {
        appData.USARTreadBuffer[appData.USARTreadIdx++] = PLIB_USART_ReceiverByteReceive(APP_USART_RX_ID);
    }
}

// INT Handling Write 
void APP_USART_Write(void) {
    int i;
    if (appData.USARTreadIdx > 0) {
        while (!PLIB_USART_TransmitterBufferIsFull(APP_USART_TX_ID) & (appData.USARTwriteIdx > 0)) {
            PLIB_USART_TransmitterByteSend(APP_USART_TX_ID, appData.USARTwriteBuffer[0]);
            appData.USARTwriteIdx--;
            for (i = 1; i <= appData.USARTwriteIdx; i++) { appData.USARTwriteBuffer[i - 1] = appData.USARTwriteBuffer[i]; }
        }
    }
}
#endif // of ifdef APP_USE_USART

//***************************************************
// This is the standard texts replied to host
//***************************************************
char nokPrompt[] = "NOK\n";
char okPrompt[] = "OK\n";
char str2int[12] = "\0";

void APP_Tasks ( void )
{
    int i;
    // check the application state
    switch ( appData.state ) {
        // Application's initial state
        case APP_STATE_INIT:
            // Turn Off LED
            LEDR_Clear;
            LEDY_Clear;
            LEDG_Set;
            appData.state = APP_STATE_LCD_INIT;
            break;
        case APP_STATE_LCD_INIT:
            APP_CheckTimedLED();
            if (APP_LCD_Init()) {
                LEDG_Clear;
                APP_LCD_Print( 3, 0, "LCD ready");
                // -> use of sprintf for string formatting
                // Time representation
                APP_LCD_Print( 0, 0, " 00:00:00 --- 0000");
                APP_LCD_Print( 1, 0, "POEnet");
                appData.state = APP_LCD_UPDATE;
#ifdef APP_USE_USB
                appData.LCD_Return_AppState = APP_STATE_USB_INIT;
#else                
                appData.LCD_Return_AppState = APP_STATE_HOLD;
#endif // of else APP_USE_USB
            }
            break;
#ifdef APP_USE_USB
        // USB Status:
        // U (LCD_Line[0][10]) = USB_DEVICE_Open successfull
        // S (LCD_Line[0][11]) = USB_DEVICE_CDC_EventHandlerSet
        // B (LCD_Line[0][12]) = Device attached (Vbus is powered)
        case APP_STATE_USB_INIT:
            // Signal init by setting error LED
            LEDR_Set;
            // Open the device layer 
            appData.deviceHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE );
            if (appData.deviceHandle != USB_DEVICE_HANDLE_INVALID) {
                // Register a callback with device layer to get event notification (for end point 0) 
                USB_DEVICE_EventHandlerSet(appData.deviceHandle, APP_USBDeviceEventHandler, 0);
                // Attach the device 
                USB_DEVICE_Attach (appData.deviceHandle);
                appData.state = APP_STATE_USB_CONFIGURATION;
                // Fillup Display with USB Status
                appData.LCD_Line[0][10] = 'U';
                appData.state = APP_LCD_UPDATE;
                appData.LCD_Return_AppState = APP_STATE_USB_CONFIGURATION;
            }
            break;
        case APP_STATE_USB_CONFIGURATION:
            // if the USB device is reset by the Host we return to this state (checked by APP_USBStateReset)
            // Check if the device was configured 
            if(appData.isConfigured) {
                // If the device is configured then lets start reading 
                appData.state = APP_STATE_SCHEDULE_READ;
                // Clear error LED
                LEDR_Clear;
                // Fillup Display with USB Status
                appData.LCD_Line[0][12] = 'B';
                APP_LCD_Print( 1, 7, "ready");
                appData.state = APP_LCD_UPDATE;
                appData.LCD_Return_AppState = APP_STATE_SCHEDULE_READ;
            }
            break;
        case APP_STATE_SCHEDULE_READ:
            if(APP_USBStateReset()) { break; }
            // Clear Read Buffer and then wait for data send by Host
            //memset(appData.readBuffer, 0, APP_READ_BUFFER_SIZE); -> doesn't work
            for (i = 0; i < USB_BUFFER_SIZE; i++) { appData.readBuffer[i] = '\0'; }
            LEDG_Set; // Set acceptance LED
            appData.state = APP_STATE_WAIT_FOR_READ_COMPLETE;
            break;
        case APP_STATE_WAIT_FOR_READ_COMPLETE:
            if(APP_USBStateReset()) { break; }
            if (appData.isReadComplete) {
                appData.isReadComplete = false;
                LEDG_Clear; // Clear acceptance LED
                appData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
                USB_DEVICE_CDC_Read (appData.cdcInstance, &appData.readTransferHandle,
                        appData.readBuffer, USB_BUFFER_SIZE);
                if (appData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID) {
                    appData.state = APP_STATE_ERROR;
                    break;
                }
                // Update LCD with count bytes read 
                i = strlen( appData.readBuffer);
                appData.LCD_Line[0][17] = numChar[i%10];
                i /= 10;
                appData.LCD_Line[0][16] = numChar[i%10];
                i /= 10;
                appData.LCD_Line[0][15] = numChar[i%10];
                i /= 10;
                appData.LCD_Line[0][14] = numChar[i];
                switch (appData.readBuffer[0]) {
                    case 'U':
                        //POE.net Message -> pass to interpreter
                        POEnet_Interpret(&appData.readBuffer[1]);
                        APP_LCD_Print( 1, 7, "              ");
                        appData.state = APP_STATE_POENET_COMMAND;
                        appData.POEnet_Return_AppState = APP_STATE_SCHEDULE_WRITE;
                        break;
                    case 'T':
                        switch (appData.readBuffer[1]) {
                            case '1':
                                APP_LCD_ClearLine(1);
                                APP_LCD_Print(1, 0, &appData.readBuffer[2]);
                                break;
                            case '2':
                                APP_LCD_ClearLine(2);
                                APP_LCD_Print(2, 0, &appData.readBuffer[2]);
                                break;
                            case '3':
                                APP_LCD_ClearLine(3);
                                APP_LCD_Print(3, 0, &appData.readBuffer[2]);
                                break;
                            default:
                                strcpy(appData.writeBuffer, "NOK\n");
                                appData.writeCount = strlen(appData.writeBuffer);
                                appData.state = APP_STATE_SCHEDULE_WRITE;
                                break;
                        }
                        // set a text message
                        break;
                    default:
                        // ignore Message
                        strcpy(appData.writeBuffer, "see http://wiki69.pst69.homeip.net/index.php/PIC32_USART for POE.net messages\n");
                        appData.writeCount = strlen(appData.writeBuffer);
                        appData.state = APP_STATE_SCHEDULE_WRITE;
                        break;
                }
                ClearBuffer(appData.readBuffer);
            }
            break;
        case APP_STATE_SCHEDULE_WRITE:
            if(APP_USBStateReset()) { break; }
            // Setup the write 
            appData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
            appData.isWriteComplete = false;
            if (!appData.writeCount) {
                USB_DEVICE_CDC_Write(appData.cdcInstance, &appData.writeTransferHandle,
                        nokPrompt, 4, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
            } else {
                USB_DEVICE_CDC_Write(appData.cdcInstance, &appData.writeTransferHandle,
                        appData.writeBuffer, appData.writeCount, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
            }
            appData.state = APP_STATE_WAIT_FOR_WRITE_COMPLETE;
            break;
        case APP_STATE_WAIT_FOR_WRITE_COMPLETE:
            if(APP_USBStateReset()) { break; }
            // Check if a character was sent. The isWriteComplete
            // flag gets updated in the CDC event handler
            if (appData.isWriteComplete) {
                ClearBuffer(appData.writeBuffer);
                appData.writeCount = 0;
                appData.state = APP_STATE_SCHEDULE_READ;
            }
            break;
#endif // of ifdef APP_USE_USB
        case APP_STATE_POENET_COMMAND:
            POEnet_GetCommand(&appData.POEnetCommand[0]);
            APP_LCD_Print( 1, 8, &appData.POEnetCommand[0]);
            if (!strcmp(&appData.POEnetCommand[0],"net")) {
                // handle net command
                POEnet_GetNewNodeId(&appData.POEnet_NodeId);
            }
            appData.writeBuffer[0] = 'U';
            POEnet_Output(&appData.writeBuffer[1]);
            appData.writeCount = strlen(appData.writeBuffer);
            appData.LCD_Return_AppState = appData.POEnet_Return_AppState;
            appData.state = APP_LCD_UPDATE;
            break;
        case APP_LCD_UPDATE:
            APP_CheckTimedLED();
            if (APP_I2C_Ready()) {
                APP_LCD_Update();
                appData.state = appData.LCD_Return_AppState;
            }
            break;
        case APP_STATE_ERROR:
            APP_CheckTimedLED();
            if (APP_RegisterTimedLED( 200)) {
                LEDR_Clear;
                appData.state = APP_STATE_HOLD;
            }
            break;
        case APP_STATE_HOLD:
            APP_CheckTimedLED();
#ifdef APP_USE_USB
            if(APP_USBStateReset()) { break; }
#endif // of ifdef APP_USE_USB
            break;
        // The default state should never be executed. 
        default:
            break;
    }
}