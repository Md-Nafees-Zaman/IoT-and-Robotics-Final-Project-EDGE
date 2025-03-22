HardwareSerial sim800(1);  // Using UART1, you can also use UART2 if needed
#define SIM800_RX 16  // ESP32 RX pin connected to SIM800L TX
#define SIM800_TX 17  // ESP32 TX pin connected to SIM800L RX
void setup() {
  Serial.begin(115200);  // For debugging
  sim800.begin(9600, SERIAL_8N1, SIM800_RX, SIM800_TX);  // Adjust baud rate and pin numbers

  delay(1000);
  Serial.println("Testing SIM800L with AT Command...");
  sim800.println("AT");  // Send AT command
  delay(1000);
  while (sim800.available()) {
    Serial.write(sim800.read());  // Read and print the response
  }
}

void loop() {
  // Continuously check for available data
  if (sim800.available()) {
    Serial.write(sim800.read());  // Read and print response from SIM800L
  }
}
