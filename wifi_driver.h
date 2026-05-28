#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

void wifi_driver_init();

void mqtt_loop();

void mqtt_send_alarm(const char* event_type, int level);

#endif