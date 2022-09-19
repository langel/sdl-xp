#include <math.h>
#include <stdio.h>
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

	#define pixels_per_point 16
	#define point_array_length 32
	float points[point_array_length] = { 0.f };
	for (int i = 0; i < point_array_length; i++) {
		points[i] = squirrel3_zero_float(i + 1337, 1337);
	}
	int lastylin = 0;
	int lastybic = 0;

	float x_pos = 0.f;
	float x_vel = 0.00753f;
	int pos_int = 0;
	float pos_dec = 0.f;

	// frame throttler variables
	uint64_t start;
	uint64_t end;
	double elapsed;

	devpipe_init();

	int running = 1;
	int frame_counter = 0;
	while (running) {
		// check for main.c updates
		if (++frame_counter % 30 == 0) {
			if (devpipe_check_update()) running = 0;
		}

		start = SDL_GetPerformanceCounter();

		// clear surface
		for (uint32_t i = 0; i < surface_pixel_count; i++) {
			surface_pixels[i] = 0;
		}

		// draw reference lines
		for (int x = 0; x < texture_w; x++) {
			// green mid lines
			surface_pixels[x + (int) ((float) texture_h * 0.275f) * texture_w] += 0x001f00ff;
			surface_pixels[x + (int) ((float) texture_h * 0.725f) * texture_w] += 0x001f00ff;
			// purple median line
			surface_pixels[x + (int) ((float) texture_h * 0.50f) * texture_w] += 0x1f001fff;
			// red danger lines
			surface_pixels[x + (int) ((float) texture_h * 0.05f) * texture_w] += 0x3f0000ff;
			surface_pixels[x + (int) ((float) texture_h * 0.95f) * texture_w] += 0x3f0000ff;
		}
		// vertical grid lines
		for (int y = 0; y < texture_h; y++) {
			for (int i = 0; i < 9; i++) {
				int x = i * texture_w * 0.125 - texture_w * 0.0625;
				if (x > 0 && x < texture_w) {
					surface_pixels[x + y * texture_w] += 0x001f00ff;
				}
			}
		}

		// draw linear and bicubic interpretations of points
		for (int x = 0; x < texture_w; x++) {
			float pos = (float) x / (float) pixels_per_point + x_pos;
			pos_int = (int) pos;
			pos_dec = pos - pos_int;

			int y;

			int bound_y(int y) {
				if (y < 0) y = 0;
				if (y >= texture_h) y = texture_h - 1;
				return y;
			}

			void y_backfill(int lasty, int cury, uint32_t color_add) {
				// draw up
				for (int i = lasty + 1; i < cury; i++) {
					surface_pixels[x + texture_w * i] += color_add;
				}
				// draw down
				for (int i = lasty - 1; i > cury; i--) {
					surface_pixels[x + texture_w * i] += color_add;
				}
			}

			float point_at_pos(int pos) {
				return points[pos % point_array_length];
			}

			// y of linear
			y = (int) roundf((float) texture_h * lerp(point_at_pos(pos_int), point_at_pos(pos_int + 1), pos_dec));
			y = bound_y(y);
			if (x) y_backfill(lastylin, y, 0xf0700000);
			surface_pixels[x + texture_w * y] += 0xf0700000;
			lastylin = y;

			// y of bicubic
			float cuboints[4];
			cuboints[0] = (pos_int == 0) ? 0.f : point_at_pos(pos_int - 1);
			cuboints[1] = point_at_pos(pos_int + 0);
			cuboints[2] = point_at_pos(pos_int + 1);
			cuboints[3] = point_at_pos(pos_int + 2);
			y = (int) ((float) texture_h * cuberp(cuboints, pos_dec));
			y = bound_y(y);
			if (x) y_backfill(lastybic, y, 0x0070f000);
			surface_pixels[x + texture_w * y] += 0x0070f000;
			lastybic = y;
		}
		x_pos += x_vel;
		points[(pos_int + 2) % point_array_length] = squirrel3_zero_float(pos_int + 1337, 1337);

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
					running = 0;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = 0;
							devpipe_kill_cycle();
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
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
