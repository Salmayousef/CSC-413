
#include <LiquidCrystal.h>
#include <NewPing.h>

#define TRIGGER_PIN  9  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     10  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 1000 // Maximum distance we want to ping for (in centimeters).
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



// Define pin for the thermistors and buzzer
const int thermistorPin1 = A0;
const int thermistorPin2 = A1;  
const int buzzerPin = 7; 

int currentMoodIndex = 0;  // Current index of the mood in the array

unsigned long previousMillis = 0;        // Stores last time temperature was updated
const long interval = 1800000;           // Interval at which to sound the buzzer (30 minutes)
int delayTime = 3500;   // length of time each message is displayed
 
const char* moods[] = {"sad", "stressed", "angry", "relaxed"};

const char* messages[] = {"Good Job!", "Keep Going!", "Lookin' Good!"};

void setup() {
  pinMode(buzzerPin, OUTPUT);
  lcd.begin(16,2);
  analogReference(DEFAULT);
}

void loop() {

  unsigned long currentMillis = millis(); //time elapsed
  float temp = readThermistor1Temperature();
  float weather = readThermistor2Temperature();

  lcd.clear();

  // time for thermistor to adjust
  lcd.setCursor(0,0);
  lcd.print("Reading your");
  lcd.setCursor(0,1);
  lcd.print("mood...");

  delay(delayTime);

  //determine mood from skin temp
  if (temp > 32){
    currentMoodIndex = 2;
  } else if (temp > 30) {
    currentMoodIndex = 1;
  } else if (temp < 28) {
    currentMoodIndex = 0;
  } else {
    currentMoodIndex = 3;
  }
  
  clearRow(1);
  clearRow(0);
  
  // print current mood
  lcd.setCursor(0, 0);  
  lcd.print("You seem ");
  lcd.print(moods[currentMoodIndex]);
  lcd.setCursor(0, 1);
  lcd.print("today...");

  delay(delayTime);
  clearRow(1);
  clearRow(0);
  delay(2);

  // print supportive mood message
  lcd.setCursor(0, 0);  
  if (currentMoodIndex == 0) {
    lcd.print("I'm sorry to");
    lcd.setCursor(0, 1);
    lcd.print("hear that! :(");
  } else if (currentMoodIndex == 1) {
    lcd.print("Things have been");
    lcd.setCursor(0, 1);
    lcd.print("pretty crazy");
  } else if (currentMoodIndex == 2) {
    lcd.print("I'm sorry your");
    lcd.setCursor(0, 1);
    lcd.print("day is not good");
  } else if (currentMoodIndex == 3) {
    lcd.print("Amazing! Let's");
    lcd.setCursor(0, 1);
    lcd.print("keep it going!");
  }

  delay(delayTime);
  clearRow(1);
  clearRow(0);
  delay(2);

  // print paint it out text
  lcd.setCursor(0, 0);
  lcd.print("Let's paint it");
  lcd.setCursor(0, 1);
  lcd.print("out!!");
  
  delay(delayTime);
  clearRow(1);
  clearRow(0);
  delay(2);

  // print paint suggestion
  lcd.setCursor(0, 0);
  lcd.print("Let's try a");
  lcd.setCursor(0, 1);
  lcd.print(getPaintingSuggestion(currentMoodIndex));

  delay(delayTime);
  clearRow(1);
  clearRow(0);
  
  if (weather < 20) {
    lcd.setCursor(0, 0);
    lcd.print("its a bit cold");
  } else if (weather >= 20 && weather < 25) {
    lcd.setCursor(0, 0);
    lcd.print("its moderate");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("its a bit hot");
  }
    lcd.setCursor(0, 1);
    lcd.print("out today...");
  
  delay(delayTime);
  clearRow(1);
  clearRow(0);

  // print paint type suggestion
  lcd.setCursor(0, 0);
  lcd.print("Let's use");
  lcd.setCursor(0, 1);
  lcd.print(getPaintTypeSuggestion(temp));

  delay(delayTime);
  clearRow(1);
  clearRow(0);

  // wait for user to start painting
  float distance = sonar.ping_cm();;
  while (distance > 15 || distance == 0){
    lcd.setCursor(0, 0);
    lcd.print("Start now!");
    distance = sonar.ping_cm();
  }

  // wait for user to stop painting, repeat motivational messages
  int messageCounter = 0;
  while (distance < 200 && distance != 0) {
    distance = sonar.ping_cm();
    delay(delayTime);
    clearRow(1);
    clearRow(0);
    
    lcd.print(messages[messageCounter]);
 
    if (messageCounter == 2){
      messageCounter = 0;
    } else {
      messageCounter++;
    }
    
    // Check if it's time to take a break, sound the buzzer if so.
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      clearRow(1);
      clearRow(0);
      lcd.setCursor(0, 0);
      lcd.print("Take a break!");
      soundBuzzer();
      delay(delayTime);
    }
  } 

  delay(delayTime);
  clearRow(1);
  clearRow(0);
  lcd.print("WOW! it looks");
  lcd.setCursor(0, 1);
  lcd.print("fantastic!!");

  delay(delayTime);
  clearRow(1);
  clearRow(0);
  lcd.setCursor(0, 0);
  lcd.print("I hope youre");
  lcd.setCursor(0, 1);
  lcd.print("feeling better!!");

  delay(10000);
}

float readThermistor1Temperature() {
  int val = analogRead(thermistorPin1);
  float resistance = (1023.0 / val - 1) * 10000; // 10k resistor in series
  float temperature = 1.0 / (log(resistance / 10000) / 3950 + 1 / 298.15) - 273.15; // Calculate temperature in Celsius
  return temperature;
}

float readThermistor2Temperature() {
  int val = analogRead(thermistorPin2);
  float resistance = (1023.0 / val - 1) * 10000; // 10k resistor in series
  float temperature = 1.0 / (log(resistance / 10000) / 3950 + 1 / 298.15) - 273.15; // Calculate temperature in Celsius
  return temperature;
}

const char* getPaintingSuggestion(int mood) {
  if (mood == 0) {
    return "landscape";
  } else if (mood == 1) {
    return "portrait";
  } else if (mood == 2) {
    return "abstract";
  } else if (mood == 3) {
    return "cityscape";
  }
  return "Free Style"; // Default if something goes wrong
}

const char* getPaintTypeSuggestion(float temp) {
  if (temp < 10) {
    return "oil paint";
  } else if (temp >= 10 && temp < 20) {
    return "acrylic paint";
  } else {
    return "watercolor paint";
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
