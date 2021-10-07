#include <math.h>
#include <stdio.h>
#include "SDL2/SDL.h"
//#include "lib/rng-lfsr.h"
#include "fcl/fcl_time.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int FPS = 60;

#define CANVAS_MAX_W 300
#define CANVAS_MAX_H 200
#define CANVAS_MIN 12
int CANVAS_WIDTH = 420;
int CANVAS_HEIGHT = 200;
int WINDOW_WIDTH = 1366;
int WINDOW_HEIGHT = 768;

uint32_t fire_palette[256];
uint8_t fire_data[CANVAS_MAX_W][CANVAS_MAX_H];

int collide(int x, int y, int box_x, int box_y, int box_xx, int box_yy) {
	// box_xx and box_yy are precalculated position + dimension
	if (x < box_x) return 0;
	if (y < box_y) return 0;
	if (x > box_xx) return 0;
	if (y > box_yy) return 0;
	return 1;
}

int main(int argc, char *argv[]) {
	
	// lets make the fire palette!! \:U/
	for (int i = 0; i <= 255; i++) {
		uint32_t color = 0;
		/*
		if (i <= 127) {
			color = 255 << 24; // red
			color += (255 - (i * 2)) << 16; //green
			color += 255; // alpha
		}
		else {
			color = 255 << 24; // red
			color = (255 - ((i - 128) * 2)) << 24; // red
			color += 255 - ((i - 128) * 2); // alpha
		}
		//color += MAX(255 - i - 223, 0) << 8; //blue
		fire_palette[255 - i] = color;
		*/
		if (i <= 127) {
			color = ((i - 128) * 2) << 24; // red
		}
		else {
			color = 255 << 24; // red
		}
		float curve = (float) i / 255.0;
		float green_curve = (curve*4.0 + curve*curve) / 4.0;
		if (green_curve > 1.0f) green_curve = 1.0f;
		//color += MAX(255 - i - 64, 0) << 16; //green
		color += (int)(((float) i) * green_curve) << 16; //green
		curve *= curve;
		curve *= curve;
		color += (int)(((float)i) * curve) << 8; //blue
		fire_palette[i] = color;
//		printf("%d\n",color);
	}

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
			
	SDL_Texture * canvas_texture = SDL_CreateTexture(renderer,
				SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
				CANVAS_WIDTH, CANVAS_HEIGHT);

	//    SDL_RenderSetLogicalSize(renderer, width, height);
	SDL_RenderSetIntegerScale(renderer, 1);


	unsigned int * pixels = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * 4);

	int running = 1;
	int framecount = 11111;
	int rect_x, rect_y, rect_w, rect_h, rect_xx, rect_yy;
	float rect_pos_x, rect_pos_y;
	float x_ratio;
	float y_ratio;

	while (running) {

		if (framecount >= 60) {
			framecount = 0;
			CANVAS_WIDTH = rand() % (CANVAS_MAX_W - CANVAS_MIN) + CANVAS_MIN;
			CANVAS_HEIGHT = rand() % (CANVAS_MAX_H - CANVAS_MIN) + CANVAS_MIN;
			// make sure the window can fit the canvas
			SDL_SetWindowMinimumSize(window, CANVAS_WIDTH, CANVAS_HEIGHT);
			// make new canvas texture
			SDL_DestroyTexture(canvas_texture);
			canvas_texture = SDL_CreateTexture(renderer,
				SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
				CANVAS_WIDTH, CANVAS_HEIGHT);
			rect_w = (int) (float) CANVAS_WIDTH * 0.281373f;
			rect_h = (int) (float) CANVAS_HEIGHT * 0.5f;
			x_ratio = (float) CANVAS_MAX_W / (float) CANVAS_WIDTH;
			y_ratio = (float) CANVAS_MAX_H / (float) CANVAS_HEIGHT;
//printf("%d %d %f %f\n", CANVAS_WIDTH, CANVAS_HEIGHT, x_ratio, y_ratio);
		}


		// update fire data
		for (int x = 0; x < CANVAS_MAX_W; x++) {
			// noise for bottom row
			fire_data[x][CANVAS_MAX_H - 1] = (rand() % 2) ? 0 : 255;
		}
		int color_bottom, color_left, color_right;
		for (int y = 0; y < CANVAS_MAX_H - 1; y++) {
			for (int x = 0; x < CANVAS_MAX_W; x++) {
				if (y == CANVAS_MAX_H - 2) {
					color_bottom = 255;
				}
				else {
					color_bottom = fire_data[(x) % CANVAS_MAX_W][(y + 2) % CANVAS_MAX_H];
				}
				if (x == 0) color_left = 0;
				else color_left = fire_data[(x - 1 + CANVAS_MAX_W) % CANVAS_MAX_W][(y + 1) % CANVAS_MAX_H];
				if (x == CANVAS_MAX_W - 1) color_right = 0;
				else color_right = fire_data[(x + 1) % CANVAS_MAX_W][(y + 1) % CANVAS_MAX_H];

				int color_id = ((color_left
					+ fire_data[(x) % CANVAS_MAX_W][(y + 1) % CANVAS_MAX_H]
					+ color_right
					+ color_bottom)
					* 32) / 129;
				fire_data[x][y] = color_id;
			/*
				uint8_t color_id = 255;
				// left edge
				if (x == 0) {
					color_id = (fire_data[x][y+1] + fire_data[x+1][y+1] + fire_data[x][
				}
				// right edge
				else if (x == CANVAS_MAX_W - 1) {
				}
				// not edgte
				else {
				}
				fire_data[x][y] = color_id;
				*/
			}
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
					color += (int) ((1 - (float) y / (float) CANVAS_HEIGHT) * 255.f) << 16; // green
					color += (int) ((1 - (float) x / (float) CANVAS_WIDTH) * 255.f) << 8; // blue
//					color += (rand() % 127) << 16; // green
//					color += 255; // alpha
				}
				else {
					//color += (int) (((float) y / (float) CANVAS_HEIGHT) * 255.f) << 24; // red
					//color += (int) (((float) x / (float) CANVAS_WIDTH) * 255.f) << 8; // blue
					/*
					color += (int) (((float) y / (float) CANVAS_HEIGHT) * 255.f) << 24; // red
					color += (int) (((float) x / (float) CANVAS_WIDTH) * 255.f) << 8; // blue
					if ((x % 2 == 0 && y % 2 == 1) || (x % 2 == 1 && y % 2 == 0)) {
						color += 255 << 16; // green
					}
					*/
					//color += (rand() % 255) << 16; // green
				}
				color += fire_palette[fire_data[lroundf((float)x * x_ratio)][(int)((float)y * y_ratio)]];
				//pixels[x + y * width] = 0xffffffff;
//				color = fire_palette[255 - y];
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
