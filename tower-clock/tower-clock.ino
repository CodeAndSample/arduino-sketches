
/* Ticking Precision: Managing a Church Clock with Arduino
**
** This is a simple Arduino sketch that will manage a church clock.
** The Arduino manages a Mannhardt church clock, holding the pendulum
** near 12 o'clock to ensure near-accurate display and chime.
** Characteristics include a 1.25-second pendulum swing, an escapement
** wheel with 90 pins, and a 12-hour reset cycle. The implementation
** involves setting the clock 1-3 minutes ahead, tracking pendulum
** oscillations, halting and restarting the clock in 12-hour cycles,
** and using interrupt control and precise time sources.
**
**
** Useful for file name
**		` SD.open(now.timestamp()+".log", FILE_WRITE) `
**
**
** Created: 2023-08-28 by codeandsample
** Last Edit: 2023-08-28
**
*/

#include <RTClib.h> // 실시간시계 라이브러리
#include <Wire.h>   // I2C통신 라이브러리
// #include "NewPing.h"

// #include <LiquidCrystal.h>

// LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
RTC_DS1307 rtc;

#define ECHO_PIN 2
#define TRIG_PIN 3
#define SERIAL_OPTION 0
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define OSCILLATIONS 3   // This should be normally 17.280
#define DELAY_TIME 1250  // Delay between each measurement in milliseconds 1250 = 1.25 seconds

int counter = 0;           // Counter to keep track of proximity detections
int counter_six_hours = 0; // Counter to keep track of proximity detections
int distanceThreshold = 5; // The threshold distance (in centimeters) to trigger a count. Adjust this as needed.

void setup()
{
    // set up for real-time clock
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    Serial.begin(9600);

#ifndef ESP8266
    while (!Serial)
        ; // wait for serial port to connect. Needed for native USB
#endif

    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        abort();
    }

    if (!rtc.isrunning())
    {
        Serial.println("RTC is NOT running, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }

    // When time needs to be re-set on a previously configured device, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    Serial.println("RTC is running, let's set the time!");
    rtc.begin();
    // lcd.begin(16, 2);

    // lcd.print("Hello World!");
}

float readDistanceCM()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    int duration = pulseIn(ECHO_PIN, HIGH);
    return duration * 0.034 / 2;
}

void loop()
{

    // set time only as seconds counting from midnight
    DateTime now = rtc.now();

    DateTime endNoon(now.year(), now.month(), now.day(), 12, 00, 0);
    DateTime endMidnight(now.year(), now.month(), now.day(), 24, 00, 0);

    // Full Timestamp of midnight and noon
    // Serial.println(String("DateTime::TIMESTAMP_TIME:\t") + endNoon.timestamp(DateTime::TIMESTAMP_FULL));
    // Serial.println(String("DateTime::TIMESTAMP_TIME:\t") + endMidnight.timestamp(DateTime::TIMESTAMP_FULL));

    // Full Timestamp
    long differenceInSecondsBetweenNightAndDay = endNoon.unixtime() - endMidnight.unixtime();
    long differenceInSecondsNoon = endNoon.unixtime() - now.unixtime();
    long differenceInSecondsMidnight = endMidnight.unixtime() - now.unixtime();

    Serial.println(String("Difference in seconds between noon and midnight:\t") + differenceInSecondsBetweenNightAndDay);
    Serial.println(String("Difference in seconds between now and noon:\t") + differenceInSecondsNoon);
    Serial.println(String("Difference in seconds between now and midnight:\t") + differenceInSecondsMidnight);

    // loop for real time clock
    float distance = readDistanceCM();

    // bool isNearby = distance < 10;

    Serial.print("Measured distance: ");
    Serial.println(readDistanceCM());

    // This is the original code from the proximity sensor example
    // if (distance < 10)
    // {
    //     digitalWrite(LED_BUILTIN, HIGH);
    //     digitalWrite(13, HIGH);
    // }
    // else
    // {
    //     digitalWrite(LED_BUILTIN, LOW);
    //     digitalWrite(13, LOW);
    // }

    // delay(50);

    // If an object is detected closer than the threshold, increase the counter
    if (distance > 0 && distance < distanceThreshold)
    {
        // Increment the counter
        counter++;

        Serial.println("It's close!");

        // Turn on the LED if the counter reaches the threshold
        if (counter >= OSCILLATIONS)
        {
            digitalWrite(LED_BUILTIN, HIGH);
            Serial.println("LED on");

            // Check if the time is 12:00:00 (either midnight or noon)
        }
        // Check if the difference between now and noon is positive, else use midnight as an orientation
        if (differenceInSecondsNoon > 0)
        {
            // Check if the difference between now and noon is less than 2 seconds
            if (differenceInSecondsNoon < 3)
            {
                // If the time is 12:00:00, reset the counter and turn off the LED. I. e. release the pendulum.
                digitalWrite(LED_BUILTIN, LOW);
                counter = 0; // Reset the counter if it reaches 17280
            }
        }
        else
        {
            // Check if the difference between now and midnight is less than 2 seconds
            if (differenceInSecondsMidnight > 0 && differenceInSecondsMidnight < 3)
            {
                // If the time is 24:00:00, reset the counter and turn off the LED. I. e. release the pendulum.
                digitalWrite(LED_BUILTIN, LOW);

                counter = 0; // Reset the counter if it reaches 17280
            }
        }
        // Debug information to show the counter
        Serial.print("Counter: ");
        Serial.println(counter);
    }

    // Debug information to show the distance
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println("cm");

    // Datetime
    // if (SERIAL_OPTION)
    // {
    //     Serial.print(now.year());
    //     Serial.print("/");
    //     Serial.print(now.month());
    //     Serial.print("/");
    //     Serial.print(now.day());
    //     Serial.print(" ");

    //     Serial.print(now.hour());
    //     Serial.print(":");
    //     Serial.print(now.minute());
    //     Serial.print(":");
    //     Serial.print(now.second());
    //     Serial.print("\n");
    // }

    // lcd.setCursor(0, 0);
    // lcd.print("DATE: ");
    // lcd.print(now.year());
    // lcd.print("/");
    // lcd.print(now.month());
    // lcd.print("/");
    // lcd.print(now.day());
    // lcd.setCursor(0, 1);
    // lcd.print("TIME: ");
    // lcd.print(now.hour());
    // lcd.print(":");
    // lcd.print(now.minute());
    // lcd.print(":");
    // lcd.print(now.second());
    // lcd.print("Hello World!");
    // // set the cursor to column 0, line 1
    // // (note: line 1 is the second row, since counting begins with 0):
    // lcd.setCursor(0, 1);
    // // print the number of seconds since reset:
    // lcd.print(millis() / 1000);
    // Delay
    delay(DELAY_TIME);
}