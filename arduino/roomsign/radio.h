#ifndef RADIO_H
#define RADIO_H

#define RADIO_CE_PIN 5
#define RADIO_CSN_PIN 6
#define WAITING_TIMEOUT 5000

#define OPERATION_FETCH 1

struct Request {
    char operation;
};

void radioConfigure();
void radioFetch();
void radioRead();

#endif
