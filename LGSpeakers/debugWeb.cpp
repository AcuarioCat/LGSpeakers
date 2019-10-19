#include "debugWeb.h"

extern AsyncWebSocket wsd;

void DebugWeb::sendWeb(const char* data)
{
	//char temp[10];
	char* debugString;
	debugString = (char*)malloc(strlen(data) + 50);

	sprintf(debugString, "[%d] %s", ESP.getFreeHeap(), data);
	Serial.print(debugString);
	if (wifiActive == true) {
		wsd.textAll(debugString);
	}
	free(debugString);
}
void DebugWeb::sendWebln(const char* data)
{
	//char temp[10];
	char* debugString;
	debugString = (char*)malloc(strlen(data) + 50);

	//ntpTime.toCharArray(temp, 9);
	sprintf(debugString, "[%d] %s", ESP.getFreeHeap(), data);
	Serial.println(debugString);
	if (wifiActive == true) {
		wsd.textAll(debugString);
	}
	//delay(300);
	free(debugString);
}

DebugWeb debugWeb;
