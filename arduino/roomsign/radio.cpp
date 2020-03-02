#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include "radio.h"
#include "printf.h"
#include "status.h"
#include "epaper.h"
#include "macro.h"
#include "sleep.h"

const uint16_t channel = 90;
const uint16_t node_address = 00;
const uint16_t target_address = 01;

RF24 radio(RADIO_CE_PIN, RADIO_CSN_PIN);
RF24Network network(radio);

void radioConfigure() {
    printf_begin();

    radio.begin();
    network.begin(channel, node_address);

    // set the PA Level low to prevent power supply related issues
    // radio.setPALevel(RF24_PA_LOW);

    radio.setDataRate(RF24_2MBPS);

    // radio.setAutoAck(1);
    // radio.enableDynamicPayloads();
    // radio.setRetries(5, 15);
    // radio.openWritingPipe(tx_pipe);
    // radio.openReadingPipe(1, rx_pipe);
    // radio.startListening();
    radio.printDetails();

    // initial fetch
    radioFetch();
}

void radioFetch() {
    radio.powerUp();
    // send signal
    network.update();
    SERIAL_PRINT(F("sending signal"));
    byte payload = 0;
    RF24NetworkHeader header(target_address);
    if (network.write(header, &payload, 1)) {
        SERIAL_PRINTLN(F("\tok, wait for response"));
        unsigned long started_waiting_at = millis();
        bool timeout = false;
        while (!timeout && !network.available()) {
            network.update();
            if (millis() - started_waiting_at > WAITING_TIMEOUT) {
                timeout = true;
            }
        }
        if (timeout) {
            SERIAL_PRINTLN(F("\ttimeout"));
        } else {
            radioRead();
        }
    } else {
        SERIAL_PRINTLN(F("\tfail"));
    }
    radio.powerDown();
    epaperDisplay();
}

uint32_t bits_to_int(byte *bits, int len) {
    int v = 0;
    for (int i = 0; i < len; i++) {
        v = (v << 1) + bits[i];
    }
    return v;
}

char bits_to_char(byte *bits) {
    uint32_t v = bits_to_int(bits, 6);
    if (v == 0) return '\0';
    if (v == 1) return ' ';
    if (v >= 2 && v < 10 + 2) return v - 2 + '0';
    if (v >= 10 + 2 && v < 10 + 2 + 26) return v - 2 - 10 + 'A';
    return v - 2 - 10 - 26 + 'a';
}

void bits_to_str(char *target, byte *bits, int len) {
    for (int i = 0; i < len - 1; i++) {
        target[i] = bits_to_char(bits);
        bits += 6;
    }
    target[len - 1] = '\0';
    return;
}

void bits_to_time(char *target, byte *bits) {
    for (int i = 0; i < 2; i++) {
        int v = bits_to_int(bits, 11);
        int hour = v / 60;
        int hour12 = hour % 12;
        int minute = v % 60;
        sprintf(target, "%02d:%02d%s", hour12, minute, (hour / 12 ? "AM" : "PM"));
        bits += 11;
        target += 7;
        if (i == 0) {
            target[0] = '-';
            ++target;
        }
    }
}

void radioRead() {
    network.update();
    while (network.available()) {
        SERIAL_PRINTLN(F("receiving data"));

        RF24NetworkHeader header;
        byte payload[sizeof(Event)];
        network.read(header, payload, sizeof(payload));
        
        SERIAL_PRINTLN(F("\tok"));

        // check for update
        bool equal = true;
        for (int i = 0; i < sizeof(payload); ++i) {
            if (payload[i] != ((byte*)&status.event)[i]) {
                equal = false;
                break;
            }
        }
        if (equal) {
            SERIAL_PRINTLN(F("\tno change"));
            continue;
        }
        memcpy(&status.event, payload, sizeof(Event));
        
        // // decode payload
        // byte bits[256];
        // for (int i = 0; i < 32; ++i) {
        //     for (int j = 7; j >= 0; --j) {
        //         bits[(i << 3) + j] = payload[i] & 1;
        //         payload[i] >>= 1;
        //     }
        // }
        // Event decoded;
        // uint8_t offset = 0;
        // decoded.available = bits[0];
        // offset += 1;
        // bits_to_str(decoded.summary, bits + offset, sizeof(decoded.summary));
        // offset += 6 * (sizeof(decoded.summary) - 1);
        // bits_to_time(decoded.time, bits + offset);
        // offset += 2 * 11;
        // bits_to_str(decoded.creator, bits + offset, sizeof(decoded.creator));
        // offset += 6 * (sizeof(decoded.creator) - 1);
        // for (int i = 0; i < sizeof(decoded.key_id); i++) {
        //     decoded.key_id[i] = bits_to_int(bits + offset, 8);
        //     offset += 8;
        // }

        // memcpy(&status.event, &decoded, sizeof(Event));
        status.updated = true;
#ifdef DEBUG
        Serial.println(F("got response:"));
        Serial.print(F("\tavailable: "));
        Serial.println(status.event.available);
        Serial.print(F("\tsummary: "));
        Serial.println(status.event.summary);
        Serial.print(F("\ttime: "));
        Serial.println(status.event.time);
        Serial.print(F("\tcreator: "));
        Serial.println(status.event.creator);
        Serial.print(F("\tkey id: "));
        for (int i = 0; i < sizeof(status.event.key_id); ++i) {
            Serial.print((uint8_t)status.event.key_id[i], HEX);
        }
        Serial.println();
#endif
    }
}
