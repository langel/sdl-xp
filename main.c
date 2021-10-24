#include <stdio.h>
#include <SDL2/SDL.h>

#include "char_roms/_lib.c"

int window_w = 1200;
int window_h = 800;


int main(int argc, char * argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Event event;
	SDL_Window * window = SDL_CreateWindow("watttt", 200, 200,
		window_w, window_h, SDL_WINDOW_RESIZABLE);
	SDL_Renderer * renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_PRESENTVSYNC);

	SDL_Texture * font = char_rom_create_texture(renderer, "char_roms/eagle_pc_cga.bin", 8);
	SDL_Rect font_get = { 0, 0, window_w, 8 };
	SDL_Rect font_set = { 0, 0, window_w, 8 };


	int running = 1;
	while (running) {

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, font, &font_get, &font_set);
		//SDL_RenderCopy(renderer, font, NULL, NULL);
		SDL_RenderPresent(renderer);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYDOWN:
					printf( "keydown: %8s\n", SDL_GetKeyName( event.key.keysym.sym ) );
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = 0;
							break;
					}
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						printf("window size changed\n");
						window_w = event.window.data1;
						window_h = event.window.data2;
						printf("window size changed: %d x %d\n", window_w, window_h);
					}
					break;
			}
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
