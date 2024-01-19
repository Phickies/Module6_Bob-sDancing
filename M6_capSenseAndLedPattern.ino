#include <CapacitiveSensor.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

// Pin to use to for controller
#define CON_R_PIN_OUT A0
#define CON_L_PIN_OUT A1
#define CON_R_PIN_IN A2
#define CON_L_PIN_IN A3

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
bool isShakedR;
bool isShakedL;

const int MPU_ADDR = 0x68;
int16_t accelerometer_x, accelerometer_y, accelerometer_z;  // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z;                             // variables for gyro raw datas

char tmp_str[7];  // temporary variable used in convert function

void setup() {

  Serial.begin(9600);

  pinMode(CON_R_PIN_OUT, OUTPUT);
  pinMode(CON_L_PIN_OUT, OUTPUT);
  pinMode(CON_R_PIN_IN, INPUT);
  pinMode(CON_L_PIN_IN, INPUT);
  digitalWrite(CON_R_PIN_IN, LOW);
  digitalWrite(CON_L_PIN_IN, LOW);

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

  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);  // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B);                  // PWR_MGMT_1 register
  Wire.write(0);                     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
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
  patternNum = random(1, 7);
  while (patternNum == prevPatternNum) {
    patternNum = random(1, 7);
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
  if (patternNum == 5) digitalWrite(CON_R_PIN_OUT, HIGH);
  else digitalWrite(CON_R_PIN_OUT, LOW);

  if (patternNum == 6) digitalWrite(CON_L_PIN_OUT, HIGH);
  else digitalWrite(CON_L_PIN_OUT, LOW);
}

void checkTiles() {
  // Check if the correct tile was stepped on
  if (patternNum == 1 && !tile1) missed++;
  if (patternNum == 2 && !tile2) missed++;
  if (patternNum == 3 && !tile3) missed++;
  if (patternNum == 4 && !tile4) missed++;
  if (patternNum == 5 && !isShakedR) missed++;
  if (patternNum == 6 && !isShakedL) missed++;

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
  isShakedR = (digitalRead(CON_R_PIN_IN) == LOW);
  isShakedL = acceleratorRead();
}

void resetTiles() {
  tile1 = tile2 = tile3 = tile4 = false;
  isShakedR = false;
  isShakedL = false;
}

void updateLEDs() {
  strip1.show();
  strip2.show();
  strip3.show();
  strip4.show();
  stripbob.show();
  stripbob.clear();  // Clear after showing to prepare for the next update
}

void updateController() {
  if (isShakedR) {
    digitalWrite(CON_R_PIN_OUT, LOW);
    Serial.println("isShaked RIGHT");
  }
  if (isShakedL) {
    digitalWrite(CON_L_PIN_OUT, LOW);
    Serial.println("isShaked LEFT");
  }
}

char* convert_int16_to_str(int16_t i) {  // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

bool acceleratorRead() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);                         // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false);              // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7 * 2, true);  // request a total of 7*2=14 registers

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accelerometer_x = Wire.read() << 8 | Wire.read();  // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read() << 8 | Wire.read();  // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read() << 8 | Wire.read();  // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  gyro_x = Wire.read() << 8 | Wire.read();           // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = Wire.read() << 8 | Wire.read();           // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = Wire.read() << 8 | Wire.read();           // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)

  // print out data
  Serial.print("aX = ");
  Serial.print(convert_int16_to_str(accelerometer_x));
  Serial.print(" | aY = ");
  Serial.print(convert_int16_to_str(accelerometer_y));
  Serial.print(" | aZ = ");
  Serial.print(convert_int16_to_str(accelerometer_z));
  // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  Serial.print(" | tmp = ");
  Serial.print(" | gX = ");
  Serial.print(convert_int16_to_str(gyro_x));
  Serial.print(" | gY = ");
  Serial.print(convert_int16_to_str(gyro_y));
  Serial.print(" | gZ = ");
  Serial.print(convert_int16_to_str(gyro_z));
  Serial.println();

  // delay
  delay(10);

  return false;

}
