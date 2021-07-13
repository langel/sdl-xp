#include <math.h>
#include <stdio.h>
#include "SDL2/SDL.h"
#include "lib/rng-lfsr.h"

int SCREEN_WIDTH = 600;
int SCREEN_HEIGHT = 800;
int FPS = 60;

const double TAO = M_PI * 2;

// common sample rates:
uint32_t sample_freq_common[] = { 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 96000, 192000 };
float sine_freq = 420.0;
float samples_per_sine;
float vol_left, vol_right;
float duty_pos = 0.0;
float duty_rate = 0.0;

void audio_callback(void* userdata, uint8_t* byte_stream, int byte_stream_length) {
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

	uint32_t sample_freq = sample_freq_common[6];

	SDL_memset(&want, 0, sizeof(want));
	want.freq = sample_freq;
	want.format = AUDIO_F32SYS;
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
	sample_freq = have.freq;

	SDL_Delay(42);
	//SDL_LockAudioDevice(audio_device);
	SDL_PauseAudioDevice(audio_device, 0);

	SDL_Event event;
	SDL_Renderer *renderer;
	SDL_Window *window;

	SDL_Rect window_rect;
	window_rect.x = window_rect.y = 0;
	window_rect.w = SCREEN_WIDTH;
	window_rect.h = SCREEN_HEIGHT;

	int i, j, x, y;
	int x_min, x_max, x_dir, x_pos, y_pos;

	int display_last_id = SDL_GetNumVideoDisplays();
	printf("displays %d \n", display_last_id);
	display_last_id--;
	SDL_Rect display_bounds;
	SDL_GetDisplayUsableBounds(display_last_id, &display_bounds);
	/*
	if (SDL_GetDisplayBounds(display_last_id, &display_bounds) != 0) {
	    SDL_Log("SDL_GetDisplayBounds failed: %s", SDL_GetError());
	}
	*/
	x = display_bounds.x + (display_bounds.w - SCREEN_WIDTH) / 2;
	y = display_bounds.y + (display_bounds.h - SCREEN_HEIGHT) / 2;

	x_min = display_bounds.x;
	x_max = display_bounds.x + display_bounds.w - SCREEN_WIDTH - 1;
	x_dir = 2;
	x_pos = x;
	y_pos = y;

	printf("min x %d\n", x_min);
	printf("max x %d\n", x_max);

	window = SDL_CreateWindow("sine420panning", x, y, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, 0, 0);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


	SDL_Color colors[256];
	for (i = 0; i < 256; i++) {
		colors[i].r = rng16(i);
		colors[i].g = rng16(i);
		colors[i].b = rng16(i);
		colors[i].a = 255;

	}


	int running = 1;
	while (running) {

		SDL_SetWindowPosition(window, x_pos, y_pos);
		SDL_SetRenderDrawColor(renderer, 8, 8, 8, 24);
		SDL_RenderFillRect(renderer, &window_rect);

		samples_per_sine = (float)sample_freq / sine_freq;
		duty_rate = 1 / samples_per_sine;
		vol_left = (float)(x_pos - x_min) / (float)(x_max - x_min);
		vol_right = 1 - vol_left;

		// draw sine
		SDL_SetRenderDrawColor(renderer, 8, 111, 8, 200);
		for (j = 0; j < SCREEN_WIDTH; j++) {
			x = j + x_pos;
			y = (int)(SCREEN_HEIGHT >> 1) + (sin(x / samples_per_sine) * (SCREEN_HEIGHT >> 2));
			SDL_RenderDrawPoint(renderer, j, y);
			SDL_RenderDrawPoint(renderer, j, y + 1);
			SDL_RenderDrawPoint(renderer, j, y + 2);
		}

		// random pixels
		for (j = 0; j < 666; j++) {
			x = (rng8(SCREEN_WIDTH) + j) % SCREEN_WIDTH;
			y = (rng8(SCREEN_HEIGHT) + j) % SCREEN_HEIGHT;
			rng16(0);
			i = rng16(256);
			SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, 255);
			SDL_RenderDrawPoint(renderer, x, y);
		}

		// blit that shit
		SDL_RenderPresent(renderer);

		x_pos += x_dir;
		if (x_pos >= x_max) x_dir = -x_dir;
		if (x_pos <= x_min) x_dir = -x_dir;
	
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYDOWN:
					printf( "keydown: %8s ", SDL_GetKeyName( event.key.keysym.sym ) );
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = 0;
							break;
						case SDLK_UP:
							sine_freq *= 1.01;
							break;
						case SDLK_DOWN:
							sine_freq *= 0.99;
							break;
					}
					printf(" ::       pitch: %f\n", sine_freq);
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
