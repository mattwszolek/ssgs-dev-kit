#ifndef _I2C_H_
#define _I2C_H_

#include <avr/io.h>

void init_i2c(void);
void i2c_start(uint8_t addr);
void i2c_send(uint8_t byte);

inline void i2c_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

#endif