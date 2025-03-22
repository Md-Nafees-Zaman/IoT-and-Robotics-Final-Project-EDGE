#include <Wire.h>
#include <RTClib.h>



RTC_DS3231 rtc;  // Create an RTC object

void setup() {
  Serial.begin(115200);
  Wire.begin();  // Start the I2C communication

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Check if the RTC has lost power and set the time if necessary
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time...");
   // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set the RTC time to compile time if lost power
  }

  Serial.println("Enter date and time in the following format:");
  Serial.println("YYYY-MM-DD HH:MM:SS");
  Serial.println("For example: 2025-02-09 15:30:00");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');  // Read the input from Serial Monitor
    input.trim();  // Trim any whitespace

    if (input.length() == 19) {
      // Parse the input (format: YYYY-MM-DD HH:MM:SS)
      int year = input.substring(0, 4).toInt();
      int month = input.substring(5, 7).toInt();
      int day = input.substring(8, 10).toInt();
      int hour = input.substring(11, 13).toInt();
      int minute = input.substring(14, 16).toInt();
      int second = input.substring(17, 19).toInt();

      // Set the RTC time
      rtc.adjust(DateTime(year, month, day, hour, minute, second));

      Serial.print("Time set to: ");
      Serial.print(year);
      Serial.print("-");
      Serial.print(month);
      Serial.print("-");
      Serial.print(day);
      Serial.print(" ");
      Serial.print(hour);
      Serial.print(":");
      Serial.print(minute);
      Serial.print(":");
      Serial.println(second);
    } else {
      Serial.println("Invalid format. Please enter time in YYYY-MM-DD HH:MM:SS format.");
    }
  }
}
