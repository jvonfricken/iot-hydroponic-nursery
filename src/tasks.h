#ifndef TASKS_H
#define TASKS_H

void clearWiFiSettings(void *pvParameters);
void monitorWIFIHealth(void *pvParameters);

void handleCommands(void *pvParameters);
void handlePostRequestPostbacks(void *pvParameters);
void monitorMQTTHealth(void *pvParameters);

#endif