#include <Adafruit_Fingerprint.h>

// Set the RX and TX pins for the fingerprint sensor (choose available GPIO pins on ESP32)
#define RX_PIN 16
#define TX_PIN 17

// Initialize HardwareSerial on UART2 (ESP32 has multiple serial ports)
HardwareSerial mySerial(2);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(115200);
  mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN); // Initialize fingerprint sensor on chosen pins
  delay(100);

  Serial.println("\n\nAdafruit Fingerprint Sensor Enrollment");

  finger.begin(57600);
  delay(5);

  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor detected!");
  } else {
    Serial.println("Fingerprint sensor not detected :(");
    while (1) { delay(1); }
  }

  Serial.println("Ready to enroll multiple fingerprints.");
  Serial.println("Type an ID number (1 to 1000) and press Enter to start enrollment.");
}

void loop() {
  if (Serial.available() > 0) {
    int id = Serial.parseInt();
    if (id < 1 || id > 1000) {
      Serial.println("Please enter a valid ID between 1 and 1000.");
      return;
    }
    enrollFingerprint(id);  // Call enroll function with the specified ID
  }
}

// Function to enroll a new fingerprint
void enrollFingerprint(int id) {
  Serial.print("Enrolling ID #"); Serial.println(id);
  delay(1000);
  
  int p = -1;
  Serial.println("Place your finger on the sensor...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      Serial.print(".");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
    } else if (p == FINGERPRINT_IMAGEFAIL) {
      Serial.println("Imaging error");
    } else if (p != FINGERPRINT_OK) {
      Serial.println("Unknown error");
    }
    delay(100);
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting image");
    return;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
    delay(100);
  }

  Serial.println("Place the same finger again...");
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      Serial.print(".");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
    } else if (p == FINGERPRINT_IMAGEFAIL) {
      Serial.println("Imaging error");
    } else if (p != FINGERPRINT_OK) {
      Serial.println("Unknown error");
    }
    delay(100);
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting image");
    return;
  }

  Serial.print("Creating model for ID #"); Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return;
  } else {
    Serial.println("Unknown error");
    return;
  }

  Serial.print("Storing model for ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Fingerprint enrolled successfully!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store at this location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash memory");
  } else {
    Serial.println("Unknown error");
  }
}
