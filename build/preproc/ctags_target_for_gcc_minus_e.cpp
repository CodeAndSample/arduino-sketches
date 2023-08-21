# 1 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock/tower-clock.ino"
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

# 23 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock/tower-clock.ino" 2
// #include "NewPing.h"

RTC_DS1307 rtc;






// NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

int counter = 0; // Counter to keep track of proximity detections
int distanceThreshold = 10; // The threshold distance (in centimeters) to trigger a count. Adjust this as needed.

DateTime beginning = rtc.now();
void setup()
{
    beginning = rtc.now();
    // set up for real-time clock
    pinMode(13, 0x1);
    pinMode(3, 0x1);
    pinMode(2, 0x0);

    Serial.begin(9600);


    while (!Serial)
        ; // wait for serial port to connect. Needed for native USB


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
        rtc.adjust(DateTime((reinterpret_cast<const __FlashStringHelper *>(
# 65 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock/tower-clock.ino" 3
                           (__extension__({static const char __c[] __attribute__((__progmem__)) = ("Aug 21 2023"); &__c[0];}))
# 65 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock/tower-clock.ino"
                           )), (reinterpret_cast<const __FlashStringHelper *>(
# 65 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock/tower-clock.ino" 3
                                        (__extension__({static const char __c[] __attribute__((__progmem__)) = ("17:36:57"); &__c[0];}))
# 65 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock/tower-clock.ino"
                                        ))));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }

    // When time needs to be re-set on a previously configured device, the
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

float readDistanceCM()
{
    digitalWrite(3, 0x0);
    delayMicroseconds(2);
    digitalWrite(3, 0x1);
    delayMicroseconds(10);
    digitalWrite(3, 0x0);
    int duration = pulseIn(2, 0x1);
    return duration * 0.034 / 2;
}

void loop()
{

    // set time only as seconds counting from midnight
    DateTime now = rtc.now();

    DateTime endNoon(now.year(), now.month(), now.day(), 12, 0, 0);
    DateTime endMidnight(now.year(), now.month(), now.day(), 24, 0, 0);

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

    // If an object is detected closer than the threshold, increase the counter
    if (distance > 0 && distance < distanceThreshold)
    {
        // Increment the counter
        counter++;

        // Turn on the LED if the counter reaches the threshold
        if (counter >= 7 /* This should be normally 17280*/)
        {
            digitalWrite(13, true);
            // Check if the time is 12:00:00 (either midnight or noon)
        }
        // Check if the difference between now and noon is positive, else use midnight as an orientation
        if (differenceInSecondsNoon > 0)
        {
            // Check if the difference between now and noon is less than 2 seconds
            if (differenceInSecondsNoon < 2)
            {
                // If the time is 12:00:00, reset the counter and turn off the LED. I. e. release the pendulum.
                digitalWrite(13, false);
                counter = 0; // Reset the counter if it reaches 17280
            }
        }
        else
        {
            // Check if the difference between now and midnight is less than 2 seconds
            if (differenceInSecondsMidnight < 2)
            {
                // If the time is 24:00:00, reset the counter and turn off the LED. I. e. release the pendulum.
                digitalWrite(13, false);
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

    // Delay
    delay(1000);
}
