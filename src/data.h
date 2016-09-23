#pragma once

#include "FS.h"

#include "pb_common.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "pb.h"
#include "reading.pb.h"

class Data
{
public:
	Data();
	~Data();

    // Read
    bool Read(char *filename, uint8_t *buffer, size_t message_length);

	// Write To Memory
	bool Write(char *filename, uint8_t *buffer, size_t message_length);

	bool SerializeReading(uint8_t *buffer, int buffer_length, size_t &message_length, int sensor, double value, int long average_over_seconds, double longitude, double latitude, int unit, int long time);

	bool DeserializeReading(ReadingMessage &message, size_t &message_length, uint8_t *buffer);

private:

};

/*
message ReadingGroupMessage {
    repeated ReadingMessage readings = 1;
}

message ReadingMessage {
    optional int32 sensor = 1;
    optional double value = 2;
    optional int64 average_over_seconds = 3;
    optional double longitude = 4;
    optional double latitude = 5;
    optional int32 unit = 6;
    optional int64 time = 7;
}
*/
