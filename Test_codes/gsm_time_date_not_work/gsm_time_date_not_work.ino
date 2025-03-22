#include <HardwareSerial.h>

#define RXD2 16  // ESP32 RX2 (Connected to GSM TX)
#define TXD2 17  // ESP32 TX2 (Connected to GSM RX)

HardwareSerial gsmSerial(2);  // Use UART2 for GSM module

// List of pre-registered phone numbers (Bangladeshi format)
String registeredNumbers[] = {"8801712345678", "8801812345678", "8801912345678", "8801907925486"};
int totalNumbers = sizeof(registeredNumbers) / sizeof(registeredNumbers[0]);

// Structure for storing call logs
struct CallLog {
  String phoneNumber;
  String dateTime;
};

CallLog callLogs[10];  // Array to store the call logs
int logIndex = 0;  // Index for storing logs

void setup() {
    Serial.begin(115200);
    gsmSerial.begin(9600, SERIAL_8N1, RXD2, TXD2);

    Serial.println("ESP32 GSM Call Verification System Ready!");

    // Get time from the GSM module
    getNetworkTime();
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

            // Get current date and time from GSM network
            String dateTime = getNetworkTime();

            // Save call log
            if (logIndex < 10) {
                callLogs[logIndex].phoneNumber = callerID;
                callLogs[logIndex].dateTime = dateTime;
                logIndex++;
            }

            if (isRegisteredNumber(callerID)) {
                Serial.println("✅ Verified");
            } else {
                Serial.println("❌ Not Verified");
            }

            // Print the call logs (optional for debugging)
            Serial.println("Call Log:");
            for (int i = 0; i < logIndex; i++) {
                Serial.print("Phone: ");
                Serial.print(callLogs[i].phoneNumber);
                Serial.print(" Time: ");
                Serial.println(callLogs[i].dateTime);
            }
        }
    }
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

// Function to get date and time from the GSM network
String getNetworkTime() {
    gsmSerial.println("AT+CCLK?");  // Send AT command to get the current time from the network
    delay(1000);  // Wait for a response from the GSM module

    if (gsmSerial.available()) {
        String response = gsmSerial.readString();
        Serial.println("GSM Network Time: " + response);

        // Look for "+CCLK: " in the response
        int start = response.indexOf("+CCLK: \"");
        if (start != -1) {
            start += 8;  // Move to the start of the date/time
            int end = response.indexOf("\"", start);
            if (end != -1) {
                String networkTime = response.substring(start, end);

                // Network time format is: "yy/MM/dd,hh:mm:ss+zz"
                // Example: "23/02/09,15:32:45+48"
                // Extract the necessary parts from the response

                String date = networkTime.substring(0, 5);  // "yy/MM/dd"
                String time = networkTime.substring(6, 14);  // "hh:mm:ss"
                String timeZoneOffset = networkTime.substring(15, 17);  // "zz" (timezone offset)

                // Parse the date and time
                String year = "20" + date.substring(0, 2);  // Convert "yy" to "20yy"
                String month = date.substring(3, 5);
                String day = date.substring(6, 8);

                String hour = time.substring(0, 2);
                String minute = time.substring(3, 5);
                String second = time.substring(6, 8);

                // Convert to Bangladesh Time (UTC +6)
                int hourInt = hour.toInt();
                int timeZone = timeZoneOffset.toInt();
                hourInt += (6 - timeZone);  // Adjust to Bangladesh Standard Time (UTC +6)

                if (hourInt >= 24) {
                    hourInt -= 24;  // Adjust if hour exceeds 24
                    int dayInt = day.toInt();
                    dayInt += 1;  // Move to the next day if needed
                    day = String(dayInt);
                } else if (hourInt < 0) {
                    hourInt += 24;  // Adjust if hour goes below 0 (negative)
                    int dayInt = day.toInt();
                    dayInt -= 1;  // Move to the previous day if needed
                    day = String(dayInt);
                }

                // Format the adjusted date and time
                String dateTime = year + "-" + month + "-" + day + " " + String(hourInt) + ":" + minute + ":" + second;
                return dateTime;
            }
        }
    }
    return "";
}
