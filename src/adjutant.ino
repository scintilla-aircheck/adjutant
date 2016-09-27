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
#include "reading.pb.h"

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
SDS021 dust = SDS021();
PSTAT gas = PSTAT(PSTAT0_PIN, PSTAT1_PIN, PSTAT2_PIN, MENB_PIN);
//Components::SKM61 gps = SKM61();

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

	uint32_t realSize = ESP.getFlashChipRealSize();
    uint32_t ideSize = ESP.getFlashChipSize();
    FlashMode_t ideMode = ESP.getFlashChipMode();

    Serial.print("Flash real id:   ");
    Serial.println(ESP.getFlashChipId());
    Serial.print("Flash real size: ");
    Serial.println(realSize);

    Serial.print("Flash ide size:  ");
    Serial.println(ideSize);
    //Serial.printf("Flash ide speed: %u\n", ESP.getFlashChipSpeed());
    //Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

    if(ideSize != realSize) {
        Serial.println("Flash Chip configuration wrong!\n");
    } else {
        Serial.println("Flash Chip configuration ok.\n");
    }

	Serial.println("Hardware serial initialized.\n");

	// Initialize I2C connection
	Serial.print("Initializing I2C bus...");
	Wire.pins(I2CSDA_PIN, I2CSCL_PIN);
	Wire.begin();
	Serial.println("Done!\n");

    /*
	// Initialize software serial connection
	Serial.print("Initializing dust sensor...");
	dust.Begin();
	dust.PassiveMode(true);
	dust.Awake(false);
	dust.Update();
	Serial.println("Done!\n");
	*/

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
	size_t message_length;
	int buffer_length = ReadingGroupMessage_size;
	uint8_t write_buffer[buffer_length];
	Serial.print("sizeof(write_buffer): ");
	Serial.println(sizeof(write_buffer));
	uint8_t read_buffer[buffer_length];
	//ReadingMessage message;
	ReadingMessage messages[3];
	//messages[0] = message;
	ReadingGroupMessage group_message = ReadingGroupMessage_init_zero;
	char filename[9] = "test.txt";

	data.BuildReading(messages[0], 1, 1.0, 60, 1.1, 1.2, 2, 3);
	data.BuildReading(messages[1], 2, 2.0, 120, 2.1, 2.2, 4, 6);
	data.BuildReading(messages[2], 3, 3.0, 180, 3.1, 3.2, 6, 9);

    Serial.println("setup::");
    data.PrintReading(messages[0]);
    Serial.println("setup::");
    data.PrintReading(messages[1]);
    Serial.println("setup::");
    data.PrintReading(messages[2]);

	data.BuildReadingGroup(group_message, messages, 3);

	//data.SerializeReading(write_buffer, buffer_length, message_length, message);

	data.SerializeReadingGroup(write_buffer, buffer_length, message_length, group_message);
	//data.DeserializeReading(message, message_length, write_buffer);
	data.DeserializeReadingGroup(group_message, message_length, write_buffer);
	data.Write(filename, write_buffer, message_length);
	data.Read(filename, read_buffer, message_length);
    //data.DeserializeReading(message, message_length, read_buffer);
    data.DeserializeReadingGroup(group_message, message_length, read_buffer);

	Serial.println("Done!\n");

	Serial.println("- - - - - - - - - -\n\n");
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

	Serial.print("VOUT: ");
	Serial.println(gas.ADC(), 4);

	delay(5 * 1000);

	Serial.println("- - - - - - - - - -");
}
