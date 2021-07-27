// from here : https://en.wikipedia.org/wiki/16:9_aspect_ratio#Common_resolutions
uint16_t fcl_window_common_widths[16] = { 256, 426, 640, 848, 854, 960, 1024, 1280, 1366, 1600, 1920, 2560, 3200, 3840, 5120, 7680 };
uint16_t fcl_window_common_heights[16] = { 144, 240, 360, 480, 480, 540, 576, 720, 768, 900, 1080, 1440, 1800, 2160, 2880, 4320 };

SDL_Window *fcl_window_object;
SDL_Rect fcl_window_display_rect;
SDL_Rect fcl_window_display_bounds;
uint8_t fcl_window_display_count;

uint16_t fcl_window_width;
uint16_t fcl_window_height;
int16_t fcl_window_pos_x;
int16_t fcl_window_pos_x_max;
int16_t fcl_window_pos_x_min;
int16_t fcl_window_pos_y;
int16_t fcl_window_pos_y_max;
int16_t fcl_window_pos_y_min;

void fcl_window_init() {
	fcl_window_display_count = SDL_GetNumVideoDisplays();
	// XXX temporarily use last display
	SDL_GetDisplayUsableBounds(fcl_window_display_count - 1, &fcl_window_display_bounds);
}

void fcl_window_create(const char *title, int16_t pos_x, int16_t pos_y, uint16_t width, uint16_t height) {
	// width stuff
	fcl_window_width = width;
	fcl_window_pos_x = pos_x;
	fcl_window_pos_x_min = fcl_window_display_bounds.x;
	fcl_window_pos_x_max = fcl_window_display_bounds.x + fcl_window_display_bounds.w - fcl_window_width;
	// height stuff
	fcl_window_height = height;
	fcl_window_pos_y = pos_y;
	fcl_window_pos_y_min = fcl_window_display_bounds.y;
	fcl_window_pos_y_max = fcl_window_display_bounds.y + fcl_window_display_bounds.w - fcl_window_height;
	fcl_window_object = SDL_CreateWindow(title, pos_x, pos_y, width, height, 0);
}

void fcl_window_set_position(int16_t pos_x, int16_t pos_y) {
	SDL_SetWindowPosition(fcl_window_object, pos_x, pos_y);
}

void fcl_window_center() {
	int16_t pos_x = fcl_window_display_bounds.x + (fcl_window_display_bounds.w - fcl_window_width) / 2;
	int16_t pos_y = fcl_window_display_bounds.y + (fcl_window_display_bounds.h - fcl_window_height) / 2;
	fcl_window_set_position(pos_x, pos_y);
}
