const int trigPin  = 8;
const int echoPinX = 10;
const int echoPinY = 11;
const int echoPinZ = 12;

int sampleInterval = 250;
int baudRate = 9600;

void setup() {
  // initialize serial communication:
  Serial.begin(baudRate);

  pinMode(echoPinX, INPUT);
  pinMode(echoPinY, INPUT);
  pinMode(echoPinZ, INPUT);
}

void loop()
{
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  float x = messure(echoPinX);
  float y = messure(echoPinY);
  float z = messure(echoPinZ);
  sendJSON(x, y, z);

  delay(sampleInterval);
}

void sendJSON(float x, float y, float z) {
    String s = "{ \"x\":" + String(x) + ", \"y\":" + String(y) + ", \"z\":"  + String(z) +"}";
    Serial.println(s);
}

float messure(int pin) {
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return microsecondsToCentimeters(pulseIn(pin, HIGH));
}

float microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
