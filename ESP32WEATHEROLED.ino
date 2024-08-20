
//to adjust time, type in the new time in this format: YYYY MM DD HH MM SS
//example: 2024 08 16 07 52 30


#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>

#include <Adafruit_GFX.h>

#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <RTClib.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//more parameters
//buzzer digital pin
const int buzzerpin = 18;


// Define your time zone offset in hours
const int timeZoneOffset = -7;  // Adjust this according to your time zone


//don't change

bool buttonpress = false;
const int color = WHITE;
DateTime now;
int hour;


//weather variables
int tempf;
String desc;
int humid;


const int tempx = 0;
const int tempy = 30;
const int humidityx = 0;
const int humidityy = 45;


int mintemp;
int maxtemp;


int page = 0;

//button pins
const int button1pin = 36;
const int button2pin = 39;


const int maxpage = 3;

//x and y coordinates for the time
const int xtime = 10;
const int ytime = 13;

//x and y coordinates for the date
const int xdate = 40;
const int ydate = 45;

//size of the time, date, and seconds
const int sizetime = 3;
const int sizedate = 1.5;
const int sizeseconds = 2;

//x and y coordinates for seconds
const int xseconds = 100;
const int yseconds = 10;


//set the time for the alarm
const int alarmhour = 6;
const int alarmminute = 25;

//alarm x and y coordinates and size
const int alarmx = 0;
const int alarmy = 55;
const int alarmsize = 1;


//loop speed
const int loopdelay = 10;



//api get delay (seconds)
const int apidelay = 180;


int rotation = 0;

int count;

uint32_t sunset;
uint32_t sunrise;



//weather coordinates
const int descsize = 2;
const int descx = 0;
const int descy = 5;
//Put your WiFi Credentials here
const char* ssid = "Marchi Madness";
const char* password = "perfecttomato722";

//URL Endpoint for the API
String URL = "https://api.openweathermap.org/data/2.5/weather?";
String ApiKey = "15cf2d015cd7d48a268e81051ae6349c";

// Replace with your location Credentials
String lat = "33.14682422003651";
String lon = "-117.29170886069518";

RTC_DS1307 rtc;






void setup() {
  Serial.begin(115200);
  count = ((apidelay * 1000) / loopdelay) + 1;
  pinMode(buzzerpin, OUTPUT);
  pinMode(button1pin, INPUT);
  pinMode(button2pin, INPUT);
  //WIFI SETUP
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //OLED setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.setRotation(rotation);
  display.clearDisplay();
  //rtc setup

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}
void alarm() {

  tone(buzzerpin, 700);
  display.invertDisplay(true);
  delay(250);
  display.invertDisplay(false);
  noTone(buzzerpin);
  delay(250);
}
void displayscreen() {
  display.clearDisplay();
  display.setTextColor(WHITE);

  if (page == 0) {

    display.setCursor(xtime, ytime);
    display.setTextSize(sizetime);
    //hour
    if (now.hour() < 13) {
      hour = now.hour();
    }
    if (now.hour() > 12) {
      hour = now.hour() - 12;
    }
    if (now.hour() == 0) {
      hour = 12;
    }
    display.print(hour);
    display.print(":");
    //minute
    if (now.minute() < 10) {
      display.print("0");
    }

    display.print(now.minute());

    //display.print(" ");
    display.setTextSize(sizeseconds);
    display.print(now.second());


    display.setCursor(xdate, ydate);
    display.setTextSize(sizedate);
    display.print(now.month());
    display.print("/");
    display.print(now.day());
    display.print("/");
    display.print(now.year());
  }
  if (page == 1) {




    display.setTextSize(descsize);
    display.setCursor(descx, descy);
    display.print(desc);

    display.setCursor(tempx, tempy);
    display.print(tempf);
    display.print((char)247);
    display.print(" F");

    display.setCursor(humidityx, humidityy);
    display.print(humid);
    display.print("% humidity");
  }

  if (page == 2) {

    display.setTextSize(1);
    display.setCursor(0, 15);
    display.print("Min of ");
    display.setTextSize(3);
    display.print(mintemp);
    display.print((char)247);
    display.setTextSize(1);
    display.print("F");


    display.setCursor(0, 40);
    display.setTextSize(1);
    display.print("Max of ");
    display.setTextSize(3);
    display.print(maxtemp);
    display.print((char)247);
    display.setTextSize(1);
    display.print("F");
  }

  if (page == 3) {
    DateTime sunsetTime = DateTime(sunset);
    DateTime sunriseTime = DateTime(sunrise);

    int localHourSunset = sunsetTime.hour() + timeZoneOffset;
    if (localHourSunset >= 24) localHourSunset -= 24;
    if (localHourSunset < 0) localHourSunset += 24;



    int localHourSunrise = sunriseTime.hour() + timeZoneOffset;
    if (localHourSunrise >= 24) localHourSunrise -= 24;
    if (localHourSunrise < 0) localHourSunrise += 24;



    if(now.hour()>localHourSunrise && now.hour()<localHourSunset){

          display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Sun Down: ");
    if (localHourSunset > 12) {

      localHourSunset = localHourSunset - 12;
    }
    if (localHourSunrise > 12) {

      localHourSunrise = localHourSunrise - 12;
    }
    display.setTextSize(2);
    display.print(localHourSunset);
    display.print(":");
    display.print(sunsetTime.minute());

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 45);
    display.print(localHourSunrise);
    display.print(":");
    display.print(sunriseTime.minute());



    }



  }









  display.display();
}
void adjustRTC() {
  if (Serial.available() >= 19) {  // Check if enough characters are available
    int yearadjust = Serial.parseInt();
    int monthadjust = Serial.parseInt();
    int dayadjust = Serial.parseInt();
    int houradjust = Serial.parseInt();
    int minuteadjust = Serial.parseInt();
    int secondadjust = Serial.parseInt();

    rtc.adjust(DateTime(yearadjust, monthadjust, dayadjust, houradjust, minuteadjust, secondadjust));
    Serial.println("RTC adjusted.");
  }
}



void loop() {
  now = rtc.now();
  adjustRTC();
  // wait for WiFi connection
  if (WiFi.status() == WL_CONNECTED && count > (apidelay * 1000) / loopdelay) {
    count = 0;
    HTTPClient http;

    //Set HTTP Request Final URL with Location and API key information
    http.begin(URL + "lat=" + lat + "&lon=" + lon + "&units=metric&appid=" + ApiKey);

    // start connection and send HTTP Request
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {

      //Read Data as a JSON string
      String JSON_Data = http.getString();
      Serial.println(JSON_Data);

      //Retrieve some information about the weather from the JSON format
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, JSON_Data);
      JsonObject obj = doc.as<JsonObject>();

      //Display the Current Weather Info
      const char* description = obj["weather"][0]["description"].as<const char*>();
      const float temp = obj["main"]["temp"].as<float>();
      const float humidity = obj["main"]["humidity"].as<float>();

      const float temp_min = obj["main"]["temp_min"].as<float>();
      const float temp_max = obj["main"]["temp_max"].as<float>();


      unsigned long sunrisetime = obj["sys"]["sunrise"].as<unsigned long>();
      unsigned long sunsettime = obj["sys"]["sunset"].as<unsigned long>();

      //put OLED displays here

      tempf = round((temp * 1.8) + 32);
      desc = description;
      humid = humidity;
      mintemp = round((temp_min * 1.8) + 32);
      maxtemp = round((temp_max * 1.8) + 32);
      sunset = sunsettime;
      sunrise = sunrisetime;


      Serial.println(sunsettime);


    } else {
      Serial.println("Error!");

      Serial.print("Can't Get DATA!");
    }

    http.end();
  }






  displayscreen();


  //alarm mechanism
  if (alarmhour == now.hour() && alarmminute == now.minute() && buttonpress == false) {
    while (buttonpress == false) {
      now = rtc.now();
      alarm();
      displayscreen();

      if (digitalRead(button1pin) == HIGH || digitalRead(button2pin) == HIGH) {

        buttonpress = true;
      }
    }

    if (now.minute() != alarmminute) {

      buttonpress = false;
    }
  }


  if (digitalRead(button2pin) == HIGH) {
    if (page >= maxpage) {
      page = 0;
    } else {
      page++;
    }
    tone(buzzerpin,900);
    delay(500);
    noTone(buzzerpin);
  }




  count++;
  //Wait for 30 seconds
  delay(loopdelay);
}
