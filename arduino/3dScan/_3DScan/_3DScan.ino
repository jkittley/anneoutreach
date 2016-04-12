
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
const int   TOLLERENCE_X  = 150;      // If you add x and x2 how far from table width is acceptable
const int   TOLLERENCE_Y  = 150;      // If you add y and y2 how far from table depth is acceptable
const float CARRAIGE_WIDTH = 60.0;    // Width of carraige in MM
const float TABLE_WIDTH = 1020.0;     // Width of table in MM
const float TABLE_DEPTH = 610.0;      // Depth of table in MM
const float TABLE_HEIGHT = 215.0;     // Height of table in MM (sensor to floor)
const int   MEASURE_INTERVAL = 200;   // Period between meassurements
const int   SAMPLES_PER_MEASURE = 10; // Number of samples used to create a messurement
const int   BAUD_RATE = 9600;         // Serial comms rate

// TMP storage vars
int prev_x1 = 0;
int prev_x2 = 0;
int prev_y1 = 0;
int prev_y2 = 0;


void setup() {
  // initialize serial
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    // Wait for serial to establish
  }
  Serial.println("{ \"status\":\"Online\"");
   
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

int sampleCounter = 0;
float best_x_diff  = -1;
float pause = MEASURE_INTERVAL / SAMPLES_PER_MEASURE / 5;
float x  = 0;
float y  = 0;
float z  = 0;
Average<float> ave_y(SAMPLES_PER_MEASURE);
Average<float> ave_z(SAMPLES_PER_MEASURE);

void loop() {  
  float x1 = (CARRAIGE_WIDTH/2) + dist(trigPinX, echoPinX);
  delay(pause);
  float x2 = (CARRAIGE_WIDTH/2) + dist(trigPinX2, echoPinX2);
  delay(pause);
  float y1 = (CARRAIGE_WIDTH/2) + dist(trigPinY, echoPinY);  
  delay(pause);
  float y2 = 0.0;
  delay(pause);
  float z1 = TABLE_HEIGHT - dist(trigPinZ, echoPinZ);
  delay(pause);
  
  ave_y.push(floor(y1));
  ave_z.push(floor(z1));

  // Get best X
  float diff_x = floor(abs(x1-x2));
  if (best_x_diff == -1 || diff_x < best_x_diff) {
    best_x_diff = diff_x;
    x = x1;
  }

  // Return X, Y, Z when all samples taken
  if (sampleCounter == SAMPLES_PER_MEASURE) {
    // Average ones
    y = ave_y.mode();
    z = ave_z.mode();
    // Prevent negative values
    if (x<0) x = 0;
    if (y<0) y = 0;
    if (z<0) z = 0;
    sendJSON(x, y, z, x1, y1, x2, y2);
    // Reset
    sampleCounter = 0;
    x = 0;
    y = 0;
    z = 0;
    best_x_diff = -1;    
  }
  sampleCounter++;
}



// Output the sensor data
void sendJSON(float x, float y, float z, float x1, float y1, float x2, float y2) {
    String s = "{";
    // Basics
    s += "\"x\":"     + String(int(floor(x)))     + ",";
    s += "\"y\":"     + String(int(floor(y)))     + ",";
    s += "\"z\":"     + String(int(floor(z)))     + ",";
    // Debug
    s += "\"x1\":"    + String(x1)    + ",";
    s += "\"y1\":"    + String(y1)    + ",";
    s += "\"x2\":"    + String(x2)    + ",";
    s += "\"y2\":"    + String(y2);
    
    s += "}";
    Serial.println(s);
}


float dist(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    return microsecondsToMM(pulseIn(echoPin, HIGH, PULSE_TIMEOUT));
}

float microsecondsToMM(long microseconds) {
  return (microseconds / 29.0 / 2.0) * 10;
}




