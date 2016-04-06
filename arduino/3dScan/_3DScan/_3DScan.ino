
const int trigPin  = 3;
const int echoPinX = 8;
const int echoPinY = 10;
const int echoPinZ = 9;

const int PULSE_TIMEOUT_X = 10000;
const int PULSE_TIMEOUT_Y = 10000;
const int PULSE_TIMEOUT_Z = 10000;

int sampleInterval = 250;
int samples_per_reading = 1;
int baudRate = 9600;

void setup() {
  // initialize serial communication:
  Serial.begin(baudRate);
  Serial.println("ONLINE");
  pinMode(trigPin, OUTPUT);
  pinMode(echoPinX, INPUT);
  pinMode(echoPinY, INPUT);
  pinMode(echoPinZ, INPUT);
}

void loop() { 
  float x = messure(echoPinX, PULSE_TIMEOUT_X);
  float y = messure(echoPinY, PULSE_TIMEOUT_Y);
  float z = messure(echoPinZ, PULSE_TIMEOUT_Z);
  sendJSON(x, y, z);
  delay(sampleInterval);
}

void sendJSON(float x, float y, float z) {
    String s = "{ \"x\":" + String(x) + ", \"y\":" + String(y) + ", \"z\":"  + String(z) +"}";
    Serial.println(s);
}

float messure(int pin, int timeout) {
  //float samples[samples_per_reading];
  // Collect samples
  //for(int i=0; i< samples_per_reading; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    //samples[i] = microsecondsToCentimeters(pulseIn(pin, HIGH, timeout));
    //delayMicroseconds(10);
  //}
  return microsecondsToCentimeters(pulseIn(pin, HIGH, timeout));
  //return avgSamples(samples);
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


