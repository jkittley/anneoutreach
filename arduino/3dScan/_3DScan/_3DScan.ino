
#include <Average.h>

const int trigPinX = 2;
const int echoPinX = 3;

const int trigPinY = 4;
const int echoPinY = 5;

const int trigPinZ = 6;
const int echoPinZ = 7;

const int PULSE_TIMEOUT_X = 50000;
const int PULSE_TIMEOUT_Y = 10000;
const int PULSE_TIMEOUT_Z = 10000;

int sampleInterval = 250;
int samples_per_reading = 30;
int baudRate = 9600;

Average<float> ave(samples_per_reading);

void setup() {
  // initialize serial communication:
  Serial.begin(baudRate);
  Serial.println("ONLINE");
  pinMode(trigPinX, OUTPUT);
  pinMode(trigPinY, OUTPUT);
  pinMode(trigPinZ, OUTPUT);
  pinMode(echoPinX, INPUT);
  pinMode(echoPinY, INPUT);
  pinMode(echoPinZ, INPUT);
}

void loop() {  
  float y = messure(trigPinY, echoPinY, PULSE_TIMEOUT_Y);
  float z = messure(trigPinZ, echoPinZ, PULSE_TIMEOUT_Z);
  float x = messure(trigPinX, echoPinX, PULSE_TIMEOUT_X);
  sendJSON(x, y, z);
}

void sendJSON(float x, float y, float z) {
    String s = "{ \"x\":" + String(x) + ", \"y\":" + String(y) + ", \"z\":"  + String(z) +"}";
    Serial.println(s);
}

float messure(int trigPin, int echoPin, int timeout) {
  float smax = 0;
  float stot = 0;
  ave.clear();
  // Collect samples
  for(int i=0; i< samples_per_reading; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    float dist = microsecondsToCentimeters(pulseIn(echoPin, HIGH, timeout));
    smax = max(smax, dist);
    stot += dist;
    ave.push(dist);
    // Pause before continuing
    delay(sampleInterval / samples_per_reading / 3);
  }
  return ave.mode();
  //return stot / samples_per_reading;
  //return smax;
}

float microsecondsToCentimeters(long microseconds) {
  return microseconds / 29.0 / 2.0;
}

float avgSamples(float samples[]) {
  float total = 0;
  //Serial.print("[");
  for(int i=0; i< samples_per_reading; i++) {
    //Serial.print(samples[i]);
    //Serial.print(",");
    total = total + samples[i];
  }
  //Serial.println("]");
  return total / samples_per_reading;
}


