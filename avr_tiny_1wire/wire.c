/*

Tiny AVR 1-Wire library
Software implementation

*/

#include <stdbool.h>
#include "wire.h"
#include <util/delay.h>
#include <util/delay_basic.h>
#include <avr/interrupt.h>



uint8_t wire_reset(void)
{
	cli();
    WIRE_DDR &= ~(1<<WIRE_PIN);				// port to input
    WIRE_PORT &= ~(1<<WIRE_PIN);			// disable pull-up

    if (!(WIRE_PORTIN & (1<<WIRE_PIN)))		// if line is low, then it is busy
        return 0;

    WIRE_DDR |= (1<<WIRE_PIN);              // set port output low
    _delay_us(8*WIRE_T);					// wait 8T (after min 480us of line low all devices on the bus are reseted)

    WIRE_DDR &= ~(1<<WIRE_PIN);				// release bus (port input no pull-up)
	
											// 15-60us after line is released slave device should pull line to low for 60-240us (common is 120us for DS18B20)
											// master should remain line high for min 480us
    bool gotPresencePulse = 0;
    for (uint8_t i = 24; i > 0; i--) {      // wait 4T (240us) for presence pulse
        if (!(WIRE_PORTIN & (1<<WIRE_PIN)))
            gotPresencePulse = 1;
        _delay_us(10);
    }

    if (!gotPresencePulse)
        return 0;
	
	_delay_us(WIRE_T_REC);							// recovery time
	sei();
	return 1;
}


void wire_writeByte(uint8_t byte)
{
	// time slot durations is 60us <= Tslot <= 120us
	// recovery time 1us <= Trec <= infinity
	cli();
	for (uint8_t i = 8; i > 0; i--) {
		if (byte & 0x01) {					// transmit 1
			WIRE_DDR |= (1<<WIRE_PIN);		// the bus should be pulled high in first 15us after pulled low, because after 15us slave samples the bus
			_delay_us(3);					// Tlow, min 1us
			WIRE_DDR &= ~(1<<WIRE_PIN);		// pull bus high
			_delay_us(WIRE_T-5);
		} else {							// transmit 0
			WIRE_DDR |= (1<<WIRE_PIN);
			_delay_us(WIRE_T);				// Tslot
			WIRE_DDR &= ~(1<<WIRE_PIN);		// pull bus high
		}
		_delay_us(WIRE_T_REC);				// recovery time
		
		byte >>= 1;
	}
	
	sei();
}

uint8_t wire_readByte(void)
{
	uint8_t byte = 0;
	uint8_t bitValue = 0;

	cli();
	for (uint8_t i = 8; i > 0; i--) {
        WIRE_DDR |= (1<<WIRE_PIN);			// pull bus low
	 	_delay_us(3);
		
	 	WIRE_DDR &= ~(1<<WIRE_PIN);			// release bus
	 	_delay_us(WIRE_T_REC);				// delay before sample data
		
	 	bitValue = (WIRE_PORTIN & (1<<WIRE_PIN))||0;		// sample line
		byte = (byte>>1);
	 	byte |= (bitValue<<7);
	 	_delay_us(WIRE_T-10);
	 }

	_delay_us(WIRE_T_REC);					// recovery time
	sei();

	return byte;
}

uint8_t wire_readBit(void)
{
    uint8_t bitValue = 0;
    cli();
	WIRE_DDR |= (1<<WIRE_PIN);			// pull bus low (port output low)
	_delay_us(3);
	WIRE_DDR &= ~(1<<WIRE_PIN);			// release bus
	_delay_us(WIRE_T_REC);

	bitValue = (WIRE_PORTIN & (1<<WIRE_PIN)) || 0;		// sample line, logic OR to get logic value

	_delay_us(WIRE_T-WIRE_T_REC-3);
    _delay_us(1);                       // min recovery time between time slots
	sei();

	return bitValue;
}