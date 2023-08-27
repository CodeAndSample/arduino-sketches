
/*
14CORE TEST CODE FOR PROXIMITY SENSOR
Metal Detection with 3 wire sensor
http://www.14core.com
*/

float metalDetected;
int monitoring;
int metalDetection = 1;

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);

  monitoring = analogRead(metalDetection);
  metalDetected = (float)monitoring * 100 / 1024.0;

  if (monitoring > 250)
  {
    Serial.println("Metal is Detected");
    digitalWrite(LED_BUILTIN, HIGH);
  }

  delay(1000);
}