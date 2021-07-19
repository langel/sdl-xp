#include <math.h>
#include <stdio.h>
#include "SDL2/SDL.h"
#include "lib/rng-lfsr.h"
#include "fcl/fcl.h"

int FPS = 48;

int CANVAS_WIDTH = 420;
int CANVAS_HEIGHT = 200;
int WINDOW_WIDTH = 860;
int WINDOW_HEIGHT = 540;


int main(int argc, char *argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Event event;
	SDL_Renderer *renderer;
	SDL_Window *window;
	SDL_Texture *texture;

	SDL_Rect window_rect;
	window_rect.x = window_rect.y = 0;
	window_rect.w = WINDOW_WIDTH;
	window_rect.h = WINDOW_HEIGHT;

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
	x = display_bounds.x + (display_bounds.w - WINDOW_WIDTH) / 2;
	y = display_bounds.y + (display_bounds.h - WINDOW_HEIGHT) / 2;

	x_min = display_bounds.x;
	x_max = display_bounds.x + display_bounds.w - WINDOW_WIDTH - 1;
	x_dir = 2;
	x_pos = x;
	y_pos = y;

	printf("min x %d\n", x_min);
	printf("max x %d\n", x_max);

	window = SDL_CreateWindow("sine420panning", x, y, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, 0, 0);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	fcl_audio_init();
	fcl_audio_set_sine_freq(sine_freq);

	fcl_time_set_fps(FPS);
	fcl_video_set_dimensions(CANVAS_WIDTH, CANVAS_HEIGHT);
	for (i = 0; i < 255; i++) {
		fcl_video_set_color_index(i, rng16(i), rng16(i), rng16(i));
	}
	fcl_video_set_color_index(255, 8, 111, 8);

	int running = 1;
	while (running) {

		SDL_SetWindowPosition(window, x_pos, y_pos);
		SDL_SetRenderDrawColor(renderer, 8, 8, 8, 24);
		SDL_RenderFillRect(renderer, &window_rect);

		// draw sine
		SDL_SetRenderDrawColor(renderer, 8, 111, 8, 200);
		for (j = 0; j < CANVAS_WIDTH; j++) {
			x = j + x_pos;
			y = (int)(CANVAS_HEIGHT >> 1) + (int)(sin(x / samples_per_sine) * (CANVAS_HEIGHT >> 2));
			fcl_video_set_pixel(j, y, 255);
			/*
			SDL_RenderDrawPoint(renderer, j, y);
			SDL_RenderDrawPoint(renderer, j, y + 1);
			SDL_RenderDrawPoint(renderer, j, y + 2);
			*/
			
		}

		// random pixels
		for (j = 0; j < 666; j++) {
			x = (rng8(WINDOW_WIDTH) + j) % WINDOW_WIDTH;
			y = (rng8(WINDOW_HEIGHT) + j) % WINDOW_HEIGHT;
			rng16(0);
			i = rng16(256);
			SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, 255);
			SDL_RenderDrawPoint(renderer, x, y);
		}

		// pan audio
		fcl_audio_set_sine_pan((float)(x_pos - x_min) / (float)(x_max - x_min));

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
					fcl_audio_set_sine_freq(sine_freq);
					break;
			}
		}

		fcl_time_wait_next_frame();

	}

	fcl_audio_shutdown();
	SDL_Quit();

	return 0;

}
