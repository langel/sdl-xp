

// focus window
void window_focus(SDL_Window * window) {
	// XXX seems to be confusion here
	SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
	SDL_SetWindowAlwaysOnTop(window, SDL_FALSE);
	SDL_RaiseWindow(window);
}
// force window on top
void window_ontop_set_true(SDL_Window * window) {
	SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
}
// disable force window top
void window_ontop_set_false(SDL_Window * window) {
	SDL_SetWindowAlwaysOnTop(window, SDL_FALSE);
}


void window_state_load(SDL_Window * window) {
	// XXX somewhere needs to check pos/size is visible on system
	//int display_count = SDL_GetNumVideoDisplays(window);
	//int display_current = SDL_GetWindowDisplayIndex(window);
	//SDL_Rect display_bounds = SDL_GetDisplayBounds(display_current, display_bounds);
	struct stat buff;
	if (stat("window.cfg", &buff) != 0) return;
	SDL_Rect rect;
	FILE * fp = fopen("window.cfg", "r");
	fread(&rect, sizeof(struct SDL_Rect), 1, fp);
	fclose(fp);
	SDL_SetWindowPosition(window, rect.x, rect.y);
	SDL_SetWindowSize(window, rect.w, rect.h);
}

void window_state_save(SDL_Window * window) {
	SDL_Rect rect;
	SDL_GetWindowPosition(window, &rect.x, &rect.y);
	// XXX struggling to remember window size
	SDL_GetRendererOutputSize(SDL_GetRenderer(window), &rect.w, &rect.h);
//	SDL_GetWindowSize(window, &rect.w, &rect.h);
	FILE * fp = fopen("window.cfg", "w");
	fwrite(&rect, sizeof(struct SDL_Rect), 1, fp);
	fclose(fp);
}
