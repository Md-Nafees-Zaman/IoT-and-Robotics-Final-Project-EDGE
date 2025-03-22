#include <HardwareSerial.h>

#define RXD2 16  // ESP32 RX2 (Connected to GSM TX)
#define TXD2 17  // ESP32 TX2 (Connected to GSM RX)

HardwareSerial gsmSerial(1);  // Use UART2 for GSM module

// List of pre-registered phone numbers (Bangladeshi format)
String registeredNumbers[] = {"8801712345678", "8801812345678", "8801912345678", "8801907925486"};
int totalNumbers = sizeof(registeredNumbers) / sizeof(registeredNumbers[0]);

void setup() {
    Serial.begin(9600);
    gsmSerial.begin(9600, SERIAL_8N1, RXD2, TXD2);

    Serial.println("ESP32 GSM Call Verification System Ready!");
}

void loop() {
    if (gsmSerial.available()) {
        String data = gsmSerial.readString();  // Read GSM response
        Serial.println("GSM Data: " + data);

        // Extract incoming phone number
        String callerID = extractCallerID(data);

        if (callerID.length() > 0) {
            Serial.print("Incoming Call from: ");
            Serial.println(callerID);

            if (isRegisteredNumber(callerID)) {
                Serial.println("✅ Verified");
            } else {
                Serial.println("❌ Not Verified");
            }
        }
    }
    delay(100);
    Serial.println("g");
}

// Function to extract caller ID from GSM response
String extractCallerID(String gsmResponse) {
    int start = gsmResponse.indexOf("+CLIP: \"");  // Find "+CLIP: " in response
    if (start == -1) return "";  // No caller ID found

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

// Function to check if a number is registered
bool isRegisteredNumber(String number) {
    for (int i = 0; i < totalNumbers; i++) {
        if (number == registeredNumbers[i]) {
            return true;
        }
    }
    return false;
}
