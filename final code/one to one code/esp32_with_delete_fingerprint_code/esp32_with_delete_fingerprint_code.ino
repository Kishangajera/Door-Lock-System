#include <Adafruit_Fingerprint.h>

// Define RX and TX pins for ESP32
#define Finger_Rx 16  // GPIO16
#define Finger_Tx 17  // GPIO17

// Use hardware serial port for the fingerprint sensor
HardwareSerial mySerial(2); // Use UART2 for the fingerprint sensor
Adafruit_Fingerprint finger(&mySerial);

void setup() {
  // Start serial communication with PC
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\n\nFingerprint Sensor - Delete Finger Example");

  // Start serial communication with the fingerprint sensor
  mySerial.begin(57600, SERIAL_8N1, Finger_Rx, Finger_Tx);

  // Initialize the fingerprint sensor
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found and verified!");
  } else {
    Serial.println("Could not find fingerprint sensor. Check wiring!");
    while (1) delay(10); // Halt execution here if no sensor found
  }
}

void loop() {
  Serial.println("Enter the ID # (1 to 1000) of the fingerprint to delete:");

  uint16_t id = readNumber();
  if (id < 1 || id > 1000) {
    Serial.println("Invalid ID. Must be between 1 and 1000.");
    return;
  }

  Serial.printf("Deleting fingerprint ID #%d...\n", id);
  uint8_t result = deleteFingerprint(id);

  if (result == FINGERPRINT_OK) {
    Serial.println("Fingerprint deleted successfully!");
  } else if (result == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error with fingerprint sensor.");
  } else if (result == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete fingerprint at that location.");
  } else if (result == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to sensor flash memory.");
  } else {
    Serial.printf("Unknown error occurred: 0x%02X\n", result);
  }

  delay(2000); // Add a delay before next operation
}

uint16_t readNumber() {
  uint16_t num = 0;
  while (num == 0) {
    while (!Serial.available());
    num = (uint16_t)Serial.parseInt(); // Ensure full range for ID
  }
  return num;
}

uint8_t deleteFingerprint(uint16_t id) {
  return finger.deleteModel(id);
}
