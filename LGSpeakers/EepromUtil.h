#ifndef _EEPROMUTIL_h
#define _EEPROMUTIL_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <EEPROM.h>

//
// Absolute min and max eeprom addresses.
// Actual values are hardware-dependent.
//
// These values can be changed e.g. to protect
// eeprom cells outside this range.
//
const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 511;

class EepromUtil
{
public:
	void eeprom_erase_all(byte b);
	void eeprom_serial_dump_xy(int x, int y);
	void eeprom_serial_dump_column();
	void eeprom_serial_dump_table(int bytesPerRow);
	word eeprom_read_word(word i);
	byte eeprom_read_byte(word i);
	long eeprom_read_long(word address);
	boolean eeprom_read_word(word addr, word* value);
	boolean eeprom_is_addr_ok(word addr);
	boolean eeprom_write_bytes(word startAddr, const byte* array, int numBytes);
	boolean eeprom_read_bytes(word startAddr, byte array[], int numBytes);
	boolean eeprom_write_byte(word addr, byte value);
	boolean eeprom_write_word(word addr, word value);
	void eeprom_write_long(word address, long value);
	boolean eeprom_write_string(word addr, const char* string);
	boolean eeprom_write_string(word addr, const char * string, int numBytes);
	boolean eeprom_read_string(word addr, char* buffer, int bufSize, bool full = false);

};

extern EepromUtil eepromUtil;

#endif	//_EEPROMUTIL_h