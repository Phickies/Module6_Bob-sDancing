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
#define CAP1_SEN_MIN 10000
#define CAP2_SEN_MIN 30000
#define CAP3_SEN_MIN 45000
#define CAP4_SEN_MIN 30000

// Configure game speed
#define PATTERN_SPEED 1000
#define GAME_LENGTH 30000

// Initiate Bool Struct
typedef struct {
  bool tile1, tile2, tile3, tile4;
  bool isShakedR, isShakedL;
} Bool_Struct;

// Setting up NeoPixel library
Adafruit_NeoPixel strip1(LED_TILE_COUNT, LED_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_TILE_COUNT, LED_PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3(LED_TILE_COUNT, LED_PIN3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4(LED_TILE_COUNT, LED_PIN4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripB(LED_BOB_COUNT, LED_PIN_BOB, NEO_GRB + NEO_KHZ800);

// Setting up Capacitive Sensor
CapacitiveSensor cs1 = CapacitiveSensor(2, 1);
CapacitiveSensor cs2 = CapacitiveSensor(4, 3);
CapacitiveSensor cs3 = CapacitiveSensor(6, 5);
CapacitiveSensor cs4 = CapacitiveSensor(8, 7);

int missedPoint = 0;
Adafruit_NeoPixel strip_list[NUM_STRIPS] = { strip1, strip2, strip3, strip4, stripB };

void setup() {
  Serial.begin(9600);

  // Initialize controller pin
  pinMode(CON_R_PIN_OUT, OUTPUT);
  pinMode(CON_L_PIN_OUT, OUTPUT);
  pinMode(CON_R_PIN_IN, INPUT);
  pinMode(CON_L_PIN_IN, INPUT);

  // Initialize NeoPixel object
  for (int i = 0; i < NUM_STRIPS; i++) {
    strip_list[i].begin();
    strip_list[i].setBrightness(255);
    strip_list[i].clear();
    strip_list[i].show();
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
    if (digitalRead(CON_R_PIN_IN) == LOW) break;
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

  startAnimation();

  while (curTime - strTime <= GAME_LENGTH) {
    if (curTime - preTime >= PATTERN_SPEED) {
      updatePattern(&patternNum, &b);
      preTime = curTime;
    }
    // readSensors(patternNum, &b);
    updateActuator(patternNum, &b);
    curTime = millis();
  }

  clearPreviousPatternLights(patternNum);

  return;
}

void gameOver() {
  Serial.println("-----------------------");
  Serial.println("-------GAMEOVER--------");
  Serial.println("-----------------------");
  Serial.print("You missed: ");
  Serial.print(missedPoint);
  Serial.println(" points");
  Serial.println("-----------------------");

  int maxScore = GAME_LENGTH / PATTERN_SPEED;
  int score = maxScore - missedScore;

  int ledScore = map(score, 0, maxScore, 0, 24);

  // Light up green Led for score
  for (int i = LED_BOB_COUNT; i > LED_BOB_COUNT - ledScore; i--) {
    stripB.setPixelColor(i, 0, 255, 0);
    stripB.show();
    delay(500);
  }

  Serial.print("You earn: ");
  Serial.print(ledScore);
  Serial.println(" points");
  Serial.println("-----------------------");
  delay(5000);
  return;
}

void updatePattern(int* patternNum, Bool_Struct* b) {
  checkMissed(*patternNum, b);
  *b = (Bool_Struct){ false };  // Reset all tiles

  // Randomly select a new pattern
  int newPattern;
  do {
    newPattern = random(1, 7);
  } while (newPattern == *patternNum);

  clearPreviousPatternLights(*patternNum);
  *patternNum = newPattern;

  setPattern(*patternNum);
}

void readSensors(int patternNum, Bool_Struct* b) {
  bool previousTile1 = b->tile1;
  bool previousTile2 = b->tile2;
  bool previousTile3 = b->tile3;
  bool previousTile4 = b->tile4;
  bool previousShakedR = b->isShakedR;
  bool previousShakedL = b->isShakedL;

  b->tile1 = (cs1.capacitiveSensor(30) > CAP1_SEN_MIN);
  b->tile2 = (cs2.capacitiveSensor(30) > CAP2_SEN_MIN);
  b->tile3 = (cs3.capacitiveSensor(30) > CAP3_SEN_MIN);
  b->tile4 = (cs4.capacitiveSensor(30) > CAP4_SEN_MIN);

  if (patternNum == 5) {
    b->isShakedR = (digitalRead(CON_R_PIN_IN) == LOW);
  }

  // else if (patternNum == 6) {
  //     b->isShakedL = (digitalRead(CON_L_PIN_IN) == LOW);
  // }

  // Feedback for tile press
  if (b->tile1 && !previousTile1) Serial.println("Tile 1 pressed");
  if (b->tile2 && !previousTile2) Serial.println("Tile 2 pressed");
  if (b->tile3 && !previousTile3) Serial.println("Tile 3 pressed");
  if (b->tile4 && !previousTile4) Serial.println("Tile 4 pressed");

  // Feedback for controller shake
  if (b->isShakedR && !previousShakedR) Serial.println("Right controller shaked");
  if (b->isShakedL && !previousShakedL) Serial.println("Left controller shaked");
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
  }
  strip1.show();
  strip2.show();
  strip3.show();
  strip4.show();
}

void checkMissed(int patternNum, Bool_Struct* b) {
  bool missedAction = false;

  switch (patternNum) {
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
    Serial.println(patternNum);
  }
}

void setPattern(int patternNum) {
  Adafruit_NeoPixel* targetStrip = nullptr;
  switch (patternNum) {
    case 1: targetStrip = &strip1; break;
    case 2: targetStrip = &strip2; break;
    case 3: targetStrip = &strip3; break;
    case 4: targetStrip = &strip4; break;
  }

  if (targetStrip != nullptr) {
    for (int i = 0; i < LED_TILE_COUNT; i++) {
      targetStrip->setPixelColor(i, 255, 255, 255);
      targetStrip->show();
    }
  }

  // Reset controller vibration
  digitalWrite(CON_R_PIN_OUT, patternNum == 5 ? HIGH : LOW);
  digitalWrite(CON_L_PIN_OUT, patternNum == 6 ? HIGH : LOW);

  Serial.print("Pattern activated is: ");
  Serial.println(patternNum);
}

void idleAnimation() {
  // Your idleAnimation code here
  uint32_t yellow = stripB.Color(255, 255, 0);  // Adjust RGB values if needed

  for (int i = 0; i < LED_BOB_COUNT; i++) {
    stripB.setPixelColor(i, yellow);
  }

  stripB.show();  // Update the strip to show the new color
}

void startAnimation() {
  // Your startAnimation code here
  for (int i = 0; i < LED_BOB_COUNT; i++) {
    stripB.setPixelColor(i, 0, 0, 255);
  }
  stripB.show();  // Update the strip to show the new color
}

void clearPreviousPatternLights(int patternNum) {
  Adafruit_NeoPixel* previousStrip = nullptr;
  switch (patternNum) {
    case 1: previousStrip = &strip1; break;
    case 2: previousStrip = &strip2; break;
    case 3: previousStrip = &strip3; break;
    case 4: previousStrip = &strip4; break;
  }
  if (previousStrip != nullptr) {
    previousStrip->clear();
    previousStrip->show();
  }
}
