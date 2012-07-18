#include <EEPROM.h>
#include "WaterRule.h"

WaterRule::WaterRule(Hour h, Minute m, Duration d) {
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
