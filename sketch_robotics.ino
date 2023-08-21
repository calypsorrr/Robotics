const int controlPin1 = 2;             // connected to pin 7 on the H-bridge
const int controlPin2 = 3;             // connected to pin 2 on the H-bridge
const int enablePin = 9;               // connected to pin 1 on the H-bridge
const int directionSwitchPin = 4;      // connected to the switch for direction
const int onOffSwitchStateSwitchPin = 5; // connected to the switch for turning the motor on and off
const int resetButtonPin = 8;          // connected to the reset button (use one of the existing buttons)
const int potPin = A0;                 // connected to the potentiometer's output
const int photoresistorPin = A1;       // connected to the photoresistor
const int ledPin = 6;                  // connected to an LED
const int limitResetThreshold = 1000;  // Photoresistor reading threshold for resetting the motor position

// create some variables to hold values from your inputs
int onOffSwitchState = 0;               // current state of the on/off switch
int previousOnOffSwitchState = 0;       // previous position of the on/off switch
int directionSwitchState = 0;           // current state of the direction switch
int previousDirectionSwitchState = 0;   // previous state of the direction switch
int resetButtonState = 0;               // current state of the reset button
int previousResetButtonState = 0;       // previous state of the reset button

int motorEnabled = 0;                   // Turns the motor on/off
int motorSpeed = 0;                    // speed of the motor
int motorDirection = 1;                // current direction of the motor

const int resetDelay = 5000;  // Delay in milliseconds before resetting the motor (adjust as needed)
unsigned long motorOffTime = 0; // Time when the motor was turned off

bool motorNeedsReset = true; // Flag to indicate if the motor needs a reset

void setup() {
  // initialize the inputs and outputs
  pinMode(directionSwitchPin, INPUT);
  pinMode(onOffSwitchStateSwitchPin, INPUT);
  pinMode(controlPin1, OUTPUT);
  pinMode(controlPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(photoresistorPin, INPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);
  
  // pull the enable pin LOW to start
  digitalWrite(enablePin, LOW);
}

void loop() {
  // read the value of the on/off switch
  onOffSwitchState = digitalRead(onOffSwitchStateSwitchPin);
  delay(1);

  // read the value of the direction switch
  directionSwitchState = digitalRead(directionSwitchPin);

  // read the value of the pot and divide by 4 to get a value that can be
  // used for PWM
  motorSpeed = analogRead(potPin) / 4;

  // read the value of the photoresistor
  int photoresistorValue = analogRead(photoresistorPin);

  // Check if the photoresistor value is below 1000
  if (photoresistorValue < 1000) {
    // Turn off the motor
    motorEnabled = 0;
    analogWrite(enablePin, 0);
    // Record the time when the motor was turned off
    motorOffTime = millis();
    // Turn on the LED
    digitalWrite(ledPin, HIGH);
  } else {
    // Turn off the LED
    digitalWrite(ledPin, LOW);
  }

  // Check if it's time to restart the motor (after the resetDelay period)
  if (motorEnabled == 0 && millis() - motorOffTime >= resetDelay) {
    // Turn on the motor
    motorEnabled = 1;
  }

  // Check if the on/off button changed state since the last loop()
  if (onOffSwitchState != previousOnOffSwitchState) {
    // change the value of motorEnabled if pressed
    if (onOffSwitchState == HIGH) {
      motorEnabled = !motorEnabled;
    }
  }

  // if the direction button changed state since the last loop()
  if (directionSwitchState != previousDirectionSwitchState) {
    // change the value of motorDirection if pressed
    if (directionSwitchState == HIGH) {
      motorDirection = !motorDirection;
    }
  }

  // change the direction the motor spins by talking to the control pins
  // on the H-Bridge
  if (motorDirection == 1) {
    digitalWrite(controlPin1, HIGH);
    digitalWrite(controlPin2, LOW);
  } else {
    digitalWrite(controlPin1, LOW);
    digitalWrite(controlPin2, HIGH);
  }

  // if the motor is supposed to be on
  if (motorEnabled == 1) {
    // PWM the enable pin to vary the speed
    analogWrite(enablePin, motorSpeed);
  } else { // if the motor is not supposed to be on
    // turn the motor off
    analogWrite(enablePin, 0);
  }

  // Read the state of the reset button
  resetButtonState = digitalRead(resetButtonPin);

  // Check if the reset button is pressed
  if (resetButtonState == LOW) {
    // Allow the motor to be turned on regardless of the photoresistor value
    motorEnabled = 1;
  }

  // Check if the motor needs a reset
  if (motorNeedsReset) {
    // Check if the phototransistor reading is below the threshold (cardboard strip is blocking)
    int photoresistorValue = analogRead(photoresistorPin);
    if (photoresistorValue < limitResetThreshold) {
      // Implement logic to reset the motor to the initial position
      // This could involve moving the motor until the phototransistor reading is above the threshold
      // Example:
      // while (analogRead(photoresistorPin) < limitResetThreshold) {
      //   // Move the motor in the reset direction
      //   // (you may need to change the motorDirection and controlPin1/2 settings)
      //   digitalWrite(controlPin1, HIGH);
      //   digitalWrite(controlPin2, LOW);
      //   analogWrite(enablePin, motorSpeed);
      // }
      // Stop the motor
      // analogWrite(enablePin, 0);
      
      // Once the motor is at the initial position, reset the flag
      motorNeedsReset = false;
    }
  }

  // Save the current on/off switch state as the previous
  previousDirectionSwitchState = directionSwitchState;
  // Save the current switch state as the previous
  previousOnOffSwitchState = onOffSwitchState;
  // Save the current reset button state as the previous
  previousResetButtonState = resetButtonState;
}
