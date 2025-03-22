#define SIM800_RX 16  // ESP32 RX pin connected to SIM800L TX
#define SIM800_TX 17  // ESP32 TX pin connected to SIM800L RX
#define SIM800_BAUD 9600  // Baud rate for SIM800L module

HardwareSerial sim800(1);  // Use UART1 for communication

void setup() {
  Serial.begin(115200);  // For debugging
  sim800.begin(SIM800_BAUD, SERIAL_8N1, SIM800_RX, SIM800_TX);  // Initialize SIM800L communication
  
  delay(1000);
  Serial.println("Initializing SIM800L...");
  
  // Test if SIM800L is responding
  sim800.println("AT");  // Send AT command
  delay(1000);
  
  while (sim800.available()) {
    Serial.write(sim800.read());  // Print response from SIM800L
  }
  
  // Set the SMS message format to text mode
  sim800.println("AT+CMGF=1");  
  delay(100);
  Serial.println("SMS format set to text mode.");
  
  // Set the SIM800L to read all SMS
  sim800.println("AT+CMGL=\"ALL\"");  // Read all messages
  delay(1000);
  Serial.println("Reading all SMS...");

  // Read and display any SMS messages received
  while (sim800.available()) {
    Serial.write(sim800.read());  // Print the SMS content to Serial Monitor
  }
}

void loop() {
  // Continuously check for any new incoming SMS messages
  if (sim800.available()) {
    String smsContent = "";
    
    // Read the SMS message
    while (sim800.available()) {
      char c = sim800.read();  // Read each character from the SIM800L
      smsContent += c;  // Add the character to the smsContent string
    }
    
    Serial.println("New SMS Received:");
    Serial.println(smsContent);  // Print the received SMS content
    
    // Here, you can add additional logic to process the SMS content, 
    // such as checking for specific keywords or commands within the message.
  }
  
  delay(10000);  // Check for new messages every 10 seconds
}
