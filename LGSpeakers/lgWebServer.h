// pcsWebServer.h

#ifndef _PCSWEBSERVER_h
#define _PCSWEBSERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

//#define DEBUG_ESP_OTA 1
#include <ArduinoJson.h>
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include <ArduinoOTA.h>
#include "SystemState.h"
#include "lgSoundBar.h"
#include "debugWeb.h"

void setupOTAUpdate(void);
void setupWebServer();
String buildSettingsJson(bool);

#endif

