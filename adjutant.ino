/*
Name:       Adjutant.ino
Author:     Konrad R.K. Ludwig
*/

#include <Wire.h>

#include "libraries/SDS021/SDS021.h"
#include "libraries/MCP3425/MCP3425.h"

#include "SPEC.h"
#include "PSTAT.h"

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

void setup ()
{
	// Intialize UART connection
	Serial.begin(9600);
	while (!Serial); // Necessary for USB
	Serial.println("Hardware serial initialized.");

	// Initialize I2C connection
	Serial.print("Initializing I2C bus...");
	Wire.pins(I2CSDA_PIN, I2CSCL_PIN);
	Wire.begin();
	Serial.println("Done!");

	// Initialize software serial connection
	Serial.print("Initializing dust sensor...");
	dust.Begin();
	dust.PassiveMode(true);
	dust.Awake(false);
	dust.Update();
	Serial.println("Done!");

	// Initialize SPEC sensors
	Serial.print("Initializing PSTAT circuit...");
	gas.Begin();
	gas.ADC(true, MCP3425::EResolution::d16Bit, MCP3425::EGain::x1);
	gas.Configure(0, SPEC::CO);
	//gas.Configure(1, SPEC::O3);
	Serial.println("Done!");

	Serial.println("- - - - - - - - - -");
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
