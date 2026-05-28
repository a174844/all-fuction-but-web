#ifndef SOUND_EVENT_DETECT_H
#define SOUND_EVENT_DETECT_H

#include <stdint.h>
#include "config.h"

void sound_event_detect_init();
SoundEvent sound_event_detect_run(int16_t *audio, int len);

#endif