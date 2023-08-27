# Ticking Precision: Managing a Tower Clock with Arduino

## Objective

The Arduino manages the tower (church) clock (a Mannhardt clock), ensuring that it displays and chimes the time with near accuracy by momentarily holding the pendulum near the 12 o'clock position. The clock is intentionally set to run slightly faster, and after a predetermined duration, it is halted until the correct time arrives, at which point it is reactivated.
The Mannhardt clock has the following characteristics: The pendulum takes 1.11 seconds for one swing, translating to 2.22 seconds for a complete oscillation. An escapement wheel with 90 pins drives this pendulum, so after 200 seconds, the watch returns to its original state. However, this description is incomplete without considering the percussion mechanism, which resets only after 12 hours. Thus, an effective control circuit operates over a 12-hour cycle, or 43,200 seconds (which equates to 38,880 tick-tocks or 19,440 pendulum oscillations).
If the time surpasses 12 o'clock, the clock chimes with four quarter-hour strikes and 12 hourly strikes.


## Implementation

1. **Setting the Clock Ahead**: The clock is configured so that it runs 1-3 minutes ahead over a 12-hour span.
2. **Starting Time Alignment**: Both the clock and Arduino are initialized at 12 o'clock standard time.
3. **Pendulum Oscillation Counting**: Arduino keeps track of pendulum oscillations using a proximity sensor and halts the clock after 17,280 pendulum oscillations. This is done using an SSR relay for the electrical activation of the magnetic pendulum catcher control via Arduino.
4. **Counting Until Noon or Midnight**: Arduino continues to count seconds until 12 o'clock am or pm.
5. **Restarting the Clock**: Arduino sends a signal to reset the pendulum catcher, thus restarting the clock.
6. **Repeating Pendulum Count**: Arduino resumes counting pendulum swings and stops the clock once again after 17,280 swings.
7. **Looping the Process**: The process returns to step 4 and continues to repeat.
8. **Summer Wintertime**: When setting the summer or wintertime, the RTC clock will release a 12 o'clock signal before the 12-hour pendulum stop (pendulum catcher) finishes. The clock will stand still until the next 12 o'clock signal is advanced. Alternatively, when the RTC adds one hour to the 12-hour cycle, the release of the pendulum will be delayed by one hour.

### Insights

- **Interrupt Control**: Keep in mind that Arduino has only 2 interruptible inputs.
- **Time Sources**: Consider using the Arduino RTC clock or combining Arduino with a standard time signal for accurate timekeeping.
  
## Todos and Improvements

### Relay Pin

To control a magnet using a relay connected to an Arduino, you'll typically use a digital output pin to trigger the relay on or off. This turns the magnet on or off by completing or breaking its circuit. Here is a simple example:

**Hardware Requirements**

- Arduino Board (e.g., Arduino Uno)
- Relay Module
- Magnet (usually an electromagnet)
- Jumper Wires
- External power source for the magnet if needed

**Circuit Connections**

1. Connect the VCC pin on the relay to the 5V pin on the Arduino.
2. Connect the GND pin on the relay to the GND pin on the Arduino.
3. Connect the IN1 or Signal pin on the relay to a digital pin on the Arduino (let's say pin 7).
4. Connect the magnet to the relay, usually on the NO (Normally Open) and COM (Common) terminals if you want the magnet to be off when the relay is off.
5. If your magnet requires an external power source, make sure to connect it according to its specifications.

Here is a simple Arduino code snippet that will turn the magnet on for 5 seconds and then off for 5 seconds:

```arduino
// Relay is connected to pin 7
const int relayPin = 7;

void setup() {
  // Initialize the relay pin as an output
  pinMode(relayPin, OUTPUT);
}

void loop() {
  // Turn the relay on (which will turn the magnet on)
  digitalWrite(relayPin, HIGH);
  
  // Wait for 5 seconds
  delay(5000);

  // Turn the relay off (which will turn the magnet off)
  digitalWrite(relayPin, LOW);
  
  // Wait for 5 seconds
  delay(5000);
}
```

This is a simple example. You can extend this to include sensors, buttons, or even Internet-based controls to turn the magnet on or off based on more complex conditions.

**Note**: Always ensure you understand the power requirements and limitations of both your relay and your magnet. Make sure to follow safety guidelines when working with high currents or voltages.

### NPN Proximity Sensor

Using an NPN proximity sensor with an Arduino involves reading the sensor's output to determine whether an object is close. Since you're interested in using an interrupt, the sensor can notify the Arduino immediately when an object is detected, instead of having the Arduino constantly poll the sensor.

Here's how you might wire it up and write the code:

**Hardware Requirements**

- Arduino Board (e.g., Arduino Uno)
- NPN Proximity Sensor
- 10k Ohm Resistor (for pull-up, optional)
- Jumper Wires

**Circuit Connections**

1. Connect the sensor's VCC (usually brown) to the Arduino's 5V pin.
2. Connect the sensor's GND (usually blue) to the Arduino's GND.
3. Connect the sensor's signal output (usually black) to one of the Arduino's interrupt pins (2 or 3 on an Arduino Uno).
4. Optionally, you can connect a 10k Ohm pull-up resistor between the signal output and VCC.

**Arduino Code**

```arduino
// Proximity sensor is connected to pin 2 (interrupt pin 0 on Uno)
const int sensorPin = 2;

// Relay is connected to pin 7
const int relayPin = 7;

// Variable to hold sensor status
volatile bool objectDetected = false;

void setup() {
  // Initialize the relay pin as an output
  pinMode(relayPin, OUTPUT);
  
  // Initialize the sensor pin as an input
  pinMode(sensorPin, INPUT);
  
  // Attach an interrupt to the sensor pin
  attachInterrupt(digitalPinToInterrupt(sensorPin), objectDetectedISR, RISING);
}

void loop() {
  if (objectDetected) {
    // Turn the relay on (which will turn the magnet on)
    digitalWrite(relayPin, HIGH);

    // Reset the flag
    objectDetected = false;
  }
  else {
    // Turn the relay off (which will turn the magnet off)
    digitalWrite(relayPin, LOW);
  }
  
  // You can add more code here if needed
}

void objectDetectedISR() {
  objectDetected = true;
}
```

**How It Works**

- When the proximity sensor detects an object, it sends a "high" signal.
- The interrupt service routine (`objectDetectedISR`) sets `objectDetected` to `true`.
- The `loop` function checks the `objectDetected` flag. If `true`, it turns on the relay (and the magnet).

**Note**: Because we're using the `RISING` mode in `attachInterrupt`, the interrupt will be triggered when the sensor output goes from LOW to HIGH. If you want to detect both the appearance and disappearance of an object, you could use `CHANGE` instead and modify the ISR and main code accordingly.

**Safety Notice**: Always make sure to understand the electrical specifications of your components and ensure they are compatible with each other. Use appropriate protective measures to handle any high voltage or current.