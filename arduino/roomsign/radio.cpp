#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "radio.h"
#include "printf.h"
#include "status.h"

const uint64_t rx_pipe = 0xF0F0F0F0E1;
const uint64_t tx_pipe = 0xF0F0F0F0D2;

RF24 radio(CE_PIN, CSN_PIN);

void radioConfigure() {
    printf_begin();

    radio.begin();

    // set the PA Level low to prevent power supply related issues
    // radio.setPALevel(RF24_PA_LOW);

    radio.setDataRate(RF24_250KBPS);

    radio.setAutoAck(1);
    radio.enableDynamicPayloads();
    radio.setRetries(15, 15);
    radio.openWritingPipe(tx_pipe);
    radio.openReadingPipe(1, rx_pipe);
    radio.startListening();
    radio.printDetails();
}

void radioFetch() {
    // send signal
    Serial.println(F("sending fetch signal"));
    Request request = {OPERATION_FETCH};
    int count = 0;
    while (true) {
        radio.stopListening();
        if (radio.write(&request, sizeof(Request))) {
            Serial.println(F("\tok"));

            // wait for response
            radio.startListening();
            Serial.println(F("\twait for response"));
            unsigned long started_waiting_at = millis();
            bool timeout = false;
            while (!radio.available() && !timeout) {
                if (millis() - started_waiting_at > WAITING_TIMEOUT) {
                    timeout = true;
                }
            }
            if (timeout) {
                Serial.println(F("\ttimeout"));
            } else {
                break;  // succeed
            }
        } else {
            Serial.println(F("\tfail"));
        }
        ++count;
        if (count == 10) {  // try at most 10 times
            Serial.println(F("\tgive up"));
            break;
        }
        delay(1000);  // retry after one second
    }

}

void radioRead() {
    if (radio.available()) {
        Serial.println(F("receiving data"));
        char buffer[sizeof(Event)];
        uint8_t buffer_size = 0;
        while (true) {  // reading multiple packets
            uint8_t len = radio.getDynamicPayloadSize();

            // If a corrupt dynamic payload is received, it will be flushed
            if(!len){
                continue; 
            }

            char payload[len];
            radio.read(payload, len);
            Serial.print(F("\treceived payload of size "));
            Serial.println(len);

            if (buffer_size + len > sizeof(Event)) {
                // error check to avoid segfault
                Serial.println(F("\tfail, data error"));
                return;  // failed to read the radio
            }
            memcpy(buffer+buffer_size, payload, len);
            buffer_size += len;
            if (buffer_size == sizeof(Event)) {
                Serial.println(F("\tok"));
                break;
            }

            unsigned long started_waiting_at = millis();
            bool timeout = false;
            while (!radio.available() && !timeout) {
                if (millis() - started_waiting_at > WAITING_TIMEOUT) {
                    timeout = true;
                }
            }

            if (timeout) {
                Serial.println(F("\tfail, timeout"));
                return;  // failed to read the radio
            }
        }
        
        // flush buffer
        memcpy((char*)&status.event, buffer, sizeof(Event));
        status.updated = true;
        Serial.println(F("got response:"));
        Serial.print(F("\tavailable: "));
        Serial.println((int)status.event.available);
        Serial.print(F("\tsummary: "));
        Serial.println(status.event.summary);
        Serial.print(F("\ttime: "));
        Serial.println(status.event.time);
        Serial.print(F("\tcreator: "));
        Serial.println(status.event.creator);
        Serial.print(F("\tkey id: "));
        for (int i = 0; i < sizeof(status.event.key_id); ++i) {
            Serial.print((uint8_t)status.event.key_id[i], HEX);
        }
        Serial.println();
    }
}
