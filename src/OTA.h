#pragma once

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WiFi.h>

class OTA
{
public:
	OTA();
	~OTA();

	// Connect to WiFi for OTA Update
	bool Connect();
	bool Connect(const char *_ssid, const char *_password);

    // Attempt to update OTA
    void OtaUpdate();
    void OtaUpdate(const char *_ota_ip, const uint16_t _ota_port, const char *_ota_file);

private:
	char *ssid = (char *)"ssid";
    char *password = (char *)"password";
    char *ota_ip = (char *)"192.168.0.1";
    uint16_t ota_port = 1337;
    char *ota_file = (char *)"/example.bin";
    unsigned int retry_timeout_attempts = 5; // number of attempts to connect to WiFi before giving up
};
