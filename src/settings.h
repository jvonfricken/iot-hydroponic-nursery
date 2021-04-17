#ifndef SETTINGS_H
#define SETTINGS_H

typedef struct {
  char host[15];
  char client_id[15];
} t_Settings;

void setSettings(t_Settings settings);

t_Settings getSettings();

#endif