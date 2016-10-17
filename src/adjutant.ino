/*
Name:       Adjutant.ino
Author:     Konrad R.K. Ludwig
*/

// Arduino libraries
#include <Wire.h>

// 3rd party libraries
#include "pb_common.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "pb.h"
#include "reading.pb.h"

// Scintilla libraries
#include "MCP3425.h"
#include "LMP91000.h"
#include "SDS021.h"

// Adjutant libraries
#include "OTA.h"
#include "data.h"
#include "SPEC.h"
#include "PSTAT.h"

// I2C pins
#define SCL 0
#define SDA 2

// UART circuit controls
#define UART_INT 10
#define UART_EXT 15

// Gas sensor circuit controls
#define PSTAT_0 14
#define PSTAT_1 12
#define PSTAT_2 13
#define MENB 16

// Sensors and components
UARTMux uart = UARTMux(UART_EXT, UART_INT);
SDS021 dust = SDS021();
//Components::SKM61 gps = SKM61();
PSTAT gas = PSTAT(PSTAT_0, PSTAT_1, PSTAT_2, MENB);

void setup ()
{
	// Intialize UART connection
	Serial.begin(9600);
	while (!Serial); // Necessary for USB
	Serial.println("Hardware serial initialized!\n");

	// Initialize I2C connection
	Serial.print("Initializing I2C bus...");
	Wire.pins(SDA, SCL);
	Wire.begin();
	Serial.println(F("Done!\n"));

	// Initialize SPEC sensors
	Serial.println("Initializing PSTAT circuits...");
	gas.Begin();

	Serial.print("\tADC...");
	gas.ADC(true, MCP3425::EResolution::d16Bit, MCP3425::EGain::x1);
	Serial.println("Done!");

	Serial.print("\tSensors: ");
	Serial.print("CO, ");
	gas.Configure(0, SPEC::CO);
	Serial.print("O3, ");
	gas.Configure(1, SPEC::O3);
	Serial.print("NO2, ");
	gas.Configure(2, SPEC::NO2);
	Serial.print("SO2, ");
	gas.Configure(3, SPEC::SO2);
	Serial.print("H2S...");
	gas.Configure(4, SPEC::H2S);
	Serial.println("Done!\n");

	// Initialize SDS021 dust sensor

	Serial.print("Initializing SDS021 dust sensor...")
	uart.Select(UARTMux::ETarget::SDS021);

	dust.PassiveMode(true);
	dust.Awake(false);

	uart.Select(UARTMux::ETarget::FTDI);
	Serial.println("Done!");

	Serial.println(F("- - - - - - - - - -\n\n"));

}

void loop ()
{
    /*
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
    */

	/*Serial.print(F("VOUT: "));
	Serial.println(gas.ADC(), 4);*/

	Serial.println(F("- - - - - - - - - -"));

	// Wait 5 seconds
	delay(5 * 1000);

}
