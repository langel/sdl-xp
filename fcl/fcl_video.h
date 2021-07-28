

#define fcl_video_max_pixels 262144 // * 4 for 32bit color
uint32_t *fcl_video_ram[fcl_video_max_pixels];
uint16_t fcl_video_width = 420;
uint16_t fcl_video_height= 200;
SDL_Texture *fcl_video_texture;
SDL_Color fcl_video_colors[256];
SDL_Renderer *fcl_video_renderer;
SDL_Surface *fcl_video_surface;

void fcl_video_clear(uint8_t index) {
	memset(fcl_video_ram, index, fcl_video_max_pixels * sizeof(uint32_t));
	//SDL_RenderFillRect(renderer, &window_rect);
}

void fcl_video_frame_next() {
	// blit that shit
	SDL_RenderPresent(fcl_video_renderer);
	fcl_time_wait_next_frame();
}

void fcl_video_set_dimensions(int x, int y) {
	fcl_video_width = x;
	fcl_video_height = y;
}

void fcl_video_init(int x, int y) {
	fcl_video_set_dimensions(x, y);
//	fcl_video_texture = SDL_CreateTexture(fcl_video_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, fcl_video_width, fcl_video_height);
	fcl_video_clear(0);
	fcl_video_renderer = SDL_CreateRenderer(fcl_window_object, -1, 0);
	fcl_video_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, fcl_video_width, fcl_video_height, 8, 0, 0, 0, 0);
	SDL_SetPaletteColors(fcl_video_surface->format->palette, fcl_video_colors, 0, 256);
//	SDL_SetRenderDrawBlendMode(fcl_video_renderer, SDL_BLENDMODE_BLEND);
}

void fcl_video_set_color_index(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
	fcl_video_colors[index].r = r;
	fcl_video_colors[index].g = g;
	fcl_video_colors[index].b = b;
	fcl_video_colors[index].a = 255;
}

void fcl_video_set_draw_color(uint8_t index) {
	//SDL_SetRenderDrawColor(fcl_video_renderer, fcl_video_colors[index].r, fcl_video_colors[index].g, fcl_video_colors[index].b, 255);
}

void fcl_video_set_pixel(int x, int y, uint8_t index) {
	//fcl_video_ram[y * fcl_video_width + x] = fcl_video_colors[index];
}
