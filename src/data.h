#pragma once

#include "FS.h"

#include "pb_common.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "pb.h"
#include "sensor_data.pb.h"

class Data
{
public:
	Data();
	~Data();

	// Write To Memory
	bool Write(int number);

private:
    bool Serialize(uint8_t *buffer, size_t &message_length, int number);
	bool Deserialize(SensorDataMessage &message, size_t &message_length, uint8_t *buffer);
};
