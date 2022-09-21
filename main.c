#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "lib/core.c"


int texture_w = 420;
int texture_h = 200;
int window_w = 1280;
int window_h = 720;

unsigned long time_counter = 0;

int fps;

int cell_data[420][200] = { 0 };
int cell_temp[420][200] = { 0 };
int cell_seed = 0;
float chance_to_be_wall = 0.425f;

void cave_new_noise() {
	cell_seed += 1337;
	int cell_pos = 0;
	// initialize noise
	for (int x = 0; x < 420; x++) {
		for (int y= 0; y < 200; y++) {
			cell_data[x][y] = (squirrel3_zero_float(cell_pos, cell_seed) < chance_to_be_wall) ? 1 : 0;
			cell_pos++;
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
int flood_fill(int x, int y, int count, int color_old, int color_new) {
	int cell = cell_data[x][y];
	if (cell != color_old) return count;
	cell_data[x][y] = color_new;
	count++;
	count = flood_fill(x, y+1, count, color_old, color_new);
	count = flood_fill(x, y-1, count, color_old, color_new);
	count = flood_fill(x-1, y, count, color_old, color_new);
	count = flood_fill(x+1, y, count, color_old, color_new);
	return count;
}

// return 0 if no cell found
int cave_fill_cavern() {
	for (int x = 0; x < 420; x++) {
		for (int y = 0; y < 200; y++) {
			if (cell_data[x][y] == 0) {
				// return cavern data here 
				int count = flood_fill(x, y, 0, 0, 2);
				return count;
			}
		}
	}
	return 0;
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
	SDL_Window * window = SDL_CreateWindow("interpolation trial", window_rect.x, window_rect.y, window_rect.w, window_rect.h, SDL_WINDOW_RESIZABLE);
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
	cave_new_noise();
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

		if (frame_counter == 120) frame_counter = 0;
		if (frame_counter == 0) cave_new_noise();
		else if (frame_counter > 30 && frame_counter < 60 && frame_counter % 4 == 0) {
			cave_wall_update();
		}
		if (frame_counter == 75) {
			int success = cave_fill_cavern();
			if (success) {
				printf("%d ", success);
				frame_counter--;
			}
			else printf("\n");
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
