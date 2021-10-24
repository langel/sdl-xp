#include <stdio.h>
#include <SDL2/SDL.h>

#include "char_roms/_lib.c"

#define ent_count 1024

int window_w = 1200;
int window_h = 800;

struct char_ent {
	int x_dir;
	int x_pos;
	int y_dir;
	int y_pos;
	int speed;
};

struct char_ent entities[ent_count];

void entity_update(struct char_ent * ent) {
	if (ent->x_dir) {
		ent->x_pos += ent->speed;
		if (ent->x_pos >= window_w) {
			ent->x_dir = 0;
		}
	}
	else {
		ent->x_pos -= ent->speed;
		if (ent->x_pos <= 0) {
			ent->x_dir = 1;
		}
	}
	if (ent->y_dir) {
		ent->y_pos += ent->speed;
		if (ent->y_pos >= window_h) {
			ent->y_dir = 0;
		}
	}
	else {
		ent->y_pos -= ent->speed;
		if (ent->y_pos <= 0) {
			ent->y_dir = 1;
		}
	}
}

int main(int argc, char * argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Event event;
	SDL_Window * window = SDL_CreateWindow("watttt", 200, 200,
		window_w, window_h, SDL_WINDOW_RESIZABLE);
	SDL_Renderer * renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_PRESENTVSYNC);

	struct char_rom font = char_rom_create_texture(renderer, "char_roms/eagle_pc_cga.bin");
	SDL_Rect font_get = { 0, 0, window_w, 8 };
	SDL_Rect temp_set = { 100, 100, 16, 16 };

	for (int i = 0; i < ent_count; i++) {
		entities[i].x_dir = i % 2;
		entities[i].x_pos = i * 4;
		entities[i].y_dir = (i % 3) - 1;
		entities[i].y_pos = i * 2;
		entities[i].speed = (i % 5) + 1;
	}

	int running = 1;
	while (running) {

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderClear(renderer);
		for (int i = 0; i < ent_count; i++) {
			entity_update(&entities[i]);
			font_get = char_rom_get_rect(font, i % 256);
			temp_set.x = entities[i].x_pos;
			temp_set.y = entities[i].y_pos;
			SDL_RenderCopy(renderer, font.texture, &font_get, &temp_set);
		}
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
