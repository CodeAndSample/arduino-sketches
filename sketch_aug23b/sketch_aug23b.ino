// Arduino and proximity sensor

void setup()
{
  Serial.begin(9600); // initialize serial
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("Analog pin: "); // display analog values to serial
  Serial.print(analogRead(A0));
  if (analogRead(A0))
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}