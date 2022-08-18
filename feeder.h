#include "pico/audio_i2s.h"
#include "cell/spawner.h"
struct audio_buffer * fill_next_buffer(spawner* o, struct audio_buffer_pool *ap, int num_samples);
struct audio_buffer_pool *init_audio_i2s(int buffer_size, int data_pin, int clock_pin);