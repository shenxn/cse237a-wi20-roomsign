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


void serialWrite() {
    Serial.print(F("sending key through serial: "));
    for (int i = 0; i < sizeof(status.event.key_id); ++i) {
        Serial.print((uint8_t)status.event.key_id[i], HEX);
    }
    Serial.println();
    softSerial.write((uint8_t)0);
    softSerial.write(status.event.key_id, sizeof(status.event.key_id));
    softSerial.write((uint8_t)1);
}


void serialRead() {
    if (softSerial.available()) {
        uint8_t command = softSerial.read();
        if (command == 1) {  // request signal
            Serial.println(F("got request from serial"));
            serialWrite();
        }
    }
}
