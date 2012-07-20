#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "WaterRule.h"
#include <Time.h>
#include <TimeAlarms.h>

#define NUMOFDAYS 7

typedef byte Day;

class Schedule {
  unsigned int address;
  WaterRule rules[NUMOFDAYS];
public:
  Schedule(unsigned int address);
  void fetch();
  WaterRule& get(Day day);
  unsigned int store();
  unsigned int storeDay(Day day);
  void toString(char* buf);
  void time(byte hour, byte min, byte sec, byte day, byte month, unsigned int year);
  void time(byte hour, byte min, byte sec);
  void date(byte day, byte month, unsigned int year);
  void createAlarms();
  void disableAlarms();
  uint8_t hour();
  uint8_t minute();
  uint8_t second();
  uint8_t day();
  uint8_t month();
  uint16_t year();
  static char dayIndex(char* day);
};

#endif
