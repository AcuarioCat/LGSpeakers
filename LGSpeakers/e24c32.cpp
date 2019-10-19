 /**************************************************************************//**
  * \file Eeprom24C32_64.cpp
  ******************************************************************************/

  /******************************************************************************
   * Header file inclusions.
   ******************************************************************************/

#include <Arduino.h>
#include <Wire.h>

#include "e24c32.h"

   /******************************************************************************
	* Private macro definitions.
	******************************************************************************/

	/**************************************************************************//**
	 * \def EEPROM__PAGE_SIZE
	 * \brief Size of a page in EEPROM memory.
	 * This size is given by EEPROM memory datasheet.
	 ******************************************************************************/
#define EEPROM__PAGE_SIZE         32

	 /**************************************************************************//**
	  * \def EEPROM__RD_BUFFER_SIZE
	  * \brief Size of input TWI buffer.
	  * This size is equal to BUFFER_LENGTH defined in Wire library (32 bytes).
	  ******************************************************************************/
#define EEPROM__RD_BUFFER_SIZE    BUFFER_LENGTH

	  /**************************************************************************//**
	   * \def EEPROM__WR_BUFFER_SIZE
	   * \brief Size of output TWI buffer.
	   * This size is equal to BUFFER_LENGTH - 2 bytes reserved for address.
	   ******************************************************************************/
#define EEPROM__WR_BUFFER_SIZE    (BUFFER_LENGTH - 2)

	   /******************************************************************************
		* Public method definitions.
		******************************************************************************/

		/**************************************************************************//**
		 * \fn Eeprom24C32_64::Eeprom24C32_64(byte deviceAddress)
		 *
		 * \brief Constructor.
		 *
		 * \param   deviceAddress   EEPROM address on TWI bus.
		 ******************************************************************************/
Eeprom24C32_64::Eeprom24C32_64
(
	byte deviceAddress
) {
	m_deviceAddress = deviceAddress;
}

/**************************************************************************//**
 * \fn void Eeprom24C32_64::initialize()
 *
 * \brief Initialize library and TWI bus.
 *
 * If several devices are connected to TWI bus, this method mustn't be
 * called. TWI bus must be initialized out of this library using
 * Wire.begin() method.
 ******************************************************************************/
void
Eeprom24C32_64::initialize(int SDA, int SCL)
{
	Wire.begin(SDA, SCL);
}

/**************************************************************************//**
 * \fn void Eeprom24C32_64::writeByte(
 * word address,
 * byte data)
 *
 * \brief Write a byte in EEPROM memory.
 *
 * \remarks A delay of 10 ms is required after write cycle.
 *
 * \param   address Address.
 * \param   data    Byte to write.
 ******************************************************************************/
void
Eeprom24C32_64::writeByte
(
	word    address,
	byte    data
) {
	Wire.beginTransmission(m_deviceAddress);
	Wire.write(address >> 8);
	Wire.write(address & 0xFF);
	Wire.write(data);
	Wire.endTransmission();
	delay(10);
}

/**************************************************************************//**
 * \fn void Eeprom24C32_64::writeBytes(
 * word     address,
 * word     length,
 * byte*    p_data)
 *
 * \brief Write bytes in EEPROM memory.
 *
 * \param       address Start address.
 * \param       length  Number of bytes to write.
 * \param[in]   p_data  Bytes to write.
 ******************************************************************************/
void Eeprom24C32_64::writeBytes(word address, word length, byte* p_data) 
{
	// Write first page if not aligned.
	byte notAlignedLength = 0;
	byte pageOffset = address % EEPROM__PAGE_SIZE;
	if (pageOffset > 0)
	{
		notAlignedLength = EEPROM__PAGE_SIZE - pageOffset;
		if (length < notAlignedLength)
		{
			notAlignedLength = length;
		}
		writePage(address, notAlignedLength, p_data);
		length -= notAlignedLength;
	}

	if (length > 0)
	{
		address += notAlignedLength;
		p_data += notAlignedLength;

		// Write complete and aligned pages.
		byte pageCount = length / EEPROM__PAGE_SIZE;
		for (byte i = 0; i < pageCount; i++)
		{
			writePage(address, EEPROM__PAGE_SIZE, p_data);
			address += EEPROM__PAGE_SIZE;
			p_data += EEPROM__PAGE_SIZE;
			length -= EEPROM__PAGE_SIZE;
		}

		if (length > 0)
		{
			// Write remaining uncomplete page.
			writePage(address, length, p_data);
		}
	}
}

/**************************************************************************//**
 * \fn byte Eeprom24C32_64::readByte(word address)
 *
 * \brief Read a byte in EEPROM memory.
 *
 * \param   address Address.
 *
 * \return Read byte.
 ******************************************************************************/
byte Eeprom24C32_64::readByte(word address) 
{
	Wire.beginTransmission(m_deviceAddress);
	Wire.write(address >> 8);
	Wire.write(address & 0xFF);
	Wire.endTransmission();
	Wire.requestFrom(m_deviceAddress, (byte)1);
	byte data = 0;
	if (Wire.available())
	{
		data = Wire.read();
	}
	return data;
}

/**************************************************************************//**
 * \fn void Eeprom24C32_64::readBytes(
 * word     address,
 * word     length,
 * byte*    p_data)
 *
 * \brief Read bytes in EEPROM memory.
 *
 * \param       address Start address.
 * \param       length  Number of bytes to read.
 * \patam[in]   p_data  Byte array to fill with read bytes.
 ******************************************************************************/
void Eeprom24C32_64::readBytes(word    address,	word    length,	byte*   p_data) 
{
	byte bufferCount = length / EEPROM__RD_BUFFER_SIZE;
	for (byte i = 0; i < bufferCount; i++)
	{
		word offset = i * EEPROM__RD_BUFFER_SIZE;
		readBuffer(address + offset, EEPROM__RD_BUFFER_SIZE, p_data + offset);
	}

	byte remainingBytes = length % EEPROM__RD_BUFFER_SIZE;
	word offset = length - remainingBytes;
	readBuffer(address + offset, remainingBytes, p_data + offset);
}

// Writes an int variable at the specified address.
void Eeprom24C32_64::writeWord(word addr, word value) {
	byte *ptr;

	ptr = (byte*)&value;
	writeBytes(addr, sizeof(value), ptr);
}


//
// Reads an integer value at the specified address.
// Returns true if the variable is successfully read.
// Returns false if the specified address is outside the
// allowed range or too close to the maximum vlaue
// to hold all of the bytes (an int variable requires
// more than one byte).
//
//void Eeprom24C32_64::read_int(int addr, int* value) {
//	return eeprom_read_bytes(addr, (byte*)value, sizeof(int));
//}
word Eeprom24C32_64::readWord(word i) {
	byte l = readByte(i);
	byte h = readByte(i + 1);
	return word(h, l);
}


/******************************************************************************
 * Private method definitions.
 ******************************************************************************/

 /**************************************************************************//**
  * \fn void Eeprom24C32_64::writePage(
  * word     address,
  * byte     length,
  * byte*    p_data)
  *
  * \brief Write page in EEPROM memory.
  *
  * \param       address Start address.
  * \param       length  Number of bytes (EEPROM__PAGE_SIZE bytes max).
  * \param[in]   p_data  Data.
  ******************************************************************************/
void
Eeprom24C32_64::writePage
(
	word    address,
	byte    length,
	byte*   p_data
) {
	// Write complete buffers.
	byte bufferCount = length / EEPROM__WR_BUFFER_SIZE;
	for (byte i = 0; i < bufferCount; i++)
	{
		byte offset = i * EEPROM__WR_BUFFER_SIZE;
		writeBuffer(address + offset, EEPROM__WR_BUFFER_SIZE, p_data + offset);
	}

	// Write remaining bytes.
	byte remainingBytes = length % EEPROM__WR_BUFFER_SIZE;
	byte offset = length - remainingBytes;
	writeBuffer(address + offset, remainingBytes, p_data + offset);
}

/**************************************************************************//**
 * \fn void Eeprom24C32_64::writeBuffer(
 * word     address,
 * byte     length,
 * byte*    p_data)
 *
 * \brief Write bytes into memory.
 *
 * \param       address Start address.
 * \param       length  Number of bytes (EEPROM__WR_BUFFER_SIZE bytes max).
 * \param[in]   p_data  Data.
 ******************************************************************************/
void
Eeprom24C32_64::writeBuffer
(
	word    address,
	byte    length,
	byte*   p_data
) {
	Wire.beginTransmission(m_deviceAddress);
	Wire.write(address >> 8);
	Wire.write(address & 0xFF);
	for (byte i = 0; i < length; i++)
	{
		Wire.write(p_data[i]);
	}
	Wire.endTransmission();

	// Write cycle time (tWR). See EEPROM memory datasheet for more details.
	delay(10);
}

/**************************************************************************//**
 * \fn void Eeprom24C32_64::readBuffer(
 * word     address,
 * byte     length,
 * byte*    p_data)
 *
 * \brief Read bytes in memory.
 *
 * \param       address Start address.
 * \param       length  Number of bytes (EEPROM__RD_BUFFER_SIZE bytes max).
 * \param[in]   p_data  Buffer to fill with read bytes.
 ******************************************************************************/
void
Eeprom24C32_64::readBuffer
(
	word    address,
	byte    length,
	byte*   p_data
) {
	Wire.beginTransmission(m_deviceAddress);
	Wire.write(address >> 8);
	Wire.write(address & 0xFF);
	Wire.endTransmission();
	Wire.requestFrom(m_deviceAddress, length);
	for (byte i = 0; i < length; i++)
	{
		if (Wire.available())
		{
			p_data[i] = Wire.read();
		}
	}
}

