#include <SPI.h>
#include "RF24.h"
#include "radio.h"
#include "printf.h"
#include "status.h"
#include "request.h"

byte txAddress[] = "1Node";
byte rxAddress[] = "2Node";

RF24 radio(CE_PIN, CSN_PIN);

void radioConfigure() {
    printf_begin();

    radio.begin();

    // set the PA Level low to prevent power supply related issues
    radio.setPALevel(RF24_PA_LOW);

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
        unsigned int available;
        radio.read(&available, sizeof(unsigned int));
        status.available = available;
        status.updated = true;
        Serial.print(F("available: "));
        Serial.println(available);
    }
}
