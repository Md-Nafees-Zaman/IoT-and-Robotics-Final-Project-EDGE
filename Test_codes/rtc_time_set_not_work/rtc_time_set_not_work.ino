
#include <RTClib.h>
RTC_DS3231 rtc;
int flag = 0;
void setup() {
  Serial.begin(115200);

  // SETUP RTC MODULE
  if (!rtc.begin()) {
    Serial.println("RTC module is NOT found");
    Serial.flush();
    while (true)
      ;
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // automatically sets the RTC to the date & time on PC this sketch was compiled
}

void loop() {

  DateTime now = rtc.now();

  int year = now.year();
  int month = now.month();
  int day = now.day();
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();


  Serial.print("Date: ");
  Serial.print(year);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.print(day);


  Serial.print("Time: ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.print(second);

  delay(1000);  // Update every second
}
