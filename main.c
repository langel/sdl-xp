#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "lib/core.c"


int texture_w = 420;
int texture_h = 200;
int window_w = 1280;
int window_h = 720;

unsigned long time_counter = 0;

int keys_pressed[256];



SDL_Color palette[8] = {
	{ 0xf0, 0xf0, 0xdc, 0xff }, // white
	{ 0xfa, 0xc8, 0x00, 0xff }, // yellow
	{ 0x10, 0xc8, 0x40, 0xff }, // green
	{ 0x00, 0xa0, 0xc8, 0xff }, // blue
	{ 0xd2, 0x40, 0x40, 0xff }, // red
	{ 0xa0, 0x69, 0x4b, 0xff }, // brown
	{ 0x73, 0x64, 0x64, 0xff }, // grey
	{ 0x10, 0x18, 0x20, 0xff }, // black
};


int main(int argc, char* args[]) {

	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS);

	SDL_Event event;
	SDL_Rect window_rect = { 200, 200, window_w, window_h };
	SDL_Window * window = SDL_CreateWindow("lerp trial", window_rect.x, window_rect.y, window_rect.w, window_rect.h, SDL_WINDOW_RESIZABLE);
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

	int surface_pixel_count = texture_w * texture_h;
	int surface_size = texture_w * texture_h * 4;
	int surface_width = texture_w * 4;
	uint32_t * surface_pixels = malloc(surface_size);
	SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, texture_w, texture_h);

	SDL_SetRenderTarget(renderer, NULL);
	renderer_set_color(renderer, &palette[7]);
	SDL_RenderClear(renderer);

	float points[32] = { 0.f };
	for (int i = 0; i < 32; i++) {
		points[i] = squirrel3_zero_float(i + 1337, 1337);
	}
	int pixels_per_point = 16;

	int running = 1;
	while (running) {

		// clear surface
		for (uint32_t i = 0; i < surface_pixel_count; i++) {
			surface_pixels[i] = 0;
		}

		for (int x = 0; x < texture_w; x++) {
			float pos = (float) x / (float) pixels_per_point;
			int pos_int = (int) pos;
			float pos_dec = pos - pos_int;

			int y = 0;
			y = (int) ((float) texture_h * lerp((float) points[pos_int], (float) points[pos_int + 1], pos_dec));
			if (y < 0) y = 0;
			if (y >= texture_h) y = texture_h - 1;
			surface_pixels[x + texture_w * y] = 0xff8800ff;
			float cuboints[4];
			cuboints[0] = (pos_int == 0) ? 0.f : points[pos_int - 1];
			cuboints[1] = points[pos_int + 0];
			cuboints[2] = points[pos_int + 1];
			cuboints[3] = points[pos_int + 2];
			y = (int) ((float) texture_h * cuberp(cuboints, pos_dec));
			if (y < 0) y = 0;
			if (y >= texture_h) y = texture_h - 1;
			surface_pixels[x + texture_w * y] = 0x0088ffff;
		}

		SDL_UpdateTexture(texture, NULL, surface_pixels, surface_width);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYDOWN:
					if (keys_pressed[event.key.keysym.scancode] == 0) {
						keys_pressed[event.key.keysym.scancode] = 1;
		//				printf( "keydown: %8s %3d\n", SDL_GetKeyName(event.key.keysym.sym), event.key.keysym.scancode);
					}
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = 0;
							break;
					}
					break;
				case SDL_KEYUP:
					keys_pressed[event.key.keysym.scancode] = 0;
		//			printf( "  keyup: %8s %3d\n", SDL_GetKeyName(event.key.keysym.sym), event.key.keysym.scancode);
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_MOVED) {
						window_rect.x = event.window.data1;
						window_rect.y = event.window.data2;
						printf("window position changed: %d x %d\n", window_rect.x, window_rect.y);
					}
					if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						window_w = event.window.data1;
						window_h = event.window.data2;
						window_rect.w = window_w;
						window_rect.h = window_h;
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
