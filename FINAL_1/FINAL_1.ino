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

const int beginG1 = 17;// button to start game 1
const int beginG2 = 16; // button to start game 2
const int piezo = 7; // sound output

bool buttonG1Pressed = false; // game start logic
bool buttonG2Pressed = false; // game start logic

const int numSensors = 3; // currently there are 3 working sensors
const int numSamples = 1; // rolling average over every 1 values (over more samples causes delay)
const int calibrate = 30; // calibrated over 30 averages (adjust for accurate cal)

int sensorPins[numSensors] = {A8,A7,A9}; // sensor pins (A8 is pinch, A7 is small cyl, A9 large cyl)
int outputPins[numSensors] = {20,19,1}; // corresponding LED indicator pins
int rollingSensorVal[numSensors][numSamples];
int sensorIndex[numSensors] = {0};
int thresholds[numSensors] = {25,25,15}; //use check sensor to estimate good threshold |whatever value - calibration value [i]| >= thresholds[i] so plug in the DIFFERENCE needed to trigger

// Arrays to store sensor values
float sensorValues[numSensors]; 
float calValStore[numSensors][calibrate] = {};
int calIndex[numSensors] = {0};  
float calValue[numSensors] = {0}; 
float calValFinal[numSensors] = {0}; 

int currentMillis = 0; //current time
int score = 0; // score count for game
int mistake = 0; // mistake count
int startTime = 0; // the stored beginning value for each reaction time
int randomIndex = 0; // random index to call a random sensor 
int elapsed = 0; //elapsed time
bool correct = false; // user has triggered the correct sensor
bool incorrect = false; // user has triggered the incorrect sensor

//Gameplay 1
int duration = 6000; //calculate reaction time starts at 6 seconds to complete motion 
const int minDuration = 800; //fastest time, maybe too fast
bool timeOut = false;

//Gameplay 2
int reactionTime = 5000; // time to enter pattern
int* checkArray = nullptr; //correct array
int* gameArray = nullptr;   // gameplay array
int arraySize = 1;   //start with one entry
bool sensors[] = {false, false}; //do i need this? 
bool allCorrect = false; //enture sequence correct
bool gameEnded = false;
bool arraysAreEqual(int array1[], int array2[], int arraySize) {
    for (int i = 0; i < arraySize; i++) {
        if (array1[i] != array2[i]) {
            return false;  // Arrays are not equal if any element differs
        }
    }
    return true;  // Arrays are equal if all elements match
}

void setup() {
  pinMode(piezo, OUTPUT);
  digitalWrite(piezo, LOW); 
  digitalWrite(beginG1, LOW);
  digitalWrite(beginG2, LOW);
  analogWrite(6,20);
  // Set the pin inputs and outputs
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);
  }
  for (int i = 0; i < numSensors; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW); // Ensure outputs are initially off
  }
  
  pinMode(beginG1, INPUT_PULLUP); // Use internal pull-up resistor
  pinMode(beginG2, INPUT_PULLUP); // Use internal pull-up resistor

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

    // tone(piezo, notes[Ef4], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[F4], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[Gf4], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[Ef4], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[Gf4], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[Af4], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[Bf4], tempo*1);
    // delay(tempo*1); 
    // noTone(piezo);

    // tone(piezo, notes[Af4], tempo*.25);
    // delay(tempo*.25); 
    // noTone(piezo);
    // tone(piezo, notes[Bf4], tempo*.25);
    // delay(tempo*.25); 
    // noTone(piezo);
    // tone(piezo, notes[Af4], tempo*.25);
    // delay(tempo*.25); 
    // noTone(piezo);
    // tone(piezo, notes[Gf4], tempo*.25);
    // delay(tempo*.25); 
    // noTone(piezo);
    // tone(piezo, notes[F4], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[D4], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[F4], tempo*1);
    // delay(tempo*1); 
    // noTone(piezo);
    // tone(piezo, notes[Bf4], tempo*1);
    // delay(tempo*1); 
    // noTone(piezo);

    // tone(piezo, notes[Ef5], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[Df5], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[Bf4], tempo*1);
    // delay(tempo*1); 
    // noTone(piezo);
    // tone(piezo, notes[Gf4], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[Af4], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[Bf4], tempo*1);
    // delay(tempo*1); 
    // noTone(piezo);

    // tone(piezo, notes[Af4], tempo*.25);
    // delay(tempo*.25); 
    // noTone(piezo);
    // tone(piezo, notes[Bf4], tempo*.25);
    // delay(tempo*.25); 
    // noTone(piezo);
    // tone(piezo, notes[Af4], tempo*.25);
    // delay(tempo*.25); 
    // noTone(piezo);
    // tone(piezo, notes[Gf4], tempo*.25);
    // delay(tempo*.25); 
    // noTone(piezo);
    // tone(piezo, notes[F4], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[Df4], tempo*.5);
    // delay(tempo*.5); 
    // noTone(piezo);
    // tone(piezo, notes[Ef4], tempo*2);
    // delay(tempo*2); 
    // noTone(piezo);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("<-Press to begin");
  lcd.setCursor(0, 1);
  lcd.print("Game1-G Game2-B");
}

void loop() {
  // initialize all reading
  int buttonStateG1 = digitalRead(beginG1);
  Serial.print("buttonStateG1 = ");
  Serial.println(buttonStateG1);

  int buttonStateG2 = digitalRead(beginG2);
  Serial.print("buttonStateG2 = ");
  Serial.println(buttonStateG2);
  
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
    if (buttonG1Pressed == false && buttonG2Pressed == false){
      calValFinal[i] = calValue[i];
      for (int i = 0; i < numSensors; i++) {
        pinMode(outputPins[i], OUTPUT);
        digitalWrite(outputPins[i], LOW); // Ensure outputs are initially off
      }
    }
  }
  //Start Game 1
  if (buttonStateG1 == LOW && buttonG1Pressed == false && buttonG2Pressed == false) {
    // Button is pressed and wasn't pressed before
    buttonG1Pressed = true;
    gameEnded = false;
    startCountdown();
    lcd.clear();
    lcd.print("Score = "); // Print static text
    lcd.setCursor(8, 0); // Move the cursor to the 8th column of the 1st row
    lcd.print(score); // Print the score value
    startTime = millis();
    randomSeed(analogRead(A9)); // Use an analog pin to seed the random number generator
    randomIndex = random(numSensors); // Generate a random index between 0 and arraySize-1
    digitalWrite(outputPins[randomIndex],HIGH);
  } 

  //Start Game 2
  if (buttonStateG2 == LOW && buttonG2Pressed == false && buttonG1Pressed == false) {
    // Button is pressed and wasn't pressed before
    buttonG2Pressed = true;
    gameEnded = false;
    startCountdown();
    lcd.clear();
    lcd.print("Score = "); // Print static text
    lcd.setCursor(8, 0); // Move the cursor to the 8th column of the 1st row
    lcd.print(score); // Print the score value

    checkArray = new int[arraySize];
    gameArray = new int[arraySize]; 

    gameArray[0] = -1;

    randomSeed(analogRead(A0)); // Use an analog pin to seed the random number generator
    randomIndex = random(numSensors); // Generate a random index between 0 and arraySize-1
    checkArray[0] = randomIndex;
  } 

  //Gameplay1
  if (buttonG1Pressed == true){
    obtainReading();
    handleSensorResults();
  } else {
      Serial.println("G1OFF");
  }

  if (buttonG2Pressed == true){
    playLights();
    obtainSequence();
  } else {
      Serial.println("G2OFF");
  }
}

//Functions used by both games (if error, separate)
void startCountdown() {
  for (int i = 0; i < numSensors; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW); // Ensure outputs are initially off
  }
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
  gameEnded = true;
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
  lcd.print("<-   Play Again!");
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

  for (int i = 0; i < numSensors; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW); // Ensure outputs are initially off
  }
  
  score = 0;
  mistake = 0;
  buttonG1Pressed = false;
  buttonG2Pressed = false;
  arraySize = 1;
  incorrect = false;
  correct = false;
  allCorrect = false;

  if (checkArray != nullptr){
    delete[] checkArray;
    checkArray = nullptr;
  }
  if (gameArray != nullptr){
    delete[] gameArray;
    gameArray = nullptr;
  }
}

void obtainReading() {
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
}
//Functions only used by game 1

void handleSensorResults() {
    currentMillis = millis();
    elapsed = currentMillis - startTime;
    if (elapsed >= duration) {
        timeOut = true;
        digitalWrite(outputPins[randomIndex], LOW);
        tone(piezo, notes[D2], 500);
        mistake++;
        lcd.setCursor(mistake, 1);
        lcd.print("X");
        blinkAll();
        Serial.println("Times Out!");
        G1resetForNextRound();
    } else if (incorrect && !correct) {
        // During the time window, incorrect but not correct
        digitalWrite(outputPins[randomIndex], LOW);
        tone(piezo, notes[D2], 500);
        mistake++;
        lcd.setCursor(mistake, 1);
        lcd.print("X");
        blinkAll();
        Serial.println("Triggered incorrect!");
        G1resetForNextRound();
        return; // Exit to prevent further checks in this loop iteration
    } else if (correct && !incorrect) {
        // Correct within the time window
        digitalWrite(outputPins[randomIndex], LOW);
        tone(piezo, notes[E7], minDuration - 100);
        score++;
        lcd.setCursor(8, 0);
        lcd.print(score);
        Serial.println("Correct!");
        G1resetForNextRound();
        return; // Exit to prevent further checks in this loop iteration
    } else if (correct && incorrect) {
        // Both correct and incorrect (e.g., multiple triggers)
        digitalWrite(outputPins[randomIndex], LOW);
        tone(piezo, notes[D2], 500);
        mistake++;
        lcd.setCursor(mistake, 1);
        lcd.print("X");
        blinkAll();
        Serial.println("Triggered incorrect!");
        G1resetForNextRound();
        return; // Exit to prevent further checks in this loop iteration
      }
}

void G1resetForNextRound() {
  if (mistake>=3){
    delay(500);
    gameOver();
  } else {
    if (timeOut) {
      delay(500);
    }else {
    delay(duration - elapsed);
    }
  }
    startTime = millis();
    duration = 6000 - score*100 + mistake * 500;
    if (duration <= minDuration){
      duration = minDuration;
    }
    Serial.print("Duration:");
    Serial.println(duration);
    //randomSeed(analogRead(A0)); // Seed the random number generator
    randomIndex = random(numSensors); // Generate a random index
    digitalWrite(outputPins[randomIndex], HIGH); // Turn on the new random pin
    timeOut = false;
}

//Functions only used by game 2
void playLights() {
  Serial.println("Playing Lights");
  for (int i = 0; i < arraySize; i++) {
    digitalWrite(outputPins[checkArray[i]], HIGH);
    Serial.println(checkArray[i]);
    delay(1000);
    digitalWrite(outputPins[checkArray[i]], LOW);
    delay(250); // optional: short pause between lights
  }
  tone(piezo, notes[G5], 800);
  delay(800);
  startTime = millis();
  Serial.println("Finished Playing Lights");
}

void blinkAll() {
  for (int i = 0; i < numSensors; i++) {
  digitalWrite(outputPins[i], HIGH);  // LED ON
  }
  delay(200); 
  for (int i = 0; i < numSensors; i++) {
  digitalWrite(outputPins[i], LOW);  // LED ON
  } 
  delay(200);
  for (int i = 0; i < numSensors; i++) {
  digitalWrite(outputPins[i], HIGH);  // LED ON
  }
  delay(200); 
  for (int i = 0; i < numSensors; i++) {
  digitalWrite(outputPins[i], LOW);  // LED ON
  } 
  delay(200);
  for (int i = 0; i < numSensors; i++) {
  digitalWrite(outputPins[i], HIGH);  // LED ON
  }
  delay(200); 
  for (int i = 0; i < numSensors; i++) {
  digitalWrite(outputPins[i], LOW);  // LED ON
  } 
  delay(200);
}

void G2resetForNextRound() {
  if (mistake >= 3) {
    delay(1000);
    gameOver();
    return;
  } else {
    delay(1000);
  }
  if (allCorrect){
    // Store the old pattern before updating
    int* oldCheckArray = checkArray;

    // Increase size
    arraySize = score + 1;

    // Allocate new arrays with the new size
    checkArray = new int[arraySize];
    gameArray = new int[arraySize];
    
    for (int i = 0; i < arraySize - 1; i++) {
      gameArray[i] = -1;
    }

  // Copy previous pattern into the new array
    for (int i = 0; i < arraySize - 1; i++) {
      checkArray[i] = oldCheckArray[i];
    }

  // Add one new random step to the pattern
    //randomSeed(analogRead(A0));
    randomIndex = random(numSensors);
    checkArray[arraySize - 1] = randomIndex;

    // Free old memory
    if (oldCheckArray != nullptr) {
      delete[] oldCheckArray;
    }
  }

  allCorrect = false;
  correct = false;
  incorrect = false;
}

void obtainSequence() {
  for (int i = 0; i < arraySize; i++) {
    elapsed = 0;
    while (elapsed < reactionTime) {
      currentMillis = millis();
      elapsed = currentMillis - startTime;  
      for (int j = 0; j < numSensors; j++) {
        int sensorVal= analogRead(sensorPins[j]);
        rollingSensorVal[j][sensorIndex[j]] = sensorVal;
        sensorIndex[j] = (sensorIndex[j] + 1) % numSamples;
        long sum = 0;
        for (int k = 0; k < numSamples; k++) {
          sum += rollingSensorVal[j][k];
        }
        float average = (float)sum / numSamples;
        sensorValues[j] = average;
        if (abs(sensorValues[j] - calValFinal[j]) > thresholds[j]) {
          digitalWrite(outputPins[j],HIGH);
          delay(300);
          Serial.println(String("Sensor ") + j + " Triggered");
          digitalWrite(outputPins[j],LOW);
          gameArray[i] = j;
          if (gameArray[i] == checkArray[i]) {
            tone(piezo, notes[E7], 1000);
            delay(1250);
            startTime = millis();
            correct = true;  // Exit the inner for loop once a correct match is found
            Serial.println("step correct");
          } else if(gameArray[i] != checkArray[i] && !incorrect) {
            tone(piezo, notes[D2], 1000);
            delay(1000);
            incorrect = true; 
            mistake++;
            allCorrect = false;
            Serial.println("incorrect sensor triggered");
            lcd.setCursor(mistake, 1);
            lcd.print("X");
            blinkAll();
            break;
          }
        }
      }
      if (correct || incorrect) {
        delay(1000);
        break;
      }
    }

    // After the while loop, check if elapsed time exceeds reactionTime and no correct match was found
    if (elapsed >= reactionTime && !correct && !incorrect) {  // Corrected: use '&& !correct' instead of '&& correct = false'
      tone(piezo, notes[D2], 1000);
      delay(1000);
      mistake++;
      incorrect = true;
      allCorrect = false;
      lcd.setCursor(mistake, 1);
      lcd.print("X");
      blinkAll();
      Serial.println("Times Out!");
    }
    if (incorrect) {
      G2resetForNextRound();
      break;
    }
    correct = false; // Reset correct flag for the next round
    incorrect = false;  
  }
  if (!incorrect && gameArray != nullptr && checkArray != nullptr) {
    if (arraysAreEqual(gameArray, checkArray, arraySize)) {
        allCorrect = true; // If arrays are equal, set allCorrect to true
    }
  }

  if (allCorrect && !gameEnded) {
    tone(piezo, notes[E7], 300); // Play sound when correct
    delay(500);
    tone(piezo, notes[E7], 300); // Play sound when correct
    delay(500);
    tone(piezo, notes[E7], 300); // Play sound when correct
    delay(500);
    score++; // Increment score
    lcd.setCursor(8, 0); // Set cursor position on LCD
    lcd.print(score); // Display the score
    Serial.println("All Correct!"); // Output to serial monitor
    G2resetForNextRound(); // Reset for the next round
  }
  allCorrect = false;
}




