#include <EEPROM.h>
#include "WaterRule.h"

static timeDayOfWeek_t getTimeDayOfWeek(int day) {
  switch(day) {
    case 0: return dowSunday;
    case 1: return dowMonday;
    case 2: return dowTuesday;
    case 3: return dowWednesday;
    case 4: return dowThursday;
    case 5: return dowFriday;
    case 6: return dowSaturday;  
    default: return dowInvalid;
  }
}

void start_watering() {
  digitalWrite(A4, HIGH);
}

void stop_watering() {
  digitalWrite(A4, LOW);
}

WaterRule::WaterRule() {
  day = 0;
  hour = 0;
  minute = 0;
  duration = 0;
  startAlarmId = dtINVALID_ALARM_ID;
  stopAlarmId = dtINVALID_ALARM_ID;
}

void WaterRule::setDay(byte day) {
  this->day = day;
}

unsigned int WaterRule::store(unsigned int address) {
  EEPROM.write(address++, hour);
  EEPROM.write(address++, minute);
  EEPROM.write(address++, duration);
  EEPROM.write(address++, enabled);
  return address;
}

void WaterRule::createAlarms() {
  if (enabled) {
    if (startAlarmId != dtINVALID_ALARM_ID) {
      Alarm.free(startAlarmId);
      startAlarmId = dtINVALID_ALARM_ID;
    }
    if (stopAlarmId != dtINVALID_ALARM_ID) {
      Alarm.free(stopAlarmId);
      stopAlarmId = dtINVALID_ALARM_ID;
    }
    startAlarmId = Alarm.alarmRepeat(getTimeDayOfWeek(day), hour, minute, 00, start_watering);
    if (startAlarmId == dtINVALID_ALARM_ID) {
      Serial.println("i was given INVALIDZ");
    }
    // TODO: handle minute + duration > 60
    stopAlarmId = Alarm.alarmRepeat(getTimeDayOfWeek(day), hour, minute + duration, 00, stop_watering);
    if (stopAlarmId == dtINVALID_ALARM_ID) {
      Serial.println("i was given INVALIDZ");
    }
  }
  else {
    if (startAlarmId != dtINVALID_ALARM_ID) {
      Alarm.free(startAlarmId);
      startAlarmId = dtINVALID_ALARM_ID;
    }
    if (stopAlarmId != dtINVALID_ALARM_ID) {
      Alarm.free(stopAlarmId);
      stopAlarmId = dtINVALID_ALARM_ID;
    }
  }
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
  enabled = true;
  createAlarms();
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

unsigned int WaterRule::toString(char* buf) {
  return sprintf(buf, "set at %d:%d, for %d minutes", hour, minute, duration);
}

byte WaterRule::isEnabled() {
  return enabled;
}

void WaterRule::setEnabled(byte enabled) {
  this->enabled = enabled;
  createAlarms();
}
