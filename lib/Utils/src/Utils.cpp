#include <limits.h>

#include "Utils.h"

const char * version = "HullOS Version R1.3";

unsigned long ulongDiff(unsigned long end, unsigned long start)
{
  if (end >= start)
  {
    return end - start;
  }
  else
  {
    return ULONG_MAX - start + end + 1;
  }
}


