/* 
 * File:    POEusb.c
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-20
 * Description:
 *   USB CDC library
 * Major Changes:
 *   Version 0: alpha development
 */

#include "POEi2clcd.h"

#ifdef APP_USE_USB
// *****************************************************************************
// USB specific includes
// *****************************************************************************
#include "system/common/sys_module.h"
#include "usb/usb_device.h"
#include "usb/usb_chapter_9.h"
#include "usb/usb_cdc.h"
#include "usb/usb_device_cdc.h"


// *****************************************************************************
// global variable declarations (possibly extern in Header)
// *****************************************************************************
SYS_MODULE_OBJ usbDevObject;

// vvvv from system_init.c vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

// Endpoint Table needed by the Device Layer.
uint8_t __attribute__((aligned(512))) endpointTable[USB_DEVICE_ENDPOINT_TABLE_SIZE];

//  USB Device Desciptor for CDC (MCP demo)
const USB_DEVICE_DESCRIPTOR fullSpeedDeviceDescriptor = {
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

//  Device Configuration Decriptor
const uint8_t fullSpeedConfigurationDescriptor1[] = {
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

// String descriptors.
// Language code string descriptor 0
const struct {
    uint8_t bLength;
    uint8_t bDscType;
    uint16_t string[1];
} sd000 = {
    sizeof(sd000),          // Size of this descriptor in bytes
    USB_DESCRIPTOR_STRING,  // STRING descriptor type
    {0x0409}                // Language ID
};

// Manufacturer string descriptor 1
const struct {
    uint8_t bLength;        // Size of this descriptor in bytes
    uint8_t bDscType;       // STRING descriptor type
    uint16_t string[25];    // String
} sd001 = {
    sizeof(sd001),
    USB_DESCRIPTOR_STRING,
    {'M','i','c','r','o','c','h','i','p',' ',
     'T','e','c','h','n','o','l','o','g','y',' ','I','n','c','.'}
};

// Product string descriptor 2
const struct {
    uint8_t bLength;        // Size of this descriptor in bytes
    uint8_t bDscType;       // STRING descriptor type
    uint16_t string[22];    // String
} sd002 = {
    sizeof(sd002),
    USB_DESCRIPTOR_STRING,
    {'S','i','m','p','l','e',' ','C','D','C',' ',
     'D','e','v','i','c','e',' ','D','e','m','o' }
};

// Array of string descriptors
USB_DEVICE_STRING_DESCRIPTORS_TABLE stringDescriptors[3] = {
    (const uint8_t *const)&sd000,
    (const uint8_t *const)&sd001,
    (const uint8_t *const)&sd002
};

// Array of full speed config descriptors
USB_DEVICE_CONFIGURATION_DESCRIPTORS_TABLE fullSpeedConfigDescSet[1] = {
    fullSpeedConfigurationDescriptor1
};

// USB CDC Device Function Driver Initialization Data Structure
const USB_DEVICE_CDC_INIT  cdcInit = {
    .queueSizeRead = 1,                     // Read Queue Size
    .queueSizeWrite = 1,                    // Write Queue Size
    .queueSizeSerialStateNotification = 1   // Serial State Notification Queue Size
};

// USB Device Layer Function Driver Registration Table
const USB_DEVICE_FUNCTION_REGISTRATION_TABLE funcRegistrationTable[1] = {
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

// USB Device Layer Master Descriptor Table
const USB_DEVICE_MASTER_DESCRIPTOR usbMasterDescriptor = {
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

// USB Device Layer Initialization Data
USB_DEVICE_INIT usbDevInitData = {
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

// ^^^^ from system_init.c ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// vvvv from app.h vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

// USB: Device layer handle returned by device layer open function
USB_DEVICE_HANDLE    deviceHandle;
USB_DEVICE_CDC_INDEX cdcInstance;
// USB: Device configured state
bool                 isConfigured;
// USB device powered (attached) by Host
bool                 isAttached;
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

// buffers are controlled by the app

// ^^^^ from app.h ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


// *****************************************************************************
// Initialization routine
// *****************************************************************************

void USB_SYS_Init(void) {
    // Initialize the USB device layer
    usbDevObject = USB_DEVICE_Initialize (USB_DEVICE_INDEX_0 , ( SYS_MODULE_INIT* ) & usbDevInitData);
    // Check if the object returned by the device layer is valid
    SYS_ASSERT((SYS_MODULE_OBJ_INVALID != sysObjects.usbDevObject), "Invalid USB DEVICE object");
    
// vvvv from app.c vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // -> should it be here ???
    deviceHandle = -1;
    cdcInstance = 0;
    isConfigured = false;
    isAttached = false;
    getLineCodingData.dwDTERate = 9600;
    getLineCodingData.bCharFormat = 0;
    getLineCodingData.bParityType = 0;
    getLineCodingData.bDataBits = 8;
    readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    isReadComplete = true;
    isWriteComplete = true;
// ^^^^ from app.c ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    
}

// *****************************************************************************
// ISR
// *****************************************************************************
// Interrupt framing routine and interrupt configuration are done in the APP
// As of high importance the ISR should have one of the highest priorities
 
void USB_SYS_ISR(void) {
    USB_DEVICE_Tasks_ISR(usbDevObject);
}

// *****************************************************************************
// permanent system task
// *****************************************************************************
void USB_SYS_Loop(void) {
    // Device layer tasks routine. CDC tasks gets called from device layer tasks 
    USB_DEVICE_Tasks(usbDevObject);
}

// *****************************************************************************
// check for device reset
// *****************************************************************************
bool USB_StateReset ( void ) {
    if (!isConfigured) {
        // -> appData.state = APP_STATE_USB_CONFIGURATION by app
        readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
        isReadComplete = true;
        isWriteComplete = true;
        return true;
    } else {
        return false;
    }
}

//******************************************************
// USB Device Layer Events - Application Event Handler
//******************************************************
void USB_DeviceEventHandler ( 
    USB_DEVICE_EVENT event
,   void * eventData
,   uintptr_t context 
) {
    uint8_t * configuredEventData;
    switch ( event ) {
        case USB_DEVICE_EVENT_RESET:
            isConfigured = false;
            break;
        case USB_DEVICE_EVENT_CONFIGURED:
            // Check the configuration. We only support configuration 1 
            configuredEventData = (uint8_t *)eventData;
            if ( *configuredEventData == 1) {
                // Register the CDC Device application event handler here.
                // Note how the appData object pointer is passed as the
                // user data 
                USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0,
                        USB_DeviceCDCEventHandler, 0);
                // Mark that the device is now configured 
                isConfigured = true;
            }
            break;
        case USB_DEVICE_EVENT_POWER_DETECTED:
            // VBUS was detected. We can attach the device 
            USB_DEVICE_Attach(deviceHandle);
            isAttached = true;
            break;
        case USB_DEVICE_EVENT_POWER_REMOVED:
            // VBUS is not available any more. Detach the device. 
            USB_DEVICE_Detach(deviceHandle);
            isAttached = false;
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
USB_DEVICE_CDC_EVENT_RESPONSE USB_DeviceCDCEventHandler (
    USB_DEVICE_CDC_INDEX index 
,   USB_DEVICE_CDC_EVENT event 
,   void * pData
,   uintptr_t userData
) {
    //APP_DATA * appDataObject;
    //appDataObject = (APP_DATA *)userData;
    USB_CDC_CONTROL_LINE_STATE * evControlLineStateData;
    uint16_t * evBreakData;
    switch ( event ) {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:
            // This means the host wants to know the current line
            // coding. This is a control transfer request. Use the
            // USB_DEVICE_ControlSend() function to send the data to
            // host.
            USB_DEVICE_ControlSend(deviceHandle, &getLineCodingData,
                    sizeof(USB_CDC_LINE_CODING));
            break;
        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:
            // This means the host wants to set the line coding.
            // This is a control transfer request. Use the
            // USB_DEVICE_ControlReceive() function to receive the
            // data from the host 
            USB_DEVICE_ControlReceive(deviceHandle, &setLineCodingData,
                    sizeof(USB_CDC_LINE_CODING));
            break;
        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:
            // This means the host is setting the control line state.
            // Read the control line state. We will accept this request
            // for now. 
            evControlLineStateData = (USB_CDC_CONTROL_LINE_STATE *)pData;
            controlLineStateData.dtr = evControlLineStateData->dtr;
            controlLineStateData.carrier = evControlLineStateData->carrier;
            USB_DEVICE_ControlStatus(deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_SEND_BREAK:
            // This means that the host is requesting that a break of the
            // specified duration be sent. Read the break duration
            evBreakData = (uint16_t *)pData;
            breakData = *evBreakData;
            break;
        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:
            // This means that the host has sent some data
            isReadComplete = true;
            break;
        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:
            // The data stage of the last control transfer is
            // complete. For now we accept all the data
            USB_DEVICE_ControlStatus(deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:
            // This means the GET LINE CODING function data is valid. We dont
            // do much with this data in this demo.
            break;
        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:
            // This means that the data write got completed. We can schedule
            // the next read.
            isWriteComplete = true;
            break;
        default:
            break;
    }
    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}

bool USB_Open(void) {
    bool retVal;
    retVal = false;
    deviceHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE );
    if (deviceHandle != USB_DEVICE_HANDLE_INVALID) {
        // Register a callback with device layer to get event notification (for end point 0) 
        USB_DEVICE_EventHandlerSet(deviceHandle, USB_DeviceEventHandler, 0);
        // Attach the device ???
        USB_DEVICE_Attach (deviceHandle);
        isAttached = true;
        retVal = true;
    }
    return(retVal);
}

bool USB_isConfigured(void) {
    return isConfigured;
}

bool USB_isAttached(void) {
    return isAttached;
}

void USB_PrepareWrite(void * buffer, int size) {
    writeTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    isWriteComplete = false;
    USB_DEVICE_CDC_Write(
        cdcInstance
    ,   &writeTransferHandle
    ,   buffer
    ,   size
    , USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE
    );
}

bool USB_isWriteComplete(void) {
    if (isWriteComplete) {
        isWriteComplete = false;
        return true;
    } else {
        return false;
    }
}

bool USB_isReadComplete(void) {
    if (isReadComplete) {
        isReadComplete = false;
        return true;
    } else {
        return false;
    }
}

bool USB_ReadToBuffer(void * buffer, int maxSize) {
    bool retVal;
    retVal = true;
    readTransferHandle = USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID;
    USB_DEVICE_CDC_Read (
        cdcInstance
    ,   &readTransferHandle
    ,   buffer
    ,   maxSize
    );
    if (readTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID) {
        retVal = false;
    }
    return (retVal);
}


#endif //ifdef APP_USE_USB
