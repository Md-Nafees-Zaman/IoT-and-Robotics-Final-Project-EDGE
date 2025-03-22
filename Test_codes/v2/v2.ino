#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <Keypad.h>
#include <ESP32Servo.h>
#include <SD.h>
#define RX_PIN 16
#define TX_PIN 17
#define SD_CS 5
HardwareSerial gsmSerial(2);
LiquidCrystal_PCF8574 lcd(0x27);
Servo doorServo;

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

String adminPassword = "1234";
bool isAdmin = false;

struct CallLog {
  String number;
  String dateTime;
};

void setup() {
  Serial.begin(115200);
  gsmSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  Wire.begin(21, 22);
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  doorServo.attach(15);
  doorServo.write(0);
  pinMode(2, OUTPUT);
  lcd.print("Initializing...");

  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card failed!");
    lcd.print("SD Card Error");
    while (1)
      ;
  }
  Serial.println("SD Card initialized");

  loadPassword();
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
      if (isRegistered(incomingNumber)) {
        unlockDoor();
        logCall(incomingNumber);
      }
    }
  }
}

bool isRegistered(String number) {
  File file = SD.open("/users.txt");
  if (!file) return false;
  while (file.available()) {
    String line = file.readStringUntil('\n');
    if (line.indexOf(number) != -1) {
      file.close();
      return true;
    }
  }
  file.close();
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
    lcd.clear();
    lcd.print("1:Add 2:Remove");
    lcd.setCursor(0, 1);
    lcd.print("3:Pwd 4:Logs");
    char choice = customkeypad.waitForKey();
    if (choice == '1') addNumber();
    else if (choice == '2') removeNumber();
    else if (choice == '3') changePassword();
    else if (choice == '4') viewLogs();
  }
}

void addNumber() {
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
  Serial.println(newNumber);

  lcd.clear();
  lcd.print("Done");
  delay(2000);
  File file = SD.open("/users.txt", FILE_APPEND);
  if (!file) File file = SD.open("/users.txt", FILE_WRITE);
  lcd.clear();
  lcd.print("File opened !");
  delay(2000);
  lcd.clear();
  if (file) {
    file.println(newNumber);
    file.close();
    lcd.clear();
    lcd.print("Added !");
  } else lcd.print("Not Added !");

  delay(2000);
}

void removeNumber() {
  // Implement number removal logic
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
  File file = SD.open("/password.txt", FILE_WRITE);
  if (file) {
    file.println(newPass);
    file.close();
    adminPassword = newPass;
    lcd.clear();
    lcd.print("Password Set!");
    delay(2000);
  }
}

void viewLogs() {
  lcd.clear();
  lcd.print("Logs:");
  File file = SD.open("/logs.txt");
  if (!file) return;
  while (file.available()) {
    Serial.println(file.readStringUntil('\n'));
  }
  file.close();
  delay(5000);
}

void logCall(String number) {
  File file = SD.open("/logs.txt", FILE_APPEND);
  if (file) {
    file.println(number);
    file.close();
  }
}

void loadPassword() {
  File file = SD.open("/password.txt");
  if (file) {
    adminPassword = file.readStringUntil('\n');
    file.close();
  }
}