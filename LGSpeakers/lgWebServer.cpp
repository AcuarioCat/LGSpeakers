// 
// 
// 

#include "lgWebServer.h"
#include <ESP8266SSDP.h>
#include <StreamString.h>
#include "LGSpeakers.h"
#include "debugWeb.h"

//SSDP properties
constexpr auto modelName = "LG Sound Bar";
constexpr auto friendlyName = "LG Sound Bar";
constexpr auto modelNumber = "";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocket wsd("/wsd");

void sendSettings();

/////////////////////////////////////  SSDP network discovery //////////////////////////////////////
void setupSSDP() {
	SSDP.setSchemaURL("description.xml");
	SSDP.setHTTPPort(80);
	SSDP.setDeviceType("upnp:rootdevice");
	SSDP.setModelName(modelName);
	SSDP.setModelNumber(modelNumber);
	SSDP.begin();

	server.on("/description.xml", HTTP_GET, [](AsyncWebServerRequest* request) {
		char hostName[25] = "LG Sound Bar";
		char buf[7];

		StreamString output;
		if (output.reserve(1024)) {
			IPAddress ip = WiFi.localIP();
			uint32_t chipId = ESP.getChipId();
			output.printf(ssdpTemplate,
				ip[0], ip[1], ip[2], ip[3],
				hostName,
				ip[0], ip[1], ip[2], ip[3],
				chipId,
				modelName,
				modelNumber,
				(uint8_t)((chipId >> 16) & 0xff),
				(uint8_t)((chipId >> 8) & 0xff),
				(uint8_t)chipId & 0xff
			);
			request->send(200, "text/xml", (String)output);
		}
		else {
			request->send(500);
		}
		});
}

void onRequest(AsyncWebServerRequest *request) {
	//Handle Unknown Request
	request->send(404);
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
	//Handle body
}

void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
	//Handle upload
}

void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
	//Handle WebSocket event
}


void setupWebServer()
{
	// Start and init debug winsocket
	ws.onEvent([](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len)
		{
			Serial.printf("wsd event: %d\n", type);
			if (type == WS_EVT_CONNECT)
			{
				//Serial.printf("wsd[%s][%u] connected\n", server->url(), client->id());
				client->ping();
			}
		});
	server.addHandler(&ws);

	// Start and init debug winsocket
	wsd.onEvent([](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len)
		{
			//Serial.printf("wsd event: %d\n", type);
			if (type == WS_EVT_CONNECT)
			{
				//Serial.printf("wsd[%s][%u] connected\n", server->url(), client->id());
				client->ping();
			}
		});
	server.addHandler(&wsd);

	server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send(SPIFFS, "/index.html");
		});

	server.on("/settings", HTTP_GET, [](AsyncWebServerRequest* request) {
		sendSettings();
		request->send(200);
		});

	server.on("/devReboot", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "text/plain", "Rebooting...");
		ss.shouldReboot = true;
	});

	server.on("/devReset", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "text/plain", "Reset and Rebooting...");
		WiFi.disconnect(true); 
	});


	server.on("/eq", HTTP_POST, [](AsyncWebServerRequest* request) {
		if (request->args() > 0)
		{
			uint8_t arr[] = { 00, 00, 0x0, 00, 00, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x20, 00, 0x0 };
			byte eqno;
			byte address;
			int setting;
			if ((request->hasArg("e")))
			{
				address = (int)request->arg("c").toInt();
				eqno = (int)request->arg("e").toInt();
				setting = (int)request->arg("v").toInt();
				DEBUG_PRINTF("eq %d, val %x\n", eqno, setting);

				LgSb.mute();

				//Write a single coefficient
				LgSb.write_register(AD835, 0x14, address);
				arr[eqno - 1] = setting;
				LgSb.write_register(AD835, 0x15, arr, sizeof(arr));
				LgSb.write_register(AD835, 0x24, 0x01);

				//write_register(AD835, 0x14, eqno);
				//write_register(AD835, 0x15, setting);
				//write_register(AD835, 0x16, setting<<8);
				//write_register(AD835, 0x17, setting<<16);
				//write_register(AD835, 0x24, 0x01);
				LgSb.setVolumeBT(AD_MASTERVOL);

				//arr[0] = setting;
				//arr[1] = setting << 8;
				//arr[2] = setting << 16;
				//write_register(AD835, 0x14, address);
				//write_register(AD835, 0x15, arr, sizeof(arr));
				//write_register(AD835, 0x24, 0x02);
			}
		}
		request->send(200);
		});

	server.on("/volume", HTTP_POST, [](AsyncWebServerRequest* request) {
		if (request->args() > 0)
		{
			byte volume;

			if ((request->hasArg("m")))
			{
				volume = (int)request->arg("m").toInt();
				LgSb.saveVolumeBT(AD_MASTERVOL, volume);
				DEBUG_PRINTF("Master Volume set to %d\n", volume);
			}
			if ((request->hasArg("l")))
			{
				volume = (int)request->arg("l").toInt();
				LgSb.saveVolumeBT(AD_CH1VOL, volume);
				DEBUG_PRINTF("Ch1 Volume set to %d\n", volume);
			}
			if ((request->hasArg("r")))
			{
				volume = (int)request->arg("r").toInt();
				LgSb.saveVolumeBT(AD_CH2VOL, volume);
				DEBUG_PRINTF("Ch2 Volume set to %d\n", volume);
			}
			if ((request->hasArg("w")))
			{
				volume = (int)request->arg("w").toInt();
				LgSb.saveVolumeBT(AD_CH3VOL, volume);
				DEBUG_PRINTF("Ch3 Volume set to %d\n", volume);
			}
			if ((request->hasArg("t")))
			{
				volume = (int)request->arg("t").toInt();
				LgSb.saveVolumeBT(AD_TREBLE, volume);
				DEBUG_PRINTF("Treble set to %d\n", volume);
			}
			if ((request->hasArg("b")))
			{
				volume = (int)request->arg("b").toInt();
				LgSb.saveVolumeBT(AD_BASS, volume);
				DEBUG_PRINTF("Bass set to %d\n", volume);
			}
		}
		request->send(200);
		});

	server.on("/setting", HTTP_POST, [](AsyncWebServerRequest* request) {
		if (request->args() > 0)
		{
			int setting;
			if ((request->hasArg("m")))
			{
				setting = (int)request->arg("m").toInt();
				if(setting == 1)
					LgSb.mute();
				else
					LgSb.unMute();
			}
			if ((request->hasArg("s")))
			{
				setting = (int)request->arg("s").toInt();
				LgSb.write_register(AD835, AD_STATEC4, setting);
				LgSb.saveSetting(E_STATEC4, setting);
				DEBUG_PRINTF("State control set to %x\n", setting);
			}
			if ((request->hasArg("r")))
			{
				setting = (int)request->arg("r").toInt();
				if (setting == 1)
					ss.bReset = true;
				if(setting == 2)
					ss.shouldReboot = true;
				DEBUG_PRINTLN("Reset to default");
			}
		}
		request->send(200);
		});

	//Serve all files. 
	server.serveStatic("/", SPIFFS, "/", "max-age=2592000");

	// Catch-All Handlers
	// Any request that can not find a Handler that can Handle it ends in the callbacks below.
	server.onNotFound(onRequest);
	server.onFileUpload(onUpload);
	server.onRequestBody(onBody);
	setupSSDP();
	server.begin();
}

//Return a string with the system parameters as a json encapsulated in a function for processing on the browser
void sendSettings() {
	const size_t capacity = JSON_OBJECT_SIZE(80);
	DynamicJsonDocument root(capacity);

	// Add values in the object
	root["master"] = LgSb.getVolumeBT(AD_MASTERVOL);
	root["ch1"] = LgSb.getVolumeBT(AD_CH1VOL);
	root["ch2"] = LgSb.getVolumeBT(AD_CH2VOL);
	root["ch3"] = LgSb.getVolumeBT(AD_CH3VOL);
	root["bass"] = LgSb.getVolumeBT(AD_BASS);
	root["treble"] = LgSb.getVolumeBT(AD_TREBLE);
	root["stateC4"] = eepromUtil.eeprom_read_byte(E_STATEC4);

	String jsonString;
	serializeJson(root, jsonString);
	ws.textAll(jsonString);
}


void sendWsMessage(String message, String key) {
	String jsonString;
	// Add values in the object
	const size_t capacity = JSON_OBJECT_SIZE(4);
	DynamicJsonDocument root(capacity);
	root[key] = message;
	serializeJson(root, jsonString);
	//Serial.println(jsonString);
	ws.textAll(jsonString);
}

void setupOTAUpdate(void)
{
	ArduinoOTA.setPassword((const char *)"<your password for OTA>");

	ArduinoOTA.onStart([]() {
		//LcdDisplay.setBacklight(true);
		//LcdDisplay.clear(true);
		DEBUG_PRINTLN(G("Start update"));
	});
	ArduinoOTA.onEnd([]() {
		DEBUG_PRINTLN("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		char res[20];
		unsigned int percent = progress / (total / 100);
		snprintf(res, sizeof(res), "Progress: %u%%", (progress / (total / 100)));
		if (percent % 10 == 0) {		//Only every 10% on LCD
			//LcdDisplay.setLine(1, 20, String(res));
			//LcdDisplay.displayLCD(1);			//Update LCD
			DEBUG_PRINTLN(res);
		}
	});
	ArduinoOTA.onError([](ota_error_t error) {
		DEBUG_PRINTF("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) DEBUG_PRINTLN(G("Auth Failed"));
		else if (error == OTA_BEGIN_ERROR) DEBUG_PRINTLN(G("Begin Failed"));
		else if (error == OTA_CONNECT_ERROR) DEBUG_PRINTLN(G("Connect Failed"));
		else if (error == OTA_RECEIVE_ERROR) DEBUG_PRINTLN(G("Receive Failed"));
		else if (error == OTA_END_ERROR) DEBUG_PRINTLN(G("End Failed"));
	});
	ArduinoOTA.begin();
}


