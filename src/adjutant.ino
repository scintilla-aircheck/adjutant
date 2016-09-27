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
    Serial.printf("Flash ide speed: %u\n", ESP.getFlashChipSpeed());
    Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

    if(ideSize != realSize) {
        Serial.println("Flash Chip configuration wrong!\n");
    } else {
        Serial.println("Flash Chip configuration ok.\n");
    }

    data.PrintSPIFFSFiles();

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

    int num_messages = 10;
    char filename[9] = "test.txt";
    int buffer_length = ReadingGroupMessage_size;

	size_t message_length;
	uint8_t buffer[buffer_length];
	//uint8_t read_buffer[buffer_length];

	ReadingMessage *messages = new ReadingMessage[num_messages];
	ReadingGroupMessage group_message = ReadingGroupMessage_init_zero;

	Serial.print("sizeof(buffer): ");
	Serial.println(sizeof(buffer));

    for(int i = 0; i < num_messages; i++) {
        data.BuildReading(messages[i], 1 * (i+1), 1.0 * (i+1), 60 * (i+1), 1.1 * (i+1), 1.2 * (i+1), 2 * (i+1), 3 * (i+1));
        Serial.println("setup::");
        data.PrintReading(messages[i]);
    }

	data.BuildReadingGroup(group_message, messages, num_messages);

    // free up some heap; we don't need messages anymore
	delete [] messages;

	//data.SerializeReading(write_buffer, buffer_length, message_length, message);

	data.SerializeReadingGroup(buffer, buffer_length, message_length, group_message);
	//data.DeserializeReading(message, message_length, write_buffer);
	data.DeserializeReadingGroup(group_message, message_length, buffer);
	data.Write(filename, buffer, message_length);
	//for (int i = 0; i < sizeof(buffer); i++) {
    //    buffer[i] = (uint8_t)'\0';
    //}
	data.Read(filename, buffer, message_length);
    //data.DeserializeReading(message, message_length, read_buffer);
    data.DeserializeReadingGroup(group_message, message_length, buffer);

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
