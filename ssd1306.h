#ifndef SSD1306_H
#define SSD1306_H

//screen data
#define OLED_ADDR 0x78
#define OLED_SZ 512

//values for the screen being (not) busy
#define SENT 1
#define SEND 2
#define SENDING 3


void ssd_update_status(uint8_t status);
uint8_t ssd_get_status(void);
void ssd_data(uint8_t * data);
void write_ssd_cmd(uint8_t byte);
void write_sixteen(void);
void init_oled(void);


#endif