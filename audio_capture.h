#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H
#include <stdint.h>
void audio_capture_init();
void audio_capture_read(int16_t *buffer, int len);
#endif