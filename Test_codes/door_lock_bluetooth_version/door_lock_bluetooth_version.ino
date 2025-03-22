#include <Wire.h>
#include <RTClib.h>
#include <SD.h>
#include <BluetoothSerial.h>

RTC_DS3231 rtc;  // Create RTC object
BluetoothSerial BTSerial;

File myFile;

#define SD_CS 5  // Chip select pin for SD card
#define BT_NAME "Smart Door Lock"

String userData = "";  // Temporary storage for incoming user data
int userID = 1;        // Serial ID (auto incremented)

void setup() {
  // Start serial communication
  Serial.begin(115200);
  BTSerial.begin(BT_NAME);  // Start Bluetooth Serial
  while (!BTSerial) {
    delay(10);  // Wait for Bluetooth to initialize
  }

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC.");
    while (1)
      ;
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time...");
    //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set RTC time to compile time
  }

  // Initialize the SD card file system
  // if (!SD.exists("/userdata.txt")) {
  //   myFile = SD.open("/userdata.txt", FILE_WRITE);
  //   if (myFile) {
  //     myFile.println("ID, Name, Phone Number, Date, Time");
  //     myFile.close();
  //   }
  // }

  Serial.println("Waiting for Bluetooth connection...");
}

void loop() {
  // Handle incoming Bluetooth data
  if (BTSerial.available()) {
    char incomingByte = BTSerial.read();

    if (incomingByte == 'a') {
      // Command 'a' -> Add a new record
      BTSerial.println("Enter Name:");
      waitForName();
    } else if (incomingByte == 's') {
      // Command 's' -> Show all records
      showAllRecords();
    } else if (incomingByte == 'd') {
      // Command 'd' -> Delete a record
      BTSerial.println("Enter ID to delete:");
      waitForDeleteID();
    }
  }
  delay(40);
}

// Wait for Name input from Bluetooth
void waitForName() {
  while (BTSerial.available() == 0)
    ;  // Wait for input
    delay(50);
  String name = BTSerial.readStringUntil('\n');
  while(name.length() < 3) name = BTSerial.readStringUntil('\n');
  Serial.println(name);
  // String name = "";
  // while (1) {
  //   char Byte2 = BTSerial.read();
  //   if (!((Byte2 == '\n' || Byte2 == '\0')  && name.length() < 3)) name = name + Byte2;
  //   else if ((Byte2 == '\n' || Byte2 == '\0') && name.length() >= 3) {
  //     name = name + Byte2;
  //     break;
  //   }
  // }
  
  BTSerial.print("Enter Phone Number:");
  waitForPhoneNumber(name);
}

// Wait for Phone Number input from Bluetooth
void waitForPhoneNumber(String name) {
  while (BTSerial.available() == 0)
    ;                                                   // Wait for input
  String phoneNumber = BTSerial.readStringUntil('\n');  // Read phone number input
  while(phoneNumber.length() < 11) phoneNumber = BTSerial.readStringUntil('\n');
  // Get current timestamp
  DateTime now = rtc.now();
  String date = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day());
  String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

  // Create a new record
  String record = String(userID) + ", " + name + ", " + phoneNumber + ", " + date + ", " + time;

  // Save the record to SD card
  myFile = SD.open("/userdata.txt", FILE_APPEND);
  if (myFile) {
    myFile.println(record);
    myFile.close();
    Serial.println("Record saved.");
    BTSerial.println("\n\nRecord saved.\n");
    userID++;  // Increment the ID for the next user
  } else {
    Serial.println("\n\nError opening file.");
    BTSerial.println("\n\nError saving record.");
  }
}

// Show all records from SD card
void showAllRecords() {
  Serial.println("Fetching all records from SD card...");
  BTSerial.println("List of all records:");

  myFile = SD.open("/userdata.txt", FILE_READ);
  if (myFile) {
    while (myFile.available()) {
      String record = myFile.readStringUntil('\n');
      Serial.println(record);
      record = record.substring(0, record.length()-1);
      BTSerial.println(record);
    }
    myFile.close();
  } else {
    Serial.println("Error opening file.");
    BTSerial.println("Error reading file.");
  }
}

// Wait for ID to delete from Bluetooth input
void waitForDeleteID() {
  while (BTSerial.available() == 0)
    ;                                                     // Wait for input
  String idToDeleteStr = BTSerial.readStringUntil('\n');  // Read the ID
   while(idToDeleteStr.length() < 1) idToDeleteStr = BTSerial.readStringUntil('\n');
  int idToDelete = idToDeleteStr.toInt();                 // Convert to integer

  deleteRecord(idToDelete);
}

// Delete a record based on ID
void deleteRecord(int id) {
  Serial.println("Deleting record with ID: " + String(id));
  BTSerial.println("Attempting to delete record with ID: " + String(id));

  File tempFile = SD.open("/userdata_temp.txt", FILE_WRITE);
  myFile = SD.open("/userdata.txt", FILE_READ);

  if (myFile && tempFile) {
    while (myFile.available()) {
      String record = myFile.readStringUntil('\n');
      int recordID = record.substring(0, record.indexOf(',')).toInt();
      if (recordID != id) {
        tempFile.println(record);  // Write records not matching ID to temp file
      }
    }
    myFile.close();
    tempFile.close();

    // Remove the original file and rename the temp file
    SD.remove("/userdata.txt");
    SD.rename("/userdata_temp.txt", "/userdata.txt");

    BTSerial.println("Record deleted.");
    Serial.println("Record deleted.");
  } else {
    BTSerial.println("Error deleting record.");
    Serial.println("Error deleting record.");
  }
}
