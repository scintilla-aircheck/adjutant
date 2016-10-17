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
UARTMux mux = UARTMux(UART_EXT, UART_INT);
SDS021 dust = SDS021();
//Components::SKM61 gps = SKM61();
PSTAT gas = PSTAT(PSTAT_0, PSTAT_1, PSTAT_2, MENB);

// OTA
OTA ota = OTA();

// Data
Data data = Data();

void ICACHE_FLASH_ATTR printFreeRam() {
    Serial.print(F("Free ram: "));
    Serial.println(ESP.getFreeHeap());
    Serial.println("");
}

void ICACHE_FLASH_ATTR setup ()
{
	// Intialize UART connection
	Serial.begin(9600);
	while (!Serial); // Necessary for USB

	uint32_t realSize = ESP.getFlashChipRealSize();
    uint32_t ideSize = ESP.getFlashChipSize();
    FlashMode_t ideMode = ESP.getFlashChipMode();

    Serial.print(F("Flash real id:   "));
    Serial.println(ESP.getFlashChipId());
    Serial.print(F("Flash real size: "));
    Serial.println(realSize);

    Serial.print(F("Flash ide size:  "));
    Serial.println(ideSize);
    Serial.print(F("Flash ide speed: "));
    Serial.println(ESP.getFlashChipSpeed());
    Serial.print(F("Flash ide mode:  "));
    Serial.println(ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN");

    if(ideSize != realSize) {
        Serial.println(F("Flash Chip configuration wrong!\n"));
    } else {
        Serial.println(F("Flash Chip configuration ok.\n"));
    }

    printFreeRam();

	Serial.println(F("Hardware serial initialized.\n"));

	// Initialize I2C connection
	Serial.print(F("Initializing I2C bus..."));
	Wire.pins(I2CSDA_PIN, I2CSCL_PIN);
	Wire.begin();
	Serial.println(F("Done!\n"));

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
	Serial.print(F("Initializing PSTAT circuit..."));
	gas.Begin();
	gas.ADC(true, MCP3425::EResolution::d16Bit, MCP3425::EGain::x1);
	gas.Configure(0, SPEC::CO);
	//gas.Configure(1, SPEC::O3);
	Serial.println("Done!\n");

	// Initialize OTA
	Serial.print(F("Initializing OTA..."));
	bool connected = ota.Connect("DLab", "endmill1");
	if(connected) {
	    // Attempt OTA Update
        Serial.print(F("\tInitializing OTA Update..."));
        ota.OtaUpdate("192.168.0.1", 1337, "/example.bin");
        Serial.println(F("\tDone!"));
	}
	Serial.println(F("Done!\n"));
	delay(1); // reset WDT

	// Test Data Serialization and Deserialization
	Serial.println(F("Initializing Data Serialization and Deserialization..."));

    int num_messages = 10;
    char filename[9] = "test.txt";
    int buffer_length = ReadingGroupMessage_size;

	size_t message_length;
	uint8_t buffer[buffer_length];
	uint8_t message_length_buffer[4];  // 4 bytes per size_t

	ReadingMessage *messages = new ReadingMessage[num_messages];
	ReadingGroupMessage group_message = ReadingGroupMessage_init_zero;

	Serial.print(F("setup:: sizeof(buffer): "));
	Serial.println(sizeof(buffer));
	Serial.print(F("setup:: sizeof(message_length): "));
	Serial.println(sizeof(message_length));

    for(int i = 0; i < num_messages; i++) {
        data.BuildReading(messages[i], 1 * (i+1), 1.0 * (i+1), 60 * (i+1), 1.1 * (i+1), 1.2 * (i+1), 2 * (i+1), 3 * (i+1));
        Serial.println(F("setup::"));
        data.PrintReading(messages[i]);
    }

	data.BuildReadingGroup(group_message, messages, num_messages);

    // free up some heap; we don't need messages anymore
    printFreeRam();
	delete [] messages;
	printFreeRam();

	data.SerializeReadingGroup(buffer, buffer_length, message_length, group_message);
	data.DeserializeReadingGroup(group_message, message_length, buffer);
	delay(1); // reset WDT

	message_length_buffer[0] = (message_length >> 24) & 0xFF;
    message_length_buffer[1] = (message_length >> 16) & 0xFF;
    message_length_buffer[2] = (message_length >> 8) & 0xFF;
    message_length_buffer[3] = message_length & 0xFF;

	data.Append(filename, message_length_buffer, 4);
	data.Append(filename, buffer, message_length);
	File f = SPIFFS.open(filename, "r");
	data.Read(f, message_length_buffer, 4);
	message_length = message_length_buffer[3] |
	                 (size_t)message_length_buffer[2] << 8 |
	                 (size_t)message_length_buffer[1] << 16 |
	                 (size_t)message_length_buffer[0] << 24;
	Serial.print(F("setup:: message_length: "));
	Serial.println(message_length);
	data.Read(f, buffer, message_length);
	f.close();

    data.DeserializeReadingGroup(group_message, message_length, buffer);

	Serial.println(F("Done!\n"));

	// Test HTTP POST
	Serial.println(F("Initializing HTTP POST..."));

    if(!connected) {
        ESP.eraseConfig();

        while(!connected) {
            connected = ota.Connect("DLab", "endmill1");
        }
    }

	if(connected) {
	    delay(1);
        HTTPClient http;
        http.begin("192.168.7.55", 8000, "/api/v1/readings/");
        http.addHeader("Content-Type", "text/plain");
        //char message_length_string[10];
        //itoa(message_length, message_length_string, 10);
        //Serial.print(F("setup:: Message Length String: "));
        //Serial.println(message_length_string);
        //http.addHeader("Content-Length", message_length_string);
        //http.addHeader("Content-Length", "378");
        Serial.print(F("setup:: sizeof(buffer): "));
	    Serial.println(sizeof(buffer));
	    Serial.print(F("setup:: buffer: "));
	    for (int i = 0; i < message_length; i++) {
            if (i > 0) Serial.print(":");
            Serial.printf("%02X", buffer[i]);
        }
        http.POST(buffer, message_length);
        http.writeToStream(&Serial);
        http.end();
	}
	Serial.println(F("Done!\n"));

	printFreeRam();

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

	Serial.print(F("VOUT: "));
	Serial.println(gas.ADC(), 4);

	delay(5 * 1000);

	Serial.println(F("- - - - - - - - - -"));
}
