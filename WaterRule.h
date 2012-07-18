#ifndef WATERRULE_H
#define WATERRULE_H

#include "Arduino.h"

typedef uint8_t Hour;
typedef uint8_t Minute;
typedef uint8_t Duration;

class WaterRule {
  Hour hour;
  Minute minute;
  Duration duration;
public:
  WaterRule();
  WaterRule(Hour h, Minute m, Duration d);
  void set(Hour h, Minute m, Duration d);
  unsigned int store(unsigned int address);
};

#endif
