bool phoneNumberExists(String phoneNumber) {
  myFile = SD.open("/userdata.txt", FILE_READ);
  if (!myFile) {
    Serial.println("Error opening file.");
    return false;
  }
  user_count = 0;
  while (myFile.available()) {
    String record = myFile.readStringUntil('\n');

    p_number = record.substring(0, record.indexOf(',') - 1);

    delay(20);
    if (p_number == phoneNumber) {
      record = record.substring(record.indexOf(',') + 1, record.indexOf(':') - 2) + " : " + record.substring(0, record.indexOf(',') - 1);
      record2 = record;
      myFile.close();
      return true;
    }
  }
  myFile.close();
  return false;
}
