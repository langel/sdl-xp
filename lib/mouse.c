
typedef struct {
	int x;
	int y;
	int rel_x;
	int rel_y;
	int button_left;
	int button_middle;
	int button_right;
	int button_x1;
	int button_x2;
} mouse_info;


mouse_info mouse_init() {
	mouse_info mouse = { 0 };
	return mouse;
}

void mouse_process(mouse_info * mouse, SDL_Rect * window) {
	int prev_x = mouse->x;
	int prev_y = mouse->y;
	uint32_t buttons = SDL_GetGlobalMouseState(&mouse->x, &mouse->y);
	mouse->x -= window->x;
	mouse->y -= window->y;
	mouse->rel_x = mouse->x - prev_x;
	mouse->rel_y = mouse->y - prev_y;
	mouse->button_left = buttons & SDL_BUTTON_LMASK;
	mouse->button_middle = buttons & SDL_BUTTON_MMASK;
	mouse->button_right = buttons & SDL_BUTTON_RMASK;
	mouse->button_x1 = buttons & SDL_BUTTON_X1MASK;
	mouse->button_x2 = buttons & SDL_BUTTON_X2MASK;
}
