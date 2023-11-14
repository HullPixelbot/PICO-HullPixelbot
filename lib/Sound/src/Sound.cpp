#include <Arduino.h>

#include "Sound.h"

void setupSound()
{
  pinMode(SOUND_GPIO, OUTPUT);
  noTone(SOUND_GPIO);
}

void playTone(int frequency, unsigned long duration)
{
  tone(SOUND_GPIO, frequency, duration);
}
