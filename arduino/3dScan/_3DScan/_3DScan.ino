
#include <Average.h>

// X Sensor pins
const int trigPinX = 6;
const int echoPinX = 7;
const int trigPinX2 = 8;
const int echoPinX2 = 9;

// Y Sensor pins
const int trigPinY = 4;
const int echoPinY = 5;
const int trigPinY2 = 2;
const int echoPinY2 = 3;

// Z Sensor pins
const int trigPinZ = 10;
const int echoPinZ = 11;

// Constants
const int   PULSE_TIMEOUT = 10000;    // Max time to wait for a sensor to respond
const int   TOLLERENCE_X  = 10;       // 
const int   TOLLERENCE_Y  = 10;       // 
const float CARRAIGE_WIDTH = 6.0;     // Width of carraige in CM
const int   TABLE_WIDTH = 102;        // Width of table in CM
const int   TABLE_DEPTH = 61;         // Depth of table in CM
const int   MEASURE_INTERVAL = 200;   // Period between meassurements
const int   SAMPLES_PER_MEASURE = 5;  // Number of samples used to create a messurement
const int   BAUD_RATE = 9600;         // Serial comms rate

// TMP storage vars
int prev_x  = 0;
int prev_x2 = 0;
int prev_y  = 0;
int prev_y2 = 0;
Average<float> ave(SAMPLES_PER_MEASURE);

void setup() {
  // initialize serial
  Serial.begin(BAUD_RATE);
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
  // Report Settings
  reportSettings();
}

void reportSettings() {
  String s = "{";
  s += "\"PULSE_TIMEOUT\":"       + String(PULSE_TIMEOUT) + ","; 
  s += "\"TOLLERENCE_X\":"        + String(TOLLERENCE_X) + ","; 
  s += "\"TOLLERENCE_Y\":"        + String(TOLLERENCE_Y) + ","; 
  s += "\"CARRAIGE_WIDTH\":"      + String(CARRAIGE_WIDTH) + ","; 
  s += "\"TABLE_WIDTH\":"         + String(TABLE_WIDTH) + ","; 
  s += "\"TABLE_DEPTH\":"         + String(TABLE_DEPTH) + ","; 
  s += "\"MEASURE_INTERVAL\":"    + String(MEASURE_INTERVAL) + ","; 
  s += "\"SAMPLES_PER_MEASURE\":" + String(SAMPLES_PER_MEASURE) + ","; 
  s += "\"BAUD_RATE\":"           + String(BAUD_RATE);
  s += "}";
  Serial.println(s);
}


void loop() {  
  float x  = CARRAIGE_WIDTH/2 + messure(trigPinX, echoPinX);
  float x2 = messure(trigPinX2, echoPinX2) - CARRAIGE_WIDTH/2;
  float y  = CARRAIGE_WIDTH/2 + messure(trigPinY, echoPinY);  
  float y2 = 0.0;
  float z  = messure(trigPinZ, echoPinZ);

  if (abs(x + x2 - TABLE_WIDTH ) < TOLLERENCE_X) {
    // Reading is good
    prev_x  = x;
    prev_x2 = x2;
  } else {
    // Reading is bad
    float diff_x  = abs(prev_x  - x);
    float diff_x2 = abs(prev_x2 - x2);
    // Which value is closest to the last good reading
    if (diff_x < diff_x2) {
      // Do nothing i.e. use x as the trusted value
    } else {
      // Use X2 as the trusted value
      x = TABLE_WIDTH - x2;      
    }
  }

  sendJSON(x, y, z, x2, y2);
}

// Output the sensor data
void sendJSON(float x, float y, float z, float x2, float y2) {
    String s = "{ \"x\":" + String(x) + ", \"y\":" + String(y) + ", \"z\":"  + String(z) + ", \"x2\":"  + String(x2) + ", \"y2\":"  + String(y2) +"}";
    Serial.println(s);
}

float messure(int trigPin, int echoPin) {
  ave.clear();
  // Collect samples
  for(int i=0; i< SAMPLES_PER_MEASURE; i++) {
    ave.push(dist(trigPin, echoPin));
    delay(float(MEASURE_INTERVAL) / SAMPLES_PER_MEASURE / 4);
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




