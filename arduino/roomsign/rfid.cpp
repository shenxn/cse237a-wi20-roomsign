#include <SPI.h>
#include <MFRC522.h>
#include "rfid.h"
#include "status.h"
#include "macro.h"

MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);


void rfidInit() {
    SPI.begin();
    mfrc522.PCD_Init();
    mfrc522.PCD_DumpVersionToSerial();
    status.authorized = false;

#ifdef SLEEP
    // sleep RFID
    mfrc522.PCD_SoftPowerDown();
#endif
}


void rfidRead() {
#ifdef SLEEP
    mfrc522.PCD_SoftPowerUp();
#endif

    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {

        bool match = true;
        
        // check ID only when not available
        if (!(uint8_t)status.event.available) {
            //Show UID on serial monitor
            SERIAL_PRINT(F("UID tag: "));
            for (byte i = 0; i < mfrc522.uid.size; i++) {
        #ifdef DEBUG
                Serial.print(mfrc522.uid.uidByte[i], HEX);
        #endif
                if (i >= sizeof(status.event.key_id) || mfrc522.uid.uidByte[i] != (uint8_t)status.event.key_id[i]) {
                    match = false;
                }
            }
            SERIAL_PRINT(F(" "));
        }

        if (match) {
            SERIAL_PRINTLN(F("granted"));
            status.authorized = true;
        } else {
            SERIAL_PRINTLN(F("denied"));
        }
    }

#ifdef SLEEP
    mfrc522.PCD_SoftPowerDown();
#endif
}
