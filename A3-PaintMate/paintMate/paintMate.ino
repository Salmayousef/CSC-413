#include <LiquidCrystal.h>
//#include <NewPing.h>

//#define TRIGGER_PIN  10  // Arduino pin tied to trigger pin on the ultrasonic sensor.
//#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
//#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters).

//NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Define pin for the button, thermistor and buzzer
const int buttonPin = 6;  
const int thermistorPin = A0;  
const int buzzerPin = 7; 

int currentMoodIndex = 0;  // Current index of the mood in the array
unsigned long lastDebounceTime = 0;  // Last time the button was pressed
unsigned long debounceDelay = 200;  // Debouncing delay in milliseconds

unsigned long previousMillis = 0;        // Stores last time temperature was updated
const long interval = 1800000;           // Interval at which to sound the buzzer (30 minutes)

const char* moods[] = {"Happy", "Sad", "Energetic", "Reflective", "Calm", "Inspired"};
const int totalMoods = sizeof(moods) / sizeof(moods[0]);

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  lcd.begin(16,2);
  lcd.print("Hello");
  analogReference(DEFAULT);
}

void loop() {
  unsigned long currentMillis = millis();
  int reading = digitalRead(buttonPin);
  float temp = readThermistorTemperature();

  if (reading == HIGH) {
    unsigned long currentTime = millis();
    if ((currentTime - lastDebounceTime) > debounceDelay) {
      currentMoodIndex = (currentMoodIndex + 1) % totalMoods;  // Cycle through moods
      lastDebounceTime = currentTime;
    }
  }

  lcd.clear();

  lcd.setCursor(0, 1);
  lcd.print("Mood: ");
  lcd.print(moods[currentMoodIndex]);

  clearRow(0); 
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" C");

  delay(2000);

  clearRow(0); 
  lcd.setCursor(0, 0);
  lcd.print(getPaintingSuggestion(currentMoodIndex));

  delay(5000);

  clearRow(0); 
  lcd.setCursor(0, 0);
  lcd.print(getPaintTypeSuggestion(temp));

  delay(5000);

  //clearRow(0); 
  //lcd.setCursor(0, 0);
  //lcd.print("Distance: ");
  //lcd.print(sonar.ping_cm());
  //lcd.print(" cm");

  //delay(5000);

  // Check if it's time to take a break, sound the buzzer if so.
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    clearRow(0); 
    lcd.setCursor(0, 0);
    lcd.print("Take a break!");
    soundBuzzer();
  }

  delay(100);  // Update delay
}

float readThermistorTemperature() {
  int val = analogRead(thermistorPin);
  float resistance = (1023.0 / val - 1) * 10000; // 10k resistor in series
  float temperature = 1.0 / (log(resistance / 10000) / 3950 + 1 / 298.15) - 273.15; // Calculate temperature in Celsius
  return temperature;
}

const char* getPaintingSuggestion(int mood) {
  if (mood == 0) {
    return "Paint Landscapes";
  } else if (mood == 1) {
    return "Paint Portraits";
  } else if (mood == 2) {
    return "Paint Abstracts";
  } else if (mood == 3) {
    return "Paint Cityscapes";
  } else if (mood == 4) {
    return "Paint Seascapes";
  } else if (mood == 5) {
    return "Paint Surrealism";
  }
  return "Free Style"; // Default if something goes wrong
}

const char* getPaintTypeSuggestion(float temp) {
  if (temp < 10) {
    return "Use Oil-based";
  } else if (temp >= 10 && temp < 20) {
    return "Use Acrylics";
  } else {
    return "Use Watercolors";
  }
}

void soundBuzzer() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzerPin, HIGH); // Turn buzzer on
    delay(1000);                   // Buzzer on for 1 second
    digitalWrite(buzzerPin, LOW);  // Turn buzzer off
    delay(1000);                   // Buzzer off for 1 second
  }
}

void clearRow(int row) {
  lcd.setCursor(0, row);
  for(int i = 0; i < 16; i++) {
    lcd.print(" "); // Print spaces over the entire row
  }
  lcd.setCursor(0, row); // Optionally reset the cursor position to the start of the row
}
