
#include "pico/stdlib.h"
#include "feeder.h"
#include "pico/audio_i2s.h"

#include <stdio.h>
#include <math.h>

struct audio_buffer * fill_next_buffer(spawner* o, struct audio_buffer_pool *ap, int num_samples) 
{
	struct audio_buffer *buffer = take_audio_buffer(ap, true);
	int16_t *samples = (int16_t *) buffer->buffer->bytes;
	
	
	for (uint i = 0; i < num_samples; i++) 
	{
		spawn(o);
		samples[i] = roundf(o->feed * 8192.0f);
	}
	
	buffer->sample_count = num_samples;
	return buffer;
}


struct audio_buffer_pool *init_audio_i2s(int buffer_size, int data_pin, int clock_pin) {

    static audio_format_t audio_format = 
	{
            .format = AUDIO_BUFFER_FORMAT_PCM_S16,
            .sample_freq = SAMPLE_RATE,
            .channel_count = 1,
    };
	

    static struct audio_buffer_format producer_format = {
            .format = &audio_format,
            .sample_stride = 2
    };

    struct audio_buffer_pool *producer_pool = audio_new_producer_pool(&producer_format, 3, buffer_size);
    bool __unused ok;
    const struct audio_format *output_format;

	struct audio_i2s_config config = {
			.data_pin = data_pin,
			.clock_pin_base = clock_pin,
			.dma_channel = 0,
			.pio_sm = 0,
	};

	output_format = audio_i2s_setup(&audio_format, &config);
	if (!output_format) 
	{
		panic("PicoAudio: Unable to open audio device.\n");
	}

	ok = audio_i2s_connect(producer_pool);
	assert(ok);
	audio_i2s_set_enabled(true);

    return producer_pool;
}

