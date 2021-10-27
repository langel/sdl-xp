#include <stdio.h>
#include <dirent.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


#define APP_TITLE "derpTASTIC"

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


void next_bg_color(unsigned char color[4]) {
	static float count = 0.f;
	count += 0.01f;
	color[0] = (int) ((float) 128 * sin_pos2perc(count));
	color[1] = (int) ((float) 128 * sin_pos2perc(count + 2));
	color[2] = (int) ((float) 128 * sin_pos2perc(count + 4));
	color[3] = 255;
}


int main(int argc, char *argv[]) {
	load_char_buffer("petscii_8032.bin", 0);
	load_char_buffer("petscii_c64.bin", 1);

	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_JPG);
	TTF_Init();

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

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
	SDL_Color text_fg = { 128, 64, 16, 255 };
	SDL_Color text_bg = { 255, 255, 255, 255};
	SDL_Surface * font_surface = TTF_RenderUTF8_Shaded(font, APP_TITLE, text_fg, text_bg);
	SDL_Texture * font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
	SDL_SetTextureBlendMode(font_texture, SDL_BLENDMODE_MOD);
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
	int image_w, image_h, image_max_x, image_max_y, image_dir_x, image_dir_y;
	SDL_QueryTexture(image_texture, NULL, NULL, &image_w, &image_h);
	SDL_Rect image_rect = { 0, 0, image_w, image_h };
			
	printf("texture resolution: %d x %d\n", texture_w, texture_h);
	printf("window resolution: %d x %d\n", window_w, window_h);

	SDL_RenderSetIntegerScale(renderer, 1);


	int running = 1;
	int char_count = char_w * char_h;
	printf("char count: %d\n", char_count);

	// PETSCII STUFF
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
	// pet petscii texture
	SDL_Texture * petscii_pet_texture = SDL_CreateTexture(renderer,
				SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
				texture_w, texture_h);
	SDL_UpdateTexture(petscii_pet_texture, NULL, pixels_pet, texture_w * 4);
	free(pixels_pet);
	SDL_SetTextureBlendMode(petscii_pet_texture, SDL_BLENDMODE_ADD);
	SDL_Rect petscii_pet_rect;
	petscii_pet_rect.w = texture_w * texture_mul;
	petscii_pet_rect.h = texture_h * texture_mul;
	// c64 petscii texture
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

	// MOUSE CURSOR SHIT
	SDL_Texture * mouse_cursor_texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 16, 16);
	SDL_SetRenderTarget(renderer, mouse_cursor_texture);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect mouse_cursor_rect = { 0, 0, 16, 16 };
	SDL_RenderFillRect(renderer, &mouse_cursor_rect);
	SDL_SetRenderTarget(renderer, NULL);

	// MOUSE CANVAS SHIT
	SDL_Texture * mouse_canvas_texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, window_w, window_h);
	SDL_SetTextureBlendMode(mouse_canvas_texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, mouse_canvas_texture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	mouse_cursor_rect = (SDL_Rect) { 0, 0, window_w, window_h };
	SDL_RenderFillRect(renderer, &mouse_cursor_rect);
	SDL_SetRenderTarget(renderer, NULL);

	int derp_speed = 10;
	double derp_angle = 0.f;
	double derp_angle_speed = 0.1f;

	int mouse_x, mouse_y;
	uint32_t mouse_buttons;
	SDL_ShowCursor(SDL_DISABLE);

	unsigned char bg_color[4];

	int frame_digit_size = 24;
	int frame_counter = 0;
	int frame_0s = 0;
	SDL_Rect frame_chr = { 0, 8, 8, 8 };
	SDL_Rect frame_dest = { 0, 8, frame_digit_size, frame_digit_size };
	char frame_count_str[22];

	while (running) {

		// FRAME COUNTER
		frame_counter += 19;
		frame_0s = ((frame_counter / 10)  % 10) + 16;
		frame_chr.x = frame_0s * 8;

		// MOUSE SHIT
		mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
		derp_angle += derp_angle_speed;
//		printf("window: %d x %d        mouse: %d x %d\n", window_w, window_h, mouse_x, mouse_y);
	
		// DERP IMAGE
		derp_speed = (window_w + window_h) * 0.0025;
		image_max_x = window_w - image_rect.w;
		if ((image_dir_x && image_rect.x >= image_max_x) || (!image_dir_x && image_rect.x <= 0)) {
			image_dir_x = !image_dir_x;
		}
		if (image_dir_x) image_rect.x += derp_speed;
		else image_rect.x -= derp_speed;
		image_max_y = window_h - image_rect.h;
		if ((image_dir_y && image_rect.y >= image_max_y) || (!image_dir_y && image_rect.y <= 0)) {
			image_dir_y = !image_dir_y;
		}
		if (image_dir_y) image_rect.y += derp_speed;
		else image_rect.y -= derp_speed;

		// PETSCII TILE SETS
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

		// RENDER THE TEXTURE LAYERS
		next_bg_color(bg_color);
		SDL_SetRenderDrawColor(renderer, bg_color[0], bg_color[1], bg_color[2], bg_color[3]);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, mouse_canvas_texture, NULL, NULL);
		SDL_RenderCopyEx(renderer, image_texture, NULL, &image_rect, derp_angle, NULL, SDL_FLIP_NONE);
		SDL_RenderCopy(renderer, petscii_pet_texture, NULL, &petscii_pet_rect);
		SDL_RenderCopy(renderer, petscii_c64_texture, NULL, &petscii_c64_rect);
		SDL_RenderCopy(renderer, font_texture, NULL, &font_rect);
		sprintf(frame_count_str, "%016u", frame_counter);
		for (int exp = 15; exp >= 0; exp--) {
			frame_dest.x = (15 - exp) * frame_digit_size;
			// binary top row
			int offset = ((frame_counter >> exp) & 1) ? 17 : 16;
			frame_chr.x = offset * 8;
			frame_dest.y = 0;
			SDL_RenderCopy(renderer, petscii_pet_texture, &frame_chr, &frame_dest);
			// decimal second row
			frame_dest.y = frame_digit_size;
			frame_chr.x = (frame_count_str[15 - exp] - 32) * 8;
			SDL_RenderCopy(renderer, petscii_pet_texture, &frame_chr, &frame_dest);
			// mouse_buttons 3rd row
			offset = ((mouse_buttons >> exp) & 1) ? 17 : 16;
			frame_chr.x = offset * 8;
			frame_dest.y = frame_digit_size * 2;
			SDL_RenderCopy(renderer, petscii_pet_texture, &frame_chr, &frame_dest);
		}
		if (mouse_x > 0 && mouse_x < window_w - 1 && mouse_y > 0 && mouse_y < window_h - 1) {
			mouse_cursor_rect = (SDL_Rect) { mouse_x - 7, mouse_y - 7, 16, 16 };
			SDL_RenderCopy(renderer, mouse_cursor_texture, NULL, &mouse_cursor_rect);
			if (mouse_buttons & 1) {
				SDL_SetRenderTarget(renderer, mouse_canvas_texture);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 127);
				SDL_RenderFillRect(renderer, &mouse_cursor_rect);
				SDL_SetRenderTarget(renderer, NULL);
			}
			else if (mouse_buttons & 4) {
				SDL_SetRenderTarget(renderer, mouse_canvas_texture);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
				SDL_RenderFillRect(renderer, &mouse_cursor_rect);
				SDL_SetRenderTarget(renderer, NULL);
			}
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
