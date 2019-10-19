// 
// 
// 
#include "SystemState.h"
#include "LGSpeakers.h"

EepromUtil eepromUtil;

void SystemState::readSettings(){
	char init[5];

	DEBUG_PRINTLN("Read settings");
	EEPROM.begin(512);
	eepromUtil.eeprom_read_string(E_INIT, init, 5);
	if (strcmp(init, "INIT") != 0) {
		resetSettings();
	}
}

void  SystemState::saveSettings() {
	EEPROM.commit();
}

void SystemState::resetSettings() {

	//EEPROM not initialised so set default values
	DEBUG_PRINTLN("Set EEPROM values to default");
	eepromUtil.eeprom_erase_all(0x00);
	eepromUtil.eeprom_write_byte(E_MASTERVOL, 8);
	eepromUtil.eeprom_write_byte(E_CH1VOL, 0x12);
	eepromUtil.eeprom_write_byte(E_CH2VOL, 0x12);
	eepromUtil.eeprom_write_byte(E_CH3VOL, 0x12);
	eepromUtil.eeprom_write_byte(E_BASS, 0x0f);
	eepromUtil.eeprom_write_byte(E_TREBLE, 0x0f);
	EEPROM.commit();
}

SystemState ss;
