#include "sound_event_detect.h"
#include <Arduino.h>

namespace {
    bool initialized = false;
    bool baby_mode = false;
    unsigned long baby_timer = 0;
}

void sound_event_detect_init() {
    initialized = true;
    Serial.println("Ready");
}

SoundEvent sound_event_detect_run(int16_t *audio, int len) {
    if (!initialized) return EVENT_NONE;

    int peak = 0;
    int high_count = 0;

    for (int i = 0; i < len; i++) {
        int a = abs(audio[i]);
        if (a > peak) peak = a;
        if (a > 3000) high_count++;
    }

    if (peak < 400) return EVENT_NONE;

    Serial.print("P:");
    Serial.print(peak);
    Serial.print(" H:");
    Serial.print(high_count);
    Serial.print(" -> ");

    // 一旦检测到婴儿特征，进入baby模式持续3秒
    if (peak > 5000 && high_count > 150) {
        baby_mode = true;
        baby_timer = millis() + 3000;
    }

    // baby模式期间，忽略其他声音
    if (baby_mode) {
        if (millis() < baby_timer) {
            Serial.println("BABY");
            return EVENT_BABY_CRY;
        } else {
            baby_mode = false;
        }
    }

    // 玻璃碎裂：P>=2000
    if (peak >= 2000) {
        Serial.println("GLASS");
        return EVENT_GLASS_BREAK;
    }

    // 敲门：400 <= P < 2000 且 H=0
    if (high_count == 0) {
        Serial.println("KNOCK");
        return EVENT_KNOCK;
    }

    Serial.println("NONE");
    return EVENT_NONE;
}