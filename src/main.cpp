#include <EEPROM.h>
#include <WiFiManager.h>
#include <constants.h>
#include <main.h>
#include <mqtt_handlers.h>
#include <settings.h>
#include <tasks.h>

WiFiManagerParameter custom_mqtt_server_client_id("serverclientid",
                                                  "MQTT Client Id",
                                                  "servername", 40);

WiFiManagerParameter custom_mqtt_server_host("serverhost", "MQTT Server Host",
                                             "host", 40);

QueueHandle_t commandQueue;
QueueHandle_t postbackQueue;

MQTTClient mqttClient;

WiFiClient wifiClient;
WiFiManager wifiManager;

t_Settings settings;

void saveConfigCallback() {
  t_Settings settings;

  strcpy(settings.client_id, custom_mqtt_server_client_id.getValue());
  strcpy(settings.host, custom_mqtt_server_host.getValue());

  setSettings(settings);
}

void setupTasks() {
  commandQueue = xQueueCreate(1, sizeof(t_MqttCommand));
  postbackQueue = xQueueCreate(1, sizeof(const char *));

  xTaskCreate(handleCommands, "Handle Commands", 4112, NULL, 1, NULL);
  xTaskCreate(handlePostRequestPostbacks, "Handle Request Postbacks", 2056,
              NULL, 1, NULL);
  xTaskCreate(monitorWIFIHealth, " Monitor WiFi Health", 2056, NULL, 1, NULL);

  xTaskCreate(monitorMQTTHealth, " Monitor MQTT Health", 2056,
              (void *)&settings, 1, NULL);
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);

  wifiManager.addParameter(&custom_mqtt_server_host);
  wifiManager.addParameter(&custom_mqtt_server_client_id);

  wifiManager.setSaveConfigCallback(saveConfigCallback);

  bool res;
  res = wifiManager.autoConnect("Nursery Table Setup",
                                "nursery-table");  // password protected ap

  if (!res) {
    Serial.println("Failed to connect");
  } else {
    Serial.println("WiFi connected");
  }
}

void IRAM_ATTR handleResetButtonPress() {
  xTaskCreate(clearWiFiSettings, "Clear WiFi Settings", 2056, NULL, 1, NULL);
}

void setupMQTT() {
  mqttClient.begin(settings.host, wifiClient);
  mqttClient.onMessage(messageReceived);
}

void setup() {
  EEPROM.begin(256);
  Serial.begin(115200);

  // setup interupts
  pinMode(RESET_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RESET_PIN), handleResetButtonPress,
                  RISING);

  // setup WiFi
  setupWiFi();

  settings = getSettings();

  Serial.print("Client Id: ");
  Serial.println(settings.client_id);

  Serial.print("Host: ");
  Serial.println(settings.host);

  // setup mqtt
  setupMQTT();

  setupTasks();
}

void loop() {}