#include <SoftwareSerial.h>

// Define the built-in LED pin on Arduino Uno (usually pin 13)
#define LED_PIN 13

// Create a SoftwareSerial object for GSM communication
SoftwareSerial gsmSerial(3, 2);  // SIM800L Tx & Rx connected to Arduino pins 3 & 2

unsigned long timeCount = 0;
bool flag = 0;

void setup() {
  // Start serial communication for debugging (via USB)
  Serial.begin(9600);

  // Start the SoftwareSerial communication with the GSM module (9600 baud is common)
  gsmSerial.begin(9600);

  // Set the LED pin as an output
  pinMode(LED_PIN, OUTPUT);

  // Wait for GSM module to initialize
  delay(10000);

  // Send the "AT+CMGF=1" command to set the GSM module to text mode
  gsmSerial.println("AT+CMGF=1");  // Set message format to text mode
  delay(1000);                     // Wait for the command to take effect

  // Enable the display of the calling phone number using AT+CLIP command
  gsmSerial.println("AT+CLIP=1");  // Request to display caller's phone number
  delay(1000);                     // Wait for the command to take effect

  // Serial.println("GSM Call Detection Initialized...");
}

void loop() {
  // Check if there is incoming data from the GSM module
  if (gsmSerial.available()) {
    String data = gsmSerial.readString();  // Read GSM response
                                           // Serial.println("GSM Data: " + data);

    // Extract incoming phone number
    String callerID = extractCallerID(data);

    if (callerID.length() > 0) {

      Serial.println(callerID);
       timeCount = millis();

      if (!flag) {
        digitalWrite(13, HIGH);
        flag = 1;
      }
    }
  }

  if (flag && millis() - timeCount > 5000) {
    flag = 0;
    digitalWrite(13, LOW);
  }

  delay(10);
}



// Function to extract caller ID from GSM response
String extractCallerID(String gsmResponse) {
  int start = gsmResponse.indexOf("+CLIP: \"");  // Find "+CLIP: " in response
  if (start == -1) return "";                    // No caller ID found

  start += 8;  // Move to the start of the number
  int end = gsmResponse.indexOf("\"", start);
  if (end == -1) return "";  // Invalid format

  String number = gsmResponse.substring(start, end);  // Extract number

  // Remove the "+" if present
  if (number.startsWith("+")) {
    number.remove(0, 1);
  }

  return number;
}
