#include <stdio.h>
#include <dirent.h>
#include <math.h>
#include <string.h>
#include "SDL2/SDL.h"


#define pixel_mul 3;
int char_w = 32;
int char_h = 16;
int texture_w;
int texture_h;
int window_w;
int window_h;



char *char_buffer[2];


void load_char_buffer(char* filename, int char_set) {
	char path[128] = "char_roms/";
	strcat(path, filename);
	printf("loading %s into char_set %d\n", path, char_set);
	FILE *file = fopen(path, "rb");
	fseek(file, 0, SEEK_END);
	uint16_t file_length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char_buffer[char_set] = malloc(file_length);
	fread(char_buffer[char_set], file_length, 1, file);
	printf("%d bytes - space for %d characters\n", file_length, file_length / 8);
	fclose(file);
}


int xy2char(int char_set, int char_index, int x, int y) {
	char buffer_byte = char_buffer[char_set][char_index * 8 + y];
	return (1 << x) & buffer_byte;
	return ((buffer_byte >> x) & 1);
}


int main(int argc, char *argv[]) {
	load_char_buffer("petscii_8032.bin", 0);
	load_char_buffer("petscii_c64.bin", 1);

	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO);

	texture_w = 8 * char_w;
	texture_h = 8 * char_h;
	window_w = texture_w * pixel_mul;
	window_h = texture_h * pixel_mul;

	int display_last_id = SDL_GetNumVideoDisplays();
	printf("display count : %d \n", display_last_id);
	display_last_id--;
	SDL_Rect display_bounds;
	SDL_GetDisplayUsableBounds(display_last_id, &display_bounds);
	if (SDL_GetDisplayBounds(display_last_id, &display_bounds) != 0) {
	    SDL_Log("SDL_GetDisplayBounds failed: %s", SDL_GetError());
	}
	int x = display_bounds.x + (display_bounds.w - window_w) / 2;
	int y = display_bounds.y + (display_bounds.h - window_h) / 2;
	
	SDL_Window * window = SDL_CreateWindow("sdl-xp", x, y,
		window_w, window_h,
		SDL_WINDOW_RESIZABLE);
	SDL_Renderer * renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_PRESENTVSYNC);
			
	SDL_Texture * canvas_texture = SDL_CreateTexture(renderer,
				SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
				texture_w, texture_h);

	printf("texture resolution: %d x %d\n", texture_w, texture_h);
	printf("window resolution: %d x %d\n", window_w, window_h);

	SDL_RenderSetIntegerScale(renderer, 1);

// XXX that should be * 4 at the end but it seg faults
	unsigned int * pixels = malloc(texture_w * texture_h * 8);

	int running = 1;
	while (running) {
		int char_count = char_w * char_h;
		for (int i = 0; i < char_count; i++) {
//		printf("%d\n",i);
			for (int x = 0; x < char_w; x++) {
				int x_p_pos = ((i * 8) % texture_w) + x;
				for (int y = 0; y < char_h; y++) {
					int y_p_pos = (y + ((i * 8) / texture_w) * 8) * texture_w;
					pixels[x_p_pos + y_p_pos] = xy2char(0, i, x, y) ? (255 << 16) + 255 : 0; 
				}
			}
		}

		// It's a good idea to clear the screen every frame,
		// as artifacts may occur if the window overlaps with
		// other windows or transparent overlays.
		SDL_RenderClear(renderer);
		SDL_UpdateTexture(canvas_texture, NULL, pixels, texture_w * 4);
		SDL_RenderCopy(renderer, canvas_texture, NULL, NULL);
		SDL_RenderPresent(renderer);

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
					}
					break;
			}
		}
	}

	SDL_Quit();
	return 0;
}
