void show_logs()
{
  Serial.println("Fetching all records...");
  BTSerial.println("\nCall Logs:\n");

  myFile = SD.open("/logs.txt", FILE_READ);
  if (myFile) {
    while (myFile.available()) {
      String record = myFile.readStringUntil('\n');
     // record = record.substring(record.indexOf(',') + 1, record.indexOf(':') - 2) + " : " + record.substring(0, record.indexOf(',') -1);
      BTSerial.println(record.substring(0, record.length()-1));
    }
    myFile.close();
  } else {
    Serial.println("Error opening file.");
    BTSerial.println("Error reading file.");
  }
}