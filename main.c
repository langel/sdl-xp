#include <math.h>
#include <stdio.h>
#include "SDL2/SDL.h"
#include "lib/rng-lfsr.h"

int SCREEN_WIDTH = 600;
int SCREEN_HEIGHT = 800;
int FPS = 60;


int main(int argc, char *argv[]) {

	SDL_Event event;
	SDL_Renderer *renderer;
	SDL_Window *window;
	int i, j, x, y;

	SDL_Init(SDL_INIT_VIDEO);

	int display_last_id = SDL_GetNumVideoDisplays();
	printf("displays %d \n", display_last_id);
	display_last_id--;
	SDL_Rect display_bounds;
	SDL_GetDisplayUsableBounds(display_last_id, &display_bounds);
	if (SDL_GetDisplayBounds(display_last_id, &display_bounds) != 0) {
	    SDL_Log("SDL_GetDisplayBounds failed: %s", SDL_GetError());
	}
	x = display_bounds.x + (display_bounds.w - SCREEN_WIDTH) / 2;
	y = display_bounds.y + (display_bounds.h - SCREEN_HEIGHT) / 2;
	
	window = SDL_CreateWindow("pixels 03", x, y, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, 0, 0);

	SDL_Color colors[256];
	for (i = 0; i < 256; i++) {
		colors[i].r = rng16(i);
		colors[i].g = rng16(i);
		colors[i].b = rng16(i);
		colors[i].a = 255;

	}

/*
	unsigned short rng_seed = 1;
	printf("rng_seed %d \n", rng_seed);
	int rng_no = rng_lfsr_16bit_next(&rng_seed, 50);
	printf("rng = %d \n", rng_no);
	printf("rng_seed %d \n", rng_seed);
*/

	int running = 1;
	while (running) {

		// random pixel
		for (j = 0; j < 666; j++) {
			x = (rng8(SCREEN_WIDTH) + j) % SCREEN_WIDTH;
			y = (rng8(SCREEN_HEIGHT) + j) % SCREEN_HEIGHT;
			rng16(0);
			i = rng16(256);
			SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
			SDL_RenderDrawPoint(renderer, x, y);
			//printf("color id %d \n", i);
	//		putpixel(screen, rng(SCREEN_WIDTH), rng(SCREEN_HEIGHT), rng(256));
		}
		SDL_RenderPresent(renderer);

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
				running = 0;
			}
		}
		SDL_Delay(1000 / FPS);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
