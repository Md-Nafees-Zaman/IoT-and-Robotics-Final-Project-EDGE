#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>  // ESP32-Compatible Library
#include <Keypad.h>
#include <RTClib.h>
#include <SD.h>
#include <ESP32Servo.h>  // if servo is used
#include <SPI.h>
#include <BluetoothSerial.h>
#include <HardwareSerial.h>

#define relay_pin 4
#define ir_pin 35
#define call_status_led 15
#define RXD2 16        // ESP32 RX2 (Connected to GSM TX)
#define TXD2 17        // ESP32 TX2 (Connected to GSM RX)
#define BUILTIN_LED 2  // Built-in LED pin for most ESP32 boards (GPIO 2)

HardwareSerial sim800(1);         // Use UART2 for GSM module
LiquidCrystal_PCF8574 lcd(0x27);  // Adjust address if needed
RTC_DS3231 rtc;
BluetoothSerial BTSerial;
//String preSavedNumbers[] = { "+8801907925486", "+1987654321", "+1122334455" };
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
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(ir_pin, INPUT);
  pinMode(relay_pin, OUTPUT);
  pinMode(call_status_led, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  digitalWrite(relay_pin, HIGH);
  digitalWrite(call_status_led, HIGH);

  Serial.begin(115200);

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
  delay(9000);

  sim800.begin(9600, SERIAL_8N1, RXD2, TXD2);

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

  lcd.clear();
  lcd.print("System Ready");

  // myFile = SD.open("/userdata.txt", FILE_WRITE);
  // if (myFile) {

  //   // myFile.println("Phone Number, Id. Name : ");
  //   myFile.close();
  // } else Serial.println("Error writing data in userdata.txt");
}

void loop() {

  bluetooth_connection();
  lcd_out();
  gsm_op();
  delay(20);
}
