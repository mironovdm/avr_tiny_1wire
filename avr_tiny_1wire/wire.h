/**
1-Wire standart speed software implementation.
*/

#include <avr/io.h>
#include <stdint.h>

#define F_CPU 2000000UL

// Port configuration
#define WIRE_PORT	PORTA
#define WIRE_DDR	DDRA
#define WIRE_PORTIN	PINA
#define WIRE_PIN	PORTA3

// Time periods
#define WIRE_T		60	// time slot in us
#define WIRE_T_REC	5	// recovery time in us. Min 1us, but may require higher value 
						// with strong pullup or high capacity of the bus.

/* Returns 0 if no presence pulse received, else 1 */
uint8_t wire_reset(void);

/* Write byte to the data line */
void wire_writeByte(uint8_t byte);

/* Read byte from data line */
uint8_t wire_readByte(void);

/* Read bit. May be usefull for polling DS18B20 conversion end */
uint8_t wire_readBit(void);