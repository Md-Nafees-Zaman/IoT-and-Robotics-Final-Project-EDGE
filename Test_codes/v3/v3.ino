#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>  // ESP32-Compatible Library
#include <Keypad.h>
#include <ESP32Servo.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>  // Include the RTC library

#define RX_PIN 16             // RX of ESP32 (Connect to TX of GSM)
#define TX_PIN 17             // TX of ESP32 (Connect to RX of GSM)
HardwareSerial gsmSerial(2);  // Use UART2

LiquidCrystal_PCF8574 lcd(0x27);  // Adjust address if needed
Servo doorServo;

// SD card chip select pin
#define SD_CS_PIN 5

// RTC object
RTC_DS3231 rtc;  // Using DS3231 RTC

// Define Keypad
const byte ROWS = 4;
const byte COLS = 4;
char hexakeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 13, 12, 14, 27 };
byte colPins[COLS] = { 26, 25, 33, 32 };
Keypad customkeypad = Keypad(makeKeymap(hexakeys), rowPins, colPins, ROWS, COLS);

// Admin settings
String adminPassword = "1234";
bool isAdmin = false;

// Define Call Log struct
struct CallLog {
  String number;
  String name;
  String dateTime;
};

CallLog logs[10];  // Store last 10 logs
int logIndex = 0;

// Registered Numbers
struct User {
  String number;
  String name;
};

User registeredUsers[5];  // Store up to 5 registered users
int userCount = 0;

// Function prototypes
void checkForCall();
bool isRegistered(String number);
void unlockDoor();
void adminMenu();
void addNumber();
void removeNumber();
void changePassword();
void viewLogs();
String getNetworkTime();
void writeLogToSD(CallLog log);
void readLogsFromSD();

void setup() {
  Serial.begin(115200);
  gsmSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);  // UART2 for GSM
  Wire.begin(21, 22);                                 // ESP32 default I2C pins (SDA: 21, SCL: 22)
  lcd.begin(16, 2);                                   // Initialize 16x2 LCD
  lcd.setBacklight(255);                              // Turn on backlight
  doorServo.attach(15);
  doorServo.write(0);  // Lock position
  pinMode(2, OUTPUT);
  lcd.setCursor(0, 0);
  lcd.print("GSM Door Lock");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);

  // Setup GSM
  gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CLIP=1");  // Enable Caller ID
  delay(1000);

  // Initialize SD Card
  if (!SD.begin(SD_CS_PIN)) {
    lcd.clear();
    lcd.print("SD Card Error");
    delay(2000);
  } else {
    lcd.clear();
    lcd.print("SD Card Ready");
    delay(2000);
  }

  // Initialize Log file on SD card
  File logFile = SD.open("/call_log.txt", FILE_WRITE);
  if (logFile) {
    logFile.println("Call Log Start:");
    logFile.close();
  } else {
    Serial.println("Error opening log file.");
  }

  // Initialize RTC
  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("RTC Error!");
    delay(2000);
  } else {
    lcd.clear();
    lcd.print("RTC Ready");
    delay(2000);
  }

  lcd.clear();
  lcd.print("System Ready");
}

void loop() {
  checkForCall();
  char key = customkeypad.getKey();
  if (key == 'A') {
    adminMenu();
  }
  delay(100);
}

void checkForCall() {
  if (gsmSerial.available()) {
    String callData = gsmSerial.readString();
    if (callData.indexOf("+CLIP:") != -1) {
      int start = callData.indexOf("\"") + 1;
      int end = callData.indexOf("\"", start);
      String incomingNumber = callData.substring(start, end);
      Serial.println(incomingNumber);
      if (isRegistered(incomingNumber)) {
        unlockDoor();
        logs[logIndex].number = incomingNumber;
        logs[logIndex].name = "Registered User";
        logs[logIndex].dateTime = getNetworkTime();
        writeLogToSD(logs[logIndex]);
        logIndex = (logIndex + 1) % 10;
      } else {
        Serial.println("Unauthorized Call!");
      }
    }
  }
}

bool isRegistered(String number) {
  for (int i = 0; i < userCount; i++) {
    if (registeredUsers[i].number == number) {
      return true;
    }
  }
  return false;
}

void unlockDoor() {
  lcd.clear();
  lcd.print("Door Unlocked");
  doorServo.write(90);
  digitalWrite(2, HIGH);
  delay(5000);
  digitalWrite(2, LOW);
  doorServo.write(0);
  lcd.clear();
  lcd.print("System Ready");
}

void adminMenu() {
  lcd.clear();
  lcd.print("Enter Password:");
  String enteredPass = "";
  while (enteredPass.length() < 4) {
    char key = customkeypad.getKey();
    if (key) {
      enteredPass += key;
      lcd.setCursor(enteredPass.length() - 1, 1);
      lcd.print("*");
    }
  }
  if (enteredPass == adminPassword) {
    isAdmin = true;
    lcd.clear();
    lcd.print("Admin Mode");
    delay(1000);
    lcd.clear();
    lcd.print("1:Add 2:Remove");
    lcd.setCursor(0, 1);
    lcd.print("3:Pwd 4:Logs");

    char choice = customkeypad.waitForKey();
    if (choice == '1') addNumber();
    else if (choice == '2') removeNumber();
    else if (choice == '3') changePassword();
    else if (choice == '4') viewLogs();
  } else {
    lcd.clear();
    lcd.print("Wrong Password!");
    delay(2000);
    lcd.clear();
    lcd.print("System Ready");
  }
}

void addNumber() {
  if (userCount >= 5) {
    lcd.clear();
    lcd.print("Limit Reached!");
    delay(2000);
    return;
  }

  lcd.clear();
  lcd.print("Enter Number:");
  String newNumber = "";
  while (newNumber.length() < 11) {
    char key = customkeypad.getKey();
    if (key) {
      newNumber += key;
      lcd.setCursor(newNumber.length() - 1, 1);
      lcd.print(key);
    }
  }

  newNumber = "+88" + newNumber;
  delay(1000);

  lcd.clear();
  lcd.print("Enter Name:");
  String newName = "";
  while (newName.length() < 5) {
    char key = customkeypad.getKey();
    if (key) {
      newName += key;
      lcd.setCursor(newName.length() - 1, 1);
      lcd.print(key);
    }
  }

  registeredUsers[userCount].number = newNumber;
  registeredUsers[userCount].name = newName;
  userCount++;

  lcd.clear();
  lcd.print("Added!");
  delay(2000);
}

void removeNumber() {
  lcd.clear();
  lcd.print("Enter Index:");
  char key = customkeypad.waitForKey();
  int index = key - '0';
  if (index >= 0 && index < userCount) {
    for (int i = index; i < userCount - 1; i++) {
      registeredUsers[i] = registeredUsers[i + 1];
    }
    userCount--;
    lcd.clear();
    lcd.print("Removed!");
  } else {
    lcd.clear();
    lcd.print("Invalid Index!");
  }
  delay(2000);
}

void changePassword() {
  lcd.clear();
  lcd.print("New Password:");
  String newPass = "";
  while (newPass.length() < 4) {
    char key = customkeypad.getKey();
    if (key) {
      newPass += key;
      lcd.setCursor(newPass.length() - 1, 1);
      lcd.print("*");
    }
  }
  adminPassword = newPass;
  lcd.clear();
  lcd.print("Password Set!");
  delay(2000);
}

void viewLogs() {
  lcd.clear();
  lcd.print("Logs:");
  for (int i = 0; i < 10; i++) {
    if (logs[i].number != "") {
      Serial.println("Caller: " + logs[i].number + " Time: " + logs[i].dateTime);
    }
  }
  delay(5000);
}

String getNetworkTime() {
  DateTime now = rtc.now();
  String time = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + " " + 
                String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  return time;
}

void writeLogToSD(CallLog log) {
  File logFile = SD.open("/call_log.txt", FILE_WRITE);
  if (logFile) {
    logFile.print("Number: ");
    logFile.print(log.number);
    logFile.print(", Name: ");
    logFile.print(log.name);
    logFile.print(", Time: ");
    logFile.println(log.dateTime);
    logFile.close();
  }
}

void readLogsFromSD() {
  File logFile = SD.open("/call_log.txt");
  if (logFile) {
    while (logFile.available()) {
      String logEntry = logFile.readStringUntil('\n');
      lcd.clear();
      lcd.print(logEntry);
      delay(3000);  // Adjust delay as needed
    }
    logFile.close();
  }
}
