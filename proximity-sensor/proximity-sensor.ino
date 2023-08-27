
/*
LJ12A3-4-ZBX-Inductive-Proximity-Sensor
made on 04 Nov 2020

// Arduino and proximity sensor

void setup ()
{
Serial.begin(9600); // initialize serial
}

void loop ()
{
Serial.print("Analog pin: "); // display analog values to serial
Serial.print(analogRead(A0));
}

https://www.phippselectronics.com/using-the-inductive-proximity-npn-sensor-lj12a3-4-z-bx-with-arduino/

Component Pin	UNO Board Pin
Brown	+5V
Black	A0
Blue	GND

*/

const int Pin = 2;

void setup()
{
    pinMode(Pin, INPUT);
    Serial.begin(9600);
}

void loop()
{
    int sensorValue = digitalRead(Pin);
    if (sensorValue == HIGH)
    {
        Serial.println("no Object");
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
    }
    else

    {
        Serial.println("Object Detected");
        delay(500);
        digitalWrite(LED_BUILTIN, HIGH);
    }
}
