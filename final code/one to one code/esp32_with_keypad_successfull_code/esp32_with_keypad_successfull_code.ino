#include <Keypad.h>

const byte ROWS = 4;  // Four rows
const byte COLS = 4;  // Four columns

// Define the symbols on the buttons of the keypad
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Connect keypad rows and columns to these ESP8266 GPIO pins
byte rowPins[ROWS] = {13, 12, 14, 27};  // GPIO pins for rows (D22, D21 , D19, D18)
byte colPins[COLS] = {26, 25, 33, 32};  // GPIO pins for columns (D5, TX2, RX2, D4)


// Create the keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  Serial.println("Keypad Test: Press keys on the keypad.");
}

void loop() {
  char key = keypad.getKey();

  // If a key is pressed
  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);
  }
}
