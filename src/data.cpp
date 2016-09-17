#include "data.h"

Data::Data() {}

Data::~Data() {}

bool Data::Serialize(uint8_t *buffer, size_t &message_length, int number) {
    bool status;

    SensorDataMessage message = SensorDataMessage_init_zero;

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    message.lucky_number = number;

    status = pb_encode(&stream, SensorDataMessage_fields, &message);
    message_length = stream.bytes_written;

    if (!status)
    {
        Serial.print("Encoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return false;
    }

    return true;
}

bool Data::Deserialize(SensorDataMessage &message, size_t &message_length, uint8_t *buffer) {
    bool status;

    message = SensorDataMessage_init_zero;

    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

    /* Now we are ready to decode the message. */
    status = pb_decode(&stream, SensorDataMessage_fields, &message);

    /* Check for errors... */
    if (!status)
    {
        Serial.print("Decoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return false;
    }

    /* Print the data contained in the message. */
    Serial.print("Your lucky number was: ");
    Serial.println(message.lucky_number);

    return true;
}

bool Data::Write(int number) {
    uint8_t buffer[128];
    size_t message_length;
    SensorDataMessage message;

    Data::Serialize(buffer, message_length, number);

    // always use this to "mount" the filesystem
    bool result = SPIFFS.begin();
    Serial.println("SPIFFS opened: " + result);

    // this opens the file "f.txt" in read-mode
    File f = SPIFFS.open("/f.txt", "r");

    if (!f) {
        Serial.println("File doesn't exist yet. Creating it");

        // open the file in write mode
        File f = SPIFFS.open("/f.txt", "w");
        if (!f) {
            Serial.println("file creation failed");
        }
        // now write two lines in key/value style with  end-of-line characters
        f.println("ssid=abc");
        f.println("password=123455secret");
    } else {
        // we could open the file
        while(f.available()) {
            //Lets read line by line from the file
            String line = f.readStringUntil('\n');
            Serial.println(line);
        }
    }
    f.close();

    Data::Deserialize(message, message_length, buffer);

    Serial.print("Your lucky number (outside the function) was: ");
    Serial.println(message.lucky_number);

    return true;
}