#include <math.h>
#include <stdio.h>
#include "SDL2/SDL.h"
//#include "lib/rng-lfsr.h"
#include "fcl/fcl_time.h"

int FPS = 60;

int CANVAS_WIDTH = 420;
int CANVAS_HEIGHT = 200;
int WINDOW_WIDTH = 1024;
int WINDOW_HEIGHT = 576;


int main(int argc, char *argv[]) {
	
	fcl_time_set_fps(FPS);

	SDL_Event event;

	 SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow("",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		  WINDOW_WIDTH, 
		  WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE);

    SDL_Renderer * renderer = SDL_CreateRenderer(window,
        -1, SDL_RENDERER_PRESENTVSYNC);

    // Since we are going to display a low resolution buffer,
    // it is best to limit the window size so that it cannot
    // be smaller than our internal buffer size.
    SDL_SetWindowMinimumSize(window, CANVAS_WIDTH, CANVAS_HEIGHT);

//    SDL_RenderSetLogicalSize(renderer, width, height);
    SDL_RenderSetIntegerScale(renderer, 1);

    SDL_Texture * screen_texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
        CANVAS_WIDTH,
		  CANVAS_HEIGHT);

	unsigned int * pixels = malloc(CANVAS_WIDTH * CANVAS_HEIGHT * 4);

	int running = 1;
	while (running) {

	  // Set every pixel to white.
	  for (int y = 0; y < CANVAS_HEIGHT; ++y)
	  {
			for (int x = 0; x < CANVAS_WIDTH; ++x)
			{
				int color = 0;
				color += (int) (((float) y / (float) CANVAS_HEIGHT) * 255.f) << 24; // red
				color += (rand() % 255) << 16; // green
				color += (int) (((float) x / (float) CANVAS_WIDTH) * 255.f) << 8; // blue
				color += 255; // alpha
				//pixels[x + y * width] = 0xffffffff;
				pixels[x + y * CANVAS_WIDTH] = color;
			}
	  }

	  // It's a good idea to clear the screen every frame,
	  // as artifacts may occur if the window overlaps with
	  // other windows or transparent overlays.
	  SDL_RenderClear(renderer);
	  SDL_UpdateTexture(screen_texture, NULL, pixels, CANVAS_WIDTH * 4);
	  SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
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

		fcl_time_wait_next_frame();

	}

//	fcl_audio_shutdown();
	SDL_Quit();

	return 0;

}
