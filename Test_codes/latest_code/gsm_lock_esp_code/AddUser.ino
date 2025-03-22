void waitForName() {
  while (BTSerial.available() == 0)
    ;
  delay(50);
  String name = BTSerial.readStringUntil('\n');
  while (name.length() < 3) name = BTSerial.readStringUntil('\n');
  BTSerial.println(name.substring(0, name.length() - 1));
  BTSerial.print("\nEnter Phone Number: ");
  waitForPhoneNumber(name);
}

void waitForPhoneNumber(String name) {
  while (BTSerial.available() == 0)
    ;
  String phoneNumber = BTSerial.readStringUntil('\n');
  while (phoneNumber.length() < 11) phoneNumber = BTSerial.readStringUntil('\n');


  phoneNumber = "+88" + phoneNumber.substring(0, phoneNumber.length());
  if (phoneNumberExists(phoneNumber)) {
    BTSerial.println("\nPhone number already exists.");
    return;
  }

  BTSerial.println(phoneNumber.substring(0, phoneNumber.length() - 1));


  userID = TOTAL_ID;
  String record = phoneNumber + ", " + String(userID) + ". " + name + " : ";

  myFile = SD.open("/userdata.txt", FILE_APPEND);
  if (myFile) {
    myFile.println(record);
    myFile.close();
    Serial.println("Record saved.");
    BTSerial.println("\nRecord saved.");
    TOTAL_ID++;

    myFile = SD.open("/total_id.txt", FILE_WRITE);
    if (myFile) {
      myFile.println(TOTAL_ID);
      myFile.close();
    } else Serial.println("Error writing data in total_id.txt");


  } else {
    Serial.println("Error opening file.");
    BTSerial.println("Error saving record.");
  }
}