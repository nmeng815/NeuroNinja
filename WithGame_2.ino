#include <LiquidCrystal.h>

// Initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Sound effects
const int tempo = 500; // in quarter note = ms  (300 = 200bpm 400 = 150bpm)
const int notes[] = {
  65,   // C2
  73,   // D2
  98,   // G2
  131,  // C3
  277,  // Df4
  294,  // D4
  311,  // Ef4
  349,  // F4
  370,  // Gf4
  392,  // G4
  415,  // Af4
  466,  // Bf4
  523,  // C5
  554,  // Df5
  622,  // Ef5
  784,  // G5
  2637  // E7
}; 
const int C2 = 0;
const int D2 = 1;
const int G2 = 2;
const int C3 = 3;
const int Df4 = 4;
const int D4 = 5;
const int Ef4 = 6;
const int F4 = 7;
const int Gf4 = 8;
const int G4 = 9;
const int Af4 = 10;
const int Bf4 = 11;
const int C5 = 12;
const int Df5 = 13;
const int Ef5 = 14;
const int G5 = 15;
const int E7 = 16;

const int beginButton = 7; // begin game
const int piezo = 9; // sound output
int currentMillis = 0;

// Define state variables
bool buttonPressed = false;

const int numSensors = 2; 
const int numSamples = 5; // rolling average over every 5 values
const int calibrate = 5; // calibrated over 100 averages

// Define sensor and output pins and thresholds for each sensor
int sensorPins[numSensors] = {A0,A1}; // Example sensor pins
int outputPins[numSensors] = {13,6}; // Example output pins
int rollingSensorVal[numSensors][numSamples];
int sensorIndex[numSensors] = {0, 0};
int thresholds[numSensors] = {250,250}; // |whatever value - calibration value [i]| >= thresholds[i] so plug in the DIFFERENCE needed to trigger

// Arrays to store sensor values
float sensorValues[numSensors];
float calValStore[numSensors][calibrate] = {0};
int calIndex[numSensors] = {0, 0};  
float calValue[numSensors] = {0}; 
float calValFinal[numSensors] = {0}; 

//Gameplay
int score = 0; // score count for game
int mistake = 0;
int duration = 6000; //calculate reaction time starts at 6 seconds to complete motion 
const int minDuration = 700;
int startTime = 0; // the stored beginning value for each reaction time
int randomIndex = 0; // random index to call a random sensor 
int elapsed = 0;
bool correct = false; // user has triggered the correct sensor
bool incorrect = false; // user has triggered the incorrect sensor

void setup() {
  pinMode(piezo, OUTPUT);
  digitalWrite(piezo, LOW); 
  // Set the pin inputs and outputs
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);
  }
  for (int i = 0; i < numSensors; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW); // Ensure outputs are initially off
  }
  
  pinMode(beginButton, INPUT_PULLUP); // Use internal pull-up resistor

  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);
  // Set up serial communication
  Serial.begin(9600);

  lcd.setCursor(0, 0);
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print("Neuro Ninja!");

  //NINJA MUSIC!!
  // tone(piezo, notes[C5], tempo*1);
  // delay(tempo*1);  // Add a delay to give time for the note to play
  // noTone(piezo);
  // tone(piezo, notes[Bf4], tempo*1);
  // delay(tempo*1); 
  // noTone(piezo);
  // tone(piezo, notes[C5], tempo*1);
  // delay(tempo*1); 
  // noTone(piezo);
  // tone(piezo, notes[Ef5], tempo*1);
  // delay(tempo*1); 
  // noTone(piezo);
  // tone(piezo, notes[C5], tempo*2);
  // delay(tempo*2); 
  // noTone(piezo);

  tone(piezo, notes[Ef5], tempo*.75);
  delay(tempo*.75); 
  noTone(piezo);
  tone(piezo, notes[Ef5], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Df5], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Df5], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Ef5], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Df5], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Ef5], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Df5], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Ef5], tempo*1);
  delay(tempo*1); 
  noTone(piezo);

  tone(piezo, notes[Ef4], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[F4], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Gf4], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Ef4], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Gf4], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Af4], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Bf4], tempo*1);
  delay(tempo*1); 
  noTone(piezo);

  tone(piezo, notes[Af4], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Bf4], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Af4], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Gf4], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[F4], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[D4], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[F4], tempo*1);
  delay(tempo*1); 
  noTone(piezo);
  tone(piezo, notes[Bf4], tempo*1);
  delay(tempo*1); 
  noTone(piezo);

  tone(piezo, notes[Ef5], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Df5], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Bf4], tempo*1);
  delay(tempo*1); 
  noTone(piezo);
  tone(piezo, notes[Gf4], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Af4], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Bf4], tempo*1);
  delay(tempo*1); 
  noTone(piezo);

  tone(piezo, notes[Af4], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Bf4], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Af4], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Gf4], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[F4], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Df4], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Ef4], tempo*2);
  delay(tempo*2); 
  noTone(piezo);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press to begin->");
}

void loop() {
  // initialize all reading
  int buttonState = digitalRead(beginButton);
    // Read the sensor value and update rolling average
  for (int i = 0; i < numSensors; i++) {
    int sensorVal= analogRead(sensorPins[i]);
    rollingSensorVal[i][sensorIndex[i]] = sensorVal;
    sensorIndex[i] = (sensorIndex[i] + 1) % numSamples;
    long sum = 0;
    for (int j = 0; j < numSamples; j++) {
      sum += rollingSensorVal[i][j];
    }
    float average = (float)sum / numSamples;
    sensorValues[i] = average;
    calValStore[i][calIndex[i]] = average;
    calIndex[i] = (calIndex[i] + 1) % calibrate;

    long sumCal = 0;
    for (int j = 0; j < calibrate; j++) {
      sumCal += calValStore[i][j];
    }
    calValue[i] = (float)sumCal / calibrate;
    if (buttonPressed == false){
      calValFinal[i] = calValue[i];
    }
  }
  if (buttonState == HIGH && buttonPressed == false) {
    // Button is pressed and wasn't pressed before
    buttonPressed = true;
    startCountdown();
    lcd.clear();
    lcd.print("Score = "); // Print static text
    lcd.setCursor(8, 0); // Move the cursor to the 8th column of the 1st row
    lcd.print(score); // Print the score value
    startTime = millis();
    randomSeed(analogRead(A0)); // Use an analog pin to seed the random number generator
    randomIndex = random(numSensors); // Generate a random index between 0 and arraySize-1
    digitalWrite(outputPins[randomIndex],HIGH);
  } 
  //Gameplay
  if (buttonPressed == true){
    correct = false;
    incorrect = false;
    for (int i = 0; i < numSensors; i++) {
    // Check if the current sensor is not the random index
      if (i != randomIndex) {
        // If the absolute difference between the sensor value and calibration value exceeds the threshold
        if (abs(sensorValues[i] - calValFinal[i]) > thresholds[i]) {
          incorrect = true; // Mark as incorrect
          Serial.println("INCORRECT");
        }
      } 
      else {
        // If the absolute difference between the sensor value and calibration value exceeds the threshold
        if (abs(sensorValues[i] - calValFinal[i]) > thresholds[i]) {
          correct = true; // Mark as correct
          Serial.println("CORRECT");
        }
      }
    }
    handleSensorResults();
    if (mistake>=3){
      gameOver();
    }
  } else {
      Serial.println("OFF");
  }
  //delay(5);
}

void startCountdown() {
  Serial.println("COUNTDOWN ACTIVE");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BEGIN IN...");
  for (int i = 3; i > 0; i--) {
    tone(piezo, notes[G4], 800); 
    lcd.setCursor(0, 1);
    lcd.print(i);
    delay(1000);
  }
  tone(piezo, notes[G5], 800); 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GO!!!");
  delay(1000);
  Serial.print("COUNTDOWN DONE");
}

void gameOver() {
  Serial.print("END");
  for (int i = 0; i < numSensors; i++) {
    digitalWrite(outputPins[i], LOW); // Ensure outputs are off
  }
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Game Over :(");
  lcd.setCursor(0,1);
  lcd.print("Final Score = ");
  lcd.setCursor(14, 1); 
  lcd.print(score); 

  tone(piezo, notes[C3], 550); 
  delay(610); 
  noTone(piezo);
  tone(piezo, notes[D2], 600);
  delay(630); 
  noTone(piezo);
  tone(piezo, notes[C2], 250);
  delay(300); 
  noTone(piezo);
  tone(piezo, notes[G2], 800);
  delay(1000); 
  noTone(piezo);

  lcd.setCursor(0,0);
  lcd.print("Play Again!   ->");
    tone(piezo, notes[Ef5], tempo*.75);

  delay(tempo*.75); 
  noTone(piezo);
  tone(piezo, notes[Ef5], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Df5], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Df5], tempo*.5);
  delay(tempo*.5); 
  noTone(piezo);
  tone(piezo, notes[Ef5], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Df5], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Ef5], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Df5], tempo*.25);
  delay(tempo*.25); 
  noTone(piezo);
  tone(piezo, notes[Ef5], tempo*1);
  delay(tempo*1); 
  noTone(piezo);

  score = 0;
  mistake = 0;
  buttonPressed = false;
}

void resetForNextRound() {
  if (mistake>=3){
    delay(500);
  } else {
    delay(duration - elapsed);
  }
    startTime = millis();
    duration = 6000 - score*100 + mistake * 500;
    if (duration <= minDuration){
      duration = minDuration;
    }
    Serial.print("Duration:");
    Serial.println(duration);
    randomSeed(analogRead(A0)); // Seed the random number generator
    randomIndex = random(numSensors); // Generate a random index
    digitalWrite(outputPins[randomIndex], HIGH); // Turn on the new random pin
}

void handleSensorResults() {
    currentMillis = millis();
    elapsed = currentMillis - startTime;
    if (elapsed >= duration) {
        digitalWrite(outputPins[randomIndex], LOW);
        tone(piezo, notes[D2], 500);
        mistake++;
        lcd.setCursor(mistake, 1);
        lcd.print("X");
        digitalWrite(outputPins[randomIndex], HIGH);  // LED ON
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], LOW);   // LED OFF
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], HIGH);  // LED ON
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], LOW);   // LED OFF
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], HIGH);  // LED ON
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], LOW);   // LED OFF
        Serial.println("Times Out!");
        resetForNextRound();
    } else if (incorrect && !correct) {
        // During the time window, incorrect but not correct
        digitalWrite(outputPins[randomIndex], LOW);
        tone(piezo, notes[D2], 500);
        mistake++;
        lcd.setCursor(mistake, 1);
        lcd.print("X");
        digitalWrite(outputPins[randomIndex], HIGH);  // LED ON
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], LOW);   // LED OFF
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], HIGH);  // LED ON
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], LOW);   // LED OFF
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], HIGH);  // LED ON
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], LOW);   // LED OFF
        Serial.println("Triggered incorrect!");
        resetForNextRound();
        return; // Exit to prevent further checks in this loop iteration
    } else if (correct && !incorrect) {
        // Correct within the time window
        digitalWrite(outputPins[randomIndex], LOW);
        tone(piezo, notes[E7], minDuration - 100);
        score++;
        lcd.setCursor(8, 0);
        lcd.print(score);
        Serial.println("Correct!");
        resetForNextRound();
        return; // Exit to prevent further checks in this loop iteration
    } else if (correct && incorrect) {
        // Both correct and incorrect (e.g., multiple triggers)
        digitalWrite(outputPins[randomIndex], LOW);
        tone(piezo, notes[D2], 500);
        mistake++;
        lcd.setCursor(mistake, 1);
        lcd.print("X");
        digitalWrite(outputPins[randomIndex], HIGH);  // LED ON
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], LOW);   // LED OFF
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], HIGH);  // LED ON
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], LOW);   // LED OFF
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], HIGH);  // LED ON
        delay(200);  // Wait for 200ms
        digitalWrite(outputPins[randomIndex], LOW);   // LED OFF
        Serial.println("Triggered incorrect!");
        resetForNextRound();
        return; // Exit to prevent further checks in this loop iteration
      }
}