
#include "Schedule.h"

char* days[] = { "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday" };

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

char Schedule::dayIndex(char* day) {
  for (byte i = 0; i < 7; i++) {
    if (strcmp(days[i], day) == 0) {
      return i;
    }
  }
  return -1;
}


unsigned int Schedule::storeDay(Day day) {
  return rules[day].store(day * sizeof(WaterRule));
}

void Schedule::toString(char* buf) {
  unsigned int pos = 0;
  for (byte i = 0; i < NUMOFDAYS; i++) {
    WaterRule& rule = rules[i];
    pos += sprintf(&buf[pos], "%s : ", days[i]);
    if (rule.isEnabled()) {
      pos += rule.toString(&buf[pos]);
      pos += sprintf(&buf[pos], "\n");
    }
    else {
      pos += sprintf(&buf[pos], "not enabled\n");
    }
  }
}
