#include <Arduino.h>
#include <ArduinoJson.h>
#include <constants.h>
#include <main.h>
#include <mqtt_handlers.h>
#include <settings.h>

void sendPostback(String uuid) {
  xQueueSend(postbackQueue, &uuid, portMAX_DELAY);
}

void handleTriggerLightCommand(t_MqttCommand *command) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, command->payload);

  if (doc["direction"] == "activate") {
    Serial.println("Activating light");
    digitalWrite(LIGHT_PIN, HIGH);
  }

  if (doc["direction"] == "deactivate") {
    Serial.println("Deactivating light");
    digitalWrite(LIGHT_PIN, LOW);
  }

  sendPostback(doc["postback_uuid"]);
}

void handleFloodTableTask(void *pvParameters) {
  DynamicJsonDocument doc(1024);
  t_MqttCommand *command = (t_MqttCommand *)pvParameters;

  deserializeJson(doc, command->payload);

  if (doc["table"] == 1) {
    Serial.println("beginning to flood table 1");
    digitalWrite(PUMP_1_PIN, HIGH);
    vTaskDelay(doc["duration"]);
    digitalWrite(PUMP_1_PIN, LOW);
    Serial.println("ending flooding table 2");
  }

  if (doc["table"] == 2) {
    Serial.println("beginning to flood table 2");
    digitalWrite(PUMP_2_PIN, HIGH);
    vTaskDelay(doc["duration"]);
    digitalWrite(PUMP_2_PIN, LOW);
    Serial.println("ending flooding table 2");
  }

  sendPostback(doc["postback_uuid"]);
}

void handleCommands(void *pvParameters) {
  TaskHandle_t xFloodTableTaskHandle = NULL;

  for (;;) {
    t_MqttCommand command;
    xQueueReceive(commandQueue, &command, portMAX_DELAY);

    switch (command.commandType) {
      case TriggerLight:
        handleTriggerLightCommand(&command);
      case FloodTable:
        if (xFloodTableTaskHandle == NULL) {
          Serial.println("flooding table");
          xTaskCreate(handleFloodTableTask, "Flood Table Task", 2056, &command,
                      1, &xFloodTableTaskHandle);
        } else {
          Serial.println("blocking flooding because task is still running");
        }
    }
  }
}

void handlePostRequestPostbacks(void *pvParameters) {
  for (;;) {
    String postbackUUID;
    xQueueReceive(postbackQueue, &postbackUUID, portMAX_DELAY);

    handlePostback(postbackUUID);
  }
}

void clearWiFiSettings(void *pvParameters) {
  // Loop for 5 seconds before resetting to validate the user intent
  for (int i = 0; i < 5; i++) {
    if (digitalRead(RESET_PIN) == LOW) {
      vTaskDelete(NULL);
      return;
    }
    vTaskDelay(1000);
  }

  Serial.println("Resetting WiFi");
  wifiManager.resetSettings();
  ESP.restart();
  vTaskDelete(NULL);
}

// Workaround because it seems like the ESP32 firmware doesn't like to
// autoreconnect if it's booted off the network
void monitorWIFIHealth(void *pvParameters) {
  for (;;) {
    vTaskDelay(10);
    if (WiFi.status() != WL_CONNECTED) {
      bool res;
      res = wifiManager.autoConnect("Nursery Table Setup",
                                    "nursery-table");  // password protected ap

      if (!res) {
        Serial.println("Failed to connect");
      } else {
        Serial.println("WiFi connected");
      }
    }

    vTaskDelay(1000);
  }
}

void monitorMQTTHealth(void *pvParameters) {
  t_Settings *settings = (t_Settings *)pvParameters;

  for (;;) {
    mqttClient.loop();

    if (!mqttClient.connected()) {
      Serial.println("connecting to MQTT...");
      while (!mqttClient.connect(settings->client_id, false)) {
        Serial.print(".");
        vTaskDelay(1000);
      }

      Serial.println("MQTT connected");

      mqttClient.subscribe(FLOOD_TABLE_TOPIC);
      mqttClient.subscribe(ACTIVATE_LIGHT_TOPIC);
    }
  }
}
