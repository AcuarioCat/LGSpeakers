#include "EepromUtil.h"
#include "debugWeb.h"

//
// Initialize eeprom memory with the specified byte.
// Default value is 0x00.
//
void EepromUtil::eeprom_erase_all(byte b = 0x00) {
	word i;

	for (i = EEPROM_MIN_ADDR; i <= EEPROM_MAX_ADDR; i++) {
		EEPROM.write(i, b);
	}
}


//
// Dump eeprom memory contents over serial port.
// For each byte, address and value are written.
//
void EepromUtil::eeprom_serial_dump_xy(int x, int y) {
	// counter
	word i;

	// byte read from eeprom
	byte b;

	// buffer used by sprintf
	char buf[10];

	for (i = EEPROM_MIN_ADDR + x; i <= EEPROM_MIN_ADDR + y; i++) {
		b = EEPROM.read(i);
		snprintf(buf, sizeof(buf), "%03X: %02X", i, b);
		DEBUG_PRINTLN(buf);
	}
}

//
// Dump eeprom memory contents over serial port.
// For each byte, address and value are written.
//
void EepromUtil::eeprom_serial_dump_column() {
	// counter
	word i;

	// byte read from eeprom
	byte b;

	// buffer used by sprintf
	char buf[10];

	for (i = EEPROM_MIN_ADDR; i <= EEPROM_MAX_ADDR; i++) {
		b = EEPROM.read(i);
		snprintf(buf, sizeof(buf), "%03X: %02X", i, b);
		DEBUG_PRINTLN(buf);
	}
}


//
// Dump eeprom memory contents over serial port in tabular form.
// Each printed row shows the value of bytesPerRow bytes
// (by default 16).
//
void EepromUtil::eeprom_serial_dump_table(int bytesPerRow = 16) {
	// address counter
	word i;

	// row bytes counter
	word j;

	// byte read from eeprom
	byte b;

	// temporary buffer for sprintf
	char buf[10];


	// initialize row counter
	j = 0;

	// go from first to last eeprom address
	for (i = EEPROM_MIN_ADDR; i <= EEPROM_MAX_ADDR; i++) {

		// if this is the first byte of the row,
		// start row by printing the byte address
		if (j == 0) {
			snprintf(buf, sizeof(buf), "%03X: ", i);
			Serial.print(buf);
		}

		// read current byte from eeprom
		b = EEPROM.read(i);

		// write byte in hex form
		snprintf(buf, sizeof(buf), "%02X ", b);

		// increment row counter
		j++;

		// if this is the last byte of the row,
		// reset row counter and use println()
		// to start a new line
		if (j == bytesPerRow) {
			j = 0;
			DEBUG_PRINTLN(buf);
		}
		// else just print the hex value with print()
		else {
			Serial.print(buf);
		}
	}
}


//
// Returns true if the address is between the
// minimum and maximum allowed values,
// false otherwise.
//
// This function is used by the other, higher-level functions
// to prevent bugs and runtime errors due to invalid addresses.
//
boolean EepromUtil::eeprom_is_addr_ok(word addr) {
	return ((addr >= EEPROM_MIN_ADDR) && (addr <= EEPROM_MAX_ADDR));
}


//
// Writes a sequence of bytes to eeprom starting at the specified address.
// Returns true if the whole array is successfully written.
// Returns false if the start or end addresses aren't between the minimum and maximum allowed values.
// When returning false, nothing gets written to eeprom.
//
boolean EepromUtil::eeprom_write_bytes(word startAddr, const byte* array, int numBytes) {
	// counter
	word i;

	// both first byte and last byte addresses must fall within
	// the allowed range  
	if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) {
		return false;
	}

	for (i = 0; i < numBytes; i++) {
		//Serial.printf("Add:%x D:%d L:%d\n", startAddr + i, array[i], numBytes);
		EEPROM.write(startAddr + i, array[i]);
	}
	EEPROM.write(startAddr + numBytes, 0);

	return true;
}


// Reads the specified number of bytes from the specified address into the provided buffer.
// Returns true if all the bytes are successfully read.
// Returns false if the star or end addresses aren't between the minimum and maximum allowed values.
// When returning false, the provided array is untouched.
//
// Note: the caller must ensure that array[] has enough space to store at most numBytes bytes.
//
boolean EepromUtil::eeprom_read_bytes(word startAddr, byte array[], int numBytes) {
	word i;

	// both first byte and last byte addresses must fall within
	// the allowed range  
	if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) {
		return false;
	}

	for (i = 0; i < numBytes; i++) {
		array[i] = EEPROM.read(startAddr + i);
	}
	return true;
}

//
// Writes a byte variable at the specified address.
// Returns true if the variable value is successfully written.
// Returns false if the specified address is outside the allowed range to store all of the bytes
//
boolean EepromUtil::eeprom_write_byte(word addr, byte value) {

	// the address must fall within the allowed range  
	if (!eeprom_is_addr_ok(addr)) {
		return false;
	}
	EEPROM.write(addr, value);
	return true;
	//byte *ptr;

	//ptr = (byte*)&value;
	//return eeprom_write_bytes(addr, ptr, sizeof(value));
}

//
// Writes a word variable at the specified address.
// Returns true if the variable value is successfully written.
// Returns false if the specified address is outside the allowed range or too close to the maximum value
// to store all of the bytes (an int variable requires more than one byte).
//
boolean EepromUtil::eeprom_write_word(word addr, word value) {
	byte *ptr;

	ptr = (byte*)&value;
	return eeprom_write_bytes(addr, ptr, sizeof(value));
}


//
// Reads an integer value at the specified address.
// Returns true if the variable is successfully read.
// Returns false if the specified address is outside the allowed range or too close to the maximum vlaue
// to hold all of the bytes (an int variable requires more than one byte).
//
boolean EepromUtil::eeprom_read_word(word addr, word* value) {
	return eeprom_read_bytes(addr, (byte*)value, sizeof(int));
}

word EepromUtil::eeprom_read_word(word i) {
	byte l = EEPROM.read(i);
	byte h = EEPROM.read(i + 1);
	return word(h, l);
}

byte EepromUtil::eeprom_read_byte(word i) {
	byte l = EEPROM.read(i);
	return l;
}

//This function will write a 4 byte (32bit) long to the eeprom at
//the specified address to address + 3.
void EepromUtil::eeprom_write_long(word address, long value)
{
	//Decomposition from a long to 4 bytes by using bitshift.
	//One = Most significant -> Four = Least significant byte
	byte four = (value & 0xFF);
	byte three = ((value >> 8) & 0xFF);
	byte two = ((value >> 16) & 0xFF);
	byte one = ((value >> 24) & 0xFF);

	//Write the 4 bytes into the eeprom memory.
	EEPROM.write(address, four);
	EEPROM.write(address + 1, three);
	EEPROM.write(address + 2, two);
	EEPROM.write(address + 3, one);
}

long EepromUtil::eeprom_read_long(word address)
{
	//Read the 4 bytes from the eeprom memory.
	long four = EEPROM.read(address);
	long three = EEPROM.read(address + 1);
	long two = EEPROM.read(address + 2);
	long one = EEPROM.read(address + 3);

	//Return the recomposed long by using bitshift.
	return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

// Writes a string starting at the specified address.
// Returns true if the whole string is successfully written.
// Returns false if the address of one or more bytes fall outside the allowed range.
// If false is returned, nothing gets written to the eeprom.
//
boolean EepromUtil::eeprom_write_string(word addr, const char* string) {
	// actual number of bytes to be written
	int numBytes;

	// we'll need to write the string contents
	// plus the string terminator byte (0x00)
	numBytes = strlen(string) + 1;

	return eeprom_write_bytes(addr, (const byte*)string, numBytes);
}

// Writes a string starting at the specified address.
// Returns true if the whole string is successfully written.
// Returns false if the address of one or more bytes fall outside the allowed range.
// If false is returned, nothing gets written to the eeprom.
//
boolean EepromUtil::eeprom_write_string(word addr, const char* string, int numBytes) {
	return eeprom_write_bytes(addr, (const byte*)string, numBytes);
}

//// Writes a string starting at the specified address.
//// Returns true if the whole string is successfully written.
//// Returns false if the address of one or more bytes fall outside the allowed range.
//// If false is returned, nothing gets written to the eeprom.
////
//boolean EepromUtil::eeprom_write_string(word addr, const char* string) {
//	bool ret;
//	char* strToWrite;
//	// actual number of bytes to be written
//	int numBytes = strlen(string) + 1;
//	strToWrite = (char *)malloc(numBytes);
//
//	// we'll need to write the string contents plus the string terminator byte (0x00)
//	//strcpy(strToWrite, string);
//	//strcat(strToWrite, '\0');
//	ret = eeprom_write_bytes(addr, (const byte*)strToWrite, numBytes);
//	free(strToWrite);
//	return ret;
//}


//
// Reads a string starting from the specified address.
// Returns true if at least one byte (even only the string terminator one) is read.
// Returns false if the start address falls outside the allowed range or declare buffer size is zero.
// The reading might stop for several reasons:
// - no more space in the provided buffer
// - last eeprom address reached
// - string terminator byte (0x00) encountered.
// - if fullString then read bufSize characters and ignore terminator
// The last condition is what should normally occur.
//
boolean EepromUtil::eeprom_read_string(word addr, char* buffer, int bufSize, bool fullString) {
	// byte read from eeprom
	byte ch;

	// number of bytes read so far
	int bytesRead;

	// check start address
	if (!eeprom_is_addr_ok(addr)) {
		return false;
	}

	// how can we store bytes in an empty buffer ?
	if (bufSize == 0) {
		return false;
	}

	// is there is room for the string terminator only,
	// no reason to go further
	if (bufSize == 1) {
		buffer[0] = 0;
		return true;
	}

	// initialize byte counter
	bytesRead = 0;

	// read next byte from eeprom
	ch = EEPROM.read(addr + bytesRead);

	// store it into the user buffer
	buffer[bytesRead] = ch;

	// increment byte counter
	bytesRead++;

	// stop conditions:
	// - the character just read is the string terminator one (0x00)
	// - we have filled the user buffer
	// - we have reached the last eeprom address
	while (((ch != 0x00) || fullString) && (bytesRead < bufSize) && ((addr + bytesRead) <= EEPROM_MAX_ADDR)) {
		// if no stop condition is met, read the next byte from eeprom
		ch = EEPROM.read(addr + bytesRead);

		// store it into the user buffer
		buffer[bytesRead] = ch;

		// increment byte counter
		bytesRead++;
	}

	// make sure the user buffer has a string terminator
	// (0x00) as its last byte
	if ((ch != 0x00) && (bytesRead >= 1)) {
		buffer[bytesRead - 1] = 0;
	}
	return true;
}

