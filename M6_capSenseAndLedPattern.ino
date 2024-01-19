#include <CapacitiveSensor.h>
#include <Adafruit_NeoPixel.h>

#define CON_PIN_OUT A0
#define CON_PIN_IN A1

// Pin to use to send signals to WS2812B
#define LED_PIN1 9
#define LED_PIN2 10
#define LED_PIN3 11
#define LED_PIN4 12
#define LED_PINbob 13

// Number of WS2812B LEDs attached to the Arduino
#define LED_COUNT 47
#define LED_COUNTbob 82

// Setting up the NeoPixel library
Adafruit_NeoPixel strip1(LED_COUNT, LED_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_COUNT, LED_PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3(LED_COUNT, LED_PIN3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4(LED_COUNT, LED_PIN4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripbob(LED_COUNTbob, LED_PINbob, NEO_GRB + NEO_KHZ800);

CapacitiveSensor cs1 = CapacitiveSensor(2, 1);  // 1.5 megohm resistor between pins 2 & 1, pin 1 is sensor pin, add wire, foil
CapacitiveSensor cs2 = CapacitiveSensor(4, 3);
CapacitiveSensor cs3 = CapacitiveSensor(6, 5);
CapacitiveSensor cs4 = CapacitiveSensor(8, 7);

int capSensor1Min;
int capSensor2Min;
int capSensor3Min;
int capSensor4Min;
int patternNum;  //Which tile to step on
int prevPatternNum;
int patternSpeed;  //The time spent on each tile
unsigned long previousMillis = 0;
int missed;
int wrong;

bool tile1;  //back right
bool tile2;  // back left
bool tile3;  //front right
bool tile4;  //front left
bool isShaked;

void setup() {

  Serial.begin(9600);

  pinMode(CON_PIN_OUT, OUTPUT);
  pinMode(CON_PIN_IN, INPUT);
  digitalWrite(CON_PIN_IN, LOW);

  capSensor1Min = 5000;
  capSensor2Min = 3000;
  capSensor3Min = 8000;
  capSensor4Min = 4000;


  patternSpeed = 2000;
  missed = 0;


  strip1.begin();              // Initialize NeoPixel object
  strip1.setBrightness(10);    // Set BRIGHTNESS to about 4% (max = 255)
  strip1.clear();              // Set all pixel colors to 'off'
  strip2.begin();              // Initialize NeoPixel object
  strip2.setBrightness(10);    // Set BRIGHTNESS to about 4% (max = 255)
  strip2.clear();              // Set all pixel colors to 'off'
  strip3.begin();              // Initialize NeoPixel object
  strip3.setBrightness(10);    // Set BRIGHTNESS to about 4% (max = 255)
  strip3.clear();              // Set all pixel colors to 'off'
  strip4.begin();              // Initialize NeoPixel object
  strip4.setBrightness(10);    // Set BRIGHTNESS to about 4% (max = 255)
  strip4.clear();              // Set all pixel colors to 'off'
  stripbob.begin();            // Initialize NeoPixel object
  stripbob.setBrightness(10);  // Set BRIGHTNESS to about 4% (max = 255)
  stripbob.clear();            // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= patternSpeed) {
    checkTiles();
    updatePattern();
    previousMillis = currentMillis;
  }

  readSensors();
  updateLEDs();
  updateController();
}

void updatePattern() {
  resetTiles();

  // Randomly select a new pattern, ensuring it's different from the previous one
  patternNum = random(1, 6);
  while (patternNum == prevPatternNum) {
    patternNum = random(1, 6);
  }
  prevPatternNum = patternNum;

  // Light up the tile according to the selected pattern
  for (int i = 0; i < LED_COUNT; i++) {
    if (patternNum == 1) strip1.setPixelColor(i, 255, 255, 255);
    else strip1.clear();

    if (patternNum == 2) strip2.setPixelColor(i, 255, 255, 255);
    else strip2.clear();

    if (patternNum == 3) strip3.setPixelColor(i, 255, 255, 255);
    else strip3.clear();

    if (patternNum == 4) strip4.setPixelColor(i, 255, 255, 255);
    else strip4.clear();
  }
  if (patternNum == 5) digitalWrite(CON_PIN_OUT, HIGH);
  else digitalWrite(CON_PIN_OUT, LOW);
}

void checkTiles() {
  // Check if the correct tile was stepped on
  if (patternNum == 1 && !tile1) missed++;
  if (patternNum == 2 && !tile2) missed++;
  if (patternNum == 3 && !tile3) missed++;
  if (patternNum == 4 && !tile4) missed++;
  if (patternNum == 5 && !isShaked) missed++;

  if (missed > 0) {
    for (int i = 0; i < LED_COUNTbob; i++) {
      stripbob.setPixelColor(i, 255, 0, 0);  // Red color for missed step
    }
  }
}

void readSensors() {
  long total1 = cs1.capacitiveSensor(30);
  long total2 = cs2.capacitiveSensor(30);
  long total3 = cs3.capacitiveSensor(30);
  long total4 = cs4.capacitiveSensor(30);

  tile1 = (total1 > capSensor1Min);
  tile2 = (total2 > capSensor2Min);
  tile3 = (total3 > capSensor3Min);
  tile4 = (total4 > capSensor4Min);
  isShaked = (digitalRead(CON_PIN_IN) == LOW);
}

void resetTiles() {
  tile1 = tile2 = tile3 = tile4 = false;
  isShaked = false;
}

void updateLEDs() {
  strip1.show();
  strip2.show();
  strip3.show();
  strip4.show();
  stripbob.show();
  stripbob.clear();  // Clear after showing to prepare for the next update
}

void updateController(){
  if (isShaked) {
    digitalWrite(CON_PIN_OUT, LOW);
    Serial.println("isShaked");
  }
}
