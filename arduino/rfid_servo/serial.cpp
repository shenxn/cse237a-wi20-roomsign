#include <Arduino.h>
#include <SoftwareSerial.h>
#include "serial.h"
#include "status.h"

SoftwareSerial softSerial(RX_PIN, TX_PIN);


void serialBegin() {
    pinMode(RX_PIN, INPUT);
    pinMode(TX_PIN, OUTPUT);
    softSerial.begin(BAUD_RATE);
}


void serialRead() {
    if (softSerial.available()) {
        uint8_t command = softSerial.read();
        if (command == 0) {  // start signal
            Serial.println(F("start reading from serial"));
            uint8_t buffer[sizeof(status.key_id)];
            uint8_t buffer_size = 0;
            unsigned long started_waiting_at = millis();
            while (true) {
                if (millis() - started_waiting_at > READ_TIMEOUT) {
                    Serial.println(F("\tfail, timeout"));
                    return;
                }
                if (softSerial.available()) {
                    uint8_t b = softSerial.read();
                    if (buffer_size == sizeof(status.key_id)) {
                        // expect end signal
                        if (b != 1) {
                            Serial.println(F("\tfail, data error"));
                            return;
                        }
                        break;
                    }
                    Serial.print(b, HEX);
                    buffer[buffer_size++] = b;
                }
            }
            
            // flush buffer
            memcpy(status.key_id, buffer, sizeof(status.key_id));
            Serial.println(F("\tok"));
        }
    }
}


void serialRequest() {
    // request for key id
    Serial.println(F("send key id request"));
    softSerial.write(uint8_t(1));
}
