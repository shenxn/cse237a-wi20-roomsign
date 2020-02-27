#include <SPI.h>
#include <MFRC522.h>
#include "rfid.h"
#include "status.h"

MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);


void rfidInit() {
    SPI.begin();
    mfrc522.PCD_Init();
    mfrc522.PCD_DumpVersionToSerial();
    status.authorized = false;
}


void rfidRead() {
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return;
    }

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    //Show UID on serial monitor
    Serial.print(F("UID tag: "));
    bool match = true;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        if (i >= sizeof(status.event.key_id) || mfrc522.uid.uidByte[i] != (uint8_t)status.event.key_id[i]) {
            match = false;
        }
    }

    if (match) {
        Serial.println(F(" granted"));
        status.authorized = true;
    } else {
        Serial.println(F(" denied"));
    }
}
