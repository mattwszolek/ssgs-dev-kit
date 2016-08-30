#include "Display.h"
#include "ssd1306.h"

#define F_CPU 16000000UL //16MHz

// Data to be sent back for USB command(s)
static uint8_t gRet[HID_IN_PACKET_1] = {0};
static uint8_t gSend = 0;


/** Buffer to hold the previously generated Keyboard HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevGenericHIDReportBuffer[HID_IN_PACKET_1];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another. This is for the Generic HID
 *  interface within the device.
 */
USB_ClassInfo_HID_Device_t Generic_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = INTERFACE_ID_Generic,
				.ReportINEndpoint             =
					{
						.Address              = GENERIC_IN_EPADDR,
						.Size                 = HID_IN_PACKET_1,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevGenericHIDReportBuffer,
				.PrevReportINBufferSize       = HID_IN_PACKET_1,
			},
	};

void rgb_led_setup(void) {
	//set up pins for output to the LED
	DDRB |= (1<<DDB5) | (1<<DDB6);
	DDRC |= (1<<DDC6);
	//set up PWM on those pins
	TCCR1A |= (1<<COM1A1) | (1<<COM1B1) | (1<<COM1A0) | (1<<COM1B0) | (1<<WGM10);
	TCCR1B |= (1<<WGM12) | (1<<CS10);
	TCCR3A |= (1<<COM3A1) | (1<<COM3A0) | (1<<WGM30);
	TCCR3B |= (1<<WGM32) | (1<<CS30);
	//set default values for the LED (0-FF)
	OCR1A = 0x40;
	OCR1B = 0x80;
	OCR3A = 0xFF;
}


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void) {
	uint8_t ssd_status;
	//Error LED
	DDRD |= (1<<DDD4);
	PORTD |= (1<<PORTD4);

	rgb_led_setup();
	init_oled();

	SetupHardware();
	GlobalInterruptEnable();
	
	for (;;)
	{
		ssd_status = ssd_get_status();
		/*Check OLED status*/
		if (ssd_status == TRANSLATE) {
			ssd_translate();
		}
		if (ssd_status == SEND) {
			write_ssd_cmd(0x21);   //column addr
			write_ssd_cmd(0);
			write_ssd_cmd(127);    //0-127

			write_ssd_cmd(0x22);   //page addr
			write_ssd_cmd(0);
			write_ssd_cmd(3);      //four pages for the 32x1128

			ssd_update_status(SENDING);;
		}
		if (ssd_status == SENDING) {
			write_sixteen();
		}
		/*Check USB status*/
		HID_Device_USBTask(&Generic_HID_Interface);
		USB_USBTask();	
	}
	
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware() {
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
	/* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
	XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
	XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

	/* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
	XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
	XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

	/* Hardware Initialization */
	USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void) {
	;
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void) {
	;
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Generic_HID_Interface);

	USB_Device_EnableSOFEvents();
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void) {
	HID_Device_ProcessControlRequest(&Generic_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void) {
	HID_Device_MillisecondElapsed(&Generic_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize) {
	if (gSend == 1 && HIDInterfaceInfo == &Generic_HID_Interface) {
		uint8_t *data = (uint8_t *)ReportData;
		data[0] = gRet[0];
		data[1] = gRet[1];
		data[2] = gRet[2];
		data[3] = gRet[3];
		data[4] = gRet[4];

		*ReportSize = HID_IN_PACKET_1;
		gSend = 0;

		return true;
	}
	return false;
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize) {
	if (ReportType == HID_REPORT_ITEM_Feature) {
		//copy data to buffer and display it
		uint8_t *data = (uint8_t*)ReportData;
		if (data[0] == 0x1) {
			ssd_data_translate(&data[1]);
		} else if (data[0] == 0x2) {
			ssd_data(&data[1]);
		} //must be 1 or 2, otherwise nothing written to screen
	} else if (HIDInterfaceInfo == &Generic_HID_Interface){
		uint8_t *data = (uint8_t *)ReportData;
		if (data[0] == 0x20) {
			switch (data[1]) {
			case 0x1: //get status of the OLED progress
				gRet[0] = ssd_get_status();
				break;
			case 0x10:
				OCR1B = data[2]; //red
				OCR3A = data[3]; //green
				OCR1A = data[4]; //blue
				gRet[0] = 1;
				break;
			default:
				gRet[0] = 0;
			}
		} else {
			gRet[0] = 0;
		}
		gSend = 1;
	}
}