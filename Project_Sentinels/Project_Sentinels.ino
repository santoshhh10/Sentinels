/*
  Project Sentinels - Auto Timeout Version
  - Detects pulses from optocoupler (active LOW)
  - Automatically adjusts timeout based on measured pulse interval
*/

const int sensorPin = 2;

unsigned long previousTime = 0;
unsigned long pulseInterval = 0;
unsigned long timeout = 5000; // default startup timeout
bool faultDetected = false;
bool pulseActive = false;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT_PULLUP);  // safer than INPUT, avoids floating
  Serial.println("--- Project Sentinels System Online ---");
  Serial.println("Monitoring for normal pulse...");
}

void loop() {
  int sensorState = digitalRead(sensorPin);

  // --- Detect falling edge (pulse start) ---
  if (sensorState == LOW && !pulseActive) {
    unsigned long currentTime = millis();

    if (previousTime > 0) {
      pulseInterval = currentTime - previousTime;
      Serial.print("Pulse Detected! Interval: ");
      Serial.print(pulseInterval);
      Serial.println(" ms");

      // auto-adjust timeout to 1.5 × measured interval
      timeout = pulseInterval * 1.5;
    }

    previousTime = currentTime;
    pulseActive = true;

    if (faultDetected) {
      Serial.println("--- PULSE RESTORED! System is back to normal. ---");
      faultDetected = false;
    }
  }

  // --- Detect rising edge (pulse end) ---
  if (sensorState == HIGH && pulseActive) {
    pulseActive = false;
  }

  // --- Timeout check ---
  if (millis() - previousTime > timeout && previousTime > 0 && !faultDetected) {
    Serial.println("******************************************");
    Serial.println("ALERT: NO PULSE DETECTED! FENCE FAULT!");
    Serial.println("******************************************");
    faultDetected = true;
  }
}