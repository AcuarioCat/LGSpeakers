// 
// 
// 

#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include "EepromUtil.h"
#include "lgSoundBar.h"
#include "LGSpeakers.h"

void LgSoundBarClass::init()
{
}

//Save volume, bass, treble value in eprom and in register
void LgSoundBarClass::saveVolumeBT(uint8_t channel, uint8_t value) {
	byte epr;

	switch (channel) {
		case AD_MASTERVOL:
			epr = E_MASTERVOL;
			break;
		case AD_CH1VOL:
			epr = E_CH1VOL;
			break;
		case AD_CH2VOL:
			epr = E_CH2VOL;
			break;
		case AD_CH3VOL:
			epr = E_CH3VOL;
			break;
		case AD_BASS:
			epr = E_BASS;
			break;
		case AD_TREBLE:
			epr = E_TREBLE;
			break;
	}
	saveSetting(epr, value);
	write_register(AD835, channel, value);
}

//Read volume setting from eprom
uint8_t LgSoundBarClass::setVolumeBT(uint8_t channel) {
	byte epr;
	byte value;

	switch (channel) {
	case AD_MASTERVOL:
		epr = E_MASTERVOL;
		break;
	case AD_CH1VOL:
		epr = E_CH1VOL;
		break;
	case AD_CH2VOL:
		epr = E_CH2VOL;
		break;
	case AD_CH3VOL:
		epr = E_CH3VOL;
		break;
	case AD_BASS:
		epr = E_BASS;
		break;
	case AD_TREBLE:
		epr = E_TREBLE;
		break;
	}
	value = readSetting(epr);
	write_register(AD835, channel, value);
	DEBUG_PRINTF("setVolumeBT Channel:%d Setting:%d\n", channel, value);

	return value;
}

//Read volume setting from eprom
uint8_t LgSoundBarClass::getVolumeBT(uint8_t channel) {
	byte epr;

	switch (channel) {
	case AD_MASTERVOL:
		epr = E_MASTERVOL;
		break;
	case AD_CH1VOL:
		epr = E_CH1VOL;
		break;
	case AD_CH2VOL:
		epr = E_CH2VOL;
		break;
	case AD_CH3VOL:
		epr = E_CH3VOL;
		break;
	case AD_BASS:
		epr = E_BASS;
		break;
	case AD_TREBLE:
		epr = E_TREBLE;
		break;
	}
	return readSetting(epr);
}

void LgSoundBarClass::saveSetting(uint8_t setting, uint8_t value) {
	eepromUtil.eeprom_write_byte(setting, value);
	EEPROM.commit();
}

uint8_t LgSoundBarClass::readSetting(uint8_t setting) {
	return eepromUtil.eeprom_read_byte(setting);
}

void LgSoundBarClass::mute() {
	uint8_t bitsWritten;

	//Mute
	bitsWritten = write_register(AD835, AD_MASTERVOL, 0x72);
	//bitsWritten = write_register(AD835, AD_CH1VOL, 0x72);
}

void LgSoundBarClass::unMute() {
	uint8_t bitsWritten;
	uint8_t setting;

	//Unmute
	setting = readSetting(AD_MASTERVOL);
	bitsWritten = write_register(AD835, AD_MASTERVOL, setting);
	//bitsWritten = write_register(AD835, AD_CH1VOL, 0x12);
}

void LgSoundBarClass::setDefault(void) {
	uint8_t arr[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x20, 0, 0 };
	uint8_t bitsWritten;
	uint8_t setting;

	//Mute while applying settings
	mute();

	//Reset eq to default
	uint8_t arrx[] = { 0x4a, 0x53, 0x67, 0x2f, 0x3b, 0x0f, 0, 0, 0, 0, 0, 0, 0x20, 0, 0 };
	
	//Reset CH1 EQ1,2,3 to default values (they are set by LG to low frequency filters)
	bitsWritten = write_register(AD835, 0x14, 0);
	write_register(AD835, 0x15, arrx, sizeof(arr));
	bitsWritten = write_register(AD835, 0x24, 0x02);

	bitsWritten = write_register(AD835, 0x14, 5);
	write_register(AD835, 0x15, arr, sizeof(arr));
	bitsWritten = write_register(AD835, 0x24, 0x02);

	bitsWritten = write_register(AD835, 0x14, 0x0a);
	write_register(AD835, 0x15, arr, sizeof(arr));
	bitsWritten = write_register(AD835, 0x24, 0x02);

	//These 3 settings are taken from the original LG settings applied to CH1
	uint8_t arr1[] = { 00, 00, 0x90, 00, 00, 0x48, 0x3f, 0x76, 0x28, 0xe0, 0x88, 0xb7, 00, 00, 0x48 };
	bitsWritten = write_register(AD835, 0x14, 0x28);		//CH3EQ1
	write_register(AD835, 0x15, arr1, sizeof(arr1));
	bitsWritten = write_register(AD835, 0x24, 0x02);

	uint8_t arr2[] = { 0xc0, 0x33, 0x9f, 0x1f, 0xc2, 0x01, 0x3f, 0xcc, 0x61, 0xe0, 0x33, 0x65, 0x20, 0x0a, 0x99 };
	bitsWritten = write_register(AD835, 0x14, 0x5a);		//CH3EQ2
	write_register(AD835, 0x15, arr2, sizeof(arr2));
	bitsWritten = write_register(AD835, 0x24, 0x02);

	uint8_t arr3[] = { 0xc0, 0x26, 0x41, 0x1f, 0xec, 0xdf, 0x3f, 0xd9, 0xb4, 0xe0, 0x26, 0x35, 0x1f, 0xec, 0xdf };
	bitsWritten = write_register(AD835, 0x14, 0x2d);		//CH3EQ3
	write_register(AD835, 0x15, arr2, sizeof(arr2));
	bitsWritten = write_register(AD835, 0x24, 0x02);

	//Set Bass Crossover to 400Hz
	bitsWritten = write_register(AD835, AD_BASSMGMT, 0x08);

	//Set state control 4 register
	ss.stateC4 = eepromUtil.eeprom_read_byte(E_STATEC4);
	DEBUG_PRINTF("State control 4:%x\n", ss.stateC4);
	bitsWritten = write_register(AD835, AD_STATEC4, ss.stateC4);

	//Set Bass and Treble to values saved in eeprom
	setVolumeBT(AD_BASS);
	setVolumeBT(AD_TREBLE);

	//Set volumes to values saved in eeprom
	setVolumeBT(AD_CH1VOL);
	setVolumeBT(AD_CH2VOL);
	setVolumeBT(AD_CH3VOL);
	setting = setVolumeBT(AD_MASTERVOL);
	DEBUG_PRINTF("Mastervol:%d\n", setting);

	//Unmute
	unMute();
}

////////////           Private functions           //////////////////////////////////////////////////
uint8_t LgSoundBarClass::write_register(uint8_t m_address, uint8_t reg, uint8_t val)
{
	Wire.beginTransmission(m_address / 2);
	Wire.write(reg);
	Wire.write(val);
	return Wire.endTransmission();
}


uint8_t LgSoundBarClass::write_register(uint8_t m_address, uint8_t reg, const uint8_t* val, size_t len)
{
	Wire.beginTransmission(m_address / 2);
	Wire.write(reg);
	Wire.write(val, len);
	return Wire.endTransmission();
}

uint8_t LgSoundBarClass::read_register(uint8_t m_address, char reg)
{
	uint8_t res;
	uint8_t address = m_address / 2;
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(reg << 8);
	Wire.requestFrom((int)address, 1);
	res = Wire.read();
	return res;
}

uint8_t LgSoundBarClass::read_register(uint8_t m_address, int reg)
{
	uint8_t res;
	uint8_t address = m_address / 2;
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(reg << 8);
	Wire.requestFrom((int)address, 1);
	res = Wire.read();
	return res;
}

LgSoundBarClass LgSb;

