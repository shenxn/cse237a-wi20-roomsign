
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>
#include "request.h"
#include "response.h"

using namespace std;

RF24 radio(22, 0);

#define INT_PIN 23

const uint64_t txAddress = 0xF0F0F0F0E1;
const uint64_t rxAddress = 0xF0F0F0F0D2;

Response response;


void sendResponse() {
    radio.stopListening();
    radio.write(&response, sizeof(Response));
    radio.startListening();

    printf("Sent response:\n");
    printf("\tsize: %d\n", sizeof(Response));
    printf("\tavailable: %d\n", (int)response.available);
    printf("\tname: %s\n", response.name);
    printf("\ttime: %s\n", response.time);
}


void intHandler() {
    printf("\nInterrupt!\n");

    if (radio.available()) {
        Request request;
        radio.read(&request, sizeof(Request));
        if (request.operation == OPERATION_FETCH) {
            sendResponse();
        }
    }
}


void setResponse() {
    unsigned int available;
    std::cout << "Available? ";
    std::cin >> available;
    if (available == 1) {
        response.available = 1;
    } else if (available == 0) {
        string str;
        std::getline(cin, str);
        std::cout << "Name? (Gustavo R.) ";
        std::cin.getline(response.name, 16, '\n');
        if (response.name[0] == '\0') {
            strcpy(response.name, "Gustavo R.");
        }
        std::cout << "Time? (11:45AM-12:03PM) ";
        std::cin.getline(response.time, 16, '\n');
        if (response.time[0] == '\0') {
            strcpy(response.time, "11:45AM-12:03PM");
        }
        response.available = 0;
    } else {
        return;
    }
    sendResponse();
}


int main(int argc, char** argv)
{

    // Setup and configure rf radio
    radio.begin();

    radio.setRetries(15, 15);

    radio.openWritingPipe(txAddress);
    radio.openReadingPipe(1, rxAddress);
    
    radio.enableDynamicPayloads();

    radio.printDetails();

    radio.startListening();

    setResponse();

    radio.maskIRQ(1, 1, 0);  // Mask tx_ok & tx_fail interrupts
    attachInterrupt(INT_PIN, INT_EDGE_FALLING, intHandler);

    while (1) {
        setResponse();
    }

    return 0;
}
