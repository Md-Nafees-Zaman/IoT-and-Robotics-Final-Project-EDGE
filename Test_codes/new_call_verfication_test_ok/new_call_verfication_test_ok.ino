#define SIM800_RX 16  // ESP32 RX pin connected to SIM800L TX
#define SIM800_TX 17  // ESP32 TX pin connected to SIM800L RX
#define SIM800_BAUD 9600  // Baud rate for SIM800L module

#define BUILTIN_LED 2  // Built-in LED pin for most ESP32 boards (GPIO 2)

HardwareSerial sim800(1);  // Use UART1 for communication

// Pre-saved phone numbers (modify these with actual numbers)
String preSavedNumbers[] = {"+8801907925486", "+1987654321", "+1122334455"};

void setup() {
  Serial.begin(115200);  // For debugging
  sim800.begin(SIM800_BAUD, SERIAL_8N1, SIM800_RX, SIM800_TX);  // Initialize SIM800L communication

  pinMode(BUILTIN_LED, OUTPUT);  // Set the built-in LED as output
  
  delay(1000);
  Serial.println("Initializing SIM800L...");

  // Test if SIM800L is responding
  sim800.println("AT");  // Send AT command
  delay(1000);
  
  while (sim800.available()) {
    Serial.write(sim800.read());  // Print response from SIM800L
  }
  
  // Enable Caller Line Identification (CLIP) to show caller's number
  sim800.println("AT+CLIP=1");  // Enable Caller ID presentation
  delay(100);
  Serial.println("Caller Line Identification (CLIP) enabled.");
}

void loop() {
  // Continuously check for incoming calls
  if (sim800.available()) {
    String callResponse = "";

    // Read the response from the SIM800L when a call is received
    while (sim800.available()) {
      char c = sim800.read();  // Read each character from the SIM800L
      callResponse += c;  // Add the character to the response string
    }

    // Print the full response from SIM800L to Serial Monitor
    Serial.println("Call Response:");
    Serial.println(callResponse);
    
    // Check if it's an incoming call (response format: +CLIP: "<number>",<type>)
    if (callResponse.indexOf("+CLIP:") != -1) {
      String callerNumber = extractPhoneNumber(callResponse);
      Serial.print("Incoming call from: ");
      Serial.println(callerNumber);

      // Check if the incoming number is in the pre-saved list
      if (isNumberInList(callerNumber)) {
        Serial.println("Match found! Turning on the LED.");
        digitalWrite(BUILTIN_LED, HIGH);  // Turn on the LED
      } else {
        Serial.println("No match found. LED will stay off.");
        digitalWrite(BUILTIN_LED, LOW);  // Turn off the LED
      }
    }
  }

  delay(10000);  // Check for calls every 10 seconds
}

// Function to extract phone number from the CLIP response
String extractPhoneNumber(String response) {
  int startIndex = response.indexOf("\"") + 1;  // Start after the first quote
  int endIndex = response.indexOf("\"", startIndex);  // Find the closing quote
  
  if (startIndex != -1 && endIndex != -1) {
    return response.substring(startIndex, endIndex);  // Extract phone number
  } else {
    return "Unknown";
  }
}

// Function to check if the incoming number matches any pre-saved numbers
bool isNumberInList(String number) {
  for (int i = 0; i < sizeof(preSavedNumbers) / sizeof(preSavedNumbers[0]); i++) {
    if (preSavedNumbers[i] == number) {
      return true;  // Number matched
    }
  }
  return false;  // No match found
}
