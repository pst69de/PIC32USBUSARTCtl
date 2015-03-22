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

#ifdef APP_LCD_I2C_ID
#include "POEi2clcd.h"
#endif // ifdef APP_LCD_I2C_ID
#ifdef APP_USE_USB
#include "POEusb.h"
#endif


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
    appData.pollSecond = false;
    appData.time.Wait = 0;
    // timing repetition 
    appData.timerCount = 0;
    appData.timerRepeat = 0;
    appData.timerExpired = false;
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
    ClearBuffer(&appData.USB_INPUT_BUF[0]);
    appData.USB_INPUT_SIZE = 0;
    appData.USB_INPUT_IDX = 0;
    ClearBuffer(&appData.USB_OUTPUT_BUF[0]);
    appData.USB_OUTPUT_SIZE = 0;
    appData.USB_OUTPUT_IDX = 0;
#endif // of ifdef APP_USE_USB
#ifdef APP_USE_UART
    // UART init
    ClearBuffer(&appData.UART_INPUT_BUF[0]);
    //for (i = 0; i < APP_UART_RX_BUFFER_SIZE; i++) { appData.UARTreadBuffer[i] = 0;};
    appData.UART_INPUT_SIZE = 0;
    appData.UART_INPUT_IDX = 0;
    ClearBuffer(&appData.UART_OUTPUT_BUF[0]);
    //for (i = 0; i < APP_UART_TX_BUFFER_SIZE; i++) { appData.UARTwriteBuffer[i] = 0;};
    appData.UART_OUTPUT_SIZE = 0;
    appData.UART_OUTPUT_IDX = 0;
#endif // of ifdef APP_USE_UART
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
                APP_LCD_PrintChar(0,2,numChar[appData.time.Hours % 10]);
                APP_LCD_PrintChar(0,1,numChar[appData.time.Hours / 10]);
            }
            // Set Minutes to Time field
            APP_LCD_PrintChar(0,5,numChar[appData.time.Minutes % 10]);
            APP_LCD_PrintChar(0,4,numChar[appData.time.Minutes / 10]);
        }
        // Set Seconds to Time field
        APP_LCD_PrintChar(0,8,numChar[appData.time.Seconds % 10]);
        APP_LCD_PrintChar(0,7,numChar[appData.time.Seconds / 10]);
        appData.pollSecond = true;
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
bool APP_CheckTimer (void) {
    bool Result = false;
    if (appData.timerExpired) {
        // Toggle LED & clear flag
        LEDR_Toggle;
        appData.timerExpired = false;
    }
    if (appData.pollSecond) {
        appData.pollSecond = false;
        if (APP_LCD_Ready) {
            if (appData.state != APP_LCD_UPDATE) {
                appData.LCD_Return_AppState = appData.state;
                appData.state = APP_LCD_UPDATE;
                Result = true;
            }
        }
    }
    return Result;
}

// USB Routines
//****************************************************
// This function is called in every step of the
// application state machine.
// even without USB, but then it does nothing
//****************************************************
bool APP_USBStateReset(void) {
    bool retVal;
    retVal = false;
#ifdef APP_USE_USB
    // This function returns true if the device was reset
    if(!appData.isConfigured) {
        // Clear Display from USB Status
        APP_LCD_PrintChar(0,11,'-');
        APP_LCD_PrintChar(0,12,'-');
        appData.state = APP_LCD_UPDATE;
        appData.LCD_Return_AppState = APP_STATE_USB_CONFIGURATION;
        //appData.state = APP_STATE_USB_CONFIGURATION;
        appData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        appData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        appData.isReadComplete = true;
        appData.isWriteComplete = true;
        retVal = true;
    }
#endif // ifdef APP_USE_USB
    return(retVal);
}

#ifdef APP_USE_USB
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
                APP_LCD_PrintChar(0,11,'S');
            }
            break;
        case USB_DEVICE_EVENT_POWER_DETECTED:
            // VBUS was detected. We can attach the device 
            USB_DEVICE_Attach(appData.deviceHandle);
            APP_LCD_PrintChar(0,12,'B');
            break;
        case USB_DEVICE_EVENT_POWER_REMOVED:
            // VBUS is not available any more. Detach the device. 
            USB_DEVICE_Detach(appData.deviceHandle);
            APP_LCD_PrintChar(0,12,'-');
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

// LCD I2C Routines see POEi2clcd.h / POEi2clcd.c

#ifdef APP_USE_UART
// UART routines 
// INT Handling Read
void APP_UART_Read(void) {
    APP_LCD_PrintChar(0,18,'*');
    while (PLIB_USART_ReceiverDataIsAvailable(APP_UART_RX_ID)) {
        uint8_t readByte = PLIB_USART_ReceiverByteReceive(APP_UART_RX_ID);
        char dispChar = (char)readByte;
        if (!readByte) { dispChar = (char)0xdb; }
        if (appData.UART_INPUT_IDX < 40) {
            if (appData.UART_INPUT_IDX < 20) {
                APP_LCD_PrintChar(2,appData.UART_INPUT_IDX,dispChar);
            } else {
                APP_LCD_PrintChar(3,appData.UART_INPUT_IDX - 20,dispChar);
            }
        }
        appData.UART_INPUT_BUF[appData.UART_INPUT_IDX++] = readByte;
        // POE.net: zero is sent for termination
        if (!readByte) {
            appData.UART_INPUT_SIZE = appData.UART_INPUT_IDX;
            APP_LCD_PrintChar(0,18,'R');
        }
    }
}

// INT Handling Write 
void APP_UART_Write(void) {
    if (appData.UART_OUTPUT_IDX == appData.UART_OUTPUT_SIZE) {
        appData.UART_OUTPUT_SIZE = 0;
        appData.UART_OUTPUT_IDX = 0;
        APP_LCD_PrintChar(0,19,'T');
        PLIB_USART_TransmitterDisable(APP_UART_TX_ID);
    }
    if (appData.UART_OUTPUT_SIZE > 0) {
        APP_LCD_PrintChar(0,19,'*');
        if (!PLIB_USART_TransmitterBufferIsFull(APP_UART_TX_ID) & (appData.UART_OUTPUT_IDX < appData.UART_OUTPUT_SIZE)) {
            PLIB_USART_TransmitterByteSend(APP_UART_TX_ID, appData.UART_OUTPUT_BUF[ appData.UART_OUTPUT_IDX++]);
        }
    }
}
#endif // of ifdef APP_USE_UART

//***************************************************
// This is the standard texts replied to host
//***************************************************
char nokPrompt[] = "NOK\n";
char okPrompt[] = "OK\n";
char str2int[12] = "\0";

void APP_Tasks ( void )
{
    int i;
#ifdef APP_USE_USB
    // separate regular calls (at time USB Handling)
    USB_SYS_Loop();
#endif
    // check the application state
    switch ( appData.state ) {
        // Application's initial state
        case APP_STATE_INIT:
            // Turn Off LED
            LEDR_Clear;
            LEDY_Clear;
            //LEDG_Set;
            LEDG_Clear;
            appData.state = APP_STATE_LCD_INIT;
            break;
        case APP_STATE_LCD_INIT:
            if (APP_LCD_Init(appData.time.milliSeconds)) {
                //LEDG_Clear;
                // -> use of sprintf for string formatting
                // Time and Status representation
                // USB Status:
                // U (LCD_Line[0][10])      = USB_DEVICE_Open successfull
                // S (LCD_Line[0][11])      = USB_DEVICE_CDC_EventHandlerSet
                // B (LCD_Line[0][12])      = Device attached (Vbus is powered)
                // # (LCD_Line[0][13])      = > = Received; < = Sending
                // 000 (LCD_Line[0][14-16]) = count Bytes 
                // RT (LCD_Line[0][18-19])  = R = UART Receiver (* if receiving); T = UART Transmitter (* if transmitting) 
                // POE.net status representation
                APP_LCD_Print( 0, 0, " 00:00:00 ---#000 RT");
                APP_LCD_Print( 1, 0, "POEnet _______ 00000");
#ifdef APP_USE_UART
                APP_LCD_Print( 2, 0, "UART");
#endif // ifdef APP_USE_UART
                APP_LCD_Print( 3, 0, "LCD");
#ifdef APP_USE_UART
                // init UART before USB
                appData.LCD_Return_AppState = APP_STATE_UART_INIT;
#else
                appData.LCD_Return_AppState = APP_STATE_HOLD;
#endif // ifdef APP_USE_UART
#ifdef APP_USE_USB
                // use of USB overrides Init of UART
                appData.LCD_Return_AppState = APP_STATE_USB_INIT;
#endif // of else APP_USE_USB
                appData.state = APP_LCD_UPDATE;
            }
            break;
#ifdef APP_USE_USB
        case APP_STATE_USB_INIT:
            if (APP_CheckTimer()) { break; }
            // Signal init by setting error LED
            //LEDR_Set;
            // Open the device layer 
            appData.deviceHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE );
            if (appData.deviceHandle != USB_DEVICE_HANDLE_INVALID) {
                // Register a callback with device layer to get event notification (for end point 0) 
                USB_DEVICE_EventHandlerSet(appData.deviceHandle, APP_USBDeviceEventHandler, 0);
                // Attach the device 
                USB_DEVICE_Attach (appData.deviceHandle);
                appData.state = APP_STATE_USB_CONFIGURATION;
                // Fillup Display with USB Status
                APP_LCD_PrintChar(0,10,'U');
                appData.state = APP_LCD_UPDATE;
                appData.LCD_Return_AppState = APP_STATE_USB_CONFIGURATION;
            }
            break;
        case APP_STATE_USB_CONFIGURATION:
            if (APP_CheckTimer()) { break; }
            // if the USB device is reset by the Host we return to this state (checked by APP_USBStateReset)
            // Check if the device was configured 
            if(appData.isConfigured) {
                // Clear error LED
                //LEDR_Clear;
                // Fillup Display with USB Status
                APP_LCD_PrintChar(0,12,'B');
                APP_LCD_Print(1,7, "ready");
                ClearBuffer(&appData.USB_INPUT_BUF[0]);
                // If the device is configured then lets start reading
                //LEDG_Set; // Set acceptance LED
#ifdef APP_USE_UART
                // if there is the UART run their init
                appData.LCD_Return_AppState = APP_STATE_UART_INIT;
#else // ifdef APP_USE_UART
                // or start waiting on input
                appData.LCD_Return_AppState = APP_STATE_POENET_INPUT;
#endif // else APP_USE_UART
                appData.state = APP_LCD_UPDATE;
            }
            break;
        case APP_STATE_USB_WRITE:
            if (APP_USBStateReset()) { break; }
            if (APP_CheckTimer()) { break; }
            // Setup the write 
            appData.writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
            appData.isWriteComplete = false;
            // Update LCD with count bytes to write
            APP_LCD_PrintChar(0,13,'<');
            i = appData.USB_OUTPUT_SIZE;
            APP_LCD_PrintChar(0,16,numChar[i%10]);
            i /= 10;
            APP_LCD_PrintChar(0,15,numChar[i%10]);
            i /= 10;
            APP_LCD_PrintChar(0,14,numChar[i%10]);
            i /= 10;
            if (i) { APP_LCD_PrintChar(0,14,'A');}
            if (!appData.USB_OUTPUT_SIZE) {
                USB_DEVICE_CDC_Write(appData.cdcInstance, &appData.writeTransferHandle,
                        nokPrompt, 4, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
            } else {
                USB_DEVICE_CDC_Write(appData.cdcInstance, &appData.writeTransferHandle,
                        appData.USB_OUTPUT_BUF, appData.USB_OUTPUT_SIZE - 1, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
            }
            appData.state = APP_STATE_USB_WAIT_WRITE_COMPLETE;
            break;
        case APP_STATE_USB_WAIT_WRITE_COMPLETE:
            if (APP_USBStateReset()) { break; }
            if (APP_CheckTimer()) { break; }
            // Check if a character was sent. The isWriteComplete
            // flag gets updated in the CDC event handler
            if (appData.isWriteComplete) {
                LEDY_Set;
                ClearBuffer(appData.USB_OUTPUT_BUF);
                appData.USB_OUTPUT_IDX = 0;
                appData.USB_OUTPUT_SIZE = 0;
                appData.state = APP_STATE_POENET_INPUT;
            }
            break;
#endif // of ifdef APP_USE_USB
#ifdef APP_USE_UART
        // Init UART
        case APP_STATE_UART_INIT:
            if (APP_CheckTimer()) { break; }
            // if UART Transmitter is enabled, it automatically asks 
            // for the 1st byte via Interrupt, so wait for the
            // transmission to be complete (in APP_STATE_POENET_OUTPUT_PREPARE)
            //PLIB_UART_TransmitterEnable(APP_UART_TX_ID);
            PLIB_USART_ReceiverEnable(APP_UART_RX_ID);
            // goto Input afterwards
            appData.LCD_Return_AppState = APP_STATE_POENET_INPUT;
            appData.state = APP_LCD_UPDATE;
            break;
#endif // of ifdef APP_USE_UART
        // POE.net reset message
        case APP_STATE_POENET_RESET:
#ifdef APP_POEnet_SECONDARY
            strcpy(&appData.POEnetSecOutputBuf[0], "U<reset/>\0");
            appData.POEnetSecOutputSize = strlen(&appData.POEnetSecOutputBuf[0]) + 1;
            appData.POEnetSecOutputIdx = 0;
#else // ifdef APP_POEnet_SECONDARY
            strcpy(&appData.POEnetPrimOutputBuf[0], "U<reset/>\0");
            appData.POEnetPrimOutputSize = strlen((&appData.POEnetPrimOutputBuf[0]) + 1;
            appData.POEnetPrimOutputIdx = 0;            
#endif // else APP_POEnet_SECONDARY
            APP_LCD_Print( 1, 8, &POEnet_reset[0]);
            appData.LCD_Return_AppState = APP_STATE_POENET_OUTPUT_PREPARE;
            appData.state = APP_LCD_UPDATE;
            break;
        // POE.net input phase
        case APP_STATE_POENET_INPUT:
            if (APP_USBStateReset()) { break; }
            if (APP_CheckTimer()) { break; }
#ifdef APP_USE_USB
            // USB is always Primary, so pass Input to command interpretation
            if (appData.isReadComplete) {
                appData.isReadComplete = false;
                LEDY_Clear;
                //LEDG_Clear; // Clear acceptance LED
                appData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
                USB_DEVICE_CDC_Read (appData.cdcInstance, &appData.readTransferHandle,
                        appData.USB_INPUT_BUF, USB_BUFFER_SIZE);
                if (appData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID) {
                    appData.state = APP_STATE_ERROR;
                    break;
                }
                // USB cuts off terminating zero's (POE.net)
                appData.USB_INPUT_SIZE = strlen( appData.USB_INPUT_BUF) + 1;
                appData.USB_INPUT_IDX = 0;
                // Update LCD with count bytes read
                APP_LCD_PrintChar(0,13,'<');
                i = appData.USB_OUTPUT_SIZE;
                APP_LCD_PrintChar(0,16,numChar[i%10]);
                i /= 10;
                APP_LCD_PrintChar(0,15,numChar[i%10]);
                i /= 10;
                APP_LCD_PrintChar(0,14,numChar[i%10]);
                i /= 10;
                if (i) { APP_LCD_PrintChar(0,14,'A');}
                // pass via LCDout to INPUTReady
                appData.state = APP_LCD_UPDATE;
                appData.LCD_Return_AppState = APP_STATE_POENET_INPUT_READY;
                break;
            }
#endif // of ifdef APP_USE_USB
#ifdef APP_USE_UART
            // UART may be Secondary, so pass Input as compiled
            // only if there is no USB processing needed
            // *****************************************************************
            // maybe there is the need of a more advanced state switching 
            // as USB inputs override backtransmissions from serial (to be send by USB)
            // *****************************************************************
            if ((appData.state == APP_STATE_POENET_INPUT) & (appData.UART_INPUT_SIZE)) {
#ifdef APP_POEnet_SECONDARY
                // pass to Buffer pass
                appData.state = APP_STATE_POENET_PASS;
#else
                // pass to INPUTReady
                appData.state = APP_STATE_POENET_INPUT_READY;
#endif
                break;
            }
#endif // of ifdef APP_USE_USB
            break;
        // POE.net input buffer filled
        case APP_STATE_POENET_INPUT_READY:
            if (APP_USBStateReset()) { break; }
            if (APP_CheckTimer()) { break; }
            switch (appData.POEnetPrimInputBuf[0]) {
                case 'U':
                    //POE.net Message -> pass to interpreter
                    POEnet_Interpret(&appData.POEnetPrimInputBuf[1]);
                    APP_LCD_Print( 1, 7, "       ");
                    appData.LCD_Return_AppState = APP_STATE_POENET_COMMAND;
                    appData.state = APP_LCD_UPDATE;
                    break;
                case 'T':
                    //switch (appData.POEnetPrimInputBuf[1]) {
                    //    case '1':
                    //        APP_LCD_ClearLine(1);
                    //        APP_LCD_Print(1, 0, &appData.POEnetPrimInputBuf[2]);
                    //        break;
                    //    case '2':
                    //        APP_LCD_ClearLine(2);
                    //        APP_LCD_Print(2, 0, &appData.POEnetPrimInputBuf[2]);
                    //        break;
                    //    case '3':
                    //        APP_LCD_ClearLine(3);
                    //        APP_LCD_Print(3, 0, &appData.POEnetPrimInputBuf[2]);
                    //        break;
                    //    default:
                    //        break;
                    //}
                    APP_LCD_ClearLine(3);
                    APP_LCD_Print(3, 0, &appData.POEnetPrimInputBuf[1]);
                    ClearBuffer(&appData.POEnetPrimInputBuf[0]);
                    appData.POEnetPrimInputSize = 0;
                    appData.POEnetPrimInputIdx = 0;
                    // set the text message
                    appData.LCD_Return_AppState = APP_STATE_POENET_INPUT;
                    appData.state = APP_LCD_UPDATE;
                    break;
                case 'B':
                    APP_LCD_Backlight = !APP_LCD_Backlight;
                    appData.LCD_Return_AppState = APP_STATE_POENET_INPUT;
                    appData.state = APP_LCD_UPDATE;
                    break;
                default:
                    // ignore Message
                    ClearBuffer(&appData.POEnetPrimInputBuf[0]);
                    appData.POEnetPrimInputSize = 0;
                    appData.POEnetPrimInputIdx = 0;
                    appData.state = APP_STATE_POENET_INPUT;
#ifdef APP_USE_USB
                    // if input comes by USB send hint on help
                    strcpy(appData.USB_OUTPUT_BUF, "see http://wiki69.pst69.homeip.net/index.php/PIC32_UART for POE.net messages\n");
                    appData.USB_OUTPUT_SIZE = strlen(appData.USB_OUTPUT_BUF) + 1;
                    appData.state = APP_STATE_USB_WRITE;
#endif // ifdef APP_USE_USB
                    break;
            }
            break;
        // POE.net command interpretation
        case APP_STATE_POENET_COMMAND:
            if (APP_USBStateReset()) { break; }
            if (APP_CheckTimer()) { break; }
            POEnet_GetCommand(&appData.POEnetCommand[0]);
            APP_LCD_Print( 1, 8, &appData.POEnetCommand[0]);
            if (!strcmp(&appData.POEnetCommand[0],&POEnet_net[0])) {
                // handle net command
                POEnet_GetNewNodeId(&appData.POEnet_NodeId);
            }
            appData.POEnetPrimOutputBuf[0] = 'U';
            POEnet_Output(&appData.POEnetPrimOutputBuf[1]);
            appData.POEnetPrimOutputSize = strlen(appData.POEnetPrimOutputBuf) + 1;
            appData.POEnetPrimOutputIdx = 0;
            appData.LCD_Return_AppState = APP_STATE_POENET_OUTPUT_PREPARE;
            appData.state = APP_LCD_UPDATE;
            break;
        // POE.net output phase
        case APP_STATE_POENET_OUTPUT_PREPARE:
            if (APP_USBStateReset()) { break; }
            if (APP_CheckTimer()) { break; }
#ifdef APP_POEnet_SECONDARY
            // USB UART Bridge -> pass to UART
            APP_LCD_PrintChar(2,6,'>');
            // initiate UART transmission by writing 1st Byte
            //APP_UART_Write();
            // if UART Transmitter is enabled, it automatically asks 
            // for the 1st byte via Interrupt
            PLIB_USART_TransmitterEnable(APP_UART_TX_ID);
            appData.state = APP_STATE_POENET_OUTPUT_READY;
#else // ifdef APP_POEnet_SECONDARY
#ifdef APP_USE_USB
            // no secondary with USB -> single USB test node, pass Output to USB
            appData.state = APP_STATE_USB_WRITE;
            // APP_STATE_USB_WRITE passes back to APP_STATE_POENET_INPUT on completion
#else // ifdef APP_USE_USB
            // no secondary -> pass Output to UART TX 
            // initiate UART transmission by writing 1st Byte
            //APP_UART_Write();
            PLIB_UART_TransmitterEnable(APP_UART_TX_ID);
            appData.state = APP_STATE_POENET_OUTPUT_READY;
#endif // else APP_USE_USB
#endif // else APP_POEnet_SECONDARY
            break;
        // POE.net output phase finished
        case APP_STATE_POENET_OUTPUT_READY:
            if (APP_USBStateReset()) { break; }
            if (APP_CheckTimer()) { break; }
            // primary Out is always UART transmission -> check for fullfillment
            if (appData.POEnetPrimOutputIdx == appData.POEnetPrimOutputSize) {
                ClearBuffer(&appData.POEnetPrimOutputBuf[0]);
                appData.POEnetPrimOutputSize = 0;
                appData.POEnetPrimOutputIdx = 0;
                appData.state = APP_STATE_POENET_INPUT;
            }
            break;
#ifdef APP_POEnet_SECONDARY
        case APP_STATE_POENET_PASS:
            if (APP_USBStateReset()) { break; }
            if (APP_CheckTimer()) { break; }
            strcpy( &appData.POEnetSecOutputBuf[0], &appData.POEnetSecInputBuf[0]);
            appData.POEnetSecOutputSize = appData.POEnetSecInputSize;
            // POE.net messages usually terminate with zero \0
            // put a LineFeed at the end of USB transmission instead
            appData.POEnetSecOutputBuf[appData.POEnetSecOutputSize - 1] = '\n';
            appData.POEnetSecOutputIdx = 0;
            ClearBuffer(&appData.POEnetSecInputBuf[0]);
            appData.POEnetSecInputSize = 0;
            appData.POEnetSecInputIdx = 0;
            // if it is Secondary the output always goes to the USB
            appData.state = APP_STATE_USB_WRITE;
            break;
#endif // ifdef APP_POEnet_SECONDARY
        case APP_LCD_UPDATE:
            if (APP_CheckTimer()) { break; }
            // don't check USB on LCD cycles -> possible conflict with USB not yet initialized
            //if(APP_USBStateReset()) { break; }
            if (APP_LCD_I2C_Ready) {
                APP_LCD_Update;
                appData.state = appData.LCD_Return_AppState;
            }
            break;
        case APP_STATE_ERROR:
            if (APP_CheckTimer()) { break; }
            if (APP_RegisterTimedLED( 200)) {
                LEDR_Clear;
                appData.state = APP_STATE_HOLD;
            }
            break;
        case APP_STATE_HOLD:
            if (APP_USBStateReset()) { break; }
            if (APP_CheckTimer()) { break; }
            break;
        // The default state should never be executed. 
        default:
            break;
    }
}