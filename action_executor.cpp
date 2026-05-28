#include "action_executor.h"
#include "screen_display.h"
#include "wifi_driver.h"

#include <Arduino.h>

void action_executor_init() {

    pinMode(LED_PIN, OUTPUT);
}

void action_executor_run(SoundEvent event) {

    switch (event) {

        case EVENT_GLASS_BREAK:

            screen_display_warning("Glass Break");

            digitalWrite(LED_PIN, HIGH);

            mqtt_send_alarm("glass_break", 3);

            break;

        case EVENT_BABY_CRY:

            screen_display_warning("Baby Cry");

            mqtt_send_alarm("baby_cry", 2);

            break;

        case EVENT_KNOCK:

            screen_display_warning("Knock Knock");

            mqtt_send_alarm("knock", 1);

            break;

        default:

            return;
    }

    delay(3000);

    digitalWrite(LED_PIN, LOW);

    screen_display_normal();
}