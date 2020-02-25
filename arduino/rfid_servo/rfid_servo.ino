#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         2
#define SS_PIN          4

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myServo;

void setup() {
  Serial.begin(115200);    // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  myServo.attach(3);
  myServo.write(0);
  Serial.println("Please put your key on the reader...");
}

void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  Serial.println();
  Serial.print("Message : ");
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
    Serial.println("Authorized access");
    Serial.println();
    myServo.write(180);
    delay(5000);
    myServo.write(0);
  }
}
