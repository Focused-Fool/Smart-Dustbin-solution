#include <Servo.h>

// Pin definitions for motion detection (person approaching)
const int trigPinMotion = 9;
const int echoPinMotion = 10;

// Pin definitions for fullness detection (bin full)
const int trigPinFull = 6;
const int echoPinFull = 7;

// Servo motor pin
const int servoPin = 11;

// LED pin (to indicate bin is full)
const int ledPin = 5;

Servo myServo;

// Fullness threshold (set this based on the size of your dustbin)
int binFullThreshold = 50; // Example: 50 cm

// Boolean to track if the bin is full
bool isBinFull = false;
bool isLidOpen = false; // Track if the lid is open

void setup() {
  // Motion detection setup
  pinMode(trigPinMotion, OUTPUT);
  pinMode(echoPinMotion, INPUT);

  // Fullness detection setup
  pinMode(trigPinFull, OUTPUT);
  pinMode(echoPinFull, INPUT);

  // LED setup
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Ensure LED is off initially

  // Servo motor setup
  myServo.attach(servoPin);
  myServo.write(0); // Start with the lid closed

  // Serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // 1. Motion Detection for Opening Lid
  long durationMotion, distanceMotion;
  digitalWrite(trigPinMotion, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinMotion, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinMotion, LOW);

  durationMotion = pulseIn(echoPinMotion, HIGH);
  distanceMotion = (durationMotion * 0.034) / 2;

  // Check if the bin is not full and lid is not open before opening
  if (distanceMotion < 30 && !isBinFull && !isLidOpen) {  // If someone is near (within 30 cm) and bin is not full and lid is closed
    Serial.println("Person detected, opening lid...");
    myServo.write(90);  // Open the lid
    isLidOpen = true;   // Set lid open flag
    delay(5000);        // Keep it open for 5 seconds
    myServo.write(0);   // Close the lid
    isLidOpen = false;  // Reset lid open flag
  }

  // 2. Fullness Detection
  long durationFull, distanceFull;
  digitalWrite(trigPinFull, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinFull, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinFull, LOW);

  durationFull = pulseIn(echoPinFull, HIGH);
  distanceFull = (durationFull * 0.034) / 2;

  Serial.print("Distance to trash: ");
  Serial.println(distanceFull);  // Print distance for debugging

  if (!isBinFull && distanceFull < binFullThreshold) {  // If the bin just became full
    Serial.println("Dustbin is full! Turning on LED...");
    digitalWrite(ledPin, HIGH);  // Turn on the red LED
    isBinFull = true;            // Set the flag that the bin is full
  }

  // 3. Check if the bin is emptied
  if (isBinFull && distanceFull > binFullThreshold) {  // If the bin is emptied (trash level dropped)
    Serial.println("Dustbin is emptied! Turning off LED...");
    digitalWrite(ledPin, LOW);  // Turn off the red LED
    isBinFull = false;          // Reset the full status
  }

  delay(5000);  // Small delay before looping again
}
