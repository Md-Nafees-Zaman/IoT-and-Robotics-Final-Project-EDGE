
void gsm_op() {
  // Continuously check for incoming calls
  if (sim800.available()) {

    //    if (!gsm_time_flag) {
    //      gsm_time_flag = 1;
    //      gsm_time = millis();
    //    }
    //    if (millis() - gsm_time < 4000)
    //{
    String callResponse = "";

    // Read the response from the SIM800L when a call is received
    while (sim800.available()) {
      char c = sim800.read();  // Read each character from the SIM800L
      callResponse += c;       // Add the character to the response string
    }

    // Print the full response from SIM800L to Serial Monitor
    Serial.println("Call Response:");
    //Serial.println(callResponse);

    // Check if it's an incoming call (response format: +CLIP: "<number>",<type>)
    if (callResponse.indexOf("+CLIP:") != -1) {
      String callerNumber = extractPhoneNumber(callResponse);
      // Serial.print("Incoming call from: ");
      // Serial.println(callerNumber);

      if (callerNumber.length() > 0) {
        digitalWrite(call_status_led, LOW);
        timer = millis();
        DateTime now = rtc.now();
        Serial.print("Incoming Call from: ");
        Serial.println(callerNumber);

        myFile = SD.open("/logs.txt", FILE_APPEND);

        if (phoneNumberExists(callerNumber)) {
          Serial.println(record2);
          digitalWrite(BUILTIN_LED, HIGH);
          flag2 = 1;
          if (! lock_opened)
            digitalWrite(relay_pin, LOW);

          //Serial.println(digitalRead(ir_pin));

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
              myFile.println(callerNumber);
          }
        }

        if (!flag) {
          String date = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day());
          String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
          myFile.println(date + "  " + time);
          myFile.close();
          flag = 1;
        }
      } else {
      }
      // Check if the incoming number is in the pre-saved list
      // if (isNumberInList(callerNumber)) {
      //   Serial.println("Match found! Turning on the LED.");
      //   digitalWrite(BUILTIN_LED, HIGH);  // Turn on the LED
      // } else {
      //   Serial.println("No match found. LED will stay off.");
      //   digitalWrite(BUILTIN_LED, LOW);  // Turn off the LED
      // }
    }

    delay(5000);
    //  }
    //  else gsm_time_flag = 0;
  }

  if (flag && millis() - timer > 7000) {
    digitalWrite(BUILTIN_LED, LOW);
    digitalWrite(call_status_led, HIGH);
    digitalWrite(relay_pin, HIGH);
    flag = 0;
    flag2 = 0;
    lock_opened = 0;
  }

  if (digitalRead(ir_pin)) {
    if (!open_flag) {
      open_flag = 1;
      open_time = millis();
    }
    lock_opened = 1;
    // open_time_flag =
  }

  //  if (flag) {
  //    if (millis() - timer > 7000) {
  //      digitalWrite(BUILTIN_LED, LOW);
  //      digitalWrite(call_status_led, HIGH);
  //      digitalWrite(relay_pin, HIGH);
  //      flag = 0;
  //      flag2 = 0;
  //    } else if (!digitalRead(ir_pin) && flag2) digitalWrite(relay_pin, LOW);
  // else {
  //   digitalWrite(relay_pin, HIGH);
  //   flag2 = 0;
  // }
  ///}
}


// Function to extract phone number from the CLIP response
String extractPhoneNumber(String response) {
  int startIndex = response.indexOf("\"") + 1;        // Start after the first quote
  int endIndex = response.indexOf("\"", startIndex);  // Find the closing quote

  if (startIndex != -1 && endIndex != -1) {
    return response.substring(startIndex, endIndex);  // Extract phone number
  } else {
    return "Unknown";
  }
}

// Function to check if the incoming number matches any pre-saved numbers
// bool isNumberInList(String number) {
//   for (int i = 0; i < sizeof(preSavedNumbers) / sizeof(preSavedNumbers[0]); i++) {
//     if (preSavedNumbers[i] == number) {
//       return true;  // Number matched
//     }
//   }
//   return false;  // No match found
// }
