
int vibratePin = 13;
int inputPin = 3;

boolean isShake = false;

void setup() {
  Serial.begin(9600);

  pinMode(vibratePin, OUTPUT);
  pinMode(inputPin, INPUT);
  digitalWrite(inputPin, LOW);
}

void loop() {         

/* 
 *  Testing the vibrating motor
 */
  static unsigned long previousMillis = 0;
  const long interval = 3000;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    digitalWrite(vibratePin, HIGH);
  }

/*
 * Main code below
 */
  
  if (digitalRead(inputPin) == LOW)
  {
    delay(20);
    digitalWrite(vibratePin, LOW);
    isShake = true;
    Serial.println("detect shock!");
  }
  else if (!isShake)
  {
    delay(20);
    digitalWrite(vibratePin, HIGH);
    Serial.println("nothing");
  }
}
