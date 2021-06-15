#include <math.h>
#include <stdio.h>
#include "SDL2/SDL.h"

int SCREEN_WIDTH = 600;
int SCREEN_HEIGHT = 800;
int FPS = 60;

const double TAO = M_PI * 2;

// common sample rates:
uint32_t sample_freq_common[] = { 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 96000, 192000 };
uint16_t sine_freq = 420;
float samples_per_sine;

void audio_callback(void* userdata, uint8_t* byte_stream, int byte_stream_length) {
	static long pos = 0;
	float* float_stream = (float*) byte_stream;
	uint32_t i;
	uint32_t float_stream_length = byte_stream_length >> 2;
	for (i = 0; i < float_stream_length; ++i) {
		float_stream[i] = sin((pos / samples_per_sine) * TAO) * 0.25;
		pos++;
	}
}

void log_spec(SDL_AudioSpec *as) {
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

int main(int argc, char *argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_AudioSpec want, have;
	SDL_AudioDeviceID audio_device;

	uint32_t sample_freq = sample_freq_common[0];

	SDL_memset(&want, 0, sizeof(want));
	want.freq = sample_freq;
	want.format = AUDIO_F32SYS;
	want.channels = 1;
	want.samples = 1024;
	want.callback = audio_callback;
	audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (audio_device == 0) {
		SDL_Log("Failed to open audio: %s\n", SDL_GetError());
	}
	printf(" want spec:\n");
	log_spec(&want);
	printf(" have spec:\n");
	log_spec(&have);
	sample_freq = have.freq;

	SDL_Delay(42);
	//SDL_LockAudioDevice(audio_device);
	SDL_PauseAudioDevice(audio_device, 0);

	SDL_Event event;
	int running = 1;

	while (running) {

		samples_per_sine = (float)sample_freq / (float)sine_freq;
	
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYDOWN:
					printf( ", Name: %s", SDL_GetKeyName( event.key.keysym.sym ) );
					printf(" keydown \n");
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = 0;
							break;
						case SDLK_UP:
							sine_freq++;
							break;
						case SDLK_DOWN:
							sine_freq--;
							break;
					}
					break;
			}
			SDL_Delay(1);
		}

	}

	//SDL_UnlockAudioDevice(audio_device);
	SDL_CloseAudio();
	SDL_Quit();

	return 0;

}
