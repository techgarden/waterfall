#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "WaterRule.h"
#define NUMOFDAYS 7

typedef byte Day;

class Schedule {
  unsigned int address;
  WaterRule rules[NUMOFDAYS];
public:
  Schedule(unsigned int address);
  void fetch();
  WaterRule get(Day day);
  unsigned int store();
  unsigned int storeDay(Day day);
};

#endif
