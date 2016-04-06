
const int trigPin  = 3;
const int echoPinX = 8;
const int echoPinY = 10;
const int echoPinZ = 9;

const int PULSE_TIMEOUT_X = 6000;
const int PULSE_TIMEOUT_Y = 6000;
const int PULSE_TIMEOUT_Z = 3500;

int sampleInterval = 250;
int samples_per_reading = 1; //30;
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
  float total = 0;
  for(int i=0; i< samples_per_reading; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    total = total + microsecondsToCentimeters(pulseIn(pin, HIGH, timeout));
  }
  
  return total / samples_per_reading;
}

float microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

void setHome() {
  Serial.println("--> Setting Zero Point");
}

