

#define fcl_video_max_pixels 262144 // * 4 for 32bit color
uint32_t *fcl_video_ram[fcl_video_max_pixels];
uint16_t fcl_video_width = 420;
uint16_t fcl_video_height= 200;
SDL_Texture fcl_video_texture;
SDL_Color fcl_video_colors[256];

void fcl_video_clear(int index) {
	memset(fcl_video_ram, 0, fcl_video_max_pixels * sizeof(uint32_t));
}

void fcl_video_init(renderer) {
	fcl_video_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, fcl_video_width, fcl_video_height);
	fcl_video_clear(0);
}

void fcl_video_set_color_index(int index, int r, int g, int b) {
	fcl_video_colors[index].r = r;
	fcl_video_colors[index].g = g;
	fcl_video_colors[index].b = b;
	fcl_video_colors[index].a = 255;
}

void fcl_video_set_dimensions(int x, int y) {
	fcl_video_width = x;
	fcl_video_height = y;
}

void fcl_video_set_draw_color(uint8_t index) {
	SDL_SetRenderDrawColor(renderer, fcl_video_colors[index].r, fcl_video_colors[index].g, fcl_video_colors[index].b, 255);
}

void fcl_video_set_pixel(int x, int y, int index) {
	fcl_video_ram[y * fcl_video_width + x] = fcl_video_colors[index];
}
