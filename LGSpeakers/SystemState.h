// SystemState.h

#ifndef _SYSTEMSTATE_h
#define _SYSTEMSTATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif
#include <EEPROM.h>
#include "EepromUtil.h"
#include "debugWeb.h"

class SystemState
{
  public:
	void saveSettings();
	void readSettings();
	void resetSettings();

	bool bReset = false;
	static unsigned long last_loop;

	//Dynamic variables
	bool shouldReboot = false;
	bool bMute = false;
	bool bResetVol = false;

	//LG Sound bar values
	byte masterVolume = 0;
	byte ch1Volume = 0;
	byte ch2Volume = 0;
	byte ch3Volume = 0;
	byte stateC4 = 0;
};

extern SystemState ss;

#endif

