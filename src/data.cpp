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

bool Data::PrintReading(ReadingMessage message) {

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

bool Data::PrintSPIFFSFiles() {
    String str = "";
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
        str += dir.fileName();
        str += " / ";
        str += dir.fileSize();
        str += "\n";
    }
    Serial.println(str); // - See more at: http://www.esp8266.com/viewtopic.php?f=32&t=8459#sthash.Ia8ICfTh.dpuf
}

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

    //group_message = ReadingGroupMessage_init_zero;

    ReadingMessage *ptr = message;
    int counter = 0;

    group_message.readings_count = num_messages;

    for(int i = 0; i < num_messages; i++) {
        group_message.readings[i] = message[i];
    }

    /*
    while(ptr < message + num_messages) {
        group_message.readings[counter] = *ptr;
        ptr++;
        counter++;
    }
    */

    return true;
}

// Serialize data into a uint8_t buffer representation of ReadingMessage
//bool Data::SerializeReading(uint8_t *buffer, int buffer_length, size_t &message_length, int sensor, double value, int long average_over_seconds, double longitude, double latitude, int unit, int long time) {
bool Data::SerializeReading(uint8_t *buffer, int buffer_length, size_t &message_length, ReadingMessage &message) {

    bool status;

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_length);

    Serial.print("buffer_length: ");
    Serial.println(buffer_length);

    status = pb_encode(&stream, ReadingMessage_fields, &message);
    message_length = stream.bytes_written;

    if(!status) {
        Serial.print("SerializeReading:: Encoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return false;
    }

    Serial.print("SerializeReading:: buffer: ");
    Serial.write(buffer, message_length);
    Serial.println("");
    Serial.print("SerializeReading:: Message length: ");
    Serial.println(message_length);

    return true;
}

bool Data::SerializeReadingGroup(uint8_t *buffer, int buffer_length, size_t &message_length, ReadingGroupMessage &group_message) {

    bool status;

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_length);

    Serial.print("buffer_length: ");
    Serial.println(buffer_length);

    status = pb_encode(&stream, ReadingGroupMessage_fields, &group_message);
    message_length = stream.bytes_written;

    if(!status) {
        Serial.print("SerializeReadingGroup:: Encoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return false;
    }

    Serial.print("SerializeReadingGroup:: buffer: ");
    Serial.write(buffer, message_length);
    Serial.println("");
    Serial.print("SerializeReadingGroup:: Message length: ");
    Serial.println(message_length);

    return true;
}

// Deserialize uint8_t buffer representation of ReadingMessage into ReadingMessage
bool Data::DeserializeReading(ReadingMessage &message, size_t &message_length, uint8_t *buffer) {

    bool status;

    message = ReadingMessage_init_zero;

    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

    /* Now we are ready to decode the message. */
    status = pb_decode(&stream, ReadingMessage_fields, &message);

    /* Check for errors... */
    if(!status) {
        Serial.print("Decoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return false;
    }

    /* Print the data contained in the message. */
    Serial.println("DeserializeReading::");
    PrintReading(message);

    return true;
}

bool Data::DeserializeReadingGroup(ReadingGroupMessage &group_message, size_t &message_length, uint8_t *buffer) {

    bool status;

    group_message = ReadingGroupMessage_init_zero;

    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

    /* Now we are ready to decode the message. */
    status = pb_decode(&stream, ReadingGroupMessage_fields, &group_message);

    /* Check for errors... */
    if(!status) {
        Serial.print("Decoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return false;
    }

    Serial.print("DeserializeReadingGroup:: Message length: ");
    Serial.println(message_length);

    /* Print the data contained in the message. */
    Serial.println("DeserializeReadingGroup:: ");
    for(int i = 0; i < group_message.readings_count; i++) {
        PrintReading(group_message.readings[i]);
    }

    return true;
}

bool Data::Read(char *filename, uint8_t *buffer, size_t message_length) {

    String line;
    File f = SPIFFS.open(filename, "r");

    if(f) {
        if(f.available()) {
            f.read(buffer, message_length);
        } else {
            Serial.println("Read:: File not available.");
        }

        Serial.print("Read:: Here's what we read as a uint8_t: ");
        Serial.write(buffer, message_length);
        Serial.println("");

        f.close();
    } else {
        Serial.println("Read:: Tried to open file that doesn't exist.");
        Serial.print("Read:: Named: ");
        Serial.println(filename);

        return false;
    }

    return true;
}

bool Data::Write(char *filename, uint8_t *buffer, size_t message_length) {

    // always use this to "mount" the filesystem
    bool result = SPIFFS.begin();
    Serial.print("Write:: SPIFFS opened: ");
    Serial.println(result);

    Serial.println("Write:: Opening file in write mode.");
    // open the file in write mode
    File f = SPIFFS.open(filename, "w");
    if(!f) {
        Serial.println("Write:: file creation failed");
    }
    // now write one line
    f.write(buffer, message_length);
    //f.write('\n');
    Serial.print("Write:: Here's what we wrote: ");
    Serial.write(buffer, message_length);
    Serial.println("");
    Serial.print("Write:: Message length: ");
    Serial.println(message_length);

    f.close();

    return true;
}