#include <Stepper.h>
#include <Arduino.h>

const int stepsPerRevolution = 2048;//4096;//2048;  // Typical for many stepper motors
const float gearDiameter = 14.8;     // in mm
const float pi = 3.141592653589793;
const float circumference = pi * gearDiameter;
const float stepsPerMillimeter = stepsPerRevolution / circumference;
const float maxTide = 2.88;  // Max tide in meters
const float minTide = 0.21;  // Min tide in meters
const float gearBarLength = 40;  // Length of the gear bar in mm
const float conversionFactor = gearBarLength / (maxTide - minTide);
float currentPositionMM = 0;

// Define stepper motor connections
Stepper stepper1(stepsPerRevolution, 9, 11, 10, 12);
Stepper stepper2(stepsPerRevolution, 3, 5, 4, 6);

// Define pins for moon phases LEDs
int ledPin1 = A2;
int ledPin2 = A3;
int ledPin3 = A4;
int ledPin4 = A5;

// day indicator
int dayIndicatorLED = A1; 

// Define pin for button
int buttonPin = 2;

// Define a structure to hold tide data and its size
struct TideData {
    float* data;
    int size;
};

// Tides data
float fullMoonTides[] = {4, 2.27, 1.67, 2.72, 0.61, 4, 2.31, 1.97, 2.81, 0.36, 4, 2.39, 2.23, 2.87, 0.22, 3, 2.49, 2.44, 2.88, 4, 0.21, 2.59, 2.58, 2.83, 4, 0.29, 2.66, 2.65, 2.75, 4, 0.45, 2.68, 2.65, 2.65};
float lastQuarterTides[] = {2, 0.63, 2.66, 2, 0.82, 2.61, 4, 1.01, 2.54, 1.96, 1.99, 3, 1.2, 2.47, 1.77, 4, 1.95, 1.39, 2.44, 1.56, 4, 1.95, 1.59, 2.48, 1.32, 4, 2.02, 1.8, 2.53, 1.08, 4, 2.12, 1.99, 2.59, 0.87};
float newMoonTides[] = {4, 2.23, 2.17, 2.63, 0.69, 4, 2.35, 2.33, 2.68, 0.56, 4, 2.46, 2.46, 2.73, 0.49, 2, 2.78, 0.47, 2, 2.81, 0.48, 1, 2.79, 2, 0.53, 2.73};
float firstQuarterTides[] = {2, 0.62, 2.65, 2, 0.74, 2.6, 2, 0.92, 2.57, 4, 1.14, 2.6, 1.65, 1.96, 3, 1.42, 2.68, 1.23, 4, 2.01, 1.73, 2.79, 0.81}; 

// Create instances of the structure
TideData tidePhases[] = {
    {fullMoonTides, sizeof(fullMoonTides) / sizeof(fullMoonTides[0])},
    {lastQuarterTides, sizeof(lastQuarterTides) / sizeof(lastQuarterTides[0])},
    {newMoonTides, sizeof(newMoonTides) / sizeof(newMoonTides[0])},
    {firstQuarterTides, sizeof(firstQuarterTides) / sizeof(firstQuarterTides[0])}
};


// Define moon phase
int moonPhase = 0;
int currentTideIndex = 0;  // Index in the current tide array
int dayTideCount = 0;

void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(dayIndicatorLED, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Set speed and initial positions for steppers
  stepper1.setSpeed(7);
  stepper2.setSpeed(7);
  handleMoonPhase(moonPhase); // Initialize LEDs

}

void loop() {
    simulateTide(tidePhases[moonPhase]);
    handleMoonPhase(moonPhase); // Update LEDs based on new phase
    moonPhase = (moonPhase + 1) % 4;  // Manually advance to next phase
    delay(1000); // Delay to slow down phase transition for visualization
}

void handleMoonPhase(int phase) {
  // Turn on LEDs based on moon phase
  if (phase == 0) {
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
    digitalWrite(ledPin4, HIGH);
  } else if (phase == 1) {
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, LOW);
    digitalWrite(ledPin4, LOW);
  } else if (phase == 2) {
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
    digitalWrite(ledPin4, LOW);
  } else if (phase == 3) {
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, HIGH);
    digitalWrite(ledPin4, HIGH);
  }
}

void simulateTide(TideData tides){
  int i = 0;  // Index to traverse through the entire data array
  Serial.begin(9600);

    while (i < tides.size) {
        int dayTideCount = (int)tides.data[i++];  // Read the number of tides for this day, increment i to point to next tide height
        Serial.print("day tide count: "); Serial.println(dayTideCount);
        dayIndicator();  // Signal a new day's tide simulation

        // Loop through the number of tides for this day
        for (int j = 0; j < dayTideCount && i < tides.size; j++) {
            float currentTide = tides.data[i];  // Read the current tide height
            float targetPositionMM = (currentTide - minTide) * conversionFactor;  // Calculate target position in mm on the gear bar

            // Calculate steps to move from current position to target position
            float deltaMM = targetPositionMM - currentPositionMM;
            int steps = round(deltaMM * stepsPerMillimeter);

            Serial.print("Current Tide: "); Serial.println(currentTide);
            Serial.print("Target Position (mm): "); Serial.println(targetPositionMM);
            Serial.print("Current Position (mm): "); Serial.println(currentPositionMM);
            Serial.print("Delta (mm): "); Serial.println(deltaMM);
            Serial.print("Steps to move: "); Serial.println(steps);

            int direction = (steps >= 0) ? 1 : -1; // Determine the direction based on the sign of steps

            for (int i = 0; i < abs(steps); i++) {
                stepper1.step(direction); // Step motor 1 forward or backward by 1 step
                stepper2.step(direction); // Step motor 2 forward or backward by 1 step
                delay(10); // Short delay to allow for stepper motors to respond; adjust as needed for your application
            }


            // Update current position
            currentPositionMM = targetPositionMM;

            i++;  // Move to the next tide height or the next day's tide count
        }
    }

}

void dayIndicator(){
  digitalWrite(dayIndicatorLED, HIGH); // Indicate new day
  delay(1000); // Visible indication
  digitalWrite(dayIndicatorLED, LOW);

}





