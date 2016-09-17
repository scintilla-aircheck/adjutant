/*
Name:       Adjutant.ino
Author:     Konrad R.K. Ludwig
*/

#include <Wire.h>

#include "SDS021.h"
#include "MCP3425.h"
#include "pb_common.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "pb.h"
#include "sensor_data.pb.h"

#include "SPEC.h"
#include "PSTAT.h"
#include "OTA.h"
#include "data.h"

// I2C pins
#define I2CSDA_PIN 2
#define I2CSCL_PIN 0

// UART circuit controls
#define SSRX_PIN 14
#define SSTX_PIN 12

// Gas sensor circuit controls
#define PSTAT0_PIN 4
#define PSTAT1_PIN 5
#define PSTAT2_PIN 13
#define MENB_PIN 16

// Sensors and components
SDS021 dust = SDS021(SSRX_PIN, SSTX_PIN);
PSTAT gas = PSTAT(PSTAT0_PIN, PSTAT1_PIN, PSTAT2_PIN, MENB_PIN);
//Components::SKM61 gps = SKM61(SSRX_PIN, SSTX_PIN);

// OTA
OTA ota = OTA();

// Data
Data data = Data();

void setup ()
{
	// Intialize UART connection
	Serial.begin(9600);
	while (!Serial); // Necessary for USB
	delay(1000);
	Serial.println("Hardware serial initialized.\n");

	// Initialize I2C connection
	Serial.print("Initializing I2C bus...");
	Wire.pins(I2CSDA_PIN, I2CSCL_PIN);
	Wire.begin();
	Serial.println("Done!\n");

	// Initialize software serial connection
	Serial.print("Initializing dust sensor...");
	dust.Begin();
	dust.PassiveMode(true);
	dust.Awake(false);
	dust.Update();
	Serial.println("Done!\n");

	// Initialize SPEC sensors
	Serial.print("Initializing PSTAT circuit...");
	gas.Begin();
	gas.ADC(true, MCP3425::EResolution::d16Bit, MCP3425::EGain::x1);
	gas.Configure(0, SPEC::CO);
	//gas.Configure(1, SPEC::O3);
	Serial.println("Done!\n");

	// Initialize OTA
	Serial.print("Initializing OTA...");
	if(ota.Connect("DLab", "endmill1")) {
	    // Attempt OTA Update
        Serial.print("\tInitializing OTA Update...");
        ota.OtaUpdate("192.168.0.1", 1337, "/example.bin");
        Serial.println("\tDone!");
	}
	Serial.println("Done!\n");

	// Test Data Serialization and Deserialization
	Serial.println("Initializing Data Serialization and Deserialization...");
	data.Write(23);
	Serial.println("Done!\n");

	Serial.println("- - - - - - - - - -\n\n");
}

void loop ()
{
	dust.Awake(true);

	delay(5 * 1000);

	dust.Query();

	delay(1000);

	if (dust.Update())
	{
		Serial.print("PM 2.5: ");
		Serial.println(dust.PM2_5(), 1);

		Serial.print("PM 10: ");
		Serial.println(dust.PM10(), 1);
	}

	delay(1000);

	dust.Awake(false);

	Serial.print("VOUT: ");
	Serial.println(gas.ADC(), 4);

	delay(5 * 1000);

	Serial.println("- - - - - - - - - -");
}
