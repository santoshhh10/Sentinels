/*
  Project Sentinels - With Ignore Mode
  - Reads pulses from optocoupler (555 output)
  - Type 'X' in Serial Monitor to toggle ignore mode
  - When ignore is ON → Arduino acts like no pulses are arriving
*/

const int sensorPin = 2;

unsigned long previousTime = 0;
unsigned long pulseInterval = 0;
unsigned long timeout = 5000; 
bool faultDetected = false;
bool pulseActive = false;
bool ignorePulse = false;  // NEW flag

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT_PULLUP);
  Serial.println("--- Project Sentinels System Online ---");
  Serial.println("Monitoring for normal pulse...");
  Serial.println("Type 'X' in Serial Monitor to toggle ignore mode.");
}

void loop() {
  int sensorState = digitalRead(sensorPin);

  // --- Toggle ignore mode from Serial ---
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'X' || c == 'x') {
      ignorePulse = !ignorePulse;
      Serial.print("Ignore mode: ");
      Serial.println(ignorePulse ? "ON (simulating fault)" : "OFF (normal)");
    }
  }

  // --- Detect falling edge (pulse start) ---
  if (sensorState == LOW && !pulseActive && !ignorePulse) {
    unsigned long currentTime = millis();

    if (previousTime > 0) {
      pulseInterval = currentTime - previousTime;
      Serial.print("Pulse Detected! Interval: ");
      Serial.print(pulseInterval);
      Serial.println(" ms");
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