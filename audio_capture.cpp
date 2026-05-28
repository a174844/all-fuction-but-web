#include "audio_capture.h"
#include <driver/i2s.h>

void audio_capture_init() {
}

void audio_capture_read(int16_t *buffer, int len) {
    size_t bytesRead;
    i2s_read(I2S_NUM_0, buffer, len * sizeof(int16_t), &bytesRead, portMAX_DELAY);
}