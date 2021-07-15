#include "SDL2/SDL.h"

const double TAO = M_PI * 2;

// common sample rates:
uint32_t sample_freq_common[] = { 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 96000, 192000 };
uint32_t sample_freq;
float sine_freq = 420.0;
float samples_per_sine;
float vol_left, vol_right;
float duty_pos = 0.0;
float duty_rate = 0.0;

void fcl_audio_process(void* userdata, uint8_t* byte_stream, int byte_stream_length) {
	float* float_stream = (float*) byte_stream;
	uint32_t i;
	uint32_t float_stream_length = byte_stream_length >> 2;
	for (i = 0; i < float_stream_length; i+=2) {
		float_stream[i+1] = sin(duty_pos * TAO) * 0.5 * vol_left;
		float_stream[i] = sin(duty_pos * TAO) * 0.5 * vol_right;
		duty_pos += duty_rate;
		if (duty_pos > 1) duty_pos--;
	}
}

void fcl_audio_set_sine_freq(float freq) {
	samples_per_sine = (float)sample_freq / sine_freq;
	duty_rate = 1 / samples_per_sine;
	printf(" samples per sine: %7f\n", samples_per_sine);
}

void fcl_audio_set_sine_pan(float pan) {
	vol_left = pan;
	vol_right = 1 - vol_left;
}

void fcl_audio_log_spec(SDL_AudioSpec *as) {
	printf(
		" freq______%5d\n"
		" format____%5d\n"
		" channels__%5d\n"
		" silence___%5d\n"
		" samples___%5d\n"
		" size______%5d\n\n",
		(int) as->freq,
		(int) as->format,
		(int) as->channels,
		(int) as->silence,
		(int) as->samples,
		(int) as->size
	);
}

void fcl_audio_init() {

	SDL_AudioSpec want, have;
	SDL_AudioDeviceID audio_device;

	sample_freq = sample_freq_common[6];

	SDL_memset(&want, 0, sizeof(want));
	want.freq = sample_freq;
	want.format = AUDIO_F32SYS;
	want.channels = 2;
	want.samples = 1024;
	want.callback = fcl_audio_process;
	audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (audio_device == 0) {
		SDL_Log("Failed to open audio: %s\n", SDL_GetError());
	}
	printf(" want spec:\n");
	fcl_audio_log_spec(&want);
	printf(" have spec:\n");
	fcl_audio_log_spec(&have);
	sample_freq = have.freq;

	// XXX why is this here?
	SDL_Delay(42);
	SDL_PauseAudioDevice(audio_device, 0);
}

void fcl_audio_shutdown() {
	SDL_CloseAudio();
}
