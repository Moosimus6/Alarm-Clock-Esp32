


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


//don't change

bool buttonpress = false;
const int color = WHITE;
DateTime now;
int hour;





//button pins
const int button1pin = 36;
const int button2pin = 39;



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



int rotation = 0;







RTC_DS1307 rtc;






void setup() {
  Serial.begin(115200);

  pinMode(buzzerpin, OUTPUT);
  pinMode(button1pin, INPUT);
  pinMode(button2pin, INPUT);


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








  //Wait for 30 seconds
  delay(loopdelay);
}
