// Example program:
// Using SDL2 to create an application window

#include <stdio.h>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {

    SDL_Window *window;                    // Declare a pointer

    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "An SDL2 window",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        640,                               // width, in pixels
        480,                               // height, in pixels
//        SDL_WINDOW_OPENGL                  // flags - see below
//	SDL_WINDOW_METAL
//	SDL_WINDOW_VULKAN
	0
    );

    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

	uint32_t flags = SDL_GetWindowFlags(window);
	if (flags & SDL_WINDOW_OPENGL) printf("window supports opengl\n");		
	if (flags & SDL_WINDOW_VULKAN) printf("window supports vulkan\n");		
	if (flags & SDL_WINDOW_METAL) printf("window supports metal\n");		

    // The window is open: could enter program loop here (see SDL_PollEvent())

//    SDL_Delay(3000);  // Pause execution for 3000 milliseconds, for example

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}
