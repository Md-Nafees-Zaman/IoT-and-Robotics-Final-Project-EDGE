#include <HardwareSerial.h>
#include <Wire.h>
#include <RTClib.h>
#include <SD.h>
#include <SPI.h>
#include <LiquidCrystal_PCF8574.h>  // Include the LCD library for PCF8574
#include <Keypad.h>  // Include Keypad library

#define RXD2 16  // ESP32 RX2 (Connected to GSM TX)
#define TXD2 17  // ESP32 TX2 (Connected to GSM RX)

HardwareSerial gsmSerial(2);  // Use UART2 for GSM module
RTC_DS3231 rtc;              // RTC object for DS3231

#define SD_CS 5  // SD card chip select pin (adjust as needed)
#define LCD_ADDR 0x27  // I2C address of the LCD (common values: 0x27 or 0x3F)
#define LCD_COLUMNS 16  // LCD width (columns)
#define LCD_ROWS 2      // LCD height (rows)

String registeredNumbers[] = {"8801712345678", "8801720510987", "8801521579417", "8801907925486"};
int totalNumbers = sizeof(registeredNumbers) / sizeof(registeredNumbers[0]);

File logFile;
LiquidCrystal_PCF8574 lcd(LCD_ADDR);  // Initialize the LCD object for PCF8574

// Keypad configuration
const byte ROW_NUM    = 4;  // Four rows
const byte COLUMN_NUM = 4;  // Four columns
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte pin_rows[ROW_NUM] =  { 13, 12, 14, 27 }; // Row pins connected to ESP32 GPIO
byte pin_column[COLUMN_NUM] ={ 26, 25, 33, 32 }; // Column pins connected to ESP32 GPIO

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

void setup() {
    Serial.begin(115200);
    gsmSerial.begin(9600, SERIAL_8N1, RXD2, TXD2);

    // Initialize RTC
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    // Initialize SD card
    if (!SD.begin(SD_CS)) {
        Serial.println("SD card initialization failed!");
        while (1);
    }

    // Initialize LCD
    lcd.begin(LCD_COLUMNS, LCD_ROWS);
    lcd.setBacklight(255); 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ESP32 Call Log");

    // Open or create the log file on SD card
    logFile = SD.open("/call_log.txt", FILE_WRITE);
    if (logFile) {
        logFile.println("Call Log Start:");
        logFile.close();
    } else {
        Serial.println("Error opening file");
    }
}

void loop() {
    char key = keypad.getKey();
    if (key) {
        // If the 'A' key is pressed, show all the logs
        if (key == 'A') {
            showAllLogs();
        }
    }

    if (gsmSerial.available()) {
        String data = gsmSerial.readString();  // Read GSM response
        Serial.println("GSM Data: " + data);

        // Extract incoming phone number
        String callerID = extractCallerID(data);

        if (callerID.length() > 0) {
            Serial.print("Incoming Call from: ");
            Serial.println(callerID);

            // Get current time and date from RTC
            String currentTime = getCurrentTime();

            // Check if the caller is registered
            if (isRegisteredNumber(callerID)) {
                Serial.println("✅ Verified");
                logCallData(callerID, currentTime, "Verified");
                displayLogData(callerID, currentTime, "Verified");
            } else {
                Serial.println("❌ Not Verified");
                logCallData(callerID, currentTime, "Not Verified");
                displayLogData(callerID, currentTime, "Not Verified");
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

// Function to get current time and date from RTC
String getCurrentTime() {
    DateTime now = rtc.now();
    char dateTime[20];
    sprintf(dateTime, "%02d/%02d/%04d %02d:%02d:%02d", now.day(), now.month(), now.year(),
            now.hour(), now.minute(), now.second());
    return String(dateTime);
}

// Function to log call data to SD card
void logCallData(String callerID, String callTime, String verificationStatus) {
    logFile = SD.open("/call_log.txt", FILE_WRITE);
    if (logFile) {
        logFile.print("Index: ");
        logFile.print(millis() / 1000);  // Using system uptime as an index
        logFile.print(", Caller: ");
        logFile.print(callerID);
        logFile.print(", Time: ");
        logFile.print(callTime);
        logFile.print(", Status: ");
        logFile.println(verificationStatus);
        logFile.close();
    } else {
        Serial.println("Error opening file for writing");
    }
}

// Function to display the latest log data on the LCD
void displayLogData(String callerID, String callTime, String verificationStatus) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Caller: ");
    lcd.print(callerID.substring(0, 10));  // Display only the first 10 digits for brevity

    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.print(callTime.substring(0, 16));  // Display only the date and time portion

    delay(2000);  // Display the info for 2 seconds
    lcd.clear();  // Clear LCD for next update
}

// Function to show all the logs from the SD card
void showAllLogs() {
    File logFile = SD.open("/call_log.txt", FILE_READ);
    if (logFile) {
        int lineCount = 0;
        String logData = "";
        while (logFile.available()) {
            char c = logFile.read();
            logData += c;

            // Display the log in chunks on the LCD (with paging if necessary)
            if (logData.length() > 32) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(logData.substring(0, 16));  // First 16 chars on the first line
                lcd.setCursor(0, 1);
                lcd.print(logData.substring(16, 32)); // Next 16 chars on the second line
                delay(2000);  // Wait for 2 seconds before scrolling
                logData = logData.substring(32);  // Remove the displayed portion
            }
        }
        logFile.close();
    } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Error opening log");
        delay(2000);
    }
}
