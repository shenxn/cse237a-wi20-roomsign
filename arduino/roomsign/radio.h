#ifndef RADIO_H
#define RADIO_H

#define CE_PIN 5
#define CSN_PIN 6
#define WAITING_TIMEOUT 500000  // 500 ms

void radioConfigure();
void radioFetch();
void radioRead();

#endif
