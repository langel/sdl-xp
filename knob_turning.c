#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "char_roms/_lib.c"

int window_w = 1200;
int window_h = 800;

unsigned char bg_color[4];

float sin_pos2perc(float sin_pos) {
	return (sin(sin_pos) + 1.0f) * 0.5f;
}

void next_bg_color(unsigned char color[4]) {
	static float count = 0.f;
	count += 0.01f;
	color[0] = (int) ((float) 128 * sin_pos2perc(count));
	color[1] = (int) ((float) 128 * sin_pos2perc(count + 2));
	color[2] = (int) ((float) 128 * sin_pos2perc(count + 4));
	color[3] = 255;
}



int main(int argc, char* args[]) {

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);

	SDL_Event event;
	SDL_Window * window = SDL_CreateWindow("watttt", 200, 200,
		window_w, window_h, SDL_WINDOW_RESIZABLE);
	SDL_Renderer * renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_PRESENTVSYNC);

	char_rom font = char_rom_create_texture(renderer, "char_roms/eagle_pc_cga.bin");

	const char * knob_path = "assets/knob-small.png";
	SDL_Surface * knob_surface = IMG_Load(knob_path);
	if (!knob_surface) {
		printf("Failed to load image at %s: %s\n", knob_path, SDL_GetError());
		return 1;
	}
	SDL_Texture * knob_texture = SDL_CreateTextureFromSurface(renderer, knob_surface);
	SDL_SetTextureBlendMode(knob_texture, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(knob_surface);
//	SDL_Rect knob_rect = { 200, 200, 72, 72};
	SDL_Rect knob_rect = { 200, 200, 500, 500};
	float knob_rot = 0.f;
	float knob_speed = 0.777f;
	float knob_max_turn = 270.f;

	SDL_Texture * value;
	SDL_Rect value_rect = { 750, 250, 200, 20 };
	char value_string[10];
	
	int running = 1;
	while (running) {

		// background
		next_bg_color(bg_color);
		SDL_SetRenderDrawColor(renderer, bg_color[0], bg_color[1], bg_color[2], bg_color[3]);
		SDL_RenderClear(renderer);

		// knob
		SDL_RenderCopyEx(renderer, knob_texture, NULL, &knob_rect, knob_rot, NULL, SDL_FLIP_NONE);

		// value
		sprintf(value_string, "% 8.3f", knob_rot);
		value = char_rom_get_texture_from_string(renderer, font, value_string);
		SDL_RenderCopy(renderer, value, NULL, &value_rect);

		// spit that shit to the screeeen
		SDL_RenderPresent(renderer);

		// next frame stuff i guess
		knob_rot += knob_speed;
		if (knob_rot > knob_max_turn * 0.5f || knob_rot < -(knob_max_turn * 0.5f)) {
			knob_speed = -knob_speed;
		}



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
