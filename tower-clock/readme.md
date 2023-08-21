# Ticking Precision: Managing a Church Clock with Arduino

## Objective

The Arduino manages the church clock (a Mannhardt clock), ensuring that it displays and chimes the time with near accuracy by momentarily holding the pendulum near the 12 o'clock position. The clock is intentionally set to run slightly faster, and after a predetermined duration, it is halted until the correct time arrives, at which point it is reactivated.
The Mannhardt clock has the following characteristics: The pendulum takes 1.25 seconds for one swing, translating to 2.5 seconds for a complete oscillation. An escapement wheel with 90 pins drives this pendulum, so after 225 seconds, the watch returns to its original state. However, this description is incomplete without considering the percussion mechanism, which resets only after 12 hours. Thus, an effective control circuit operates over a 12-hour cycle, or 43,200 seconds (which equates to 34,560 tick-tocks or 17,280 pendulum oscillations).
If the time surpasses 12 o'clock, the clock chimes with four quarter-hour strikes and 11 hourly strikes [SIC].

## Implementation

1. **Setting the Clock Ahead**: The clock is configured so that it runs 1-3 minutes ahead over a 12-hour span.
2. **Starting Time Alignment**: Both the clock and Arduino are initialized at 12 o'clock standard time.
3. **Pendulum Oscillation Counting**: Arduino keeps track of pendulum oscillations using a proximity sensor and halts the clock after 17,280 pendulum oscillations. This is done using an SSR relay for the electrical activation of the magnetic pendulum catcher control via Arduino.
4. **Counting Until Noon or Midnight**: Arduino continues to count seconds until 12 o'clock am or pm.
5. **Restarting the Clock**: Arduino sends a signal to reset the pendulum catcher, thus restarting the clock.
6. **Repeating Pendulum Count**: Arduino resumes counting pendulum swings and stops the clock once again after 17,280 swings.
7. **Looping the Process**: The process returns to step 4 and continues to repeat.

### Hints

- **Interrupt Control**: Keep in mind that Arduino has only 2 interruptible inputs.
- **Time Sources**: Consider using the Arduino RTC clock or combining Arduino with a standard time signal for accurate timekeeping.
  