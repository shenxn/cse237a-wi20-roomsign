#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include "radio.h"
#include "printf.h"
#include "status.h"

const uint8_t channel = 90;
const uint64_t nodeAddress = 00;
const uint64_t targetAddress = 01;

RF24 radio(CE_PIN, CSN_PIN);
RF24Network network(radio);

void radioConfigure() {
    printf_begin();

    radio.begin();

    // set the PA Level low to prevent power supply related issues
    // radio.setPALevel(RF24_PA_LOW);

    network.begin(channel, nodeAddress);

    // radio.printDetails();
}

void radioFetch() {
    // radio.stopListening();

    // // send signal
    // Serial.println(F("sending fetch signal"));
    // Request request = {OPERATION_FETCH};
    // radio.write(&request, sizeof(Request));

    // radio.startListening();
}

void radioRead() {
    network.update();

    if (network.available()) {
        RF24NetworkHeader header;
        network.read(header, &status.event, sizeof(Event));
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
    }
}
