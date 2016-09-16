#include "i2c.h"
#include "ssd1306.h"

static uint16_t ssdIndex = 0;
static uint8_t ssdBuffer[OLED_SZ] = {0};
static uint8_t ssdTranslateBuffer[OLED_SZ] = {0};
static uint8_t ssdSend = SENT;


// Translates one page of the oled display.
// Input should be the first byte of the page.
static void inverse_translate (uint8_t * input, uint8_t * output) {
	for (int section = 0; section < 16; ++section) {
		int omup = section * 8;
		output[0 + omup] = 
					((input[0 + section] & 0x80) >> 7) | 
					((input[16 + section] & 0x80) >> 6) |
					((input[32 + section] & 0x80) >> 5) |
					((input[48 + section] & 0x80) >> 4) |
					((input[64 + section] & 0x80) >> 3) |
					((input[80 + section] & 0x80) >> 2) |
					((input[96 + section] & 0x80) >> 1) |
					((input[112 + section] & 0x80) >> 0);

		output[1 + omup] = 
					((input[0 + section] & 0x40) >> 6) | 
					((input[16 + section] & 0x40) >> 5) |
					((input[32 + section] & 0x40) >> 4) |
					((input[48 + section] & 0x40) >> 3) |
					((input[64 + section] & 0x40) >> 2) |
					((input[80 + section] & 0x40) >> 1) |
					((input[96 + section] & 0x40) >> 0) |
					((input[112 + section] & 0x40) << 1);

		output[2 + omup] = 
					((input[0 + section] & 0x20) >> 5) | 
					((input[16 + section] & 0x20) >> 4) |
					((input[32 + section] & 0x20) >> 3) |
					((input[48 + section] & 0x20) >> 2) |
					((input[64 + section] & 0x20) >> 1) |
					((input[80 + section] & 0x20) >> 0) |
					((input[96 + section] & 0x20) << 1) |
					((input[112 + section] & 0x20) << 2);

		output[3 + omup] = 
					((input[0 + section] & 0x10) >> 4) | 
					((input[16 + section] & 0x10) >> 3) |
					((input[32 + section] & 0x10) >> 2) |
					((input[48 + section] & 0x10) >> 1) |
					((input[64 + section] & 0x10) >> 0) |
					((input[80 + section] & 0x10) << 1) |
					((input[96 + section] & 0x10) << 2) |
					((input[112 + section] & 0x10) << 3);

		output[4 + omup] = 
					((input[0 + section] & 0x08) >> 3) | 
					((input[16 + section] & 0x08) >> 2) |
					((input[32 + section] & 0x08) >> 1) |
					((input[48 + section] & 0x08) << 0) |
					((input[64 + section] & 0x08) << 1) |
					((input[80 + section] & 0x08) << 2) |
					((input[96 + section] & 0x08) << 3) |
					((input[112 + section] & 0x08) << 4);

		output[5 + omup] = 
					((input[0 + section] & 0x04) >> 2) | 
					((input[16 + section] & 0x04) >> 1) |
					((input[32 + section] & 0x04) >> 0) |
					((input[48 + section] & 0x04) << 1) |
					((input[64 + section] & 0x04) << 2) |
					((input[80 + section] & 0x04) << 3) |
					((input[96 + section] & 0x04) << 4) |
					((input[112 + section] & 0x04) << 5);

		output[6 + omup] = 
					((input[0 + section] & 0x02) >> 1) | 
					((input[16 + section] & 0x02) >> 0) |
					((input[32 + section] & 0x02) << 1) |
					((input[48 + section] & 0x02) << 2) |
					((input[64 + section] & 0x02) << 3) |
					((input[80 + section] & 0x02) << 4) |
					((input[96 + section] & 0x02) << 5) |
					((input[112 + section] & 0x02) << 6);

		output[7 + omup] = 
					((input[0 + section] & 0x01) >> 0) | 
					((input[16 + section] & 0x01) << 1) |
					((input[32 + section] & 0x01) << 2) |
					((input[48 + section] & 0x01) << 3) |
					((input[64 + section] & 0x01) << 4) |
					((input[80 + section] & 0x01) << 5) |
					((input[96 + section] & 0x01) << 6) |
					((input[112 + section] & 0x01) << 7);
	}
}

void ssd_translate(void) {
	static uint8_t translate_step = 0;
	translate_step &= PAGE_MASK;
	inverse_translate(&ssdTranslateBuffer[translate_step * PAGE_SIZE], &ssdBuffer[translate_step * PAGE_SIZE]);
	translate_step++;
	if (translate_step == NUM_PAGES) { //there are four pages
		ssdSend = SEND;
	}
}

void ssd_data_translate(uint8_t * data) {
	for (uint16_t i = 0; i < OLED_SZ; ++i) {
		ssdTranslateBuffer[i] = data[i];
	}
	ssdSend = TRANSLATE;
}

void ssd_update_status(uint8_t status) {
	ssdSend = status;
}

uint8_t ssd_get_status(void) {
	return ssdSend;
}

void ssd_data(uint8_t * data) {
	for (uint16_t i = 0; i < OLED_SZ; ++i) {
		ssdBuffer[i] = data[i];
	}
	ssdSend = SEND;
}

void write_ssd_cmd(uint8_t byte) {
	i2c_start(OLED_ADDR);
	i2c_send(0x0);
	i2c_send(byte);
	i2c_stop();
}

// On the Adafruit's github they had divided data
// packets into portions of sixteen, so I did the
// same here.
void write_sixteen(void) {
	//wait until we can start sending data
	i2c_start(OLED_ADDR);
	//i2c_send(OLED_ADDR);

	i2c_send(0x40);
	for (uint8_t i = 0; i < 16; ++i) {
		i2c_send(ssdBuffer[ssdIndex++]);
	}
	i2c_stop();
	if (ssdIndex == OLED_SZ) {
		ssdIndex = 0;
		ssdSend = SENT;
	}
}

// https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
// Order is different than in the flow chart, but it is
// the same as on the Adafruit's github for the init of
// the screne
void init_oled(void) {
	init_i2c();

	write_ssd_cmd(0xAE);                    // Display Off

	write_ssd_cmd(0xD5);                    // Set Display Clk Div 0xD5
	write_ssd_cmd(0x80);                    // Recommneded resistor ratio 0x80

	write_ssd_cmd(0xA8);                    // Set Multiplex 0xA8
	write_ssd_cmd(0x1F);                    // 1/32 Duty Cycle 

	write_ssd_cmd(0xD3);                    // Set Display Offset 0xD3
	write_ssd_cmd(0x0);                     // no offset

	write_ssd_cmd(0x40);                    // Set start line at line 0 - 0x40 
	write_ssd_cmd(0x8D);                    // Charge Pump -0x8D
	write_ssd_cmd(0x14);

	write_ssd_cmd(0x20);                    // Set Memory Addressing Mode
	write_ssd_cmd(0x00);                    // 0x00 - Horizontal
	write_ssd_cmd(0xA1);                    // Segremap - 0xA1
	write_ssd_cmd(0xC8);                    // COMSCAN DEC 0xC8 C0
	write_ssd_cmd(0xDA);                    // Set COM Pins0xDA
	write_ssd_cmd(0x02);
	write_ssd_cmd(0x81);                    // Set Contrast 0x81
	write_ssd_cmd(0x8F);
	write_ssd_cmd(0xD9);                    // Set Precharge 0xd9
	write_ssd_cmd(0xF1);

	write_ssd_cmd(0xDB);                    // Set VCOM Detect - 0xDB
	write_ssd_cmd(0x40);
	write_ssd_cmd(0xA4);                    // DISPLAY ALL ON RESUME - 0xA4
	write_ssd_cmd(0xA6);                    // Normal Display 0xA6 (Invert A7)
	write_ssd_cmd(0xAF);                    //--turn on oled panel - AF
}