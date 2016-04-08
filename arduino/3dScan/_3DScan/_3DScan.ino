
#include <Average.h>


const int trigPinX = 6;
const int echoPinX = 7;
const int trigPinX2 = 8;
const int echoPinX2 = 9;

const int trigPinY = 4;
const int echoPinY = 5;
const int trigPinY2 = 2;
const int echoPinY2 = 3;

const int trigPinZ = 10;
const int echoPinZ = 11;

const int PULSE_TIMEOUT = 10000;
const int TOLLERENCE_X_MM = 20; 
const int TOLLERENCE_Y_MM = 20; 

// In CM
const int CARRAIGE_WIDTH = 6; // Extra 2 is for sensor lenses
const int TABLE_MAX_X = 102;
const int TABLE_MAX_Y = 61;

const int SAMPLE_INTERVAL = 50;
const int SAMPLES_PER_MEASURE = 10;
const int BAUD_RATE = 9600;

int prev_x  = 0;
int prev_x2 = 0;

int prev_y  = 0;
int prev_y2 = 0;

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
  float x  = messureSingle(trigPinX, echoPinX);
  float x2 = messureSingle(trigPinX2, echoPinX2);
  float y  = messureSingle(trigPinY, echoPinY);  
  float z = messureSingle(trigPinZ, echoPinZ);

  if (abs(x + x2 + CARRAIGE_WIDTH - TABLE_MAX_X ) < 10) {
    // Reading is good
    prev_x  = x;
    prev_x2 = x2;
  } else {
    // Reading is bad
    float diff_x  = abs(prev_x  - x);
    float diff_x2 = abs(prev_x2 - x2);
    if (diff_x < diff_x2) {
      // Do nothing i.e. use x as the trusted value
    } else {
      // Use X2 as the trusted value
      x = TABLE_MAX_X - x2;      
    }
  }

  
  sendJSON(x, y, z);
  //sendJSON(x, y, z, x2, y2);
}

void sendJSON(float x, float y, float z) {
    String s = "{ \"x\":" + String(x) + ", \"y\":" + String(y) + ", \"z\":"  + String(z) +"}";
    Serial.println(s);
}

void sendJSON(float x, float y, float z, float x2, float y2) {
    String s = "{ \"x\":" + String(x) + ", \"y\":" + String(y) + ", \"z\":"  + String(z) + ", \"x2\":"  + String(x2) + ", \"y2\":"  + String(y2) +"}";
    Serial.println(s);
}

float messureSingle(int trigPin, int echoPin) {
  ave.clear();
  // Collect samples
  for(int i=0; i< SAMPLES_PER_MEASURE; i++) {
    ave.push(dist(trigPin, echoPin));
    delay(float(SAMPLE_INTERVAL) / SAMPLES_PER_MEASURE / 4);
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




