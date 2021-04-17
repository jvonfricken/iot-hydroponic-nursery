#include <EEPROM.h>

#include "settings.h"

void setSettings(t_Settings settings) {
  EEPROM.put(0, settings);
  EEPROM.commit();
}

t_Settings getSettings() {
  Serial.println("Getting");
  t_Settings settings;
  EEPROM.get(0, settings);
  return settings;
};