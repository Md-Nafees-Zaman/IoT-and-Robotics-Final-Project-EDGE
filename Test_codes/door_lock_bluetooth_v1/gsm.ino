
void gsm_op() {

  if (gsmSerial.available()) {
    String data = gsmSerial.readString();  // Read GSM response
    Serial.println("GSM Data: " + data);

    // Extract incoming phone number
    String callerID = extractCallerID(data);

    if (callerID.length() > 0) {

      DateTime now = rtc.now();
      Serial.print("Incoming Call from: ");
      Serial.println(callerID);

      myFile = SD.open("/logs.txt", FILE_APPEND);

      if (phoneNumberExists(callerID)) {
        Serial.println(record2);
        digitalWrite(2, HIGH);
        Serial.println("✅ Verified");
        myFile = SD.open("/logs.txt", FILE_APPEND);
        if (myFile)
          myFile.println(record2);


        delay(90000);
        digitalWrite(2, LOW);
      } else {
        Serial.println("❌ Not Verified");
        myFile = SD.open("/logs.txt", FILE_APPEND);
        if (myFile)
          myFile.println(callerID);
        delay(90000);
      }

      String date = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day());
      String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
      myFile.println(date + "  " + time + "\n");
      myFile.close();
    }
  }
}


// Function to extract caller ID from GSM response
String extractCallerID(String gsmResponse) {
  int start = gsmResponse.indexOf("+CLIP: \"");  // Find "+CLIP: " in response
  if (start == -1) return "";                    // No caller ID found

  start += 8;  // Move to the start of the number
  int end = gsmResponse.indexOf("\"", start);
  if (end == -1) return "";  // Invalid format

  String number = gsmResponse.substring(start, end);  // Extract number

  // Remove the "+" if present
  if (number.startsWith("+")) {
    number.remove(0, 1);
  }

  return number;
}

// // Function to check if a number is registered
// bool isRegisteredNumber(String number) {
//   for (int i = 0; i < totalNumbers; i++) {
//     if (number == registeredNumbers[i]) {
//       return true;
//     }
//   }
//   return false;
// }
