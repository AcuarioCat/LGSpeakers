/*
 Name:		LGSpeakers.ino
 Created:	10/9/2019 4:23:31 PM
 Author:	Nigel Morton
*/
#include "lgSoundBar.h"
#include <Arduino.h>
#include <Wire.h>
#include <Ticker.h>
#include "LGSpeakers.h"
#include "lgWebServer.h"
#include "SystemState.h"
#include "lgSoundBar.h"
#include "debugWeb.h"

const char* ssid = "Terrapico";
const char* password = "5432167890";
const char* hostName = "LGSoundBar";

unsigned long last_loop;
Ticker tickerOSWatch, tickerReset;

//When power off/on values aren't reestablished so force a reset of the values
void resetVolumes(void) { ss.bResetVol = true; ss.bReset = true;
}

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);

	EEPROM.begin(512);
	ss.readSettings();

	WiFi.mode(WIFI_STA);	//WIFI_STA
	WiFi.hostname(hostName);
	WiFi.begin(ssid, password);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.printf("WiFi Failed!\n");
		debugWeb.wifiActive = false;
		return;
	}
	DEBUG_PRINTLN("WiFi connected!");
	Serial.printf("%d.%d.%d.%d\n", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
	debugWeb.wifiActive = true;

	SPIFFS.begin();
	DEBUG_PRINTLN("SPIFFS started");
	setupWebServer();
	setupOTAUpdate();

	Wire.begin(13, 12);       //Begins I2C communication at pin (SDA A4, SCL A5)
	LgSb.setDefault();

	//uint8_t res = read_register(EEPR, E_VOL1);
	//DEBUG_PRINTF("Vol 1: %d\n", res); 
	//res = read_register(EEPR, E_VOL2);
	//DEBUG_PRINTF("Vol 2: %d\n", res);
	//Set up watchdog timer

	DEBUG_PRINTLN(G("Setup watchdog"));
	last_loop = millis();
	tickerOSWatch.attach_ms(((OSWATCH_RESET_TIME / 3) * 1000), osWatch);
	tickerReset.attach(1, resetVolumes);
}

// the loop function runs over and over again until power down or reset
void loop() {
	ArduinoOTA.handle();

	last_loop = millis();
	if (ss.bReset && !ss.bMute) {
		LgSb.setDefault();
		ss.bReset = false;
		ss.bResetVol = false;
	}

	if (ss.shouldReboot) {
		DEBUG_PRINTLN("Rebooting...");
		delay(100);
		ESP.restart();
	}
	
	delay(100);
}

///////////////////////////////////////////////////////////////////////////////////
// Heartbeat task - resetting the watchdog timer periodically 
void ICACHE_RAM_ATTR osWatch(void) {
	unsigned long t = millis();
	unsigned long last_run = abs(t - last_loop);
	if (last_run >= (OSWATCH_RESET_TIME * 1000)) {
		// save the hit here to eeprom or to rtc memory if needed
		ESP.restart();  // normal reboot 
		//ESP.reset();  // hard reset
	}
}
