// lgSoundBar.h

#ifndef _LGSOUNDBAR_h
#define _LGSOUNDBAR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "SystemState.h"
#include "debugWeb.h"

#define AD835			0x6a
#define AD_MASTERVOL	0x03		//Write 0x72 to 0x03 and 0x04 for mute, restore value to 0x04, write 0x08 to 0x03 to unmute
//Power off, write 0x72 to 0x03 and 0x04
//Power on, write 0x08 to 0x03, write 0x1e to 0x04
#define AD_CH1VOL		0x04		//Volume - Steps 2 per increment (0x1a, 0x1c ...) higher, quieter
#define AD_CH2VOL		0x05		//Volume - Steps 2 per increment (0x1a, 0x1c ...) higher, quieter
#define AD_CH3VOL		0x06		//Volume - Steps 2 per increment (0x1a, 0x1c ...) higher, quieter
#define AD_BASS			0x07		//Bass/treble tone boost and cut (Depends on state control 4 bit 6)
#define AD_TREBLE		0x08
#define AD_BASSMGMT		0x09		//Bass management
#define AD_STATEC4		0x0a		//State control 4

class LgSoundBarClass
{
 protected:


 public:
	void init();
	void mute();
	void unMute();
	void setDefault(void);
	void saveSetting(uint8_t setting, uint8_t value);
	uint8_t readSetting(uint8_t setting);

	void saveVolumeBT(uint8_t channel, uint8_t value);
	uint8_t setVolumeBT(uint8_t channel);
	uint8_t getVolumeBT(uint8_t channel);


//private:
	uint8_t write_register(uint8_t m_address, uint8_t reg, uint8_t val);
	uint8_t write_register(uint8_t m_address, uint8_t reg, const uint8_t* val, size_t len);
	uint8_t read_register(uint8_t m_address, char reg);
	uint8_t read_register(uint8_t m_address, int reg);
};

extern LgSoundBarClass LgSb;

#endif

