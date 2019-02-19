# AVR tiny 1-wire library
Simple 1-Wire software implementation for AVR MCU.


### Example
Reading temperature from DS18B20:
```C
wire_reset();
wire_writeByte(0xCC);      // Skip ROM command
wire_writeByte(0x44);      // Convert T command
_delay_ms(750);            // Delay for conversion

wire_reset();
wire_writeByte(0xCC);    // Skip ROM command
wire_writeByte(0xBE);    // Read scratchpad command

int16_t temperature = wire_readByte();  // Read temperature LSB
temperature |= (wire_readByte()<<8);    // Read temperature MSB
```
