#include <Arduino.h>
#include "Utils.h"
#include "Storage.h"
#include "Motors.h"
#include "Pixels.h"
#include "Distance.h"
#include "Sound.h"
#include "Commands.h"
#include "Variables.h"
#include "Script.h"

void setup() {
  Serial.begin(115200);
  setupStorage();
  setupMotors();
  setupPixels();
  setupDistanceSensor(100);
  setupSound();
  setupVariables();
  setupRemoteControl();
  startProgramExecution(STORED_PROGRAM_OFFSET);
}

void loop() {
  updateRobot();
  updateDistanceSensor();
  updateLightsAndDelay(!commandsNeedFullSpeed());
}

