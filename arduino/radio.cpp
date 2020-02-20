#include <SPI.h>
#include "RF24.h"
#include "radio.h"
#include "printf.h"

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
    while (1) {
        radio.stopListening();

        // send signal
        Serial.println(F("sending fetch signal"));
        unsigned int op = OPERATION_FETCH;
        radio.write(&op, sizeof(unsigned int));

        // get response
        radio.startListening();
        unsigned long startedWaitingAt = micros();
        boolean timeout = false;
        while (!radio.available()) {
            if (micros() - startedWaitingAt > WAITING_TIMEOUT) {
                timeout = true;
                break;
            }
        }
        if (timeout) {
            Serial.println(F("failed - time out"));
        } else {
            unsigned int status;
            radio.read(&status, sizeof(unsigned int));
            Serial.print(F("status: "));
            Serial.println(status);
            break;
        }
        
        // retry after 1 second
        delay(1000);
    }
}
