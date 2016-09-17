#include "OTA.h"

OTA::OTA() {}

OTA::~OTA() {}

bool OTA::Connect()
{
    WiFi.begin(ssid, password);
	Serial.println("");
	Serial.println("Connecting To WiFi For OTA Update");
	Serial.println("");

	// Wait for connection
	for(unsigned int i = 0; i < retry_timeout_attempts; i++) {
	    if(WiFi.status() == WL_CONNECTED) {
	        Serial.println("");
            Serial.print("Connected to ");
            Serial.println(ssid);
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            return true;
	    }
	    Serial.print (".");
	    delay(500);
	}

	Serial.println("");
    Serial.print("Connection failed.");
    Serial.println("");

    return false;
}

bool OTA::Connect(const char *_ssid, const char *_password)
{
    ssid = (char*)_ssid;
    password = (char*)_password;

    return OTA::Connect();
}

void OTA::OtaUpdate()
{
    t_httpUpdate_return ret = ESPhttpUpdate.update(ota_ip, ota_port, ota_file, "");
        switch(ret) {
        case HTTP_UPDATE_FAILED:
            Serial.println("[update] Update failed.");
            Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            Serial.println();
            Serial.println();
            Serial.println();
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("[update] Update no Update.");
            break;
        case HTTP_UPDATE_OK:
            Serial.println("[update] Update ok."); // may not called we reboot the ESP
            break;
    }
}

void OTA::OtaUpdate(const char *_ota_ip, const uint16_t _ota_port, const char *_ota_file)
{
    ota_ip = (char*)_ota_ip;
    ota_port = _ota_port;
    ota_file = (char*)_ota_file;
    OTA::OtaUpdate();
}