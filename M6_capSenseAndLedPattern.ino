#include <CapacitiveSensor.h>
#include <Adafruit_NeoPixel.h>

// Pin to use to send signals to WS2812B
#define LED_PIN1 9
#define LED_PIN2 10
#define LED_PIN3 11
#define LED_PIN4 12
#define LED_PINbob 13

// Number of WS2812B LEDs attached to the Arduino
#define LED_COUNT 47
#define LED_COUNTbob 20

// Setting up the NeoPixel library
Adafruit_NeoPixel strip1(LED_COUNT, LED_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_COUNT, LED_PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3(LED_COUNT, LED_PIN3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4(LED_COUNT, LED_PIN4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripbob(LED_COUNTbob, LED_PINbob, NEO_GRB + NEO_KHZ800);

CapacitiveSensor cs1 = CapacitiveSensor(2,1); // 1.5 megohm resistor between pins 2 & 1, pin 1 is sensor pin, add wire, foil
CapacitiveSensor cs2 = CapacitiveSensor(4,3);
CapacitiveSensor cs3 = CapacitiveSensor(6,5);
CapacitiveSensor cs4 = CapacitiveSensor(8,7);

int capSensorMin;
int patternNum; //Which tile to step on
int prevPatternNum; 
int beatNum; //The number of step
int patternSpeed; //The speed of the game

bool tile1; //back right
bool tile2; // back left
bool tile3; //front right
bool tile4; //front left

void setup(){

Serial.begin(9600);
capSensorMin = 5000; //minimum value of the sensor to consider pressed

beatNum = 0;
patternSpeed = 5;

  strip1.begin();           // Initialize NeoPixel object
  strip1.setBrightness(10); // Set BRIGHTNESS to about 4% (max = 255)
  strip1.clear(); // Set all pixel colors to 'off'
  strip2.begin();           // Initialize NeoPixel object
  strip2.setBrightness(10); // Set BRIGHTNESS to about 4% (max = 255)
  strip2.clear(); // Set all pixel colors to 'off'
  strip3.begin();           // Initialize NeoPixel object
  strip3.setBrightness(10); // Set BRIGHTNESS to about 4% (max = 255)
  strip3.clear(); // Set all pixel colors to 'off'
  strip4.begin();           // Initialize NeoPixel object
  strip4.setBrightness(10); // Set BRIGHTNESS to about 4% (max = 255)
  strip4.clear(); // Set all pixel colors to 'off'
  stripbob.begin();           // Initialize NeoPixel object
  stripbob.setBrightness(10); // Set BRIGHTNESS to about 4% (max = 255)
  stripbob.clear(); // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
}

void loop(){

long start = millis();

//Runs once per beat
if ((start/10000)*patternSpeed > beatNum){
  patternNum = random(1,5);
  if(patternNum == prevPatternNum){
  patternNum = random(1,5);    
  }
  prevPatternNum= patternNum;
  
  beatNum = beatNum + 1;
  Serial.println(patternNum);

  if (patternNum == 1){
 for(int i=0; i<LED_COUNT; i++) {
    // Set the i-th LED to white
    strip1.setPixelColor(i, 255, 255, 255);
    if(tile1 == true){
    strip1.clear();   // Send the updated pixel colors to off
    }
  }
}else{
    strip1.clear();   // Send the updated pixel colors to off
  }

if (patternNum == 2){
 for(int i=0; i<LED_COUNT; i++) {
    // Set the i-th LED to white
    strip2.setPixelColor(i, 255, 255, 255);
  }
}else{
    strip2.clear();   // Send the updated pixel colors off.
  }

if (patternNum == 3){
 for(int i=0; i<LED_COUNT; i++) {
    // Set the i-th LED to white
    strip3.setPixelColor(i, 255, 255, 255);
  }
}else{
    strip3.clear();   // Send the updated pixel colors to the hardware.
  }

if (patternNum == 4){
 for(int i=0; i<LED_COUNT; i++) {
    // Set the i-th LED to white
    strip4.setPixelColor(i, 255, 255, 255);
  }
}else{
    strip4.clear();   // Send the updated pixel colors to the hardware.
  }
}

long total1 = cs1.capacitiveSensor(30);
long total2 = cs2.capacitiveSensor(30);
long total3 = cs3.capacitiveSensor(30);
long total4 = cs4.capacitiveSensor(30);

//Check the capSensors
if (total1 > capSensorMin){
  strip1.clear();
}

if (total2 > capSensorMin){
  strip2.clear();
}

if (total3 > capSensorMin){
  strip3.clear();
}

if (total4 > capSensorMin){
  strip4.clear();
}

/*
//Check sensor values
Serial.print("s1 "); // tab character for debug window spacing
Serial.println(total1); // print sensor output 1
Serial.print("s2 "); // tab character for debug window spacing
Serial.println(total2); // print sensor output 
Serial.print("s3 "); // tab character for debug window spacing
Serial.println(total3); // print sensor output 
Serial.print("s4 "); // tab character for debug window spacing
Serial.println(total4); // print sensor output 
Serial.print("\n"); // tab character for debug window spacing
*/

strip1.show();   // Send the updated pixel colors to the hardware.
strip2.show();   // Send the updated pixel colors to the hardware.
strip3.show();   // Send the updated pixel colors to the hardware.
strip4.show();   // Send the updated pixel colors to the hardware.
stripbob.show();   // Send the updated pixel colors to the hardware.


}