#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

// RFID Configuration
#define RST_PIN         9          
#define SS_PIN          10         

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD setup
MFRC522 mfrc522(SS_PIN, RST_PIN);   // RFID instance
byte accessUID[4] = {0x26, 0xBE, 0x28, 0xF9}; // Authorized RFID UID

// Fingerprint Configuration
#define RX_PIN 2
#define TX_PIN 3
SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Pins for feedback and lock control
int bluePIN = 4;
int redPIN = 5;
int buzzerPIN = 6;
int switchPI = 7;


void setup() {
  pinMode(bluePIN, OUTPUT);
  pinMode(redPIN, OUTPUT);
  pinMode(buzzerPIN, OUTPUT);
  pinMode(switchPI, OUTPUT);
  
  
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println(F("Scan PICC or place finger..."));

  mySerial.begin(57600);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found!");
  } else {
    Serial.println("Fingerprint sensor not found");
    while (1) delay(1);
  }
  finger.getTemplateCount();
  Serial.print("Fingerprints enrolled: ");
  Serial.println(finger.templateCount);
}

void loop() {
  
  // Check for RFID tag
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    if (isAuthorizedRFID()) {
      grantAccess("RFID");
    } else {
      denyAccess("RFID");
    }
    mfrc522.PICC_HaltA();
  }

  // If no RFID card, check fingerprint
  else if (getFingerprintID() > 0) {
    grantAccess("Fingerprint");
  }
  delay(100);
}

bool isAuthorizedRFID() {
  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != accessUID[i]) return false;
  }
  return true;
}

int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

void grantAccess(String method) {
  Serial.println("Access Granted by " + method);
  lcd.setCursor(0, 0);
  lcd.print("Access Granted");
  digitalWrite(bluePIN, HIGH);
  //digitalWrite(buzzerPIN, HIGH);
  digitalWrite(switchPI, HIGH);
  delay(4000);
  digitalWrite(bluePIN, LOW);
  //digitalWrite(buzzerPIN, LOW);
  digitalWrite(switchPI, LOW);
  lcd.clear();
}

void denyAccess(String method) {
  Serial.println("Access Denied by " + method);
  lcd.setCursor(0, 0);
  lcd.print("Access Denied");
  digitalWrite(redPIN, HIGH);
  digitalWrite(buzzerPIN, HIGH);
  delay(5000);
  digitalWrite(redPIN, LOW);
  digitalWrite(buzzerPIN, LOW);
  lcd.clear();
}
