#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "SDL.h"
#include "SDL_image.h"
#include "lib/core.c"


int texture_w = 420;
int texture_h = 200;
int window_w = 1280;
int window_h = 720;

unsigned long time_counter = 0;

int fps;

int cell_data[420][200] = { 0 };
int cell_temp[420][200] = { 0 };
// 29414 
int cell_seed = 49469;
float chance_to_be_wall = 0.425f;

void cave_new_noise() {
	int cell_pos = 0;
	// initialize noise
	for (int x = 0; x < 420; x++) {
		for (int y= 0; y < 200; y++) {
			cell_data[x][y] = (squirrel3_zero_float(cell_pos, cell_seed) < chance_to_be_wall) ? 1 : 0;
			cell_pos++;
		}
	}
	cell_seed += 1337;
}

void cave_clear(int color) {
	for (int x = 0; x < 420; x++) {
		for (int y = 0; y < 200; y++) {
			cell_data[x][y] = color;
		}
	}
}

void cave_wall_update() {
	memcpy(cell_temp, cell_data, sizeof(int) * 420 * 200);
	for (int x = 0; x < 420; x++) {
		for (int y = 0; y < 200; y++) {
			int wall_count = 0;
			// cells above
			if (y > 0) {
				if (x > 0) wall_count += cell_data[x - 1][y - 1];
				wall_count += cell_data[x][y - 1];
				if (x < 419) wall_count += cell_data[x + 1][y - 1];
			}
			// cells next door
			if (x > 0) wall_count += cell_data[x - 1][y];
			if (x < 419) wall_count += cell_data[x + 1][y];
			// cells below
			if (y < 199) {
				if (x > 0) wall_count += cell_data[x - 1][y + 1];
				wall_count += cell_data[x][y + 1];
				if (x < 419) wall_count += cell_data[x + 1][y + 1];
			}
			// update cell value
			int is_wall = cell_data[x][y];
			if (wall_count < 3 && is_wall) cell_temp[x][y] = 0;
			if (wall_count > 4 && !is_wall) cell_temp[x][y] = 1;
			if (x == 0 || x == 419 || y == 0 || y == 199) cell_temp[x][y] = 1;
		}
	}
	memcpy(cell_data, cell_temp, sizeof(int) * 420 * 200);
}

// returns cell count filled
int fill_stats[4];
int flood_fill(int x, int y, int count, int color_old, int color_new) {
	if (count == 0) {
		fill_stats[0] = 420; fill_stats[1] = 0;
		fill_stats[2] = 200; fill_stats[3] = 0;
	}
	int cell = cell_data[x][y];
	if (cell != color_old) return count;
	cell_data[x][y] = color_new;
	if (x < fill_stats[0]) fill_stats[0] = x;
	if (x > fill_stats[1]) fill_stats[1] = x;
	if (y < fill_stats[2]) fill_stats[2] = y;
	if (y > fill_stats[3]) fill_stats[3] = y;
	count++;
	count = flood_fill(x, y+1, count, color_old, color_new);
	count = flood_fill(x, y-1, count, color_old, color_new);
	count = flood_fill(x-1, y, count, color_old, color_new);
	count = flood_fill(x+1, y, count, color_old, color_new);
	return count;
}

typedef struct {
	int x;
	int y;
	int count;
	SDL_Rect rect;
	int stats[4];
} cavern_struct;
cavern_struct caverns[256] = { 
	{ 0, 0, 0, { 0, 0, 0, 0 } } 
};
int cavern_index;
int cavern_count;
int best_cavern;
SDL_Rect cavern_target;
SDL_Rect cavern_origin;
void caverns_clear() {
	cavern_index = 0;
	for (int i = 0; i < 256; i++) caverns[i].count = 0;
}
int cavern_mod_frames;
float cavern_mod[4];
float cavern_scale[4];

// return 0 if no cell found
cavern_struct cave_fill_cavern() {
	for (int x = 0; x < 420; x++) {
		for (int y = 0; y < 200; y++) {
			if (cell_data[x][y] == 0) {
				// return cavern data here 
				int count = flood_fill(x, y, 0, 0, 2);
				return (cavern_struct) { x, y, count,
					{ fill_stats[0], fill_stats[2], fill_stats[1] - fill_stats[0] + 1, fill_stats[3] - fill_stats[2] + 1 },
					{ fill_stats[0], fill_stats[1], fill_stats[2], fill_stats[3] }
				};
			}
		}
	}
	return (cavern_struct) { 0, 0, 0, { 0, 0, 0, 0} };
}


u32 colors[3] = {
	0x7f3f1fff, // floor
	0x111111ff, // wall
	0xbfbf1fff, // filled
};


int main(int argc, char* args[]) {

	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS);

	SDL_Event event;
	SDL_Rect window_rect = { 200, 200, window_w, window_h };
	SDL_Window * window = SDL_CreateWindow("Deterministic Cavern Generation", window_rect.x, window_rect.y, window_rect.w, window_rect.h, SDL_WINDOW_RESIZABLE);
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_DisplayMode display_mode;
	SDL_GetDisplayMode(0, 0, &display_mode);
	fps = display_mode.refresh_rate;

	int surface_pixel_count = texture_w * texture_h;
	int surface_size = texture_w * texture_h * 4;
	int surface_width = texture_w * 4;
	uint32_t * surface_pixels = malloc(surface_size);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	SDL_Texture * fcv_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, texture_w, texture_h);
	#define overscale_amount 3
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
	SDL_Texture * overscale_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, texture_w * overscale_amount, texture_h * overscale_amount);


	// frame throttler variables
	uint64_t start;
	uint64_t end;
	double elapsed;

	window_state_load(window, "window");
	devpipe_init(window);

	// init surface noise
	for (uint32_t i = 0; i < surface_pixel_count; i++) {
		surface_pixels[i] = 0;
	}


	int running = 1;
	void kill() {
		running = 0;
		window_state_save(window, "window");
		devpipe_kill_cycle();
	}
	int frame_counter = 0;
	while (running) {

		start = SDL_GetPerformanceCounter();
		cavern_struct cavern;

		if (frame_counter == 200) frame_counter = 0;
		// reset sequence
		if (frame_counter == 0) {
			printf("\n\ncell seed: %d\n", cell_seed);
			cave_new_noise();
			caverns_clear();
			best_cavern = -1;
		}
		// process noise
		else if (frame_counter > 30 && frame_counter < 60 && frame_counter % 4 == 0) {
			cave_wall_update();
		}
		// find all unique caverns
		if (frame_counter == 75) {
			cavern = cave_fill_cavern();
			if (cavern.count) {
//				printf("%d ", cavern.count);
				if (cavern.count < 2500) {
					if (cavern.count > caverns[best_cavern].count) {
						best_cavern = cavern_index;
					}
				}
				caverns[cavern_index] = cavern;
				cavern_index++;
				frame_counter--;
			}
			else {
				printf("\ncavern count: %d\n", cavern_index);
				cavern_count = cavern_index + 1;
				cavern_index = 0;
			}
		}
		// highlight chosen cavern
		if (frame_counter == 76) {
			printf("best cavern: %d\n", best_cavern);
			cavern = caverns[best_cavern];
			flood_fill(cavern.x, cavern.y, 0, 2, 0);
			cavern_index = 0;
		}
		// remove all other caverns
		if (frame_counter == 79) {
			cavern = caverns[cavern_index];
			if (cavern_index != best_cavern) {
				flood_fill(cavern.x, cavern.y, 0, 2, 1);
				//printf("%d ", cavern.count);
			}
			cavern_index++;
			if (cavern_index < cavern_count) frame_counter--;
		}
		// set target rect for final cavern
		if (frame_counter == 84) {
			cavern = caverns[best_cavern];
			printf("%d %d %d %d\n", cavern.rect.x, cavern.rect.y, cavern.rect.w, cavern.rect.h);
			memset(cell_temp, 0, sizeof(int) * 420 * 200);
			for (int x = 0; x < cavern.rect.w; x++) {
				for (int y = 0; y < cavern.rect.h; y++) {
					cell_temp[x][y] = cell_data[x + cavern.rect.x][y + cavern.rect.y];
				}
			}
			cavern_origin.x = cavern.rect.x;
			cavern_origin.y = cavern.rect.y;
			cavern_origin.w = cavern.rect.w;
			cavern_origin.h = cavern.rect.h;
			float x_scale = (420.f / (float) cavern.rect.w) * 0.8f;
			float y_scale = (200.f / (float) cavern.rect.h) * 0.8f;
			float scale = (x_scale < y_scale) ? x_scale : y_scale;
			if (scale > 4.f) scale = 4.f;
			cavern_target.w = (int) ((float) cavern.rect.w * scale);
			cavern_target.h = (int) ((float) cavern.rect.h * scale);
			cavern_target.x = (420 - cavern_target.w) / 2;
			cavern_target.y = (200 - cavern_target.h) / 2;
			printf("%d %d %d %d\n", cavern_target.x, cavern_target.y, cavern_target.w, cavern_target.h);
			// use for frame counter
			cavern_index = 0;
			cavern_mod_frames = 45;
			cavern_mod[0] = (cavern_target.x - cavern_origin.x) / (float) cavern_mod_frames;
			cavern_mod[1] = (cavern_target.y - cavern_origin.y) / (float) cavern_mod_frames;
			cavern_mod[2] = (cavern_target.w - cavern_origin.w) / (float) cavern_mod_frames;
			cavern_mod[3] = (cavern_target.h - cavern_origin.h) / (float) cavern_mod_frames;
			cavern_scale[0] = cavern_origin.x;
			cavern_scale[1] = cavern_origin.y;
			cavern_scale[2] = cavern_origin.w;
			cavern_scale[3] = cavern_origin.h;
		}
		// animate cavern to center while scaling
		if (frame_counter == 85) {
			cavern.rect.x = cavern_scale[0] += cavern_mod[0];
			cavern.rect.y = cavern_scale[1] += cavern_mod[1];
			cavern.rect.w = cavern_scale[2] += cavern_mod[2];
			cavern.rect.h = cavern_scale[3] += cavern_mod[3];
			cave_clear(1);
			float scale_x = (float) cavern_origin.w / (float) cavern.rect.w;
			float scale_y = (float) cavern_origin.h / (float) cavern.rect.h;
			for (int x = 0; x < cavern.rect.w; x++) {
				for (int y = 0; y < cavern.rect.h; y++) {
					int xt = x * scale_x;
					int yt = y * scale_y;
					cell_data[x + cavern.rect.x][y + cavern.rect.y] = cell_temp[xt][yt];
				}
			}
//			printf("%d %d %d %d\n", cavern.rect.x, cavern.rect.y, cavern.rect.w, cavern.rect.h);
			cavern_index++;
			if (cavern_index < cavern_mod_frames) frame_counter--;
			else printf("%d %d %d %d\n", cavern.rect.x, cavern.rect.y, cavern.rect.w, cavern.rect.h);
		}
		if (frame_counter > 100 && frame_counter % 4 == 0) {
			cave_wall_update();
		}


		for (int x = 0; x < 420; x++) {
			for (int y= 0; y < 200; y++) {
				surface_pixels[x + y * texture_w] = colors[cell_data[x][y]];
			}
		}



		SDL_UpdateTexture(fcv_texture, NULL, surface_pixels, surface_width);
		SDL_SetRenderTarget(renderer, overscale_texture);
		SDL_RenderCopy(renderer, fcv_texture, NULL, NULL);
		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, overscale_texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		
		// fps throttle
		end = SDL_GetPerformanceCounter();
		elapsed = (double) (end - start) / ( (double) SDL_GetPerformanceFrequency() * 1000.0);
		// only delay if framerate is above 60
		if (fps > 60) SDL_Delay(floor(1000.0 / 60.0 - elapsed));

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					kill();
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							kill();
							break;
					}
					break;
				case SDL_KEYUP:
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

		frame_counter++;

		// check for main.c updates
		if (frame_counter % 30 == 0) {
			if (devpipe_check_update()) {
				running = 0;
				window_state_save(window, "window");
			}
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
