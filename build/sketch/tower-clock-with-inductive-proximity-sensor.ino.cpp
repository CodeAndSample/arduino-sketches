#include <Arduino.h>
#line 1 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino"

//  This example and code is in the public domain and may be used without restriction and
//  without warranty.
//
// Exmple sketch - Button Switch Using An External Interrupt
// '''''''''''''''''''''''''''''''''''''''''''''''''''''''''
// This sketch demonstrates the use of a simple button switch which is processed by
// an external interrupt process.  It presents a very different and alternative approach
// for associating a button switch to an interrupt.
//
// The sketch is designed such that button status is only flagged as 'switched' AFTER
// 1. button is pressed AND then released, AND
// 2. elapse of the debounce period AFTER release
//
// Note that the associated button interrupt handler function and the button_read()
// function work together - the interrupt handler starts the on/off process and the
// button_read() function completes/concludes it.  The interrupt handler can only restart AFTER
// button reading and debounce is complete.  This ensures that only one interrupt trigger is
// processed at a time.
//
// The button switch is wired in a standard configuration with a 10K ohm pull down resister which
// ensures the digital interrupt pin is kept LOW until the button switch is pressed and
// raises it to HIGH (+5v).
//
// Operation of the button is demonstrated by toggling the in built LED on and off.
//

#define LED LED_BUILTIN // digital pin connected to LED, for testing of switch code only
bool led_status = LOW;  // start with LED off, for testing of switch code only

int button_switch = 2; // external interrupt pin

#define switched true                 // value if the button switch has been pressed
#define triggered true                // controls interrupt handler
#define interrupt_trigger_type RISING // interrupt triggered on a RISING input
#define debounce 20                   // time to wait in milli secs

volatile bool interrupt_process_status = {
    !triggered // start with no switch press pending, ie false (!triggered)
};
bool initialisation_complete = false; // inhibit any interrupts until initialisation is complete

//
// ISR for handling interrupt triggers arising from associated button switch
//
#line 46 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino"
void button_interrupt_handler();
#line 65 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino"
bool read_button();
#line 115 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino"
void setup();
#line 140 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino"
void loop();
#line 255 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino"
bool i2CAddrTest(uint8_t addr);
#line 46 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino"
void button_interrupt_handler()
{
    if (initialisation_complete == true)
    { //  all variables are initialised so we are okay to continue to process this interrupt
        if (interrupt_process_status == !triggered)
        {
            // new interrupt so okay start a new button read process -
            // now need to wait for button release plus debounce period to elapse
            // this will be done in the button_read function
            if (digitalRead(button_switch) == HIGH)
            {
                // button pressed, so we can start the read on/off + debounce cycle wich will
                // be completed by the button_read() function.
                interrupt_process_status = triggered; // keep this ISR 'quiet' until button read fully completed
            }
        }
    }
} // end of button_interrupt_handler

bool read_button()
{
    int button_reading;
    // static variables because we need to retain old values between function calls
    static bool switching_pending = false;
    static long int elapse_timer;
    if (interrupt_process_status == triggered)
    {
        // interrupt has been raised on this button so now need to complete
        // the button read process, ie wait until it has been released
        // and debounce time elapsed
        button_reading = digitalRead(button_switch);
        if (button_reading == HIGH)
        {
            // switch is pressed, so start/restart wait for button relealse, plus end of debounce process
            switching_pending = true;
            elapse_timer = millis(); // start elapse timing for debounce checking
        }
        if (switching_pending && button_reading == LOW)
        {
            // switch was pressed, now released, so check if debounce time elapsed
            if (millis() - elapse_timer >= debounce)
            {
                // dounce time elapsed, so switch press cycle complete
                switching_pending = false;             // reset for next button press interrupt cycle
                interrupt_process_status = !triggered; // reopen ISR for business now button on/off/debounce cycle complete
                return switched;                       // advise that switch has been pressed
            }
        }
    }
    return !switched; // either no press request or debounce period not elapsed
} // end of read_button function

// This is the code for the LCD and RTC.
// The selected code is written in C++ and is used to display the current date and time on an LCD screen using a real-time clock (RTC) module. The code initializes the LCD screen and RTC module, and then enters an infinite loop where it continuously reads the current date and time from the RTC module and displays it on the LCD screen. The code also includes a function to test the I2C address of the LCD screen and switch to a different address if necessary. Additionally, there is another code block that is not related to the first one, which is used to toggle an LED on and off using a button switch.

#include <LiquidCrystal_I2C.h>
#include <RTClib.h> // 실시간시계 라이브러리
#include <Wire.h>   // I2C통신 라이브러리

#define SERIAL_OPTION 1 // 1 = enable serial output, 0 = disable serial output
#define OSCILLATIONS 3  // This should be normally 17.280
#define DELAY_TIME 1250 // Delay between each measurement in milliseconds 1250 = 1.25 seconds

int counter = 0;           // Counter to keep track of proximity detections
int counter_six_hours = 0; // Counter to keep track of proximity detections

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 RTC;

void setup()
{
    if (!i2CAddrTest(0x27))
    {
        lcd = LiquidCrystal_I2C(0x3F, 16, 2);
    }
    if (SERIAL_OPTION)
        Serial.begin(9600); // 시리얼통신 초기화

    lcd.init();                 // initialize the lcd
    lcd.backlight();            // Turn on backlight
    lcd.print("hello, world!"); // Print a message to the LCD

    RTC.begin(); // 실시간시계 시작
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // Led and button switch code
    pinMode(LED, OUTPUT);
    pinMode(button_switch, INPUT);
    attachInterrupt(digitalPinToInterrupt(button_switch),
                    button_interrupt_handler,
                    interrupt_trigger_type);
    initialisation_complete = true; // open interrupt processing for business
}

void loop()
{

    DateTime now = RTC.now();

    if (SERIAL_OPTION)
    {
        Serial.print(now.year());
        Serial.print("/");
        Serial.print(now.month());
        Serial.print("/");
        Serial.print(now.day());
        Serial.print(" ");

        Serial.print(now.hour());
        Serial.print(":");
        Serial.print(now.minute());
        Serial.print(":");
        Serial.print(now.second());
        Serial.print("\n");
    }

    lcd.setCursor(0, 0);
    lcd.print("Co.: ");
    lcd.print(counter);
    lcd.print(" ");
    lcd.print("Co.-Six: ");
    lcd.print(counter);
    lcd.print(" ");
    lcd.print("DATE: ");
    lcd.print(now.year());
    lcd.print("/");
    lcd.print(now.month());
    lcd.print("/");
    lcd.print(now.day());
    lcd.setCursor(0, 1);
    lcd.print("TIME: ");
    lcd.print(now.hour());
    lcd.print(":");
    lcd.print(now.minute());
    lcd.print(":");
    lcd.print(now.second());

    //   lcd.setCursor(0, 1);// set the cursor to column 0, line 1
    // // print the number of seconds since reset:
    // lcd.print("Counter:");
    // lcd.print(millis() / 1000);

    // Led and button switch code
    // test button switch and process if pressed

    if (read_button() == switched)
    {
        // button on/off cycle now complete, so flip LED between HIGH and LOW
        led_status = HIGH - led_status; // toggle state
        digitalWrite(LED, led_status);
        // add one to the counter
        counter++;
        // set the counter to 0 if it reaches the oscillations and add one to the counter_six_hours
        if (counter == OSCILLATIONS)
        {
            digitalWrite(12, HIGH);
        }

        delay(DELAY_TIME);
        led_status = HIGH - led_status; // toggle state

        digitalWrite(LED, led_status);
    }
    else
    {
        // do other things....
    }

    DateTime endNoon(now.year(), now.month(), now.day(), 12, 00, 0);
    DateTime endMidnight(now.year(), now.month(), now.day(), 19, 03, 0);

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

    if (differenceInSecondsNoon > 0)
    {
        // Check if the difference between now and noon is less than 2 seconds
        if (differenceInSecondsNoon < 1)
        {
            // If the time is 12:00:00, reset the counter and turn off the LED. I. e. release the pendulum.
            digitalWrite(12, LOW);
            counter = 0; // Reset the counter if it reaches 17280
        }
    }
    else
    {
        // Check if the difference between now and midnight is less than 2 seconds
        if (differenceInSecondsMidnight > -1 && differenceInSecondsMidnight < 1)
        {
            // If the time is 24:00:00, reset the counter and turn off the LED. I. e. release the pendulum.
            digitalWrite(12, LOW);

            counter = 0; // Reset the counter if it reaches 17280
        }
    }
}

// The selected code is a function in C++ that tests the validity of an I2C address. It initializes the I2C communication and begins a transmission to the specified address. If the transmission is successful, the function returns true, otherwise it returns false. This function can be used to test the I2C address of any device that uses the I2C communication protocol.

bool i2CAddrTest(uint8_t addr)
{
    Wire.begin();
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0)
    {
        return true;
    }
    return false;
}

