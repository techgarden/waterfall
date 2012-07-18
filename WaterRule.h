#ifndef WATERRULE_H
#define WATERRULE_H

#include "Arduino.h"

class WaterRule {
  byte hour;
  byte minute;
  byte duration;
  byte enabled;
public:
  WaterRule();
  WaterRule(byte h, byte m, byte d);
  void set(byte h, byte m, byte d);
  unsigned int store(unsigned int address);
  void fetch(unsigned int address);
  byte getHour();
  byte getMinute();
  byte getDuration();
  String toString();
  byte isEnabled();
  void setEnabled(byte enabled);
};

#endif
