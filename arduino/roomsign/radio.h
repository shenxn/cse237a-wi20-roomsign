#ifndef RADIO_H
#define RADIO_H

#define CE_PIN 5
#define CSN_PIN 6
#define WAITING_TIMEOUT 500000  // 500 ms

struct Request {
    char operation;
};

struct Response {
    char available;
    char name[16];
    char time[16];
};

void radioConfigure();
void radioFetch();
void radioRead();

#endif
