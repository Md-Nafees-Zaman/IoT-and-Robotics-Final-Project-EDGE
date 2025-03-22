
#include <RTClib.h>
RTC_DS3231 rtc;


int a[12];
int count = 0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);


  if (!rtc.begin()) {
    Serial.println("RTC module is NOT found");
    Serial.flush();
    while (true)
      ;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    a[count] = Serial.read();
    Serial.println(a[count]);
   
  }
   count++;
  if (count > 5) {
    rtc.adjust(DateTime(a[0], a[1], a[2], a[3], a[4], a[5]));
    Serial.println("done");
    count = 0;
  }
  delay(20);
}
