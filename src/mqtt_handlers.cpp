#include <Arduino.h>
#include <MQTT.h>
#include <constants.h>
#include <main.h>
#include <mqtt_handlers.h>

const String CENTRAL_POSTBACK_TOPIC = "/central/request-postback";

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  t_MqttCommand command;

  if (topic.compareTo(FLOOD_TABLE_TOPIC)) {
    command.commandType = FloodTable;
  }

  if (topic.compareTo(ACTIVATE_LIGHT_TOPIC)) {
    command.commandType = TriggerLight;
  }
  command.payload = payload;

  xQueueSend(commandQueue, &command, 0);
}

void handlePostback(String requestUUID) {
  Serial.println("sending postback message");
  mqttClient.publish(CENTRAL_POSTBACK_TOPIC, requestUUID);
}