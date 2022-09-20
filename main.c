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


	// frame throttler variables
	uint64_t start;
	uint64_t end;
	double elapsed;

	window_state_load(window, "window");
	devpipe_init(window);

	int running = 1;
	void kill() {
		running = 0;
		window_state_save(window, "window");
		devpipe_kill_cycle();
	}
	int frame_counter = 0;
	while (running) {
		// check for main.c updates
		if (++frame_counter % 30 == 0) {
			if (devpipe_check_update()) {
				running = 0;
				window_state_save(window, "window");
			}
		}

		start = SDL_GetPerformanceCounter();

		// clear surface
		for (uint32_t i = 0; i < surface_pixel_count; i++) {
			surface_pixels[i] = 0;
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
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
