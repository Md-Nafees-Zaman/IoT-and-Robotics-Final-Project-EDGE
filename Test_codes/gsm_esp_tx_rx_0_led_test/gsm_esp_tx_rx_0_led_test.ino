// Define the built-in LED pin (ESP32 has the built-in LED on GPIO 2)
#define LED_PIN 2

// Create a HardwareSerial object for GSM communication (UART0 in this case)
HardwareSerial gsmSerial(1);  // Use UART1

void setup() {
  // Start serial communication for debugging via UART0
 // Serial.begin(115200);
  
  // Start GSM communication via UART1 (9600 baud is a common default for GSM)
  gsmSerial.begin(9600, SERIAL_8N1, 3, 1);  // Use GPIO 3 (RX) and GPIO 1 (TX)
  
  // Set the LED pin as an output
  pinMode(LED_PIN, OUTPUT);
  
  // Wait for GSM module to initialize
  // delay(3000);
  // Serial.println("GSM Call Detection Initialized...");
  
  // Set GSM module to "Text Mode" for easier command handling
  gsmSerial.println("AT+CMGF=1");  // Set message format to text mode
  delay(1000);
}

void loop() {
  // Check if there's any incoming data from GSM
  if (gsmSerial.available()) {
    String response = gsmSerial.readString();
    // Serial.println(response);
    
    // If an incoming call is detected, turn on the LED
    if (response.indexOf("RING") >= 0) {
      // Serial.println("Incoming call detected!");
      digitalWrite(LED_PIN, HIGH);  // Turn on the LED
      delay(500);  // Keep the LED on for a brief moment (e.g., 500ms)
      digitalWrite(LED_PIN, LOW);   // Turn off the LED
    }
  }
}
