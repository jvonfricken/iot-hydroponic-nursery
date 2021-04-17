#ifndef MAIN_H
#define MAIN_H

#include <MQTT.h>
#include <WiFiManager.h>

extern QueueHandle_t commandQueue;
extern QueueHandle_t postbackQueue;

extern MQTTClient mqttClient;

extern WiFiClient wifiClient;
extern WiFiManager wifiManager;

#endif