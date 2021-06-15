#include <math.h>
#include <stdio.h>
#include "SDL2/SDL.h"

int SCREEN_WIDTH = 600;
int SCREEN_HEIGHT = 800;
int FPS = 60;

float samples_per_sine;

void audio_callback(void* userdata, Uint8* buffer, int len) {
	static long pos = 0;
	for (int i = 0; i < len; ++i) {
		buffer[i] = sin(pos / samples_per_sine * M_PI * 2);
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

	SDL_Init(SDL_INIT_AUDIO);
	SDL_Event event;

	SDL_AudioSpec want, have;
	SDL_AudioDeviceID audio_device;

	SDL_memset(&want, 0, sizeof(want));
	want.freq = 48000;
	want.format = AUDIO_F32;
	want.channels = 2;
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

	SDL_Delay(42);
	SDL_PauseAudioDevice(audio_device, 0);
	samples_per_sine = (float)have.freq / 420.0;

	int running = 1;
	while (running) {
	
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
				running = 0;
			}
		}

	}

	return 0;

}
