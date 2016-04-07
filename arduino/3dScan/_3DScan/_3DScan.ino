
#include <Average.h>

const int trigPinX = 2;
const int echoPinX = 3;

const int trigPinX2 = 4;
const int echoPinX2 = 5;

const int trigPinY = 6;
const int echoPinY = 7;

const int trigPinY2 = 8;
const int echoPinY2 = 9;

const int trigPinZ = 10;
const int echoPinZ = 11;

const int PULSE_TIMEOUT = 20000;
const int TOLLERENCE_X_MM = 20; 
const int TOLLERENCE_Y_MM = 20; 

// In CM
const int CARRAIGE_WIDTH = 8 + 2; // Extra 2 is for sensor lenses
const int TABLE_MAX_X = 102 - CARRAIGE_WIDTH;
const int TABLE_MAX_Y = 61 - CARRAIGE_WIDTH;

const int SAMPLE_INTERVAL = 250;
const int SAMPLES_PER_MEASURE = 20;
const int BAUD_RATE = 9600;

Average<float> ave(SAMPLES_PER_MEASURE);

void setup() {
  // initialize serial
  Serial.begin(BAUD_RATE);
  Serial.println("ONLINE");
  // Trigger pins
  pinMode(trigPinX, OUTPUT);
  pinMode(trigPinY, OUTPUT);
  pinMode(trigPinX2, OUTPUT);
  pinMode(trigPinY2, OUTPUT);
  pinMode(trigPinZ, OUTPUT);
  // Echo pins
  pinMode(echoPinX, INPUT);
  pinMode(echoPinY, INPUT);
  pinMode(echoPinX2, INPUT);
  pinMode(echoPinY2, INPUT);
  pinMode(echoPinZ, INPUT);
}

void loop() {  
  float x = messureSingle(trigPinX, echoPinX);
  if (x > TABLE_MAX_X / 2) {
    x = messureSingle(trigPinX2, echoPinX2);
  }
  float y = messureSingle(trigPinY, echoPinY);
  if (y > TABLE_MAX_Y / 2) {
    y = messureSingle(trigPinY2, echoPinY2);
  }
  float z = messureSingle(trigPinZ, echoPinZ);
  sendJSON(x, y, z);
}

void sendJSON(float x, float y, float z) {
    String s = "{ \"x\":" + String(x) + ", \"y\":" + String(y) + ", \"z\":"  + String(z) +"}";
    Serial.println(s);
}

float messureSingle(int trigPin, int echoPin) {
  ave.clear();
  // Collect samples
  for(int i=0; i< SAMPLES_PER_MEASURE; i++) {
    ave.push(dist(trigPin, echoPin));
    delay(20);
  }
  return ave.mode();
}

float dist(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    return microsecondsToCentimeters(pulseIn(echoPin, HIGH, PULSE_TIMEOUT));
}

float microsecondsToCentimeters(long microseconds) {
  return microseconds / 29.0 / 2.0;
}




