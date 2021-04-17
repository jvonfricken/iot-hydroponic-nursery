#ifndef MQTTHANDLERS_H
#define MQTTHANDLERS_H

#include <Arduino.h>

const String FLOOD_TABLE_TOPIC = "/nursery-1/flood-table";
const String ACTIVATE_LIGHT_TOPIC = "/nursery-1/trigger-light";

void messageReceived(String &topic, String &payload);
void handlePostback(String requestUUID);

#endif