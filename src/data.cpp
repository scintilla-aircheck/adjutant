#include "data.h"

Data::Data() {}

Data::~Data() {}

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

bool Data::BuildReading(ReadingMessage &message, int sensor, double value, int long average_over_seconds, double longitude, double latitude, int unit, int long time) {

    message = ReadingMessage_init_zero;

    message.sensor = sensor;
    message.value = value;
    message.average_over_seconds = average_over_seconds;
    message.longitude = longitude;
    message.latitude = latitude;
    message.unit = unit;
    message.time = time;

    message.has_sensor = true;
    message.has_value = true;
    message.has_average_over_seconds = true;
    message.has_longitude = true;
    message.has_latitude = true;
    message.has_unit = true;
    message.has_time = true;

    return true;
}

bool Data::BuildReadingGroup(ReadingGroupMessage &group_message, ReadingMessage *message, int num_messages) {

    group_message = ReadingGroupMessage_init_zero;
    /*
    for(int i = 0; i < num_messages; i++) {
        group_message.readings[i] = message[i];
    }
    */
    return true;
}

// Serialize data into a uint8_t buffer representation of ReadingMessage
//bool Data::SerializeReading(uint8_t *buffer, int buffer_length, size_t &message_length, int sensor, double value, int long average_over_seconds, double longitude, double latitude, int unit, int long time) {
bool Data::SerializeReading(uint8_t *buffer, int buffer_length, size_t &message_length, ReadingMessage message) {

    //uint8_t _buffer[buffer_length];

    bool status;

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_length);
    //pb_ostream_t stream = pb_ostream_from_buffer(_buffer, sizeof(_buffer));

    Serial.print("buffer_length: ");
    Serial.println(buffer_length);
    //Serial.print("sizeof(_buffer): ");
    //Serial.println(sizeof(_buffer));

    status = pb_encode(&stream, ReadingMessage_fields, &message);
    message_length = stream.bytes_written;

    if (!status) {
        Serial.print("SerializeReading:: Encoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return false;
    }

    Serial.print("SerializeReading:: buffer: ");
    Serial.write(buffer, message_length);
    Serial.println("");
    //Serial.print("SerializeReading:: buffer: ");
    //Serial.write(_buffer, message_length);
    //Serial.println("");
    Serial.print("SerializeReading:: Message length: ");
    Serial.println(message_length);

    //buffer = _buffer;

    return true;
}

bool Data::SerializeReadingGroup(uint8_t *buffer, int buffer_length, size_t &message_length, ReadingGroupMessage group_message) {

    //uint8_t _buffer[buffer_length];

    bool status;

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_length);
    //pb_ostream_t stream = pb_ostream_from_buffer(_buffer, sizeof(_buffer));

    Serial.print("buffer_length: ");
    Serial.println(buffer_length);
    //Serial.print("sizeof(_buffer): ");
    //Serial.println(sizeof(_buffer));

    status = pb_encode(&stream, ReadingGroupMessage_fields, &group_message);
    message_length = stream.bytes_written;

    if (!status) {
        Serial.print("SerializeReadingGroup:: Encoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return false;
    }

    Serial.print("SerializeReadingGroup:: buffer: ");
    Serial.write(buffer, message_length);
    Serial.println("");
    //Serial.print("SerializeReading:: buffer: ");
    //Serial.write(_buffer, message_length);
    //Serial.println("");
    Serial.print("SerializeReadingGroup:: Message length: ");
    Serial.println(message_length);

    //buffer = _buffer;

    return true;
}

// Deserialize uint8_t buffer representation of ReadingMessage into ReadingMessage
bool Data::DeserializeReading(ReadingMessage &message, size_t &message_length, uint8_t *buffer) {

    //uint8_t _buffer[message_length];

    bool status;

    message = ReadingMessage_init_zero;

    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

    /* Now we are ready to decode the message. */
    status = pb_decode(&stream, ReadingMessage_fields, &message);

    /* Check for errors... */
    if (!status) {
        Serial.print("Decoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return false;
    }

    /* Print the data contained in the message. */
    Serial.println("ReadingMessage:");
    Serial.println(message.sensor);
    Serial.println(message.value);
    Serial.println((int)message.average_over_seconds);
    Serial.println(message.longitude);
    Serial.println(message.latitude);
    Serial.println(message.unit);
    Serial.println((int)message.time);

    return true;
}

bool Data::DeserializeReadingGroup(ReadingGroupMessage &group_message, size_t &message_length, uint8_t *buffer) {

    //uint8_t _buffer[message_length];

    bool status;

    group_message = ReadingGroupMessage_init_zero;

    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

    /* Now we are ready to decode the message. */
    status = pb_decode(&stream, ReadingGroupMessage_fields, &group_message);

    /* Check for errors... */
    if (!status) {
        Serial.print("Decoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return false;
    }

    /* Print the data contained in the message. */
    /*
    Serial.println("ReadingGroupMessage:");
    Serial.println(group_message.readings(0).sensor);
    Serial.println(group_message.readings(0).value);
    Serial.println((int)group_message.readings(0).average_over_seconds);
    Serial.println(group_message.readings(0).longitude);
    Serial.println(group_message.readings(0).latitude);
    Serial.println(group_message.readings(0).unit);
    Serial.println((int)group_message.readings(0).time);
    */

    return true;
}

bool Data::Read(char *filename, uint8_t *buffer, size_t message_length) {

    String line;
    File f = SPIFFS.open(filename, "r");

    if (f) {
        if (f.available()) {
            //f.readStringUntil('\n').toCharArray(buffer, sizeof(buffer));
            //buffer = (unsigned char*)f.readStringUntil('\n').c_str();
            line = f.readStringUntil('\n');
        }
        /*
        while(f.available()) {
            //Lets read line by line from the file
            String line = f.readStringUntil('\n');
            Serial.println(line);
        }
        */

        Serial.print("Here's what we read: ");
        //Serial.println((const char*)buffer);
        Serial.println(line);
        //strcpy((char *)buffer, line.c_str());
        for (int a = 0; a <= message_length; a++) {
            buffer[a] = line[a];
        }
        Serial.write(buffer, message_length);
        Serial.println("");

        f.close();
    } else {
        Serial.println("Tried to open file that doesn't exist.");
        Serial.print("Named: ");
        Serial.print(filename);

        return false;
    }

    return true;
}

bool Data::Write(char *filename, uint8_t *buffer, size_t message_length) {

    //buffer = (unsigned char*)"hijklmnop";

    // always use this to "mount" the filesystem
    bool result = SPIFFS.begin();
    Serial.print("SPIFFS opened: ");
    Serial.println(result);

    // this opens the file "f.txt" in read-mode
    //File f = SPIFFS.open(filename, "r");

    //if (!f) {
        //Serial.println("File doesn't exist yet. Creating it");
        Serial.println("Opening file in write mode.");
        // open the file in write mode
        File f = SPIFFS.open(filename, "w");
        if (!f) {
            Serial.println("file creation failed");
        }
        // now write one line with an end-of-line character
        f.write(buffer, message_length);
        f.write('\n');
        Serial.print("Here's what we wrote: ");
        Serial.write(buffer, message_length);
        Serial.println("");
    /*
    } else {
        // we could open the file
        while(f.available()) {
            //Lets read line by line from the file
            String line = f.readStringUntil('\n');
            Serial.println(line);
        }
    }
    */
    f.close();

    return true;
}