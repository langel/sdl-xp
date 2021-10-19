#include <stdio.h>
#include <dirent.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


#define APP_TITLE "TTF test"

#define window_mul 5;
#define texture_mul 3;
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
	return (1 << (7 - x)) & buffer_byte;
}


float sin_pos2perc(float sin_pos) {
	return (sin(sin_pos) + 1.0f) * 0.5f;
}


int main(int argc, char *argv[]) {
	load_char_buffer("petscii_8032.bin", 0);
	load_char_buffer("petscii_c64.bin", 1);

	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_JPG);
	TTF_Init();

	texture_w = 8 * char_w;
	texture_h = 8 * char_h;
	window_w = texture_w * window_mul;
	window_h = texture_h * window_mul;

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
	
	SDL_Window * window = SDL_CreateWindow(APP_TITLE, x, y,
		window_w, window_h,
		SDL_WINDOW_RESIZABLE);
	SDL_Renderer * renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_PRESENTVSYNC);

	// FONT STUFF
	TTF_Font * font = TTF_OpenFont("fonts/OpenSans-BoldItalic.ttf", 222);
	SDL_Color color = { 255, 255, 255, 222 };
	SDL_Surface * font_surface = TTF_RenderUTF8_Blended(font, APP_TITLE, color);
	SDL_Texture * font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
	SDL_FreeSurface(font_surface);
	int font_w, font_h;
	SDL_QueryTexture(font_texture, NULL, NULL, &font_w, &font_h);
	SDL_Rect font_rect = { 0, 0, font_w, font_h };

	// IMAGE STUFF
	SDL_Surface * image_surface = IMG_Load("assets/derp.gif");
	if (image_surface == NULL) {
		printf("derp didn't load\n");
		printf("IMG_Load: %s\n", IMG_GetError());
	}
	SDL_Texture * image_texture = SDL_CreateTextureFromSurface(renderer, image_surface);
	SDL_FreeSurface(image_surface);
	int image_w, image_h;
	SDL_QueryTexture(image_texture, NULL, NULL, &image_w, &image_h);
	SDL_Rect image_rect = { 0, 0, image_w, image_h };
			
	printf("texture resolution: %d x %d\n", texture_w, texture_h);
	printf("window resolution: %d x %d\n", window_w, window_h);

	SDL_RenderSetIntegerScale(renderer, 1);


	int running = 1;
	int char_count = char_w * char_h;
	printf("char count: %d\n", char_count);

	// create petscii textures
	int texture_pixel_space = texture_w * texture_h * 4;
	uint32_t * pixels_pet = malloc(texture_pixel_space);
	uint32_t * pixels_c64 = malloc(texture_pixel_space);
	int x_p_pos, y_p_pos;
	uint32_t color_red = (255 << 24) + (127 << 16) + 255;
	uint32_t color_blue = (255 << 8) + (127 << 16) + 255;
	uint32_t color_black = 255;
	for (int i = 0; i < char_count; i++) {
//		printf("%d\n",i);
		for (int x = 0; x < 8; x++) {
			x_p_pos = ((i * 8) % texture_w) + x;
			for (int y = 0; y < 8; y++) {
				y_p_pos = (y + ((i * 8) / texture_w) * 8) * texture_w;
//		printf("x %d %d \t\t y %d %d\n", x, x_p_pos, y, y_p_pos);
				int mem_pos = x_p_pos + y_p_pos;
				if (mem_pos > texture_pixel_space) {
					printf("overflow: x %d  y %d\n", x, y);
				}
				else {
					pixels_pet[mem_pos] = (xy2char(0, i, x, y)) ? color_red : color_black;
					pixels_c64[mem_pos] = (xy2char(1, i, x, y)) ? color_blue : color_black;
				}
			}
		}
	}
	
	SDL_Texture * petscii_pet_texture = SDL_CreateTexture(renderer,
				SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
				texture_w, texture_h);
	SDL_UpdateTexture(petscii_pet_texture, NULL, pixels_pet, texture_w * 4);
	free(pixels_pet);
	SDL_SetTextureBlendMode(petscii_pet_texture, SDL_BLENDMODE_ADD);
	SDL_Rect petscii_pet_rect;
	petscii_pet_rect.w = texture_w * texture_mul;
	petscii_pet_rect.h = texture_h * texture_mul;

	SDL_Texture * petscii_c64_texture = SDL_CreateTexture(renderer,
				SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
				texture_w, texture_h);
	SDL_UpdateTexture(petscii_c64_texture, NULL, pixels_c64, texture_w * 4);
	free(pixels_c64);
	SDL_SetTextureBlendMode(petscii_c64_texture, SDL_BLENDMODE_ADD);
	SDL_Rect petscii_c64_rect;
	petscii_c64_rect.w = texture_w * texture_mul;
	petscii_c64_rect.h = texture_h * texture_mul;

	int texture_max_x , texture_max_y;
	float x_sin_pet = 0.f;
	float y_sin_pet = 0.f;
	float x_sin_c64 = 0.f; 
	float y_sin_c64 = 0.f;
		
	texture_max_x = window_w - petscii_pet_rect.w;
	texture_max_y = window_h - petscii_c64_rect.h;
	printf("max_x : %d\n", texture_max_x);
	printf("max_y : %d\n", texture_max_y);

	while (running) {
	
		texture_max_x = window_w - petscii_pet_rect.w;
		texture_max_y = window_h - petscii_c64_rect.h;

		x_sin_pet += 0.05f;
		petscii_pet_rect.x = (int) (sin_pos2perc(x_sin_pet) * (float) texture_max_x);
		y_sin_pet += 0.032f;
		petscii_pet_rect.y = (int) (sin_pos2perc(y_sin_pet) * (float) texture_max_y);
		x_sin_c64 += 0.039f;
		petscii_c64_rect.x = (int) (sin_pos2perc(x_sin_c64) * (float) texture_max_x);
		y_sin_c64 += 0.027f;
		petscii_c64_rect.y = (int) (sin_pos2perc(y_sin_c64) * (float) texture_max_y);

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, image_texture, NULL, &image_rect);
		SDL_RenderCopy(renderer, petscii_pet_texture, NULL, &petscii_pet_rect);
		SDL_RenderCopy(renderer, petscii_c64_texture, NULL, &petscii_c64_rect);
		SDL_RenderCopy(renderer, font_texture, NULL, &font_rect);
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
					}
					break;
			}
		}
//		running = 0;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}
