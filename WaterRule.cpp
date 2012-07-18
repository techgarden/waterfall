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
  EEPROM.write(address++, hour);
  EEPROM.write(address++, minute);
  EEPROM.write(address++, duration);
  EEPROM.write(address, enabled);
  return address;
}

void WaterRule::fetch(unsigned int address) {
  hour = EEPROM.read(address++);
  minute = EEPROM.read(address++);
  duration = EEPROM.read(address++);
  enabled = EEPROM.read(address);
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

String WaterRule::toString() {
  String str = "";
  str += String("set at ") + hour + String(":") + minute + String(" for ") + duration + String(" minutes");
  return str;
}

byte WaterRule::isEnabled() {
  return enabled;
}

void WaterRule::setEnabled(byte enabled) {
  this->enabled = enabled;
}
