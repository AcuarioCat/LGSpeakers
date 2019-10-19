#ifndef _DEBUGWEB_h
#define _DEBUGWEB_h
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include "ESPAsyncWebServer.h"

extern String ntpTime;

#define G(string_literal)  (String(F(string_literal)).c_str())			//Macro to allow use of Flash for strings that can be passed to functions
#define DEBUG_PRINT(x)  debugWeb.sendWeb (x)
#define DEBUG_PRINTF(x,...)  debugWeb.sendWebF (x,__VA_ARGS__)
#define DEBUG_PRINTF(x,...)  {char buf[128]; sprintf(buf, x,__VA_ARGS__); debugWeb.sendWeb(buf);}
#define DEBUG_PRINTLN(x)  debugWeb.sendWebln (x)

#ifdef ESP8266
#define __builtin_va_start
#define __builtin_va_end
#endif

class DebugWeb
{
public:
	bool wifiActive = false;
	void sendWeb(const char* data);
	void sendWebln(const char* data);
};

extern DebugWeb debugWeb;
#endif