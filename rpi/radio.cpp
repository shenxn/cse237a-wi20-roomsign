
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>
#include "request.h"

using namespace std;

RF24 radio(22, 0);

#define INT_PIN 23

const uint8_t txAddress[] = "2Node";
const uint8_t rxAddress[] = "1Node";

unsigned int available;


void intHandler() {
    printf("Interrupt!\n");

    // if there is data ready
    if (radio.available()) {

        // read request
        Request request;
        radio.read(&request, sizeof(Request));
        radio.stopListening();

        // send response
        radio.write(&available, sizeof(unsigned int));

        // Now, resume listening so we catch the next packets.
        radio.startListening();

        // Spew it
        printf("Request operation %d\n", request.operation);
    }
}


int main(int argc, char** argv)
{

    // Setup and configure rf radio
    radio.begin();

    // optionally, increase the delay between retries & # of retries
    radio.setRetries(15, 15);
    // Dump the configuration of the rf unit for debugging
    radio.printDetails();

    radio.openWritingPipe(txAddress);
    radio.openReadingPipe(1, rxAddress);

    radio.startListening();

    std::cout << "Available? ";
    std::cin >> available;

    radio.maskIRQ(1, 1, 0);  // Mask tx_ok & tx_fail interrupts
    attachInterrupt(INT_PIN, INT_EDGE_FALLING, intHandler);

    while (1) {
        std::cout << "Available? ";
        std::cin >> available;
        radio.stopListening();
        radio.write(&available, sizeof(unsigned int));
        radio.startListening();
        printf("Sent %d\n", available);
    }

    return 0;
}
