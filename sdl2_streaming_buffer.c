#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

// stolen from https://benedicthenshaw.com/files/sdl2_streaming_buffer.c

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow("",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        //640, 480,
		  1024, 576,
        SDL_WINDOW_RESIZABLE);

    SDL_Renderer * renderer = SDL_CreateRenderer(window,
        -1, SDL_RENDERER_PRESENTVSYNC);

    int width = 420;
    int height = 200;

    // Since we are going to display a low resolution buffer,
    // it is best to limit the window size so that it cannot
    // be smaller than our internal buffer size.
    SDL_SetWindowMinimumSize(window, width, height);

//    SDL_RenderSetLogicalSize(renderer, width, height);
    SDL_RenderSetIntegerScale(renderer, 1);

    SDL_Texture * screen_texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
        width, height);

    unsigned int * pixels = malloc(width * height * 4);

    while (1)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) exit(0);
        }

        // Set every pixel to white.
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
					int color = 0;
					color += (int) (((float)y / (float) height) * 255.f) << 24; // red
					color += (rand() % 255) << 16; // green
					color += (int) (((float) x / (float) width) * 255.f) << 8; // blue
					color += 255; // alpha
         		//pixels[x + y * width] = 0xffffffff;
               pixels[x + y * width] = color;
            }
        }

        // It's a good idea to clear the screen every frame,
        // as artifacts may occur if the window overlaps with
        // other windows or transparent overlays.
        SDL_RenderClear(renderer);
        SDL_UpdateTexture(screen_texture, NULL, pixels, width * 4);
        SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
}
