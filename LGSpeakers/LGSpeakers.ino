/*
 Name:		LGSpeakers.ino
 Created:	10/9/2019 4:23:31 PM
 Author:	Nigel Morton
*/
#include "lgSoundBar.h"
#include <Arduino.h>
#include <Wire.h>
#include "LGSpeakers.h"
#include "lgWebServer.h"
#include "SystemState.h"
#include "lgSoundBar.h"
#include "debugWeb.h"

const char* ssid = "Terrapico";
const char* password = "5432167890";
const char* hostName = "LGSoundBar";

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
}

// the loop function runs over and over again until power down or reset
void loop() {
	ArduinoOTA.handle();

	if (ss.bReset) {
		LgSb.setDefault();
		ss.bReset = false;
	}

	if (ss.shouldReboot) {
		DEBUG_PRINTLN("Rebooting...");
		delay(100);
		ESP.restart();
	}
	
	delay(100);
}
