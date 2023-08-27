# 1 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino"

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


bool led_status = 0x0; // start with LED off, for testing of switch code only

int button_switch = 2; // external interrupt pin






volatile bool interrupt_process_status = {
    !true /* controls interrupt handler*/ // start with no switch press pending, ie false (!triggered)
};
bool initialisation_complete = false; // inhibit any interrupts until initialisation is complete

//
// ISR for handling interrupt triggers arising from associated button switch
//
void button_interrupt_handler()
{
    if (initialisation_complete == true)
    { //  all variables are initialised so we are okay to continue to process this interrupt
        if (interrupt_process_status == !true /* controls interrupt handler*/)
        {
            // new interrupt so okay start a new button read process -
            // now need to wait for button release plus debounce period to elapse
            // this will be done in the button_read function
            if (digitalRead(button_switch) == 0x1)
            {
                // button pressed, so we can start the read on/off + debounce cycle wich will
                // be completed by the button_read() function.
                interrupt_process_status = true /* controls interrupt handler*/; // keep this ISR 'quiet' until button read fully completed
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
    if (interrupt_process_status == true /* controls interrupt handler*/)
    {
        // interrupt has been raised on this button so now need to complete
        // the button read process, ie wait until it has been released
        // and debounce time elapsed
        button_reading = digitalRead(button_switch);
        if (button_reading == 0x1)
        {
            // switch is pressed, so start/restart wait for button relealse, plus end of debounce process
            switching_pending = true;
            elapse_timer = millis(); // start elapse timing for debounce checking
        }
        if (switching_pending && button_reading == 0x0)
        {
            // switch was pressed, now released, so check if debounce time elapsed
            if (millis() - elapse_timer >= 20 /* time to wait in milli secs*/)
            {
                // dounce time elapsed, so switch press cycle complete
                switching_pending = false; // reset for next button press interrupt cycle
                interrupt_process_status = !true /* controls interrupt handler*/; // reopen ISR for business now button on/off/debounce cycle complete
                return true /* value if the button switch has been pressed*/; // advise that switch has been pressed
            }
        }
    }
    return !true /* value if the button switch has been pressed*/; // either no press request or debounce period not elapsed
} // end of read_button function

// This is the code for the LCD and RTC.
// The selected code is written in C++ and is used to display the current date and time on an LCD screen using a real-time clock (RTC) module. The code initializes the LCD screen and RTC module, and then enters an infinite loop where it continuously reads the current date and time from the RTC module and displays it on the LCD screen. The code also includes a function to test the I2C address of the LCD screen and switch to a different address if necessary. Additionally, there is another code block that is not related to the first one, which is used to toggle an LED on and off using a button switch.

# 102 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino" 2
# 103 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino" 2
# 104 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino" 2





int counter = 0; // Counter to keep track of proximity detections
int counter_six_hours = 0; // Counter to keep track of proximity detections

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 RTC;

void setup()
{
    if (!i2CAddrTest(0x27))
    {
        lcd = LiquidCrystal_I2C(0x3F, 16, 2);
    }
    if (1 /* 1 = enable serial output, 0 = disable serial output*/)
        Serial.begin(9600); // 시리얼통신 초기화

    lcd.init(); // initialize the lcd
    lcd.backlight(); // Turn on backlight
    lcd.print("hello, world!"); // Print a message to the LCD

    RTC.begin(); // 실시간시계 시작
    RTC.adjust(DateTime((reinterpret_cast<const __FlashStringHelper *>(
# 129 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino" 3
                       (__extension__({static const char __c[] __attribute__((__progmem__)) = ("Aug 27 2023"); &__c[0];}))
# 129 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino"
                       )), (reinterpret_cast<const __FlashStringHelper *>(
# 129 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino" 3
                                    (__extension__({static const char __c[] __attribute__((__progmem__)) = ("19:02:27"); &__c[0];}))
# 129 "/Users/Qui/Documents/Programmieren/8_Arduino/tower-clock-with-inductive-proximity-sensor/tower-clock-with-inductive-proximity-sensor.ino"
                                    ))));

    // Led and button switch code
    pinMode(13 /* digital pin connected to LED, for testing of switch code only*/, 0x1);
    pinMode(button_switch, 0x0);
    attachInterrupt(((button_switch) == 2 ? 0 : ((button_switch) == 3 ? 1 : -1)),
                    button_interrupt_handler,
                    3 /* interrupt triggered on a RISING input*/);
    initialisation_complete = true; // open interrupt processing for business
}

void loop()
{

    DateTime now = RTC.now();

    if (1 /* 1 = enable serial output, 0 = disable serial output*/)
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

    if (read_button() == true /* value if the button switch has been pressed*/)
    {
        // button on/off cycle now complete, so flip LED between HIGH and LOW
        led_status = 0x1 - led_status; // toggle state
        digitalWrite(13 /* digital pin connected to LED, for testing of switch code only*/, led_status);
        // add one to the counter
        counter++;
        // set the counter to 0 if it reaches the oscillations and add one to the counter_six_hours
        if (counter == 3 /* This should be normally 17.280*/)
        {
            digitalWrite(12, 0x1);
        }

        delay(1250 /* Delay between each measurement in milliseconds 1250 = 1.25 seconds*/);
        led_status = 0x1 - led_status; // toggle state

        digitalWrite(13 /* digital pin connected to LED, for testing of switch code only*/, led_status);
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
            digitalWrite(12, 0x0);
            counter = 0; // Reset the counter if it reaches 17280
        }
    }
    else
    {
        // Check if the difference between now and midnight is less than 2 seconds
        if (differenceInSecondsMidnight > -1 && differenceInSecondsMidnight < 1)
        {
            // If the time is 24:00:00, reset the counter and turn off the LED. I. e. release the pendulum.
            digitalWrite(12, 0x0);

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
