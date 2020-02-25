#include <SPI.h>
#include "RF24.h"
#include "radio.h"
#include "printf.h"
#include "status.h"
#include "request.h"
#include "response.h"

byte txAddress[] = "1Node";
byte rxAddress[] = "2Node";

RF24 radio(CE_PIN, CSN_PIN);

void radioConfigure() {
    printf_begin();

    radio.begin();

    // set the PA Level low to prevent power supply related issues
    // radio.setPALevel(RF24_PA_LOW);

    radio.enableDynamicPayloads();

    radio.openWritingPipe(txAddress);
    radio.openReadingPipe(1, rxAddress);

    radio.startListening();
    radio.printDetails();
}

void radioFetch() {
    radio.stopListening();

    // send signal
    Serial.println(F("sending fetch signal"));
    Request request = {OPERATION_FETCH};
    radio.write(&request, sizeof(Request));

    radio.startListening();
}

void radioRead() {
    if (radio.available()) {
        Response response;
        radio.read(&response, sizeof(Response));
        status.available = response.available;
        if (!status.available) {
            strncpy(status.name, response.name, sizeof(status.name));
            strncpy(status.time, response.time, sizeof(status.time));
            status.name[sizeof(status.name) - 1] = '\0';  // prevent overflow
            status.time[sizeof(status.time) - 1] = '\0';
        }
        status.updated = true;
        Serial.println(F("Got response:"));
        Serial.print(F("\tsize: "));
        Serial.println(sizeof(Response));
        Serial.print(F("\tavailable: "));
        Serial.println(status.available);
        Serial.print(F("\tname: "));
        Serial.println(response.name);
        Serial.print(F("\ttime: "));
        Serial.println(status.time);
    }
}
