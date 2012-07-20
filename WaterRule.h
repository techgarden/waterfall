#ifndef WATERRULE_H
#define WATERRULE_H

#include "Arduino.h"
#include <Time.h>
#include <TimeAlarms.h>

class WaterRule {
  byte day;
  byte hour;
  byte minute;
  byte duration;
  byte enabled;
  byte startAlarmId, stopAlarmId;
public:
  WaterRule();
  void setDay(byte day);
  void set(byte h, byte m, byte d);
  void createAlarms();
  unsigned int store(unsigned int address);
  void fetch(unsigned int address);
  byte getHour();
  byte getMinute();
  byte getDuration();
  unsigned int toString(char* buf);
  byte isEnabled();
  void setEnabled(byte enabled);
};

#endif
