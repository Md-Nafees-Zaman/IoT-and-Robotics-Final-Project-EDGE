#include <SoftwareSerial.h>

// Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(3, 2);  // SIM800L Tx & Rx connected to Arduino pins 3 & 2

#define LED_PIN 13  // Define LED pin (built-in LED on most Arduino boards)

void setup() {
  // Begin serial communication with Arduino IDE (Serial Monitor)
  Serial.begin(9600);

  // Begin serial communication with SIM800L
  mySerial.begin(9600);

  // Initialize LED pin as output
  pinMode(LED_PIN, OUTPUT);

  // Initialize SIM800L
  sendATCommand("AT", 1000);                 // Test communication
  sendATCommand("AT+CLIP=1", 1000);          // Enable caller ID
  sendATCommand("AT+CNMI=2,1,0,0,0", 1000);  // Enable incoming call notifications

  Serial.println("Initializing... Ready to detect calls.");
}

void loop() {
  updateSerial();
}

void updateSerial() {
  while (Serial.available()) {
    mySerial.write(Serial.read());  // Forward what Serial received to Software Serial Port
  }

  static String buffer = "";  // Persistent buffer to store incoming data

  while (mySerial.available()) {
    char incomingChar = mySerial.read();
    Serial.write(incomingChar);  // Forward what Software Serial received to Serial Port

    buffer += incomingChar;

    if (incomingChar == '\n') {  // Process complete lines
      if (buffer.indexOf("RING") != -1 || buffer.indexOf("+CLIP:") != -1) {
        Serial.println("Incoming Call!");
        digitalWrite(LED_PIN, HIGH);  // Turn on LED
        delay(2500);                  // Keep LED on for 5 seconds
        digitalWrite(LED_PIN, LOW);   // Turn off LED
      }
      buffer = "";  // Clear buffer after processing
    }
  }
}

void sendATCommand(String command, int waitTime) {
  mySerial.println(command);
  delay(waitTime);
  while (mySerial.available()) {
    Serial.write(mySerial.read());  // Display response for debugging
  }
}