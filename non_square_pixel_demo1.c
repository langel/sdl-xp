#include <math.h>
#include <stdio.h>
#include "SDL2/SDL.h"
//#include "lib/rng-lfsr.h"
#include "fcl/fcl_time.h"

int FPS = 60;

int CANVAS_WIDTH = 420;
int CANVAS_HEIGHT = 200;
int WINDOW_WIDTH = 1366;
int WINDOW_HEIGHT = 768;


int collide(int x, int y, int box_x, int box_y, int box_xx, int box_yy) {
	// box_xx and box_yy are precalculated position + dimension
	if (x < box_x) return 0;
	if (y < box_y) return 0;
	if (x > box_xx) return 0;
	if (y > box_yy) return 0;
	return 1;
}

int main(int argc, char *argv[]) {
	
	fcl_time_set_fps(FPS);

	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO);

	int display_last_id = SDL_GetNumVideoDisplays();
	printf("displays %d \n", display_last_id);
	display_last_id--;
	SDL_Rect display_bounds;
	SDL_GetDisplayUsableBounds(display_last_id, &display_bounds);
	if (SDL_GetDisplayBounds(display_last_id, &display_bounds) != 0) {
	    SDL_Log("SDL_GetDisplayBounds failed: %s", SDL_GetError());
	}
	int x = display_bounds.x + (display_bounds.w - WINDOW_WIDTH) / 2;
	int y = display_bounds.y + (display_bounds.h - WINDOW_HEIGHT) / 2;
	
	SDL_Window * window = SDL_CreateWindow("", x, y,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_RESIZABLE);

	SDL_Renderer * renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_PRESENTVSYNC);
			
	SDL_Texture * canvas_texture;

	//    SDL_RenderSetLogicalSize(renderer, width, height);
	SDL_RenderSetIntegerScale(renderer, 1);


	unsigned int * pixels = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * 4);

	int running = 1;
	int framecount = 11111;
	int rect_x, rect_y, rect_w, rect_h, rect_xx, rect_yy;
	float rect_pos_x, rect_pos_y;;

	while (running) {

		if (framecount >= 60) {
			framecount = 0;
			CANVAS_WIDTH = rand() % WINDOW_WIDTH * 0.25f + 12;
			CANVAS_HEIGHT = rand() % WINDOW_HEIGHT * 0.25f + 12;
			// make sure the window can fit the canvas
			SDL_SetWindowMinimumSize(window, CANVAS_WIDTH, CANVAS_HEIGHT);
			// make new canvas texture
			SDL_DestroyTexture(canvas_texture);
			canvas_texture = SDL_CreateTexture(renderer,
				SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
				CANVAS_WIDTH, CANVAS_HEIGHT);
			rect_w = (int) (float) CANVAS_WIDTH * 0.281373f;
			rect_h = (int) (float) CANVAS_HEIGHT * 0.5f;
		}

		// update rectangle data
		rect_pos_x += 0.007f;
		// -1 on end of range because of round function
		rect_x = lroundf(((sin(rect_pos_x) + 1.f) * 0.5f) * (float) (CANVAS_WIDTH - rect_w - 1));
		rect_xx = rect_x + rect_w;
		rect_pos_y += 0.014f;
		// -1 on end of range because of round function
		rect_y = lroundf(((sin(rect_pos_y) + 1.f) * 0.5f) * (float) (CANVAS_HEIGHT - rect_h - 1));
		rect_yy = rect_y + rect_h;

		for (int y = 0; y < CANVAS_HEIGHT; ++y) {
			for (int x = 0; x < CANVAS_WIDTH; ++x) {
				uint32_t color = 0;
				if (collide(x, y, rect_x, rect_y, rect_xx, rect_yy) == 1) {
					color += (int) ((1 - (float) y / (float) CANVAS_HEIGHT) * 255.f) << 24; // red
					color += (int) ((1 - (float) x / (float) CANVAS_WIDTH) * 255.f) << 8; // blue
//					color += (rand() % 127) << 16; // green
				}
				else {
					//color += (int) (((float) y / (float) CANVAS_HEIGHT) * 255.f) << 24; // red
					//color += (int) (((float) x / (float) CANVAS_WIDTH) * 255.f) << 8; // blue
					color += (int) (((float) y / (float) CANVAS_HEIGHT) * 255.f) << 24; // red
					color += (int) (((float) x / (float) CANVAS_WIDTH) * 255.f) << 8; // blue
					if ((x % 2 == 0 && y % 2 == 1) || (x % 2 == 1 && y % 2 == 0)) {
						color += 255 << 16; // green
					}
					//color += (rand() % 255) << 16; // green
				}
				color += 255; // alpha
				//pixels[x + y * width] = 0xffffffff;
				pixels[x + y * CANVAS_WIDTH] = color;
			}
		}

		// It's a good idea to clear the screen every frame,
		// as artifacts may occur if the window overlaps with
		// other windows or transparent overlays.
		SDL_RenderClear(renderer);
		SDL_UpdateTexture(canvas_texture, NULL, pixels, CANVAS_WIDTH * 4);
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

		framecount++;

		fcl_time_wait_next_frame();

	}

//	fcl_audio_shutdown();
	SDL_Quit();

	return 0;

}
