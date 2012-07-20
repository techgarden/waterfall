
#include "Schedule.h"

char* days[] = { "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday" };

Schedule::Schedule(unsigned int address) {
  this->address = address;
}

void Schedule::createAlarms() {
  for (byte i = 0; i < NUMOFDAYS; i++) {
    rules[i].createAlarms();
  }
}

void Schedule::time(byte hour, byte min, byte sec, byte day, byte month, unsigned int year) {
  setTime(hour, min, sec, day, month, year);
  createAlarms();
}

void Schedule::time(byte hour, byte min, byte sec) {
  time(hour, min, sec, this->day(), this->month(), this->year());
}

void Schedule::date(byte day, byte month, unsigned int year) {
  time(this->hour(), this->minute(), this->second(), day, month, year);
}

uint8_t Schedule::hour() {
  return ::hour();
}

uint8_t Schedule::minute() {
  return ::minute();
}

uint8_t Schedule::second() {
  return ::second();
}

uint8_t Schedule::day() {
  return ::day();
}

uint8_t Schedule::month() {
  return ::month();
}

uint16_t Schedule::year() {
  return ::year();
}

unsigned int Schedule::store() {
  for (byte i = 0; i < NUMOFDAYS; i++) {
    address = rules[i].store(address);
  }
  return address;
}

void Schedule::fetch() {
  for (byte i = 0; i < NUMOFDAYS; i++) {
    rules[i].setDay(i);
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
