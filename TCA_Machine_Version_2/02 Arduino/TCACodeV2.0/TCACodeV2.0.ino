#include <AccelStepper.h> // Including the AccelStepper library to control stepper motors

// Motor pin configurations for step, direction, and enable
#define X_STEP_PIN    54  
#define X_DIR_PIN     55  
#define X_ENABLE_PIN  38  

#define Y_STEP_PIN    60  
#define Y_DIR_PIN     61  
#define Y_ENABLE_PIN  56  

#define Z_STEP_PIN    46  
#define Z_DIR_PIN     48  
#define Z_ENABLE_PIN  62  

#define E0_STEP_PIN   26  
#define E0_DIR_PIN    28  
#define E0_ENABLE_PIN 24  

#define E1_STEP_PIN   36  
#define E1_DIR_PIN    34  
#define E1_ENABLE_PIN 30  

// Pin configurations for mechanical stops and control buttons
#define X_MIN_PIN     3   // Limit switch for X axis minimum position
#define Y_MIN_PIN     14  // Limit switch for Y axis minimum position

#define BUTTON_PIN1   40  // Buttons for mode selection
#define BUTTON_PIN2   42  
#define BUTTON_PIN3   44  
#define BUTTON_PIN4   39  
#define BUTTON_PIN5   47  
#define REVERSE_BUTTON_PIN 16  // Button to reverse direction
#define RESET_BUTTON_PIN 23    // Button to reset the system

// Analog pin configuration for speed control
int SpeedPot = A9;
#define out 27 // Unused output pin

// Creating stepper motor objects using the AccelStepper library
AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);  
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN); 
AccelStepper stepperZ(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN); 
AccelStepper stepperE0(AccelStepper::DRIVER, E0_STEP_PIN, E0_DIR_PIN); 
AccelStepper stepperE1(AccelStepper::DRIVER, E1_STEP_PIN, E1_DIR_PIN); 

// State and control variables
bool softStop = false;    // Flag for soft stop control
bool reverse = false;     // Flag to indicate direction reversal
int potValue = 0;         // Potentiometer value for speed control
int CoilSpeed;            // Speed of coiling motor
int buttonMode = 0;       // Mode selected by button input
const int stepsPerRevolution = 400;  // Steps per motor revolution
bool X_MIN = 0;           // State of X axis minimum limit switch
bool X_MAX = 0;           // State of X axis maximum limit switch
int xmin = 0;             // Placeholder for X axis minimum state

// Speed settings and movement ratios
float speedRatio = .4857;           // Ratio for speed control
float untwistRatio = 0.2;         // Untwist ratio to compensate twisting during coiling
const int MaxSpeed = 1000;        // Maximum speed for all motors

// Motor movement distance settings
long TravelerSteps = 160000;      // Steps for traveler lead screw movement
long MandralSteps = 1333333;      // Steps for mandrel coiling motor movement
long twisting = 432000;           // Steps for twisting motor movement

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Configure pins for mechanical stops and control buttons
  pinMode(X_MIN_PIN, INPUT_PULLUP);  
  pinMode(Y_MIN_PIN, INPUT_PULLUP);  
  pinMode(BUTTON_PIN1, INPUT_PULLUP); 
  pinMode(BUTTON_PIN2, INPUT_PULLUP); 
  pinMode(BUTTON_PIN3, INPUT_PULLUP); 
  pinMode(BUTTON_PIN4, INPUT_PULLUP); 
  pinMode(BUTTON_PIN5, INPUT_PULLUP); 
  pinMode(REVERSE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);   

  // Configure pins for enabling stepper motors
  pinMode(X_ENABLE_PIN, OUTPUT);  
  pinMode(Y_ENABLE_PIN, OUTPUT);  
  pinMode(Z_ENABLE_PIN, OUTPUT);  
  pinMode(E0_ENABLE_PIN, OUTPUT); 
  pinMode(E1_ENABLE_PIN, OUTPUT); 

  // Set the maximum speed for each stepper motor
  stepperX.setMaxSpeed(MaxSpeed);
  stepperY.setMaxSpeed(MaxSpeed);
  stepperZ.setMaxSpeed(MaxSpeed);
  stepperE0.setMaxSpeed(MaxSpeed);
  stepperE1.setMaxSpeed(MaxSpeed);
}
void loop() {
  // Read button inputs to determine the current mode of operation
  if (digitalRead(BUTTON_PIN1) == LOW) {
    buttonMode = 1;  // Mode 1: Twisting operation
  } else if (digitalRead(BUTTON_PIN2) == LOW) {
    buttonMode = 2;  // Mode 2: Coil around mandrel with traveler
  } else if (digitalRead(BUTTON_PIN3) == LOW) {
    buttonMode = 3;  // Mode 3: Coil copper around mandrel
  } else if (digitalRead(BUTTON_PIN4) == LOW) {
    buttonMode = 4;  // Mode 4: Move traveler lead screw only
  } else if (digitalRead(BUTTON_PIN5) == LOW) {
    buttonMode = 5;  // Mode 5: Reset and home all motors
  } else {
    buttonMode = 0;  // No button pressed, reset all steppers
    resetAllSteppers();
  }

  // Set motor speed based on potentiometer input
  potValue = analogRead(SpeedPot);
  int range = map(potValue, 0, 1023, 0, 5);

  // Map potentiometer value to coil speed
  switch (range) {
    case 0: 
      CoilSpeed = 140;
      break;
    case 1:  
      CoilSpeed = 330;
      break;
    case 2:   
      CoilSpeed = 520;
      break;
    case 3:
      CoilSpeed = 685; 
      break;
    case 4:   
      CoilSpeed = 800; 
      break;
    case 5:   
      CoilSpeed = 1000; 
      break;
  }

  // Calculate the speed for the traveler motor based on the coil speed
  float TravelSpeed = CoilSpeed * speedRatio;

  // Mode 1: Twisting thread
  if (buttonMode == 1) {
    reverse = digitalRead(REVERSE_BUTTON_PIN) == LOW;  // Check if reverse button is pressed
    long position = reverse ? -twisting : twisting;    // Set direction based on reverse flag
    digitalWrite(Z_ENABLE_PIN, LOW);                   // Enable Z motor
    digitalWrite(E0_ENABLE_PIN, LOW);                  // Enable E0 motor

    // Set positions and speeds for motors
    stepperZ.moveTo(position);
    stepperE0.moveTo(-position);
    stepperZ.setSpeed(CoilSpeed);
    stepperE0.setSpeed(CoilSpeed);

    // Run motors to positions until the required distance is reached or conditions to stop are met
    long distance = position;
    while (abs(distance) > 10) {
      if (digitalRead(BUTTON_PIN5) == LOW) { resetAllSteppers(); break; }  // Stop if button 5 is pressed
      if (digitalRead(X_MIN_PIN) == LOW) { resetAllSteppersX(); break; }   // Stop if X limit switch is triggered
      if (digitalRead(Y_MIN_PIN) == LOW) { resetAllSteppersY(); break; }   // Stop if Y limit switch is triggered
      stepperZ.runSpeedToPosition();
      stepperE0.runSpeedToPosition();
      distance = stepperE0.distanceToGo();
    }
  }

  // Mode 2: Coil around mandrel with traveler movement
  if (buttonMode == 2) {
    reverse = digitalRead(REVERSE_BUTTON_PIN) == LOW;  // Check if reverse button is pressed
    long position1 = reverse ? MandralSteps : -MandralSteps;
    long position2 = reverse ? -TravelerSteps : TravelerSteps;

    // Enable all motors
    digitalWrite(X_ENABLE_PIN, LOW);
    digitalWrite(Y_ENABLE_PIN, LOW);
    digitalWrite(Z_ENABLE_PIN, LOW);
    digitalWrite(E0_ENABLE_PIN, LOW);
    digitalWrite(E1_ENABLE_PIN, LOW);

    // Set positions and speeds for motors
    stepperX.moveTo(position2);
    stepperY.moveTo(position2);
    stepperZ.moveTo(position1);
    stepperE0.moveTo(position1);
    stepperE1.moveTo(position1);
    stepperX.setSpeed(TravelSpeed);
    stepperY.setSpeed(TravelSpeed);
    stepperZ.setSpeed(CoilSpeed * untwistRatio);
    stepperE0.setSpeed(CoilSpeed);
    stepperE1.setSpeed(CoilSpeed);

    // Run motors to positions until the required distance is reached or conditions to stop are met
    long distance = position2;
    while (abs(distance) > 10) {
      if (digitalRead(BUTTON_PIN5) == LOW) { resetAllSteppers(); break; }  // Stop if button 5 is pressed
      if (digitalRead(X_MIN_PIN) == LOW) { resetAllSteppersX(); break; }   // Stop if X limit switch is triggered
      if (digitalRead(Y_MIN_PIN) == LOW) { resetAllSteppersY(); break; }   // Stop if Y limit switch is triggered
      stepperX.runSpeedToPosition();
      stepperY.runSpeedToPosition();
      stepperZ.runSpeedToPosition();
      stepperE0.runSpeedToPosition();
      stepperE1.runSpeedToPosition();
      distance = stepperX.distanceToGo();
    }
  }

  // Mode 3: Coil copper around mandrel (no traveler movement)
  if (buttonMode == 3) {
    reverse = digitalRead(REVERSE_BUTTON_PIN) == LOW;  // Check if reverse button is pressed
    long position = reverse ? MandralSteps : -MandralSteps;

    // Enable E0 and E1 motors
    digitalWrite(E0_ENABLE_PIN, LOW);
    digitalWrite(E1_ENABLE_PIN, LOW);

    // Set positions and speeds for motors
    stepperE0.moveTo(position);
    stepperE1.moveTo(position);
    stepperE0.setSpeed(CoilSpeed);
    stepperE1.setSpeed(CoilSpeed);

    // Run motors to positions until the required distance is reached or conditions to stop are met
    long distance = abs(position);
    while (abs(distance) > 10) {
      if (digitalRead(BUTTON_PIN5) == LOW) { resetAllSteppers(); break; }
      if (digitalRead(X_MIN_PIN) == LOW) { resetAllSteppersX(); break; }
      if (digitalRead(Y_MIN_PIN) == LOW) { resetAllSteppersY(); break; }
      stepperE0.runSpeedToPosition();
      stepperE1.runSpeedToPosition();
      distance = stepperE0.distanceToGo();
    }
  }

  // Mode 4: Move traveler lead screw only
  if (buttonMode == 4) {
    reverse = digitalRead(REVERSE_BUTTON_PIN) == LOW;  // Check if reverse button is pressed
    long position = reverse ? TravelerSteps : -TravelerSteps;

    // Enable X and Y motors
    digitalWrite(X_ENABLE_PIN, LOW);
    digitalWrite(Y_ENABLE_PIN, LOW);

    // Set positions and speeds for motors
    stepperX.moveTo(position);
    stepperY.moveTo(position);
    stepperX.setSpeed(CoilSpeed);
    stepperY.setSpeed(CoilSpeed);

    // Run motors to positions until the required distance is reached or conditions to stop are met
    long distance = position;
    while (abs(distance) > 10) {
      if (digitalRead(BUTTON_PIN5) == LOW) { resetAllSteppers(); break; }
      if (digitalRead(X_MIN_PIN) == LOW) { resetAllSteppersX(); break; }
      if (digitalRead(Y_MIN_PIN) == LOW) { resetAllSteppersY(); break; }
      stepperX.runSpeedToPosition();
      stepperY.runSpeedToPosition();
      distance = stepperX.distanceToGo();
    }
  }

  // Mode 5: Reset all motors and stop
  if (buttonMode == 5) {
    resetAllSteppers();
    delay(10);
  }

  // No button pressed
  if (buttonMode == 0) {
    delay(10);
  }

  softStop = false;  // Reset soft stop flag
}

// Function to reset all stepper motors and set positions to zero
void resetAllSteppers() {
  // Stop all stepper motors and reset positions to zero
  stepperX.stop();
  stepperY.stop();
  stepperZ.stop();
  stepperE0.stop();
  stepperE1.stop();
  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
  stepperZ.setCurrentPosition(0);
  stepperE0.setCurrentPosition(0);
  stepperE1.setCurrentPosition(0);

  // Disable all stepper motor drivers
  digitalWrite(X_ENABLE_PIN, HIGH); 
  digitalWrite(Y_ENABLE_PIN, HIGH);
  digitalWrite(Z_ENABLE_PIN, HIGH);
  digitalWrite(E0_ENABLE_PIN, HIGH);
  digitalWrite(E1_ENABLE_PIN, HIGH);

  softStop = false;  // Reset the soft stop flag
}

void resetAllSteppersX() {
  // Stop all stepper motors and reset positions to zero
  stepperX.stop();
  stepperY.stop();
  stepperZ.stop();
  stepperE0.stop();
  stepperE1.stop();
  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
  stepperZ.setCurrentPosition(0);
  stepperE0.setCurrentPosition(0);
  stepperE1.setCurrentPosition(0);

  // Disable all stepper motor drivers
  digitalWrite(X_ENABLE_PIN, HIGH); 
  digitalWrite(Y_ENABLE_PIN, HIGH);
  digitalWrite(Z_ENABLE_PIN, HIGH);
  digitalWrite(E0_ENABLE_PIN, HIGH);
  digitalWrite(E1_ENABLE_PIN, HIGH);

  // Move X motor to position 500 and reset position
  digitalWrite(X_ENABLE_PIN, LOW);  // Enable X motor
  stepperX.moveTo(500);
  stepperX.setSpeed(1000);

  long distance = 500;
  while (abs(distance) > 10) {
    stepperX.runSpeedToPosition();
    distance = stepperX.distanceToGo();
  }

  stepperX.stop();
  stepperX.setCurrentPosition(0);  // Reset position after movement

  digitalWrite(X_ENABLE_PIN, HIGH); // Disable X motor after movement

  softStop = false;  // Reset the soft stop flag

  // Stop all stepper motors again and reset positions
  stepperX.stop();
  stepperY.stop();
  stepperZ.stop();
  stepperE0.stop();
  stepperE1.stop();
  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
  stepperZ.setCurrentPosition(0);
  stepperE0.setCurrentPosition(0);
  stepperE1.setCurrentPosition(0);

  // Disable all stepper motor drivers
  digitalWrite(X_ENABLE_PIN, HIGH); 
  digitalWrite(Y_ENABLE_PIN, HIGH);
  digitalWrite(Z_ENABLE_PIN, HIGH);
  digitalWrite(E0_ENABLE_PIN, HIGH);
  digitalWrite(E1_ENABLE_PIN, HIGH);

  softStop = false;  // Reset the soft stop flag
}

void resetAllSteppersY() {
  // Stop all stepper motors and reset positions to zero
  stepperX.stop();
  stepperY.stop();
  stepperZ.stop();
  stepperE0.stop();
  stepperE1.stop();
  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
  stepperZ.setCurrentPosition(0);
  stepperE0.setCurrentPosition(0);
  stepperE1.setCurrentPosition(0);

  // Disable all stepper motor drivers
  digitalWrite(X_ENABLE_PIN, HIGH); 
  digitalWrite(Y_ENABLE_PIN, HIGH);
  digitalWrite(Z_ENABLE_PIN, HIGH);
  digitalWrite(E0_ENABLE_PIN, HIGH);
  digitalWrite(E1_ENABLE_PIN, HIGH);

  // Move X motor to position -500 and reset position
  digitalWrite(X_ENABLE_PIN, LOW);  // Enable X motor
  stepperX.moveTo(-500);
  stepperX.setSpeed(1000);

  long distance = -500;
  while (abs(distance) > 10) {
    stepperX.runSpeedToPosition();
    distance = stepperX.distanceToGo();
  }

  stepperX.stop();
  stepperX.setCurrentPosition(0);  // Reset position after movement

  digitalWrite(X_ENABLE_PIN, HIGH); // Disable X motor after movement

  softStop = false;  // Reset the soft stop flag

  // Stop all stepper motors again and reset positions
  stepperX.stop();
  stepperY.stop();
  stepperZ.stop();
  stepperE0.stop();
  stepperE1.stop();
  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
  stepperZ.setCurrentPosition(0);
  stepperE0.setCurrentPosition(0);
  stepperE1.setCurrentPosition(0);

  // Disable all stepper motor drivers
  digitalWrite(X_ENABLE_PIN, HIGH); 
  digitalWrite(Y_ENABLE_PIN, HIGH);
  digitalWrite(Z_ENABLE_PIN, HIGH);
  digitalWrite(E0_ENABLE_PIN, HIGH);
  digitalWrite(E1_ENABLE_PIN, HIGH);

  softStop = false;  // Reset the soft stop flag
}
