#include <Arduino.h>

//#define TEST_TIMER

#ifdef TEST_TIMER

#define LED 13

#ifdef ARDUINO_ARCH_ESP32

hw_timer_t *Timer0_Cfg = NULL;

void IRAM_ATTR Timer0_ISR()
{
  digitalWrite(LED, !digitalRead(LED));
}
void setup()
{
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  Timer0_Cfg = timerBegin(0, 80, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
  timerAlarmWrite(Timer0_Cfg, 50000, true);
  timerAlarmEnable(Timer0_Cfg);
}

void loop()
{
  if (Serial.available())
  {
    int ch = Serial.read();
    if (ch == 'f')
    {
      timerAlarmWrite(Timer0_Cfg, 500, true);
    }
    if (ch == 's')
    {
      timerAlarmWrite(Timer0_Cfg, 5000, true);
    }
    if (ch == 'q')
    {
      timerAlarmDisable(Timer0_Cfg);
    }
    if (ch == 'r')
    {
      timerAlarmEnable(Timer0_Cfg);
    }
  }
}

#endif

#ifdef ARDUINO_ARCH_PICO

#include <stdio.h>
#include "pico/stdlib.h"

volatile bool timer_fired = false;

struct repeating_timer timer;

bool repeating_timer_callback(struct repeating_timer *t)
{
  digitalWrite(LED, !digitalRead(LED));
  return true;
}

void setup()
{
  pinMode(LED, OUTPUT);
  add_repeating_timer_us(5000, repeating_timer_callback, NULL, &timer);
}

void loop()
{
  if (Serial.available())
  {
    int ch = Serial.read();
    if (ch == 'f')
    {
      cancel_repeating_timer(&timer);
      add_repeating_timer_us(-500, repeating_timer_callback, NULL, &timer);
    }
    if (ch == 's')
    {
      cancel_repeating_timer(&timer);
      add_repeating_timer_us(-50000, repeating_timer_callback, NULL, &timer);
    }
    if (ch == 'q')
    {
      cancel_repeating_timer(&timer);
    }
    if (ch == 'r')
    {
      add_repeating_timer_us(-5000, repeating_timer_callback, NULL, &timer);
    }
  }
}

#endif

#endif

#ifndef TEST_TIMER

#include "Utils.h"
#include "Storage.h"
#include "Motors.h"
#include "Pixels.h"
#include "Distance.h"
#include "Sound.h"
#include "Commands.h"
#include "Variables.h"
#include "Script.h"
#include "RobotWebServer.h"

void setup1()
{

}

void loop1()
{
  updateMotors();
}

void setup()
{
  Serial.begin(115200);
  setupStorage();
  setupMotors();
  setupPixels();
  setupDistanceSensor(100);
  setupSound();
  setupVariables();
  setupRemoteControl();
  startProgramExecution(STORED_PROGRAM_OFFSET);
  setupServer();
}

bool first = true;

void loop()
{
  updateServer();
  updateRobot();
  updateDistanceSensor();
  updateLightsAndDelay(!commandsNeedFullSpeed());
}

#endif