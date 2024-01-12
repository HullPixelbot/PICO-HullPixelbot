#include "Motors.h"
#include "Utils.h"

#include <Arduino.h>
#include <math.h>

#include "Storage.h"

float turningCircle;
const int countsperrev = 512 * 8; // number of microsteps per full revolution

float leftStepsPerMM;
float rightStepsPerMM;
float leftWheelCircumference;
float rightWheelCircumference;

WheelSettings activeWheelSettings;

void storeActiveWheelSettings()
{
  storeBlockIntoEEPROM((uint8_t *)&activeWheelSettings, sizeof(struct WheelSettings), WHEEL_SETTINGS_OFFSET);
}

void setActiveWheelSettings(int leftDiam, int rightDiam, int spacing)
{
  activeWheelSettings.leftWheelDiameter = leftDiam;
  activeWheelSettings.rightWheelDiameter = rightDiam;
  activeWheelSettings.wheelSpacing = spacing;
  storeActiveWheelSettings();
}

void dumpActiveWheelSettings()
{
  Serial.println(F("Wheel settings"));
  Serial.print(F("Left diameter: "));
  Serial.println(activeWheelSettings.leftWheelDiameter);
  Serial.print(F("Right diameter: "));
  Serial.println(activeWheelSettings.rightWheelDiameter);
  Serial.print(F("Wheel spacing: "));
  Serial.println(activeWheelSettings.wheelSpacing);
}

void loadActiveWheelSettings()
{

#ifdef DEBUG_LOAD_ACTIVE_WHEEL_SETTINGS
  Serial.println(F("Loading active wheel settings"));
#endif

  loadBlockFromEEPROM((uint8_t *)&activeWheelSettings, sizeof(struct WheelSettings), WHEEL_SETTINGS_OFFSET);

  if (activeWheelSettings.check != WHEEL_SETTINGS_STORED)
  {

#ifdef DEBUG_LOAD_ACTIVE_WHEEL_SETTINGS
    Serial.println(F("No settings found - using default"));
#endif

    activeWheelSettings.leftWheelDiameter = 69;
    activeWheelSettings.rightWheelDiameter = 69;
    activeWheelSettings.wheelSpacing = 110;
    activeWheelSettings.check = WHEEL_SETTINGS_STORED;
    storeActiveWheelSettings();
  }

#ifdef DEBUG_LOAD_ACTIVE_WHEEL_SETTINGS
  Serial.print(F("left diameter: "));
  Serial.print(activeWheelSettings.leftWheelDiameter);
  Serial.print(F(" right diameter: "));
  Serial.print(activeWheelSettings.rightWheelDiameter);
  Serial.print(F(" wheel spacing: "));
  Serial.println(activeWheelSettings.wheelSpacing);
#endif
}

void setupWheelSettings()
{
  leftWheelCircumference = PI * activeWheelSettings.leftWheelDiameter;
  rightWheelCircumference = PI * activeWheelSettings.rightWheelDiameter;
  turningCircle = activeWheelSettings.wheelSpacing * PI;

  leftStepsPerMM = countsperrev / leftWheelCircumference;
  rightStepsPerMM = countsperrev / rightWheelCircumference;
}

const byte motorWaveformLookup[8] = {0b01000, 0b01100, 0b00100, 0b00110, 0b00010, 0b00011, 0b00001, 0b01001};

volatile int leftMotorWaveformPos = 0;
volatile int leftMotorWaveformDelta = 0;

volatile int rightMotorWaveformPos = 0;
volatile int rightMotorWaveformDelta = 0;

volatile unsigned long leftStepCounter = 0;
volatile unsigned long leftNumberOfStepsToMove = 0;

volatile unsigned long rightStepCounter = 0;
volatile unsigned long rightNumberOfStepsToMove = 0;

volatile unsigned long minInterruptIntervalInMicroSecs = 1200;

inline void setLeft(byte bits)
{
  digitalWrite(L_MOTOR_B1, bits & 1);
  digitalWrite(L_MOTOR_B2, bits & 2);
  digitalWrite(L_MOTOR_B3, bits & 4);
  digitalWrite(L_MOTOR_B4, bits & 8);
}

inline void setRight(byte bits)
{
  digitalWrite(R_MOTOR_B1, bits & 1);
  digitalWrite(R_MOTOR_B2, bits & 2);
  digitalWrite(R_MOTOR_B3, bits & 4);
  digitalWrite(R_MOTOR_B4, bits & 8);
}

#ifdef ARDUINO_ARCH_ESP32

hw_timer_t *leftTimer = NULL;
hw_timer_t *rightTimer = NULL;

void IRAM_ATTR onLeft()
{
  // If we are not moving, don't do anything
  if (leftMotorWaveformDelta != 0)
  {
    if (++leftStepCounter > leftNumberOfStepsToMove)
    {
      leftMotorWaveformDelta = 0;
      setLeft(0);
      timerAlarmDisable(leftTimer);
    }
    else
    {
      // Move the motor one step
      setLeft(motorWaveformLookup[leftMotorWaveformPos]);

      // Update and wrap the waveform position
      leftMotorWaveformPos += leftMotorWaveformDelta;
      if (leftMotorWaveformPos == 8)
        leftMotorWaveformPos = 0;
      if (leftMotorWaveformPos < 0)
        leftMotorWaveformPos = 7;
    }
  }
}

void ARDUINO_ISR_ATTR onRight()
{
  // If we are not moving, don't do anything
  if (rightMotorWaveformDelta != 0)
  {
    if (++rightStepCounter > rightNumberOfStepsToMove)
    {
      rightMotorWaveformDelta = 0;
      setRight(0);
      timerAlarmDisable(rightTimer);
    }
    else
    {
      // Move the motor one step

      setRight(motorWaveformLookup[rightMotorWaveformPos]);

      // Update and wrap the waveform position
      rightMotorWaveformPos += rightMotorWaveformDelta;
      if (rightMotorWaveformPos == 8)
        rightMotorWaveformPos = 0;
      if (rightMotorWaveformPos < 0)
        rightMotorWaveformPos = 7;
    }
  }
}

void setupMotors()
{
  loadActiveWheelSettings();
  setupWheelSettings();

  pinMode(L_MOTOR_B1, OUTPUT);
  pinMode(L_MOTOR_B2, OUTPUT);
  pinMode(L_MOTOR_B3, OUTPUT);
  pinMode(L_MOTOR_B4, OUTPUT);

  pinMode(R_MOTOR_B1, OUTPUT);
  pinMode(R_MOTOR_B2, OUTPUT);
  pinMode(R_MOTOR_B3, OUTPUT);
  pinMode(R_MOTOR_B4, OUTPUT);

  leftTimer = timerBegin(0, 80, true);
  timerAttachInterrupt(leftTimer, &onLeft, true);

  rightTimer = timerBegin(1, 80, true);
  timerAttachInterrupt(rightTimer, &onRight, true);
}

MoveFailReason timedMoveSteps(long leftStepsToMove, long rightStepsToMove, float timeToMoveInSeconds)
{
#ifdef DEBUG_TIMED_MOVE
  Serial.println("timedMoveSteps");
  Serial.print("    Left steps to move: ");
  Serial.print(leftStepsToMove);
  Serial.print(" Right steps to move: ");
  Serial.print(rightStepsToMove);
  Serial.print(" Time to move in seconds: ");
  Serial.println(timeToMoveInSeconds);
#endif

  long leftInterruptIntervalInMicroSeconds;
  long absLeftStepsToMove = abs(leftStepsToMove);

  if (leftStepsToMove != 0)
  {
#ifdef DEBUG_TIMED_MOVE
    Serial.println("    Moving left steps..");
#endif
    leftInterruptIntervalInMicroSeconds = (long)((timeToMoveInSeconds / (double)absLeftStepsToMove) * 1000000L + 0.5);
    timerAlarmWrite(leftTimer, leftInterruptIntervalInMicroSeconds, true);
    leftNumberOfStepsToMove = absLeftStepsToMove;
    leftStepCounter = 0;
    if (leftStepsToMove > 0)
    {
      leftMotorWaveformDelta = 1;
    }
    else
    {
      leftMotorWaveformDelta = -1;
    }
    timerAlarmEnable(leftTimer);
  }

  long rightInterruptIntervalInMicroseconds;
  long absRightStepsToMove = abs(rightStepsToMove);

  if (rightStepsToMove != 0)
  {
#ifdef DEBUG_TIMED_MOVE
    Serial.println("    Moving right steps..");
#endif
    rightInterruptIntervalInMicroseconds = (long)((timeToMoveInSeconds / (double)absRightStepsToMove) * 1000000L + 0.5);
    timerAlarmWrite(rightTimer, rightInterruptIntervalInMicroseconds, true);
    rightNumberOfStepsToMove = absRightStepsToMove;
    rightStepCounter = 0;
    if (rightStepsToMove > 0)
    {
      rightMotorWaveformDelta = -1;
    }
    else
    {
      rightMotorWaveformDelta = 1;
    }
    timerAlarmEnable(rightTimer);
  }

#ifdef DEBUG_TIMED_MOVE
  Serial.print("    Left interval in microseconds: ");
  Serial.print(leftInterruptIntervalInMicroSeconds);
  Serial.print(" Right interval in microseconds: ");
  Serial.println(rightInterruptIntervalInMicroseconds);
#endif

  return Move_OK;
}

#endif

#ifdef ARDUINO_ARCH_PICOx

struct repeating_timer leftTimer;
struct repeating_timer rightTimer;

bool onLeft(struct repeating_timer *t)
{
  // If we are not moving, don't do anything
  if (leftMotorWaveformDelta != 0)
  {
    if (++leftStepCounter > leftNumberOfStepsToMove)
    {
      leftMotorWaveformDelta = 0;
      setLeft(0);
      cancel_repeating_timer(&leftTimer);
    }
    else
    {
      // Move the motor one step
      setLeft(motorWaveformLookup[leftMotorWaveformPos]);

      // Update and wrap the waveform position
      leftMotorWaveformPos += leftMotorWaveformDelta;
      if (leftMotorWaveformPos == 8)
        leftMotorWaveformPos = 0;
      if (leftMotorWaveformPos < 0)
        leftMotorWaveformPos = 7;
    }
  }
  return true;
}

bool onRight(struct repeating_timer *t)
{
  // If we are not moving, don't do anything
  if (rightMotorWaveformDelta != 0)
  {
    if (++rightStepCounter > rightNumberOfStepsToMove)
    {
      rightMotorWaveformDelta = 0;
      setRight(0);
      cancel_repeating_timer(&rightTimer);
    }
    else
    {
      // Move the motor one step
      setRight(motorWaveformLookup[rightMotorWaveformPos]);

      // Update and wrap the waveform position
      rightMotorWaveformPos += rightMotorWaveformDelta;
      if (rightMotorWaveformPos == 8)
        rightMotorWaveformPos = 0;
      if (rightMotorWaveformPos < 0)
        rightMotorWaveformPos = 7;
    }
  }
  return true;
}

void setupMotors()
{
  loadActiveWheelSettings();
  setupWheelSettings();

  pinMode(L_MOTOR_B1, OUTPUT);
  pinMode(L_MOTOR_B2, OUTPUT);
  pinMode(L_MOTOR_B3, OUTPUT);
  pinMode(L_MOTOR_B4, OUTPUT);

  pinMode(R_MOTOR_B1, OUTPUT);
  pinMode(R_MOTOR_B2, OUTPUT);
  pinMode(R_MOTOR_B3, OUTPUT);
  pinMode(R_MOTOR_B4, OUTPUT);

  add_repeating_timer_us(50000, onLeft, NULL, &leftTimer);
  add_repeating_timer_us(50000, onRight, NULL, &rightTimer);
}

MoveFailReason timedMoveSteps(long leftStepsToMove, long rightStepsToMove, float timeToMoveInSeconds)
{
#ifdef DEBUG_TIMED_MOVE
  Serial.println("timedMoveSteps");
  Serial.print("    Left steps to move: ");
  Serial.print(leftStepsToMove);
  Serial.print(" Right steps to move: ");
  Serial.print(rightStepsToMove);
  Serial.print(" Time to move in seconds: ");
  Serial.println(timeToMoveInSeconds);
#endif

  long leftInterruptIntervalInMicroSeconds;
  long absLeftStepsToMove = abs(leftStepsToMove);

  if (leftStepsToMove != 0)
  {
    cancel_repeating_timer(&leftTimer);

#ifdef DEBUG_TIMED_MOVE
    Serial.println("    Moving left steps..");
#endif
    leftInterruptIntervalInMicroSeconds = (long)((timeToMoveInSeconds / (double)absLeftStepsToMove) * 1000000L + 0.5);
    leftNumberOfStepsToMove = absLeftStepsToMove;
    leftStepCounter = 0;
    if (leftStepsToMove > 0)
    {
      leftMotorWaveformDelta = 1;
    }
    else
    {
      leftMotorWaveformDelta = -1;
    }
    leftTimer.delay_us = leftInterruptIntervalInMicroSeconds;
  }

  long rightInterruptIntervalInMicroseconds;
  long absRightStepsToMove = abs(rightStepsToMove);

  if (rightStepsToMove != 0)
  {
#ifdef DEBUG_TIMED_MOVE
    Serial.println("    Moving right steps..");
#endif
    cancel_repeating_timer(&rightTimer);

    rightInterruptIntervalInMicroseconds = (long)((timeToMoveInSeconds / (double)absRightStepsToMove) * 1000000L + 0.5);
    rightNumberOfStepsToMove = absRightStepsToMove;
    rightStepCounter = 0;
    if (rightStepsToMove > 0)
    {
      rightMotorWaveformDelta = -1;
    }
    else
    {
      rightMotorWaveformDelta = 1;
    }
    add_repeating_timer_us(rightInterruptIntervalInMicroseconds, onRight, NULL, &rightTimer);
  }

#ifdef DEBUG_TIMED_MOVE
  Serial.print("    Left interval in microseconds: ");
  Serial.print(leftInterruptIntervalInMicroSeconds);
  Serial.print(" Right interval in microseconds: ");
  Serial.println(rightInterruptIntervalInMicroseconds);
#endif

  return Move_OK;
}

#endif

#ifdef ARDUINO_ARCH_PICO

unsigned long lastMicros;

volatile unsigned long leftIntervalBetweenSteps;
volatile unsigned long rightIntervalBetweenSteps;

volatile unsigned long leftTimeOfLastStep;
volatile unsigned long rightTimeOfLastStep;

volatile unsigned long leftTimeOfNextStep;
volatile unsigned long rightTimeOfNextStep;

volatile unsigned long currentMicros;
volatile unsigned long leftTimeSinceLastStep;
volatile unsigned long rightTimeSinceLastStep;

volatile bool motorsRunning = false;

void setupMotors()
{
  loadActiveWheelSettings();
  setupWheelSettings();

  pinMode(L_MOTOR_B1, OUTPUT);
  pinMode(L_MOTOR_B2, OUTPUT);
  pinMode(L_MOTOR_B3, OUTPUT);
  pinMode(L_MOTOR_B4, OUTPUT);

  pinMode(R_MOTOR_B1, OUTPUT);
  pinMode(R_MOTOR_B2, OUTPUT);
  pinMode(R_MOTOR_B3, OUTPUT);
  pinMode(R_MOTOR_B4, OUTPUT);

  motorsRunning = true;
}

inline void startMotor(unsigned long stepLimit, unsigned long microSecsPerPulse, bool forward,
                       volatile unsigned long *motorStepLimit, volatile unsigned long *motorPulseInterval,
                       volatile int *motorDelta, volatile int *motorPos)
{
  // If we are not moving - set the delta to zero and return

  if (stepLimit == 0)
  {
    motorDelta = 0;
    return;
  }

  *motorStepLimit = stepLimit;

  *motorPulseInterval = microSecsPerPulse;

  if (forward)
  {
    *motorDelta = 1;
  }
  else
  {
    *motorDelta = -1;
  }

  *motorPos = 0;
}

void startMotors(
    unsigned long leftSteps, unsigned long rightSteps,
    unsigned long leftMicroSecsPerPulse, unsigned long rightMicroSecsPerPulse,
    bool leftForward, bool rightForward)
{
  // Set up the counters for the left and right motor moves

  lastMicros = micros();

  leftStepCounter = 0;
  rightStepCounter = 0;

  startMotor(leftSteps, leftMicroSecsPerPulse, leftForward,
             &leftNumberOfStepsToMove, &leftIntervalBetweenSteps, &leftMotorWaveformDelta, &leftMotorWaveformPos);

  startMotor(rightSteps, rightMicroSecsPerPulse, rightForward,
             &rightNumberOfStepsToMove, &rightIntervalBetweenSteps, &rightMotorWaveformDelta, &rightMotorWaveformPos);

  return;
}

MoveFailReason timedMoveSteps(long leftStepsToMove, long rightStepsToMove, float timeToMoveInSeconds)
{
#ifdef DEBUG_TIMED_MOVE
  Serial.println("timedMoveSteps");
  Serial.print("    Left steps to move: ");
  Serial.print(leftStepsToMove);
  Serial.print(" Right steps to move: ");
  Serial.print(rightStepsToMove);
  Serial.print(" Time to move in seconds: ");
  Serial.println(timeToMoveInSeconds);
#endif

  long leftInterruptIntervalInMicroSeconds;

  if (leftStepsToMove != 0)
  {
    leftInterruptIntervalInMicroSeconds = (long)((timeToMoveInSeconds / (double)abs(leftStepsToMove)) * 1000000L + 0.5);
  }
  else
  {
    leftInterruptIntervalInMicroSeconds = minInterruptIntervalInMicroSecs;
  }

  long rightInterruptIntervalInMicroseconds;

  if (rightStepsToMove != 0)
  {
    rightInterruptIntervalInMicroseconds = (long)((timeToMoveInSeconds / (double)abs(rightStepsToMove)) * 1000000L + 0.5);
  }
  else
  {
    rightInterruptIntervalInMicroseconds = minInterruptIntervalInMicroSecs;
  }

#ifdef DEBUG_TIMED_MOVE
  Serial.print("    Left interval in microseconds: ");
  Serial.print(leftInterruptIntervalInMicroSeconds);
  Serial.print(" Right interval in microseconds: ");
  Serial.println(rightInterruptIntervalInMicroseconds);
#endif

  // There's a minium gap allowed between intervals. This is set by the top speed of the motors
  // Presently set at 1000 microseconds

  if (leftInterruptIntervalInMicroSeconds < minInterruptIntervalInMicroSecs & rightInterruptIntervalInMicroseconds < minInterruptIntervalInMicroSecs)
  {
    return Left_And_Right_Distance_Too_Large;
  }

  if (leftInterruptIntervalInMicroSeconds<minInterruptIntervalInMicroSecs & rightInterruptIntervalInMicroseconds> minInterruptIntervalInMicroSecs)
  {
    return Left_Distance_Too_Large;
  }

  if (rightInterruptIntervalInMicroseconds<minInterruptIntervalInMicroSecs & leftInterruptIntervalInMicroSeconds> minInterruptIntervalInMicroSecs)
  {
    return Right_Distance_Too_Large;
  }

  // If we get here we can move at this speed.

  startMotors(abs(leftStepsToMove), abs(rightStepsToMove),
              leftInterruptIntervalInMicroSeconds, rightInterruptIntervalInMicroseconds,
              leftStepsToMove > 0, rightStepsToMove > 0);

  return Move_OK;
}

void updateMotors()
{
  if (!motorsRunning)
  {
    return;
  }

  unsigned long currentMicros = micros();

  if (leftMotorWaveformDelta != 0)
  {
    // left is moving - see if it is time to do a step
    leftTimeSinceLastStep = ulongDiff(currentMicros, leftTimeOfLastStep);
    if (leftTimeSinceLastStep >= leftIntervalBetweenSteps)
    {
      if (++leftStepCounter > leftNumberOfStepsToMove)
      {
        leftMotorWaveformDelta = 0;
        setLeft(0);
      }
      else
      {
        // Move the motor one step
        setLeft(motorWaveformLookup[leftMotorWaveformPos]);

        // Update and wrap the waveform position
        leftMotorWaveformPos += leftMotorWaveformDelta;
        if (leftMotorWaveformPos == 8)
          leftMotorWaveformPos = 0;
        if (leftMotorWaveformPos < 0)
          leftMotorWaveformPos = 7;
      }
       leftTimeOfLastStep = currentMicros - (leftTimeSinceLastStep - leftIntervalBetweenSteps);
      leftTimeOfNextStep = currentMicros + leftIntervalBetweenSteps;
    }
  }
  if (rightMotorWaveformDelta != 0)
  {
    rightTimeSinceLastStep = ulongDiff(currentMicros, rightTimeOfLastStep);

    if (rightTimeSinceLastStep >= rightIntervalBetweenSteps)
    {
      if (++rightStepCounter > rightNumberOfStepsToMove)
      {
        rightMotorWaveformDelta = 0;
        setRight(0);
      }
      else
      {
        // Move the motor one step
        setRight(motorWaveformLookup[rightMotorWaveformPos]);

        // Update and wrap the waveform position
        rightMotorWaveformPos += rightMotorWaveformDelta;
        if (rightMotorWaveformPos == 8)
          rightMotorWaveformPos = 0;
        if (rightMotorWaveformPos < 0)
          rightMotorWaveformPos = 7;
      }
      rightTimeOfLastStep = currentMicros - (rightTimeSinceLastStep - rightIntervalBetweenSteps);
      rightTimeOfNextStep = currentMicros + rightIntervalBetweenSteps;
    }
  }
}

#endif

float fastMoveSteps(long leftStepsToMove, long rightStepsToMove)
{

#ifdef DEBUG_FAST_MOVE_STEPS
  Serial.println("fastMoveSteps");
  Serial.print("    Left steps to move: ");
  Serial.print(leftStepsToMove);
  Serial.print(" Right steps to move: ");
  Serial.println(rightStepsToMove);
#endif

  // work out how long it will take to move in seconds

  float timeForLeftMoveInSeconds;

  if (leftStepsToMove != 0)
  {
    timeForLeftMoveInSeconds = ((float)abs(leftStepsToMove) * (float)minInterruptIntervalInMicroSecs) / 1000000.0;
  }
  else
  {
    timeForLeftMoveInSeconds = 0;
  }

  float timeForRightMoveInSeconds;

  if (rightStepsToMove != 0)
  {
    timeForRightMoveInSeconds = ((float)abs(rightStepsToMove) * (float)minInterruptIntervalInMicroSecs) / 1000000.0;
  }
  else
  {
    timeForRightMoveInSeconds = 0;
  }

#ifdef DEBUG_FAST_MOVE_STEPS
  Serial.print("    Left time to move: ");
  Serial.print(timeForLeftMoveInSeconds);
  Serial.print(" Right time to move: ");
  Serial.println(timeForRightMoveInSeconds);
#endif

  // Allow time for the slowest mover
  if (timeForLeftMoveInSeconds > timeForRightMoveInSeconds)
  {
    timedMoveSteps(leftStepsToMove, rightStepsToMove, timeForLeftMoveInSeconds);
    return timeForLeftMoveInSeconds;
  }
  else
  {
    timedMoveSteps(leftStepsToMove, rightStepsToMove, timeForRightMoveInSeconds);
    return timeForRightMoveInSeconds;
  }
}

int timedMoveDistanceInMM(float leftMMs, float rightMMs, float timeToMoveInSeconds)
{

#ifdef TIMED_MOVE_MM_DEBUG
  Serial.println("timedMoveDistanceInMM");
  Serial.print("    Left mms to move: ");
  Serial.print(leftMMs);
  Serial.print(" Right mms to move: ");
  Serial.print(rightMMs);
  Serial.print(" Time to move in seconds: ");
  Serial.println(timeToMoveInSeconds);
#endif

  // Do this becuase rounding errors can lead to different numbers for
  // positive and negative moves of the same length

  long leftSteps = (long)((abs(leftMMs) * leftStepsPerMM) + 0.5);
  long rightSteps = (long)((abs(rightMMs) * rightStepsPerMM) + 0.5);
  if (leftMMs < 0)
  {
    leftSteps *= -1;
  }

  if (rightMMs < 0)
  {
    rightSteps *= -1;
  }

#ifdef TIMED_MOVE_MM_DEBUG
  Serial.print("    Left steps to move: ");
  Serial.print(leftSteps);
  Serial.print(" Right steps to move: ");
  Serial.println(rightSteps);
#endif

  return timedMoveSteps(leftSteps, rightSteps, timeToMoveInSeconds);
}

int fastMoveDistanceInMM(float leftMMs, float rightMMs)
{

#ifdef FAST_MOVE_MM_DEBUG
  Serial.println("fastMoveDistanceInMM");
  Serial.print("    Left mms to move: ");
  Serial.print(leftMMs);
  Serial.print(" Right mms to move: ");
  Serial.println(rightMMs);
  Serial.print("    Left mms per step: ");
  Serial.print(leftStepsPerMM);
  Serial.print(" Right mms per step: ");
  Serial.println(rightStepsPerMM);

#endif

  long leftSteps = (long)((abs(leftMMs) * leftStepsPerMM) + 0.5);
  long rightSteps = (long)((abs(rightMMs) * rightStepsPerMM) + 0.5);

  if (leftMMs < 0)
  {
    leftSteps *= -1;
  }

  if (rightMMs < 0)
  {
    rightSteps *= -1;
  }

#ifdef FAST_MOVE_MM_DEBUG
  Serial.print("    Left steps to move: ");
  Serial.print(leftSteps);
  Serial.print(" Right steps to move: ");
  Serial.println(rightSteps);
#endif

  int result = fastMoveSteps(leftSteps, rightSteps);

  return result;
}

void rightStop()
{
  setRight(0);
  rightMotorWaveformDelta = 0;
  rightStepCounter = 0;
}

void leftStop()
{
  setLeft(0);
  leftMotorWaveformDelta = 0;
  leftStepCounter = 0;
}

void motorStop()
{
  leftStop();
  rightStop();
}

bool motorsMoving()
{
  if (rightMotorWaveformDelta != 0)
    return true;
  if (leftMotorWaveformDelta != 0)
    return true;
  return false;
}

void waitForMotorsStop()
{
  while (motorsMoving())
    delay(1);
}

// #define DEBUG_FAST_ROTATE

void fastRotateRobot(float angle)
{
  float noOfTurns = angle / 360.0;
  float distanceToRotate = noOfTurns * turningCircle;

  fastMoveDistanceInMM(distanceToRotate, -distanceToRotate);

#ifdef DEBUG_FAST_ROTATE
  Serial.print(". angle: ");
  Serial.print(angle);
  Serial.print(" noOfTurns: ");
  Serial.print(noOfTurns);
  Serial.print(" distanceToRotate: ");
  Serial.println(distanceToRotate);
#endif
}

int timedRotateRobot(float angle, float timeToMoveInSeconds)
{
  float noOfTurns = angle / 360.0;
  float distanceToRotate = noOfTurns * turningCircle;

#ifdef DEBUG_TIMED_ROTATE
  Serial.print(". angle: ");
  Serial.print(angle);
  Serial.print(" noOfTurns: ");
  Serial.print(noOfTurns);
  Serial.print(" time: ");
  Serial.print(timeToMoveInSeconds);
  Serial.print(" distanceToRotate: ");
  Serial.println(distanceToRotate);
#endif

  return timedMoveDistanceInMM(distanceToRotate, -distanceToRotate, timeToMoveInSeconds);
}

void fastMoveArcRobot(float radius, float angle)
{
  float noOfTurns = angle / 360.0;
  float absRadius = abs(radius);

  float leftDistanceToMove = noOfTurns * ((absRadius + (activeWheelSettings.wheelSpacing / 2.0)) * 2.0 * PI);
  float rightDistanceToMove = noOfTurns * ((absRadius - (activeWheelSettings.wheelSpacing / 2.0)) * 2.0 * PI);

#ifdef DEBUG_FAST_ARC
  Serial.println("fastMoveArcRobot");
  Serial.print(" radius: ");
  Serial.print(radius);
  Serial.print(" angle: ");
  Serial.print(angle);
  Serial.print(" leftDistanceToMove: ");
  Serial.print(leftDistanceToMove);
  Serial.print(" rightDistanceToMove: ");
  Serial.println(rightDistanceToMove);
#endif

  if (radius >= 0)
  {
    fastMoveDistanceInMM(leftDistanceToMove, rightDistanceToMove);
  }
  else
  {
    fastMoveDistanceInMM(rightDistanceToMove, leftDistanceToMove);
  }
}

int timedMoveArcRobot(float radius, float angle, float timeToMoveInSeconds)
{
  float noOfTurns = angle / 360.0;
  float absRadius = abs(radius);

  float leftDistanceToMove = noOfTurns * ((absRadius + (activeWheelSettings.wheelSpacing / 2.0)) * 2.0 * PI);
  float rightDistanceToMove = noOfTurns * ((absRadius - (activeWheelSettings.wheelSpacing / 2.0)) * 2.0 * PI);

#ifdef DEBUG_TIMED_ARC
  Serial.println("timedMoveArcRobot");
  Serial.print(" radius: ");
  Serial.print(radius);
  Serial.print(" angle: ");
  Serial.print(angle);
  Serial.print(" time: ");
  Serial.print(timeToMoveInSeconds);
  Serial.print(" leftDistanceToMove: ");
  Serial.print(leftDistanceToMove);
  Serial.print(" rightDistanceToMove: ");
  Serial.println(rightDistanceToMove);
#endif

  if (radius >= 0)
  {
    return timedMoveDistanceInMM(leftDistanceToMove, rightDistanceToMove, timeToMoveInSeconds);
  }
  else
  {
    return timedMoveDistanceInMM(rightDistanceToMove, leftDistanceToMove, timeToMoveInSeconds);
  }
}
