
// from here : https://en.wikipedia.org/wiki/16:9_aspect_ratio#Common_resolutions
uint16_t fcl_video_common_widths[16] = { 256, 426, 640, 848, 854, 960, 1024, 1280, 1366, 1600, 1920, 2560, 3200, 3840, 5120, 7680 };
uint16_t fcl_video_common_heights[16] = { 144, 240, 360, 480, 480, 540, 576, 720, 768, 900, 1080, 1440, 1800, 2160, 2880, 4320 };

uint16_t fcl_video_width;
uint16_t fcl_video_height;
uint32_t fcl_video_max_pixels = 262144; // * 4 for 32bit color
uint32_t *fcl_video_ram[fcl_video_max_pixels];
SDL_Texture fcl_video_texture;
SDL_Color fcl_video_color_palette[256];

void fcl_video_clear(int index) {
	memset(fcl_video_ram, 0, fcl_video_max_pixels * sizeof(uint32_t));
}

void fcl_video_init(renderer) {
	fcl_video_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, CANVAS_HEIGHT);
	fcl_video_clear(0);
}

void fcl_video_set_color_index(int index, int r, int g, int b) {
	fcl_video_color_palette[index].r = r;
	fcl_video_color_palette[index].g = g;
	fcl_video_color_palette[index].b = b;
	fcl_video_color_palette[index].a = 255;
}

void fcl_video_set_dimensions(int x, int y) {
	fcl_video_width = x;
	fcl_video_height = y;
}

void fcl_video_set_pixel(int x, int y, int index) {
	fcl_video_ram[y * fcl_video_width + x] = SDL_Color[index];
}
