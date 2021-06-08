#include <math.h>
#include "../SDL2/SDL.h"

int SCREEN_WIDTH = 600;
int SCREEN_HEIGHT = 800;

int rng(max) { 
	static int seed = 1000;
	double base = sin(seed++);
	int whole = (int)base;
	return (int)(base - whole) * max;
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("pixels", 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, SDL_WINDOW_HIDDEN);
	SDL_ShowWindow(window);
	SDL_Event event;
	int running = 1;
	while (running) {
		int x = rng(SCREEN_WIDTH);
		int y = rng(SCREEN_HEIGHT);

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = 0;
			}
		}
		SDL_Delay(32);
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
