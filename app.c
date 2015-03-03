/*
 * File:   app.c
 * Author: patrick
 *
 * Created on 2014-09-13
 */

#include "app.h"


APP_DATA appData;
/* Global allocation of structure to hold app timing */
APP_TIMING appTiming;


void APP_Initialize ( void )
{
	uint8_t i;
    /* Initialize App Timing to zero */
    appData.time.milliSeconds = 0;
    appData.time.Seconds = 0;
    appData.time.Minutes = 0;
    appData.time.Hours = 0;
    appData.time.Days = 0;
    appData.lastSecond = 59;
    appData.time.Wait = 0;
    /* timing repetition */
    appData.timerCount = 0;
    appData.timerRepeat = 0;
    appData.timerExpired = false;
    /* LCD I2C data */
    appData.LCD_State = I2C_UNINITIALIZED;
    appData.LCD_Transfer = I2C_Idle;
    for (i = 0; i < APP_LCD_BUFFER_SIZE; i++) { appData.LCD_Write[i] = 0;};
    appData.LCD_WriteIx = 0;
    for (i = 0; i < APP_LCD_BUFFER_SIZE; i++) { appData.LCD_Read[i] = 0;};
    appData.LCD_ReadIx = 0;

    /* USB Inits */
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
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
}

/* APP Time Base */
void APP_TimingCallback ( void ) {
    /* Adjust App Timing */
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
            }
        }
    }
    if (appData.timerCount > 0) {
        if (!(--appData.timerCount)) {
            appData.timerExpired = true;
            appData.timerCount = appData.timerRepeat;
        }
    }
    if (appData.time.Wait) {appData.time.Wait--;}
}
/* APP Timed LED callback registration routine */
/* boolean return for future extension e.g. with SYS_TMR-Registration */
bool APP_RegisterTimedLED( int thisTimeMs) {
    appData.timerRepeat = thisTimeMs;
    appData.timerCount = appData.timerRepeat;
    return true;
}

/* APP Timed LED routine (blink on different operation states) */
void APP_CheckTimedLED(void) {
    if (appData.timerExpired) {
        /* Toggle LED & clear flag */
        PLIB_PORTS_PinToggle(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
        appData.timerExpired = false;
    }
}

/* USB Routines */
/*****************************************************
 * This function is called in every step of the
 * application state machine.
 *****************************************************/
bool APP_StateReset(void) {
    /* This function returns true if the device
     * was reset  */
    bool retVal;
    if(appData.isConfigured == false) {
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

/*******************************************************
 * USB Device Layer Events - Application Event Handler
 *******************************************************/
void APP_USBDeviceEventHandler ( USB_DEVICE_EVENT event,
        void * eventData, uintptr_t context ) {
    uint8_t * configuredEventData;
    switch ( event ) {
        case USB_DEVICE_EVENT_RESET:
            /* Update LED to show reset state */
            PLIB_PORTS_PinSet(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
            PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDG_PIN);
            appData.isConfigured = false;
            break;
        case USB_DEVICE_EVENT_CONFIGURED:
            /* Check the configuratio. We only support configuration 1 */
            configuredEventData = (uint8_t *)eventData;
            if ( *configuredEventData == 1) {
                /* Update LED to show configured state */
                PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
                PLIB_PORTS_PinSet(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDG_PIN);
                /* Register the CDC Device application event handler here.
                 * Note how the appData object pointer is passed as the
                 * user data */
                USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0,
                        APP_USBDeviceCDCEventHandler, (uintptr_t)&appData);
                /* Mark that the device is now configured */
                appData.isConfigured = true;
            }
            break;
        case USB_DEVICE_EVENT_POWER_DETECTED:
            /* VBUS was detected. We can attach the device */
            USB_DEVICE_Attach(appData.deviceHandle);
            PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
            PLIB_PORTS_PinSet(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDG_PIN);
            break;
        case USB_DEVICE_EVENT_POWER_REMOVED:
            /* VBUS is not available any more. Detach the device. */
            USB_DEVICE_Detach(appData.deviceHandle);
            PLIB_PORTS_PinSet(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
            PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDG_PIN);
            break;
        case USB_DEVICE_EVENT_SUSPENDED:
            /* Switch LED to show suspended state */
            PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
            PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDG_PIN);
            break;
        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_ERROR:
        default:
            break;
    }
}

/*******************************************************
 * USB CDC Device Events - Application Event Handler
 *******************************************************/

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
            /* This means the host wants to know the current line
             * coding. This is a control transfer request. Use the
             * USB_DEVICE_ControlSend() function to send the data to
             * host.  */
            USB_DEVICE_ControlSend(appDataObject->deviceHandle, &appDataObject->getLineCodingData,
                    sizeof(USB_CDC_LINE_CODING));
            break;
        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:
            /* This means the host wants to set the line coding.
             * This is a control transfer request. Use the
             * USB_DEVICE_ControlReceive() function to receive the
             * data from the host */
            USB_DEVICE_ControlReceive(appDataObject->deviceHandle, &appDataObject->setLineCodingData,
                    sizeof(USB_CDC_LINE_CODING));
            break;
        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:
            /* This means the host is setting the control line state.
             * Read the control line state. We will accept this request
             * for now. */
            controlLineStateData = (USB_CDC_CONTROL_LINE_STATE *)pData;
            appDataObject->controlLineStateData.dtr = controlLineStateData->dtr;
            appDataObject->controlLineStateData.carrier =
                controlLineStateData->carrier;
            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_SEND_BREAK:
            /* This means that the host is requesting that a break of the
             * specified duration be sent. Read the break duration */
            breakData = (uint16_t *)pData;
            appDataObject->breakData = *breakData;
            break;
        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:
            /* This means that the host has sent some data*/
            appDataObject->isReadComplete = true;
            break;
        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:
            /* The data stage of the last control transfer is
             * complete. For now we accept all the data */
            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:
            /* This means the GET LINE CODING function data is valid. We dont
             * do much with this data in this demo. */
            break;
        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:
            /* This means that the data write got completed. We can schedule
             * the next read. */
            appDataObject->isWriteComplete = true;
            break;
        default:
            break;
    }
    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}

/* LCD I2C Routines */
bool APP_LCD_Init(void) {
    if (appData.LCD_State == I2C_UNINITIALIZED) {
        appData.LCD_State = I2C_MASTER_IDLE;
        appData.time.Wait = 20; // Wait at least 20 ms after power on
        appData.LCD_Init = LCD_wait_on;
    }
    switch (appData.LCD_Init) {
        case LCD_wait_on:
            if (!appData.time.Wait) {
                appData.LCD_Init = LCD_switch_4bit;
                appData.LCD_WriteIx = 2; // address & data
                appData.LCD_Write[2] = LCD_ADDRESS | I2C_WRITE;
                appData.LCD_Write[1] = LCD_COMMAND | LCD_RW_WRITE | LCD_E_PREPARE | LCD_4BIT_H;
                appData.LCD_Write[0] = LCD_COMMAND | LCD_E_WRITE | LCD_E_PREPARE | LCD_4BIT_H;
                APP_LCD_M_Write();
            }
            break;
        case LCD_switch_4bit:
            if (APP_LCD_Ready()) {
                appData.time.Wait = 4; // Wait at least 4 ms after 4bit switch
                appData.LCD_Init = LCD_4bit_wait;
            }
            break;
    }
    return false;
}

void APP_LCD_M_Write(void) {
    appData.LCD_State = I2C_MASTER_WRITE;
    appData.LCD_Transfer = I2C_MS_Start;
    PLIB_I2C_MasterStart(APP_LCD_I2C_ID);
}

void APP_LCD_Process(void) {
    switch ( appData.LCD_State) {
	case I2C_UNINITIALIZED:
    	case I2C_MASTER_IDLE:
            break;
    	case I2C_MASTER_WRITE:
            switch (appData.LCD_Transfer) {
                case I2C_MS_Start:
                    if (PLIB_I2C_TransmitterIsReady(APP_LCD_I2C_ID)) {
                        PLIB_I2C_TransmitterByteSend(APP_LCD_I2C_ID, appData.LCD_Write[appData.LCD_WriteIx--]);
                        appData.LCD_Transfer = I2C_MS_Address;
                    }
                    break;
            }
            break;
    	case I2C_MASTER_WRITE_READ:
    	case I2C_MASTER_READ:
    	case I2C_SLAVE_IDLE:
            /* data coming in -> switch to appropriate mode */
            break;
    	case I2C_SLAVE_READ:
    	case I2C_SLAVE_READ_WRITE:
    	case I2C_SLAVE_WRITE:
            break;
        /* The default state should never be executed. */
        default:
            break;
	}
}

bool APP_LCD_Ready(void) {
    return (appData.LCD_State == I2C_MASTER_IDLE);
}

/****************************************************
 * This is the standard texts replied to host
 ****************************************************/
char nokPrompt[] = "NOK\n";
char okPrompt[] = "OK\n";
char str2int[12] = "\0";

void APP_Tasks ( void )
{
    int i;
    /* check the application state*/
    switch ( appData.state ) {
        /* Application's initial state*/
        case APP_STATE_INIT:
            /* Turn Off LED */
            PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
            PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDG_PIN);
            appData.state = APP_STATE_LCD_INIT;
            break;
        case APP_STATE_LCD_INIT:
            if (APP_LCD_Init()) {
                appData.state = APP_STATE_USB_INIT;
            }
            break;
        case APP_STATE_USB_INIT:
            /* Open the device layer */
            appData.deviceHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE );
            if (appData.deviceHandle != USB_DEVICE_HANDLE_INVALID) {
                /* Register a callback with device layer to get event notification (for end point 0) */
                USB_DEVICE_EventHandlerSet(appData.deviceHandle, APP_USBDeviceEventHandler, 0);
                /* Attach the device */
                USB_DEVICE_Attach (appData.deviceHandle);
                appData.state = APP_STATE_USB_CONFIGURATION;
                PLIB_PORTS_PinSet(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
            }
            else {
                PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
                /* The Device Layer is not ready to be opened. We should try
                 * again later. */
            }
            break;
        case APP_STATE_USB_CONFIGURATION:
            /* Check if the device was configured */
            if(appData.isConfigured) {
                /* If the device is configured then lets start reading */
                appData.state = APP_STATE_SCHEDULE_READ;
                PLIB_PORTS_PinSet(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDG_PIN);
                PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
            }
            break;
        case APP_STATE_SCHEDULE_READ:
            if(APP_StateReset()) { break; }
            //memset(appData.readBuffer, 0, APP_READ_BUFFER_SIZE); -> doesn't work
            for (i = 0; i < APP_READ_BUFFER_SIZE; i++) { appData.readBuffer[i] = '\0'; }
            appData.state = APP_STATE_WAIT_FOR_READ_COMPLETE;
            break;
        case APP_STATE_WAIT_FOR_READ_COMPLETE:
            if(APP_StateReset()) { break; }
            if (appData.isReadComplete == true) {
                appData.isReadComplete = false;
                PLIB_PORTS_PinClear(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDG_PIN);
                appData.readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
                USB_DEVICE_CDC_Read (appData.cdcInstance, &appData.readTransferHandle,
                        appData.readBuffer, APP_READ_BUFFER_SIZE);
                if (appData.readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID) {
                    appData.state = APP_STATE_ERROR;
                    break;
                }
                switch (appData.readBuffer[0]) {
                    case 'A':
                    case 'a':
                        //strcpy(appData.writeBuffer, "\r\nOK");
                        //appData.writeCount = strlen(appData.writeBuffer);
                        //appData.state = APP_STATE_SAMPLE_AIN1;
                        break;
                    case 'F':
                    case 'f':
                        //appData.state = APP_STATE_SET_FREQ;
                        break;
                    case 'W':
                    case 'w':
                        //appData.state = APP_STATE_SET_WIDTH;
                        break;
                    default:
                        appData.writeCount = 0;
                        appData.state = APP_STATE_SCHEDULE_WRITE;
                        break;
                }
                /* code xample
                if (i) {
                    sprintf(appData.writeBuffer,"\r\n%c%d",appData.readBuffer[0],i);
                    appData.writeCount = strlen(appData.writeBuffer);
                }
                */
            }
            break;
        case APP_STATE_SCHEDULE_WRITE:
            if(APP_StateReset()) { break; }
            /* Setup the write */
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
            if(APP_StateReset()) { break; }
            /* Check if a character was sent. The isWriteComplete
             * flag gets updated in the CDC event handler */
            if (appData.isWriteComplete) {
                appData.state = APP_STATE_SCHEDULE_READ;
                PLIB_PORTS_PinSet(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDG_PIN);
            }
            break;
        case APP_STATE_REGISTER_TMR:
            if (APP_RegisterTimedLED( 1000)) {
                PLIB_PORTS_PinSet(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
                appData.state = APP_STATE_OUTPUT_TIME;
            }
            break;
        case APP_STATE_OUTPUT_TIME:
            APP_CheckTimedLED();
            if (appData.lastSecond != appData.time.Seconds) {
                appData.lastSecond = appData.time.Seconds;
                if ((appData.time.Seconds == 59) & (appData.time.Minutes == 0) & (appData.time.Hours == 0)) {
                    appData.state = APP_STATE_ERROR;
                }
            }
            break;
        case APP_STATE_ERROR:
            APP_CheckTimedLED();
            if (APP_RegisterTimedLED( 100)) {
                PLIB_PORTS_PinSet(APP_LED_PORTS_ID, APP_LED_PORT_CHANNEL, APP_LEDR_PIN);
                appData.state = APP_STATE_HOLD;
            }
            break;
        case APP_STATE_HOLD:
            APP_CheckTimedLED();
            break;
        /* The default state should never be executed. */
        default:
            break;
    }
}