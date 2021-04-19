#include <Arduino.h>
#include <MQTT.h>
#include <constants.h>
#include <main.h>
#include <mqtt_handlers.h>

const String CENTRAL_POSTBACK_TOPIC = "central/request-postback";

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  t_MqttCommand command;

  if (topic.equals(FLOOD_TABLE_TOPIC)) {
    Serial.println("setting command type to flood table");
    command.commandType = FloodTable;
  }

  if (topic.equals(ACTIVATE_LIGHT_TOPIC)) {
    Serial.println("setting command type to trigger light");
    command.commandType = TriggerLight;
  }
  command.payload = payload.c_str();

  xQueueSend(commandQueue, &command, 0);
}

void handlePostback(const char *requestUUID) {
  mqttClient.publish(CENTRAL_POSTBACK_TOPIC, requestUUID);
}