
// Initial angle for signal
float angle = 0.0;

// Period between each sample in milliseconds
int sampleInterval = 250;

// Init
void setup() {
  // start serial port at 9600 bps:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
}

// Loop forever
void loop() {
    
    // Convert angle to radians
    float rad = (angle * 71) / 4068;
    
    // Create three different waveforms
    float x = sin(rad) * 100;
    float y = sin(rad - PI/2 ) * 100;
    float z = sin(rad - PI/4) * 100;
    
    // Output the values to the serial port
    sendJSON(x,y,z);

    // Increase Angle and reset to zero if needed
    angle += 15;
    if (angle > 345) {
      angle = 0;
    }
     
    // Pause for sample interval   
    delay(sampleInterval);
}

void sendJSON(float x, float y, float z) {
    String s = "{ \"x\":" + String(x) + ", \"y\":" + String(y) + ", \"z\":"  + String(z) +"}";
    Serial.println(s);
}

