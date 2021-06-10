#include <math.h>
#include <stdio.h>
#include "SDL2/SDL.h"

int SCREEN_WIDTH = 600;
int SCREEN_HEIGHT = 800;
int FPS = 60;

/*

int rng(int max) { 
	static int seed = 1000;
	double base = fabs(sin(seed*=3));
	int whole = (int)base;
	printf("color red %f \n", base);
	return (int)((base - whole) * max);
}
*/

short xorshift16() {
	static unsigned short x = 1;
//	x |= x == 0;   // if x == 0, set x = 1 instead
	x ^= (x & 0x07ff) << 5;
	x ^= x >> 7;
	x ^= (x & 0x0003) << 14;
	return x & 0xffff;
}

int rng(int max) {
	unsigned short temp = xorshift16();
	/*
	printf("rng %d \n", max);
	printf("rng %d \n", temp);
	printf("rng %f \n", (float)temp / (float)0xffff);
	printf("rng %f \n", (float) (temp / (float)0xffff) * (float)max);
	*/
	return (int)( ( (float) temp / (float) 0xffff) * (float) max );
}

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
	
	printf("rx %d \n", display_bounds.x);
	printf("ry %d \n", display_bounds.y);
	printf("rw %d \n", display_bounds.w);
	printf("rh %d \n", display_bounds.h);
	printf("x %d \n", x);
	printf("y %d \n", y);

	window = SDL_CreateWindow("pixels 03", x, y, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, 0, 0);
//	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
//	SDL_SetWindowTitle(window, "pixels 02");

	SDL_Color colors[256];
	for (i = 0; i < 256; i++) {
		colors[i].r = rng(i);
		colors[i].g = rng(i);
		colors[i].b = rng(i);
		colors[i].a = 255;

	}
	//SDL_SetPalette(screen, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256);
	// a little debug
	//printf("pixels\n");

	int running = 1;
	while (running) {

		// random pixel
		for (j = 0; j < 666; j++) {
			x = (rng(SCREEN_WIDTH) + j) % SCREEN_WIDTH;
			y = (rng(SCREEN_HEIGHT) + j) % SCREEN_HEIGHT;
			rng(0);
			i = rng(256);
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
