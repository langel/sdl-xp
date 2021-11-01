#include <stdio.h>
#include <SDL2/SDL.h>

#define NOTE_COUNT 25

int window_w = 800;
int window_h = 180;

int keys_pressed[256];
int notes_on[NOTE_COUNT];
int note_to_scancode[NOTE_COUNT] = {
	29, // C-0
	22, // C#0
	27, // D-0
	7,  // D#0
	6,  // E-0
	25, // F-0
	10, // F#0
	5,  // G-0
	11, // G#0
	17, // A-0
	13, // A#0
	16, // B-0
	20, // C-1
	31, // C#1
	26, // D-1
	32, // D#1
	8,  // E-1
	21, // F-1
	34, // F#1
	23, // G-1
	35, // G#1
	28, // A-1
	36, // A#1
	24, // B-1
	12, // C-2
};
char accidentals[7] = { 1, 1, 0, 1, 1, 1, 0 };
int white_keys_to_note[15] = { 0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24 };
int black_keys_to_note[14] = { 1, 3, -1, 6, 8, 10, -1, 13, 15, -1, 18, 20, 22, -1 };

SDL_Texture * keyboard_texture;

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


void set_color(SDL_Renderer * renderer, int color_id) {
	SDL_SetRenderDrawColor(renderer, palette[color_id].r, palette[color_id].g, palette[color_id].b, palette[color_id].a);
}

void notes_update() {
	for (int i = 0; i < NOTE_COUNT; i++) {
	}
}


int main(int argc, char* args[]) {

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Event event;
	SDL_Window * window = SDL_CreateWindow("keyboard", 200, 200,
		window_w, window_h, SDL_WINDOW_RESIZABLE);
	SDL_Renderer * renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_PRESENTVSYNC);

	keyboard_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 334, 104);
	SDL_Rect keys_white_rect = { 0, 0, 20, 100 };
	SDL_Rect keys_black_rect = { 0, 0, 10, 60 };
	
	int running = 1;
	while (running) {

		SDL_SetRenderTarget(renderer, keyboard_texture);
		set_color(renderer, 6);
		SDL_RenderClear(renderer);
		
		// which notes are on?
		for (int i = 0; i < NOTE_COUNT; i++) {
			if (keys_pressed[note_to_scancode[i]]) {
				notes_on[i] = 1;
			}
			else {
				notes_on[i] = 0;
			}
		}
		// white keys
		for (int i = 0; i < 15; i++) {
			if (notes_on[white_keys_to_note[i]]) {
				set_color(renderer, 2); 
			}
			else {
				set_color(renderer, 0);
			}
			keys_white_rect.x = i * 22 + 2;
			SDL_RenderFillRect(renderer, &keys_white_rect);
		}
		// black keys
		for (int i = 0; i < 14; i++) {
			if (accidentals[i % 7]) {
				if (notes_on[black_keys_to_note[i]]) {
					set_color(renderer, 2); 
				}
				else {
					set_color(renderer, 7);
				}
				keys_black_rect.x = i * 22 + 18;
				SDL_RenderFillRect(renderer, &keys_black_rect);
			}
		}


		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, keyboard_texture, NULL, NULL);
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
