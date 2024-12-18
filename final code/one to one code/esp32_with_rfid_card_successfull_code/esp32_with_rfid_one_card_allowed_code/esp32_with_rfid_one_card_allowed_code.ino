#include <SPI.h>
#include <MFRC522.h>

// Define pins for RFID reader
#define SS_PIN 5
#define RST_PIN 4

MFRC522 rfid(SS_PIN, RST_PIN); // Create MFRC522 instance

// Define allowed UID
byte allowedUID[] = {0x93, 0xB1, 0xE0, 0xF5};

void setup() {
  Serial.begin(9600);  // Initialize serial communications
  SPI.begin();          // Initialize SPI bus
  rfid.PCD_Init();      // Initialize the MFRC522 reader
  Serial.println("Place your RFID card near the reader...");
}

void loop() {
  // Check if a new card is present
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Check if the scanned UID matches the allowed UID
  if (isAllowedUID(rfid.uid.uidByte, rfid.uid.size)) {
    Serial.println("Access Granted: Allowed Card Detected!");
  } else {
    Serial.println("Access Denied: Unauthorized Card.");
  }

  // Halt PICC (RFID card)
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

// Function to check if scanned UID matches allowed UID
bool isAllowedUID(byte* uid, byte uidSize) {
  if (uidSize != sizeof(allowedUID)) {
    return false; // UID size mismatch
  }

  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] != allowedUID[i]) {
      return false; // UID does not match
    }
  }
  return true; // UID matches
}
