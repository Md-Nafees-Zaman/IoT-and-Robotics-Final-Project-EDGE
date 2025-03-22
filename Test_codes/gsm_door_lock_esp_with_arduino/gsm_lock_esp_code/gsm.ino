void gsm_op() {

  if (ArduinoSerial.available()) {
    // Extract incoming phone number
    String callerID = ArduinoSerial.readString();
    callerID = callerID.substring(0, callerID.length() - 2);
    if (callerID.length() > 0) {
      digitalWrite(call_status_led, LOW);
      timer = millis();
      DateTime now = rtc.now();
      Serial.print("Incoming Call from: ");
      Serial.println(callerID);

      myFile = SD.open("/logs.txt", FILE_APPEND);

      if (phoneNumberExists(callerID)) {
        Serial.println(record2);
        digitalWrite(2, HIGH);
        flag2 = 1;

        Serial.println(digitalRead(ir_pin));

        Serial.println("✅ Verified");
        if (!flag) {
          myFile = SD.open("/logs.txt", FILE_APPEND);
          if (myFile)
            myFile.println(record2);
        }
      } else {
        Serial.println("❌ Not Verified");
        myFile = SD.open("/logs.txt", FILE_APPEND);
        if (!flag) {
          if (myFile)
            myFile.println(callerID);
        }
      }

      if (!flag) {
        String date = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day());
        String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
        myFile.println(date + "  " + time);
        myFile.close();
        flag = 1;
      }
    }
  }

  if (flag) {
    if (millis() - timer > 7000) {
      digitalWrite(2, LOW);
      digitalWrite(call_status_led, HIGH);
      digitalWrite(relay_pin, HIGH);
      flag = 0;
    } else if (!digitalRead(ir_pin) && flag2) digitalWrite(relay_pin, LOW);
    else {
      digitalWrite(relay_pin, HIGH);
      flag2 = 0;
    }
  }
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