/*
 * Just some basic I2C things.
 */

#include <util/twi.h>

void init_i2c(void) {
	//set SCL to 400kHz (if you use 16MHz processor set it to 0xc)
	TWSR = 0x00;
	TWBR = 0x0C;
}

void i2c_start(uint8_t addr) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	uint8_t status = TW_STATUS & 0xF8;
	//If we fail, turn off the Error LED
	if ( (status != TW_START) && (status != TW_REP_START)) PORTD &= ~(1<<PORTD4);

	TWDR = addr;
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wail until transmission completed and ACK/NACK has been received
	while(!(TWCR & (1<<TWINT)));

	// check value of TWI Status Register. Mask prescaler bits.
	status = TW_STATUS & 0xF8;
	//Again, turn off LED if there is an error
	if ( (status != TW_MT_SLA_ACK) && (status != TW_MR_SLA_ACK) ) PORTD &= ~(1<<PORTD4);

}

void i2c_send(uint8_t byte) {
	TWDR = byte;
	TWCR = (1 << TWINT)|(1 << TWEN);
	while (!(TWCR & (1 << TWINT)));

	uint8_t status = TW_STATUS & 0xF8;
	//LED turned off in case of an error
	if (status != TW_MT_DATA_ACK) PORTD &= ~(1<<PORTD4);
}