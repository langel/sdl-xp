#include <stdio.h>
#include <SDL2/SDL.h>

int texture_w = 420;
int texture_h = 200;
int window_w = 840;
int window_h = 473;

SDL_Color palette[8] = {
	{ 0xf0, 0xf0, 0xdc, 0xff }, // white
	{ 0xfa, 0xc8, 0x00, 0xff }, // yellow
	{ 0x10, 0xc8, 0x40, 0xff }, // green
	{ 0x00, 0xa0, 0xc8, 0xff }, // blue
	{ 0xd2, 0x40, 0x40, 0xff }, // red
	{ 0xa0, 0x69, 0x4b, 0xff }, // brown
	{ 0x73, 0x64, 0x64, 0xff }, // grey
	{ 0x10, 0x18, 0x20, 0xff }, // black
};



int main(int argc, char* args[]) {

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Event event;
	SDL_Window * window = SDL_CreateWindow("Over Scaling Demo", 100, 200,
		window_w, window_h, SDL_WINDOW_RESIZABLE);
	SDL_Renderer * renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_PRESENTVSYNC);

	unsigned int * pixels = malloc(texture_w * texture_h * 4);
	SDL_Texture * pixel_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, texture_w, texture_h);
	for (int x = 0; x < texture_w; x++) {
		for (int y = 0; y < texture_h; y++) {
			uint32_t color = 0;
			int color_id = (x + y) % 8;
			color += palette[color_id].r << 24;
			color += palette[color_id].g << 16;
			color += palette[color_id].b << 8;
			color += 255;
			pixels[x + y * texture_w] = color;
		}
	}
	SDL_UpdateTexture(pixel_texture, NULL, pixels, texture_w * 4);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	SDL_Texture * overscale_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, texture_w * 3, texture_h * 3);
	SDL_SetRenderTarget(renderer, overscale_texture);
	SDL_RenderCopy(renderer, pixel_texture, NULL, NULL);

	SDL_SetRenderTarget(renderer, NULL);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
	SDL_Texture * downscale_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, texture_w * 3, texture_h * 3);
	SDL_SetRenderTarget(renderer, downscale_texture);
	SDL_RenderCopy(renderer, overscale_texture, NULL, NULL);

	SDL_SetRenderTarget(renderer, NULL);
	
	int running = 1;

	while (running) {

		SDL_RenderClear(renderer);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
		SDL_RenderCopy(renderer, downscale_texture, NULL, NULL);
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
						window_w = event.window.data1;
						window_h = event.window.data2;
						printf("window size changed: %d x %d\n", window_w, window_h);
					}
					break;
			}
		}
	}

	SDL_Quit();
	return 0;
}
