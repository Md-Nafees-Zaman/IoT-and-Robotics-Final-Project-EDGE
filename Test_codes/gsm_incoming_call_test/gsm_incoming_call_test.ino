#include <HardwareSerial.h>

#define LED_BUILTIN 2   // ESP32's built-in LED (GPIO 2)
#define RXD2 16         // GSM TX to ESP32 RX2 (GPIO 16)
#define TXD2 17         // GSM RX to ESP32 TX2 (GPIO 17)

HardwareSerial gsmSerial(2);  // Use UART2 for GSM communication

void setup() {
    Serial.begin(115200);
    gsmSerial.begin(9600, SERIAL_8N1, RXD2, TXD2);  // Set up GSM module

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);  // LED OFF initially

    Serial.println("ESP32 GSM Call Detection Ready!");
}

void loop() {
    if (gsmSerial.available()) {
        String data = gsmSerial.readString();  // Read GSM response
        Serial.println("GSM Data: " + data);

        // Check if "RING" is received (indicates incoming call)
        if (data.indexOf("RING") != -1) {
            Serial.println("Incoming Call Detected!");
            digitalWrite(LED_BUILTIN, HIGH);  // Turn ON LED
            delay(5000);  // Keep LED ON for 5 seconds (adjustable)
            digitalWrite(LED_BUILTIN, LOW);   // Turn OFF LED
        }
    }
}
