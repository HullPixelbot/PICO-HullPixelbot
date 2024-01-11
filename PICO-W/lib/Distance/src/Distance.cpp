#include <Arduino.h>

#include "Utils.h"
#include "Distance.h"

volatile long pulseStartTime;
volatile long pulseWidth;

volatile DistanceSensorState distanceSensorState = DISTANCE_SENSOR_OFF;

volatile int distanceSensorReadingIntervalInMillisecs;

volatile unsigned long timeOfLastDistanceReading;

void pulseEvent()
{
    if(digitalRead(DISTANCE_ECHO_PIN)) {
    // pulse gone high - record start
    pulseStartTime = micros();
  }
  else
  {
    pulseWidth = micros() - pulseStartTime;
    distanceSensorState = DISTANCE_SENSOR_READING_READY;
  }
}

void updateDistanceSensorReadingInterval(int readingIntervalInMillisecs)
{
  distanceSensorReadingIntervalInMillisecs = readingIntervalInMillisecs;
}

inline void startDistanceSensorReading()
{
  digitalWrite(DISTANCE_TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(DISTANCE_TRIG_PIN, HIGH);
  delayMicroseconds(10);

  distanceSensorState = DISTANCE_SENSOR_AWAITING_READING;

  digitalWrite(DISTANCE_TRIG_PIN, LOW);

  // let the signals settle (actually I've no idea why this is needed)

  delay(5);
}

void setupDistanceSensor(int readingIntervalInMillisecs)
{
  if (distanceSensorState != DISTANCE_SENSOR_OFF)
    return;

  updateDistanceSensorReadingInterval(readingIntervalInMillisecs);

  pulseWidth = 0;
  pinMode(DISTANCE_TRIG_PIN, OUTPUT);
  pinMode(DISTANCE_ECHO_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(DISTANCE_ECHO_PIN), pulseEvent, CHANGE);

  startDistanceSensorReading();
}

inline void startWaitBetweenReadings()
{
  timeOfLastDistanceReading = millis();

  distanceSensorState = DISTANCE_SENSOR_BETWEEN_READINGS;
}

// checks to see if it is time to take another reading

inline void updateSensorBetweenReadings()
{
  unsigned long now = millis();
  unsigned long timeSinceLastReading = ulongDiff(now, timeOfLastDistanceReading);


  if (timeSinceLastReading >= distanceSensorReadingIntervalInMillisecs)
  {
    startDistanceSensorReading();
  }
}

void updateDistanceSensor()
{
  switch (distanceSensorState)
  {
  case DISTANCE_SENSOR_OFF:
    // if the sensor has not been turned on - do nothing
    break;

  case DISTANCE_SENSOR_ON:
    // if the sensor is on, start a reading
    startDistanceSensorReading();
    break;

  case DISTANCE_SENSOR_AWAITING_READING:
    // if the sensor is awaiting a reading - do nothing
    break;

  case DISTANCE_SENSOR_BETWEEN_READINGS:
    // if the sensor is between readings - check the timer
    updateSensorBetweenReadings();
    break;

  case DISTANCE_SENSOR_READING_READY:
    startWaitBetweenReadings();
    break;
  }
}

int getDistanceValueInt()
{
  return (int)(pulseWidth / 5.8) + distanceSensorState;
}

float getDistanceValueFloat()
{
  return (float)pulseWidth / 5.80;
}

void directDistanceReadTest()
{
  pinMode(DISTANCE_TRIG_PIN, OUTPUT);
  pinMode(DISTANCE_ECHO_PIN, INPUT);
  long duration;
  float distance;
  while (true)
  {
    digitalWrite(DISTANCE_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(DISTANCE_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(DISTANCE_TRIG_PIN, LOW);
    duration = pulseIn(DISTANCE_ECHO_PIN, HIGH);
    distance = ((float)duration / 2.0) / 29.0;
    Serial.println(distance);
    delay(500);
  }
}

void testDistanceSensor()
{
  setupDistanceSensor(10);
  //  directDistanceReadTest();
  while (true)
  {
    updateDistanceSensor();
    float distance = getDistanceValueFloat();
    Serial.println(distance);
    delay(100);
  }
}
