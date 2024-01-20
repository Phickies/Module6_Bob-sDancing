#include <CapacitiveSensor.h>
#include <Adafruit_NeoPixel.h>

// Pin use for controller
#define CON_R_PIN_OUT A0
#define CON_L_PIN_OUT A1
#define CON_R_PIN_IN A2
#define CON_L_PIN_IN A3

// Pin use for LED
#define LED_PIN1 9
#define LED_PIN2 10
#define LED_PIN3 11
#define LED_PIN4 12
#define LED_PIN_BOB 13

// Number of LED
#define LED_TILE_COUNT 47
#define LED_BOB_COUNT 82
#define NUM_STRIPS 5

// Capacitive Sensor Tunning
#define CAP1_SEN_MIN 5000
#define CAP2_SEN_MIN 5000
#define CAP3_SEN_MIN 5000
#define CAP4_SEN_MIN 5000

// Configure game speed
#define PATTERN_SPEED 2000
#define GAME_LENGTH 30000

// Initiate Bool Struct
typedef struct
{
  bool tile1, tile2, tile3, tile4;
  bool isShakedR, isShakedL;
} Bool_Struct;

// Setting up NeoPixel library
Adafruit_NeoPixel strip1(
  LED_TILE_COUNT, LED_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(
  LED_TILE_COUNT, LED_PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3(
  LED_TILE_COUNT, LED_PIN3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4(
  LED_TILE_COUNT, LED_PIN4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripB(
  LED_BOB_COUNT, LED_PIN_BOB, NEO_GRB + NEO_KHZ800);

// Setting up Capacitive Sensor
// 1.5 megaOhm resistor between pins
CapacitiveSensor cs1 = CapacitiveSensor(2, 1);
CapacitiveSensor cs2 = CapacitiveSensor(4, 3);
CapacitiveSensor cs3 = CapacitiveSensor(6, 5);
CapacitiveSensor cs4 = CapacitiveSensor(8, 7);

int missedPoint = 0;
Adafruit_NeoPixel strip_list[NUM_STRIPS] = {
  strip1, strip2, strip3, strip4, stripB
};

void setup() {
  Serial.begin(9600);

  // Initialize controller pin
  pinMode(CON_R_PIN_OUT, OUTPUT);
  pinMode(CON_L_PIN_OUT, OUTPUT);
  pinMode(CON_R_PIN_IN, INPUT);
  pinMode(CON_L_PIN_IN, INPUT);
  digitalWrite(CON_R_PIN_IN, LOW);
  digitalWrite(CON_L_PIN_IN, LOW);

  // Initialize NeoPixel object
  for (int i = 0; i < NUMS_STRIPS; i++) {
    strip_list[i].begin();
    strip_list[i].setBrightness(255);
    strip_list[i].clear();
  }
}

void loop() {
  gameIdle();
  gameStart();
  gameOver();
}

void gameIdle() {
  Serial.println("-----------------------");
  Serial.println("---------Idling--------");
  Serial.println("-----------------------");
  while (true) {
    idleAnimation();
    if (true) break;
  }
  return;
}

void gameStart() {
  unsigned long curTime = millis();
  unsigned long strTime = millis();
  unsigned long preTime = millis();

  Bool_Struct b;
  int patternNum;

  Serial.println("-----------------------");
  Serial.println("--------Started--------");
  Serial.println("-----------------------");

  startAnimation(&b);

  while (curTime - strTime <= GAME_LENGTH) {
    if (curTime - preTime >= PATTERN_SPEED) {
      updatePattern(&patternNum, &b);
      preTime = curTime;
    }
    readSensors(patternNum, &b);
    updateActuator(patternNum, &b);
    curTime = millis();
  }

  return;
}

void gameOver() {
  Serial.println("-----------------------");
  Serial.println("-------GAMEOVER--------");
  Serial.println("-----------------------");
  Serial.print("You missed: ");
  Serial.print(missedPoint);
  Serial.print("-----------");
  Serial.println("-----------------------");

  int ledScore = missedPoint
                 / GAME_LENGTH
                 / PATTERN_SPEED
                 * 24;

  // Light up geen Led for score
  for (int i = LED_BOB_COUNT;
       i > LED_BOB_COUNT - ledScore;
       i--) {
    stripB.setPixelColor(i, 0, 255, 0);
    delay(500);
  }
  Serial.print("You earn: ");
  Serial.print(ledScore);
  Serial.println(" points-------");
  Serial.println("-----------------------");
  delay(2000);
  return;
}

void updatePattern(int* patternNum, Bool_Struct* b) {
  checkMissed(patternNum, b);
  *b = (Bool_Struct){ false };  // Reset all tile

  // Randomly select a new pattern
  int newPattern;
  do {
    newPattern = random(1, 7);
  } while (newPattern == *patternNum);
  *patternNum = newPattern;

  setPattern(patternNum);
}

void readSensors(int patternNum, Bool_Struct* b) {
  b->tile1 = (cs1.capacitiveSensor(30) > CAP1_SEN_MIN);
  b->tile2 = (cs2.capacitiveSensor(30) > CAP2_SEN_MIN);
  b->tile3 = (cs3.capacitiveSensor(30) > CAP3_SEN_MIN);
  b->tile4 = (cs4.capacitiveSensor(30) > CAP4_SEN_MIN);

  if (patternNum == 5) {
    b->isShakedR = (digitalRead(CON_R_PIN_IN) == LOW);
  }
  // else if (patternNum == 6) {
  //     b->isShakedL=(digitalRead(CON_L_PIN_IN)==LOW);
  //     // b->isShakedL = (acceleratorRead());
  // }
}

void updateActuator(int patternNum, Bool_Struct* b) {
  switch (patternNum) {
    case 1:
      if (b->tile1) strip1.clear();
      break;
    case 2:
      if (b->tile2) strip2.clear();
      break;
    case 3:
      if (b->tile3) strip3.clear();
      break;
    case 4:
      if (b->tile4) strip4.clear();
      break;
    case 5:
      if (b->isShakedR)
        digitalWrite(CON_R_PIN_OUT, LOW);
      break;
    case 6:
      if (b->isShakedL)
        digitalWrite(CON_L_PIN_OUT, LOW);
      break;
    default:
      return;
  }
  Serial.println("You made it");
}

void checkMissed(int* patternNum,
                 Bool_Struct* b) {
  bool missedAction = false;

  // 'missedAction' equal to true if missed
  // tile n is false if the are not pressed
  switch (*patternNum) {
    case 1: missedAction = !b->tile1; break;
    case 2: missedAction = !b->tile2; break;
    case 3: missedAction = !b->tile3; break;
    case 4: missedAction = !b->tile4; break;
    case 5: missedAction = !b->isShakedR; break;
    case 6: missedAction = !b->isShakedL; break;
  }

  if (missedAction) {
    missedPoint++;
    Serial.print("You missed number: ");
    Serial.println(*patternNum);
  }
}

void setPattern(int* patternNum) {

  // Clear the previously lit strip
  Adafruit_NeoPixel* previousStrip = nullptr;
  switch (*patternNum) {
    case 1: previousStrip = &strip1; break;
    case 2: previousStrip = &strip2; break;
    case 3: previousStrip = &strip3; break;
    case 4: previousStrip = &strip4; break;
  }
  if (previousStrip != nullptr)
    previousStrip->clear();

  // Clear all vibration
  digitalWrite(CON_R_PIN_OUT, LOW);
  digitalWrite(CON_L_PIN_OUT, LOW);

  // Determine which strip to light up
  Adafruit_NeoPixel* targetStrip = nullptr;
  switch (*patternNum) {
    case 1: targetStrip = &strip1; break;
    case 2: targetStrip = &strip2; break;
    case 3: targetStrip = &strip3; break;
    case 4: targetStrip = &strip4; break;
    case 5: digitalWrite(CON_R_PIN_OUT, HIGH); break;
    case 6: digitalWrite(CON_L_PIN_OUT, HIGH); break;
    default: return;
  }

  // Light up the selected strip
  for (int i = 0; i < LED_TILE_COUNT; i++) {
    targetStrip->setPixelColor(i, 255, 255, 255);
  }
  // Update the strip's LEDs
  targetStrip->show();

  Serial.print("Number activated is: ");
  Serial.println(*patternNum);
}

void idleAnimation() {
  int delayTime = 50;

  //Set ambient light yellow
  for (int i = 0; i < LED_BOB_COUNT - 24; i++) {
    stripB.setPixelColor(i, 255, 255, 0);
  }
  stripB.show();

  // Sequentially light up each LED in the strips
  for (int i = 0; i < LED_TILE_COUNT; i++) {
    // Light up the i-th LED in each strip
    for (int j = 0;
         j < NUM_STRIPS - 1; j++) {
      strip_list[j].setPixelColor(i, 255, 255, 255);
      strip.list[j].show();
    }

    // Delay to create the domino effect
    delay(delayTime);

    // Turn off the i-th LED for a continuous effect
    for (int j = 0;
         j < sizeof(strip_list) - 1; j++) {
      strip_list[j].setPixelColor(i, 0, 0, 0);
    }
  }
}

void startAnimation(Bool_Struct* b) {
  int count = 0;

  while (count < 3) {
    delay(500);
    // Could add a fade in/fade out effect
    for (int i = 0; i < LED_TILE_COUNT; i++) {
      for (int j = 0;
           j < sizeof(strip_list); j++) {
        strip_list[j].setPixelColor(i, 255, 0, 0);
      }
    }
    delay(500);
    for (int i = 0; i < sizeof(strip_list); i++) {
      strip_list[i].clear();
    }
    count++;
  }

  delay(500);
  for (int i = 0; i < LED_TILE_COUNT; i++) {
    for (int j = 0; j < sizeof(strip_list); j++) {
      strip_list[j].setPixelColor(i, 0, 255, 0);
    }
  }
}
