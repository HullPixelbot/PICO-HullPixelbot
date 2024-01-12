#include <Arduino.h>

#include "Sound.h"

void setupSound()
{
#ifdef ARDUINO_ARCH_PICO
  pinMode(SOUND_GPIO, OUTPUT);
  noTone(SOUND_GPIO);
#endif
}

void playTone(int frequency, unsigned long duration)
{
#ifdef ARDUINO_ARCH_PICO
  tone(SOUND_GPIO, frequency, duration);
#endif
}
