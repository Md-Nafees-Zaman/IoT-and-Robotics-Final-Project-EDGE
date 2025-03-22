void waitForDeleteID() {
  while (BTSerial.available() == 0)
    ;
  String idToDeleteStr = BTSerial.readStringUntil('\n');
  while (idToDeleteStr.length() < 2) idToDeleteStr = BTSerial.readStringUntil('\n');
  int idToDelete = idToDeleteStr.toInt();
  BTSerial.println(idToDelete);
  deleteRecord(idToDelete);
}

void deleteRecord(int id) {

  File tempFile = SD.open("/userdata_temp.txt", FILE_WRITE);
  myFile = SD.open("/userdata.txt", FILE_READ);

  if (myFile && tempFile) {
    while (myFile.available()) {
      String record = myFile.readStringUntil('\n');
      record = record.substring(0, record.length() - 1);
      int recordID = record.substring(record.indexOf(',') + 1, record.indexOf('.')).toInt();
      Serial.println(recordID);
      if (recordID != id) {
        tempFile.println(record);
      }
    }
    myFile.close();
    tempFile.close();

    SD.remove("/userdata.txt");
    SD.rename("/userdata_temp.txt", "/userdata.txt");

    BTSerial.println("\nRecord deleted.");
    Serial.println("Record deleted.");
  } else {
    BTSerial.println("Error deleting record.");
    Serial.println("Error deleting record.");
  }
}