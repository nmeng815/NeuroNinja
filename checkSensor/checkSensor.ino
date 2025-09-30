const int numSensors = 1; 
const int numSamples = 5; // rolling average over every 5 values
const int calibrate = 1; // calibrated over 100 averages

int sensorPins[numSensors] = {A9}; // Example sensor pins
int rollingSensorVal[numSensors][numSamples];
int sensorIndex[numSensors] = {0};
int thresholds[numSensors] = {250}; // |whatever value - calibration value [i]| >= thresholds[i] so plug in the DIFFERENCE needed to trigger

// Arrays to store sensor values
float sensorValues[numSensors];
float calValStore[numSensors][calibrate] = {};
int calIndex[numSensors] = {0};  
float calValue[numSensors] = {0}; 
float calValFinal[numSensors] = {0}; 

void setup() {
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);
  }
  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < numSensors; i++) {
    int sensorVal= analogRead(sensorPins[i]);
    Serial.println(sensorVal);
    delay(500);
    }

  // for (int i = 0; i < numSensors; i++) {
  //   int sensorVal= analogRead(sensorPins[i]);
  //   rollingSensorVal[i][sensorIndex[i]] = sensorVal;
  //   sensorIndex[i] = (sensorIndex[i] + 1) % numSamples;
  //   long sum = 0;
  //   for (int j = 0; j < numSamples; j++) {
  //     sum += rollingSensorVal[i][j];
  //   }
  //   float average = (float)sum / numSamples;
  //   sensorValues[i] = average;
  //   calValStore[i][calIndex[i]] = average;
  //   calIndex[i] = (calIndex[i] + 1) % calibrate;

  //   long sumCal = 0;
  //   for (int j = 0; j < calibrate; j++) {
  //     sumCal += calValStore[i][j];
  //   }
  //   calValue[i] = (float)sumCal / calibrate;
  // }
}
