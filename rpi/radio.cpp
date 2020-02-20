
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>

using namespace std;
//
// Hardware configuration
// Configure the appropriate pins for your connections

/****************** Raspberry Pi ***********************/

// Radio CE Pin, CSN Pin, SPI Speed
// See http://www.airspayce.com/mikem/bcm2835/group__constants.html#ga63c029bd6500167152db4e57736d0939 and the related enumerations for pin information.

// Setup for GPIO 22 CE and CE0 CSN with SPI Speed @ 4Mhz
//RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_4MHZ);

// NEW: Setup for RPi B+
//RF24 radio(RPI_BPLUS_GPIO_J8_15,RPI_BPLUS_GPIO_J8_24, BCM2835_SPI_SPEED_8MHZ);

// Setup for GPIO 15 CE and CE0 CSN with SPI Speed @ 8Mhz
//RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

// RPi generic:
RF24 radio(22, 0);

/*** RPi Alternate ***/
//Note: Specify SPI BUS 0 or 1 instead of CS pin number.
// See http://tmrh20.github.io/RF24/RPi.html for more information on usage

//RPi Alternate, with MRAA
//RF24 radio(15,0);

//RPi Alternate, with SPIDEV - Note: Edit RF24/arch/BBB/spi.cpp and  set 'this->device = "/dev/spidev0.0";;' or as listed in /dev
//RF24 radio(22,0);


/****************** Linux (BBB,x86,etc) ***********************/

// See http://tmrh20.github.io/RF24/pages.html for more information on usage
// See http://iotdk.intel.com/docs/master/mraa/ for more information on MRAA
// See https://www.kernel.org/doc/Documentation/spi/spidev for more information on SPIDEV

// Setup for ARM(Linux) devices like BBB using spidev (default is "/dev/spidev1.0" )
//RF24 radio(115,0);

//BBB Alternate, with mraa
// CE pin = (Header P9, Pin 13) = 59 = 13 + 46 
//Note: Specify SPI BUS 0 or 1 instead of CS pin number. 
//RF24 radio(59,0);

const uint8_t txAddress[] = "2Node";
const uint8_t rxAddress[] = "1Node";

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

    unsigned int available;
    std::cout << "Available? ";
    std::cin >> available;

    // forever loop
    while (1) {
        // if there is data ready
        if (radio.available()) {
            // Dump the payloads until we've gotten everything
            unsigned int op;

            // Fetch the payload, and see if this was the last one.
            while (radio.available()) {
                radio.read(&op, sizeof(unsigned int));
            }
            radio.stopListening();

            radio.write(&available, sizeof(unsigned int));

            // Now, resume listening so we catch the next packets.
            // radio.startListening();

            // Spew it
            printf("Got payload %d\n", op);
            break;

            // delay(925); //Delay after payload responded to, minimize RPi CPU time

        }
        delay(100);
    }

    while (1) {
        std::cout << "Available? ";
        std::cin >> available;
        radio.write(&available, sizeof(unsigned int));
        printf("Sent %d\n", available);
    }

    return 0;
}
