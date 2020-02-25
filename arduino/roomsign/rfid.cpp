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
    Serial.print(F("UID tag :"));
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    
    Serial.println();
    Serial.print(F("Message : "));
    content.toUpperCase();
    
    /*
    key tag:    89 7F 97 D5
                09 ED 19 B9
                39 59 1A B9
                39 D5 31 C2
                59 C1 B4 C2
                39 3B 2F C2
    card tag:   B7 08 7B 44
    */
    if (content.substring(1) == "89 7F 97 D5") {
        Serial.println(F("Access granted"));
        status.authorized = true;
    } else {
        Serial.println(F("Access denied"));
    }
}
