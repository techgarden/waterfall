
#include "Schedule.h"

Schedule::Schedule(unsigned int address) {
  this->address = address;
}

unsigned int Schedule::store() {
  for (byte i = 0; i < NUMOFDAYS; i++) {
    address = rules[i].store(address);
  }
  return address;
}

void Schedule::fetch() {
  for (byte i = 0; i < NUMOFDAYS; i++) {
    rules[i].fetch(i * sizeof(WaterRule));
  }
}

WaterRule& Schedule::get(Day day) {
  return rules[day];
}

unsigned int Schedule::storeDay(Day day) {
  return rules[day].store(day * sizeof(WaterRule));
}
