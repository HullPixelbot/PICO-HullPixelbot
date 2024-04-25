#pragma once

// GPIO pin numbers for motor coil connections

enum MoveFailReason
{
    Move_OK,
    Left_Distance_Too_Large,
    Right_Distance_Too_Large,
    Left_And_Right_Distance_Too_Large
};

//#define DEBUG_TIMED_MOVE
//#define DEBUG_FAST_MOVE_STEPS
//#define FAST_MOVE_MM_DEBUG
//#define TIMED_MOVE_MM_DEBUG
//#define DEBUG_TIMED_ROTATE
//#define DEBUG_FAST_ARC
//#define DEBUG_TIMED_ARC
//#define DEBUG_LOAD_ACTIVE_WHEEL_SETTINGS

void setupMotors();
MoveFailReason timedMoveSteps(long leftStepsToMove, long rightStepsToMove, float timeToMoveInSeconds);
float fastMoveSteps(long leftStepsToMove, long rightStepsToMove);
int timedMoveDistanceInMM(float leftMMs, float rightMMs, float timeToMoveInSeconds);
int fastMoveDistanceInMM(float leftMMs, float rightMMs);
void rightStop();
void leftStop();
void motorStop();
bool motorsMoving();
void waitForMotorsStop();
void fastRotateRobot(float angle);
int timedRotateRobot(float angle, float timeToMoveInSeconds);
void fastMoveArcRobot(float radius, float angle);
int timedMoveArcRobot(float radius, float angle, float timeToMoveInSeconds);

void setActiveWheelSettings(int leftDiam, int rightDiam, int spacing);
void dumpActiveWheelSettings();
void storeActiveWheelSettings();
void loadActiveWheelSettings();
void setupWheelSettings();
void updateMotors();
