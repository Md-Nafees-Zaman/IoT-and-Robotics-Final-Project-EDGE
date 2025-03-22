void showAllRecords() {
  Serial.println("Fetching all records...");
  BTSerial.println("\nList of all Registered Numbers:\n");

  myFile = SD.open("/userdata.txt", FILE_READ);
  if (myFile) {
    while (myFile.available()) {
      String record = myFile.readStringUntil('\n');
      record = record.substring(record.indexOf(',') + 1, record.indexOf(':') - 2) + " : " + record.substring(0, record.indexOf(',') -1);
      BTSerial.println(record);
    }
    myFile.close();
  } else {
    Serial.println("Error opening file.");
    BTSerial.println("Error reading file.");
  }
}
