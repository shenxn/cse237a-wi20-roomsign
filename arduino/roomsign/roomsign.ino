#include <SPI.h>
#include <avr/power.h>
#include "radio.h"
#include "epaper.h"
#include "status.h"
#include "rfid.h"
#include "servo.h"
#include "epdif.h"
#include "macro.h"
#include "sleep.h"

void setup() {
#ifdef DEBUG
    Serial.begin(9600);
    Serial.println(F("starting"));
#endif

    // turn off not used things
    ADCSRA = 0;  // disable ADC
    power_adc_disable();
    power_twi_disable();

    // set all SS pins to high to prevent SPI conflict
    pinMode(RFID_SS_PIN, OUTPUT);
    digitalWrite(RFID_SS_PIN, HIGH);
    pinMode(CS_PIN, OUTPUT);  // epaper
    digitalWrite(CS_PIN, HIGH);
    pinMode(RADIO_CSN_PIN, OUTPUT);
    digitalWrite(RADIO_CSN_PIN, HIGH);

    SERIAL_PRINTLN(F("setup rfid"));
    rfidInit();

    SERIAL_PRINTLN(F("setup servo"));
    servoSetup();

    SERIAL_PRINTLN(F("setup epaper"));
    epaperSetup();

    // should be after epaper
    SERIAL_PRINTLN(F("setup radio"));
    radioConfigure();

    // reset clock
    status.clock = 0;
}

void loop() {
    ++status.clock;
    radioRead();
    rfidRead();
    servoUnlock();
    sleep();
}
