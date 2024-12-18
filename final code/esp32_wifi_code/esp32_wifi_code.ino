#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Your_SSID";        // Replace with your WiFi SSID
const char* password = "Your_PASSWORD"; // Replace with your WiFi Password

// MQTT broker details
const char* mqttServer = "broker.hivemq.com"; // Public MQTT broker
const int mqttPort = 1883;                   // Default MQTT port
const char* mqttTopic = "toEsp";             // Topic to subscribe to

WiFiClient espClient; // WiFi client
PubSubClient mqttClient(espClient); // MQTT client

// ESP32 Serial communication with Arduino (UART1)
HardwareSerial SerialPort(1); // Define Serial1 for Arduino

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Initialize Serial1 for communication with Arduino (GPIO 16 & 17)
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); // RX -> GPIO16, TX -> GPIO17
  
  // Connect to WiFi
  connectToWiFi();
  
  // Configure MQTT client
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(onMQTTMessage);
  
  // Connect to MQTT broker
  connectToMQTTBroker();
}

void loop() {
  // Keep MQTT connection alive
  if (!mqttClient.connected()) {
    connectToMQTTBroker();
  }
  mqttClient.loop();
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void connectToMQTTBroker() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT broker...");
    if (mqttClient.connect("ESP32Client")) { // Unique client ID
      Serial.println("Connected");
      mqttClient.subscribe(mqttTopic); // Subscribe to the topic
      Serial.println("Subscribed to topic: " + String(mqttTopic));
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void onMQTTMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  
  // Convert payload to string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  
  // Send message to Arduino Uno
  SerialPort.println(message);
}
