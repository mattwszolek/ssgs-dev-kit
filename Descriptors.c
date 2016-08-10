#include "Descriptors.h"

const uint8_t PROGMEM GenericReport[CUSTOMHID_SIZ_REPORT_DESC_SIZE] =
{
	0x06, 0xC0, 0xFF,    // Usage Page (Vendor-Defined 193)
	0x09, 0x01,          // Usage (Vendor-Defined 1)
	0xa1, 0x01,          // Collection (Application)
	0x06, 0xC1, 0xFF,    // Usage Page (Vendor-Defined 194)
	0x15, 0x00,          // Logical Minimum (0)
	0x26, 0xFF, 0x00,    // Logical Maximum (255)
	0x75, 0x08,          // Report Size (8)
	0x09, 0xF0,          // Usage (Vendor-Defined 240)
	0x95, 0x20,          // Report Count (32)
	0x81, 0x02,          // Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)
	0x09, 0xF1,          // Usage (Vendor-Defined 241)
	0x95, 0x20,          // Report Count (32)
	0x91, 0x02,          // Output (Data,Var,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0x09, 0xF2,          // Usage (Vendor-Defined 242)
	0x96, 0x00, 0x02,    // Report Count (512)
	0xB1, 0x02,          // Feature (Data,Var,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
	0xC0                 // End Collection
};

const uint8_t PROGMEM DeviceDescriptor[CUSTOMHID_SIZ_DEVICE_DESC] =
{
	0x12,                       /*bLength */
	USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
	0x10,                       /*bcdUSB */
	0x01,
	0x00,                       /*bDeviceClass*/
	0x00,                       /*bDeviceSubClass*/
	0x00,                       /*bDeviceProtocol*/
	8,                          /*bMaxPacketSize*/
	(uint8_t)USBD_VID,          /*idVendor*/
	USBD_VID>>8,                /*idVendor*/
	(uint8_t)USBD_PID,          /*idVendor*/
	USBD_PID>>8,                /*idVendor*/
	(uint8_t)FW_VERSION,        //bcdDevice
	FW_VERSION>>8,
	STRING_ID_Manufacturer,     /*Index of manufacturer  string*/
	STRING_ID_Product,          /*Index of product string*/
	//USBD_IDX_SERIAL_STR,      /*Index of serial number string*/
	0,
	USBD_CFG_MAX_NUM            /*bNumConfigurations*/
};

const uint8_t PROGMEM ConfigurationDescriptor[USB_HID_CONFIG_DESC_SIZ] =
{
	0x09,                              // bLength: Configuration Descriptor size
	USB_CONFIGURATION_DESCRIPTOR_TYPE, // bDescriptorType: Configuration
	USB_HID_CONFIG_DESC_SIZ,           // wTotalLength: Bytes returned
	0x00,
	0x01,                              // bNumInterfaces: 1 interface
	0x01,                              // bConfigurationValue: Configuration value
	0x00,                              // iConfiguration: Index of string descriptor describing the configuration
	0xA0,                              // bmAttributes: bus powered and Support Remote Wake-up
	0xC8,                              // MaxPower 400 mA: this current is used for detecting Vbus
	// Interface0 descriptor - Custom
	// 9
	0x09,                              // bLength: Interface Descriptor size
	USB_INTERFACE_DESCRIPTOR_TYPE,     // bDescriptorType: Interface descriptor type
	INTERFACE_ID_Generic,              // bInterfaceNumber: Number of Interface
	0x00,                              // bAlternateSetting: Alternate setting
	0x01,                              // bNumEndpoints
	0x03,                              // bInterfaceClass: HID
	0x00,                              // bInterfaceSubClass : 1=BOOT, 0=no boot
	0x00,                              // nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
	0x00,                              // iInterface: Index of string descriptor

	//Descriptor of Custom HID
	// 18
	0x09,                              // bLength: HID Descriptor size
	HID_DESCRIPTOR_TYPE,               // bDescriptorType: HID
	0x11,                              // bcdHID: HID Class Spec release number
	0x01,
	0x00,                              // bCountryCode: Hardware target country
	0x01,                              // bNumDescriptors: Number of HID class descriptors to follow
	0x22,                              // bDescriptorType
	CUSTOMHID_SIZ_REPORT_DESC_SIZE,    // wItemLength: Total length of Report descriptor
	0x00,
	//Descriptor of custom endpoint
	// 27
	0x07,                              // bLength: Endpoint Descriptor size
	USB_ENDPOINT_DESCRIPTOR_TYPE,      // bDescriptorType:
	GENERIC_IN_EPADDR,                 // bEndpointAddress: Endpoint Address (IN)
	0x03,                              // bmAttributes: Interrupt endpoint
	HID_IN_PACKET_1,                   // wMaxPacketSize: 32 Byte max
	0x00,
	0x0A,                              // bInterval: Polling Interval (5 ms)
	// 34
};

//That's how language is done in all examples from LUFA
const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);

//Name of manufacturer
const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"SteelSeries");

//Name of the device
const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"GameSense Dev Kit");

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
 //Copy paste from Examples from LUFA Library basically. We need to adjust size of the descriptors
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress) {
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void* Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = &DeviceDescriptor;
			Size    = CUSTOMHID_SIZ_DEVICE_DESC;
			break;
		case DTYPE_Configuration:
			Address = &ConfigurationDescriptor;
			Size    = USB_HID_CONFIG_DESC_SIZ;
			break;
		case DTYPE_String:
			switch (DescriptorNumber)
			{
				case STRING_ID_Language:
					Address = &LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case STRING_ID_Manufacturer:
					Address = &ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case STRING_ID_Product:
					Address = &ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
			}

			break;
		case HID_DTYPE_HID:
			switch (wIndex)
			{
				case INTERFACE_ID_Generic:
					Address = &ConfigurationDescriptor[9];
					Size    = STANDARD_ENDPOINT_DESC_SIZE;
					break;
			}

			break;
		case HID_DTYPE_Report:
			switch (wIndex)
			{
				case INTERFACE_ID_Generic:
					Address = &GenericReport;
					Size    = CUSTOMHID_SIZ_REPORT_DESC_SIZE;
					break;
			}

			break;
	}

	*DescriptorAddress = Address;
	return Size;
}

