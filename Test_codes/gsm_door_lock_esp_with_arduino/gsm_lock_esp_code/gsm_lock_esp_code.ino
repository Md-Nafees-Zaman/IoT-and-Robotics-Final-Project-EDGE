#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>  // ESP32-Compatible Library
#include <Keypad.h>
#include <RTClib.h>
#include <SD.h>
#include <ESP32Servo.h>
#include <SPI.h>
#include <BluetoothSerial.h>
#include <HardwareSerial.h>

#define relay_pin 4
#define ir_pin 35
#define call_status_led 15
#define RXD2 16  // ESP32 RX2 (Connected to GSM TX)
#define TXD2 17  // ESP32 TX2 (Connected to GSM RX)

// #define RXD2 3  // ESP32 RX2 (Connected to GSM TX)
// #define TXD2 1  // ESP32 TX2 (Connected to GSM RX)

HardwareSerial ArduinoSerial(2);  // Use UART2 for GSM module
LiquidCrystal_PCF8574 lcd(0x27);  // Adjust address if needed
RTC_DS3231 rtc;
BluetoothSerial BTSerial;

File myFile;
int TOTAL_ID = 0;
#define SD_CS 5
#define BT_NAME "Smart Door Lock"

int userID = 105;  // Auto-incremented ID
String p_number;
int user_count = 0;
String record2, pass_word = "4545", lcd_record_del;
bool flag = 0, flag2 = 0;
unsigned long timer = 0;
int pass_ok = 0;

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
String adminPassword = "4545";
bool isAdmin = false;


void setup() {
  Wire.begin(21, 22);  // ESP32 default I2C pins (SDA: 21, SCL: 22)
  lcd.begin(16, 2);    // Initialize 16x2 LCD
  lcd.setBacklight(255);
  lcd.setCursor(0, 0);
  lcd.print("GSM Door Lock");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  pinMode(2, OUTPUT);
  pinMode(ir_pin, INPUT);
  pinMode(relay_pin, OUTPUT);
  pinMode(call_status_led, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(relay_pin, HIGH);
  digitalWrite(call_status_led, HIGH);

  Serial.begin(115200);
  ArduinoSerial.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Serial.println("ESP32 GSM Call Verification System Ready!");

  BTSerial.begin(BT_NAME);
  while (!BTSerial) {
    delay(10);
  }

  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC.");
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time...");
    //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set RTC time to compile time
  }


   //file_initiate();

  myFile = SD.open("/total_id.txt", FILE_READ);
  if (!myFile)
    Serial.println("Error opening file for TOTAL_ID.");
  else {
    String t_id = myFile.readStringUntil('\n');
    TOTAL_ID = t_id.toInt();
  }

  Serial.println("Waiting for Bluetooth connection...");
  delay(15000);
  lcd.clear();
  lcd.print("System Ready");
}

void loop() {

  bluetooth_connection();
  lcd_out();
  gsm_op();
  delay(20);
}
