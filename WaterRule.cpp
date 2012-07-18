#include <EEPROM.h>
#include "WaterRule.h"

WaterRule::WaterRule(byte h, byte m, byte d) {
  hour = h;
  minute = m;
  duration = d;
}

WaterRule::WaterRule() {
  hour = 0;
  minute = 0;
  duration = 0;
}

unsigned int WaterRule::store(unsigned int address) {
  EEPROM.write(address, hour);
  address++;
  EEPROM.write(address, minute);
  address++;
  EEPROM.write(address, duration);
  address++;
  return address;
}

void WaterRule::fetch(unsigned int address) {
  hour = EEPROM.read(address);
  address++;
  minute = EEPROM.read(address);
  address++;
  duration = EEPROM.read(address);
}

void WaterRule::set(byte hour, byte minute, byte duration) {
  this->hour = hour;
  this->minute = minute;
  this->duration = duration;
}

byte WaterRule::getHour() {
  return hour;
}

byte WaterRule::getMinute() {
  return minute;
}

byte WaterRule::getDuration() {
  return duration;
}
