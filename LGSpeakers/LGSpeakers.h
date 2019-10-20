#ifndef LGSPEAKERS_H
#define	LGSPEAKERS_H

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

#define EEPR	0xa0
#define E_VOL1	0x1f0e
#define E_VOL2	0x1f28

//Non volatile memory storage
#define E_INIT			500			//EEPROM initialised flag
#define E_MASTERVOL		0x00		//Master volume
#define E_CH1VOL		0x01		//Channel 1 volume
#define E_CH2VOL		0x02		//Channel 2 volume
#define E_CH3VOL		0x03		//Channel 3 volume
#define E_BASS			0x04		//Bass setting
#define E_TREBLE		0x05		//Treble setting
#define E_STATEC4		0x06		//State control 4

constexpr auto OSWATCH_RESET_TIME = 300;

static const char* ssdpTemplate =
"<?xml version=\"1.0\"?>"
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
"<specVersion>"
"<major>1</major>"
"<minor>0</minor>"
"</specVersion>"
"<URLBase>http://%d.%d.%d.%d:80/description.xml</URLBase>"
"<device>"
"<deviceType>urn:schemas-upnp-org:device:BinaryLight:1</deviceType>"
"<friendlyName>%s</friendlyName>"
"<presentationURL>http://%d.%d.%d.%d</presentationURL>"
"<serialNumber>%u</serialNumber>"
"<modelName>%s</modelName>"
"<modelNumber>%s</modelNumber>"
"<modelURL></modelURL>"
"<manufacturer>NTP</manufacturer>"
"<manufacturerURL></manufacturerURL>"
"<UDN>uuid:38323636-4558-4dda-9100-cda0e6%02x%02x%02x</UDN>"
"</device>"
"</root>\r\n"
"\r\n";

#endif	/* LGSPEAKERS_H */
