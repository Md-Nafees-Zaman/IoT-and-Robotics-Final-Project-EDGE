#include <WiFi.h>
#include "time.h"
#include "sntp.h"


#include <RTClib.h>
#include <EEPROM.h>

RTC_DS3231 rtc;
char daysOfWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

//int buzz = 13;
//int addr = 17;


const char* ssid = "Zaman";
const char* password = "blacksheep";

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 21600;
const int daylightOffset_sec = 3600;

const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)

void timeScreen() {          // function to display Date and time in LCD screen
  DateTime now = rtc.now();  // take rtc time and print in display
 
 
 Serial.print("Time:");

 Serial.print(now.hour(), DEC);
 Serial.print(":");
 Serial.print(now.minute(), DEC);
 Serial.print(":");
 Serial.print(now.second(), DEC);
 Serial.setCursor(0, 1);
 Serial.print("Date: ");
 Serial.print(now.day(), DEC);
 Serial.print("/");
 Serial.print(now.month(), DEC);
 Serial.print("/");
 Serial.print(now.year(), DEC);
  delay(500);
}
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
 
 
  int s = timeinfo.tm_sec;
  //h -= 22;
  int m = timeinfo.tm_min;
  int h = timeinfo.tm_hour;
  int M = timeinfo.tm_mon;
  M++;
  int d = timeinfo.tm_mday;
  int y = timeinfo.tm_year;
  y -= 100;
  // Serial.print(h);
  // Serial.print(" : ");
  // Serial.print(m);
  // Serial.print(" : ");
  // Serial.print(s);
  // Serial.print(" : ");
  // Serial.print(d);
  // Serial.print(" : ");
  // Serial.print(M);
  // Serial.print(" : ");
  // Serial.print(y);
  // Serial.println(" : ");
  rtc.adjust(DateTime(y, M, d, h, m, s));
  //Serial.print(s, DEC);
  //Serial.print(":");
  //Serial.print(m, DEC);
  //Serial.print(":");
  //Serial.print(h, DEC);
  // //lcd.println(&timeinfo, %B %d %Y");
  //Serial.setCursor(0, 1);
  //lcd.println(&timeinfo, %a %H:%Y:%S");
}

// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval* t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

void setup() {
  Serial.begin(115200);
  if (!rtc.begin()) {
    Serial.println("RTC module is NOT found");
    Serial.flush();
    while (1)
      ;
  }

 

  // set notification call-back function
  sntp_set_time_sync_notification_cb(timeavailable);

  /**
   * NTP server address could be aquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 aquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE aquired NTP server address
   */
  sntp_servermode_dhcp(1);  // (optional)

  /**
   * This will set configured ntp servers and constant TimeZone/daylightOffset
   * should be OK if your time zone does not need to adjust daylightOffset twice a year,
   * in such a case time adjustment won't be handled automagicaly.
   */
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  /**
   * A more convenient approach to handle TimeZones with daylightOffset 
   * would be to specify a environmnet variable with TimeZone definition including daylight adjustmnet rules.
   * A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
   */
  //configTzTime(time_zone, ntpServer1, ntpServer2);

  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  int cnt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    timeScreen();
    delay(1000);
    Serial.print(".");
    cnt++;
    if(cnt>40) break;
  }
  if(cnt < 40) Serial.println(" CONNECTED");
}
int flag = 0;
void loop() {
   timeScreen();
  delay(1000);
  if (flag == 0)
    printLocalTime();  // it will take some time to sync time :)
  flag = 1;
 
}
