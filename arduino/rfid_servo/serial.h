#ifndef SERIAL_H
#define SERIAL_H

#define RX_PIN 2
#define TX_PIN 3
#define BAUD_RATE 9600
#define READ_TIMEOUT 500

void serialBegin();

void serialRead();

void serialRequest();

#endif
