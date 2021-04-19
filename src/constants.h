#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

const int PUMP_1_PIN = 15;
const int PUMP_2_PIN = 13;

const int LIGHT_PIN = 12;

const int RESET_PIN = 0;

enum MqttCommandType { TriggerLight, FloodTable };

typedef struct MqttCommand {
  MqttCommandType commandType;
  const char *payload;
} t_MqttCommand;

#endif