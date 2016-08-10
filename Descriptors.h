#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

//device info
#define USBD_VID                                0x1038
#define USBD_PID                                0x07ED
#define FW_VERSION                              0x0002

#define USB_MAX_EP0_SIZE                        8
#define USBD_CFG_MAX_NUM                        1

//USB/HID types
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05
#define HID_DESCRIPTOR_TYPE                     0x21
#define STANDARD_ENDPOINT_DESC_SIZE             0x09

//sizes of descriptors
#define CUSTOMHID_SIZ_DEVICE_DESC               18
#define USB_HID_CONFIG_DESC_SIZ                 34
#define CUSTOMHID_SIZ_REPORT_DESC_SIZE          37

//First interface
#define HID_IN_PACKET_1                         32
#define INTERFACE_ID_Generic                    0
#define GENERIC_IN_EPADDR                       (ENDPOINT_DIR_IN | 1)

//strings
#define STRING_ID_Language                      0
#define STRING_ID_Manufacturer                  1
#define STRING_ID_Product                       2

//includes
#include <avr/pgmspace.h>
#include <LUFA/Drivers/USB/USB.h>


uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif

