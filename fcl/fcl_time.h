// some more ideas about timing here:
// https://thenumbat.github.io/cpp-course/sdl2/08/08.html

static uint8_t fcl_time_fps = 60;
static float fcl_time_frame_length = 1000.f;
static float fcl_time_frame_pos = 0.f;
static uint32_t fcl_time_next_frame = 0;

void fcl_time_set_fps(int fps) {
	fcl_time_fps = fps;
	fcl_time_frame_length = 1000.f / (float)fps;
}

void fcl_time_wait_next_frame() {
	//printf("%7f  ", fcl_time_frame_pos);
	uint32_t now = SDL_GetTicks();
	//printf("%d  ", now);
	uint32_t huh = 0;
	if (now < fcl_time_next_frame) {
		huh = fcl_time_next_frame - now;
		SDL_Delay(huh);
	}
	//printf("%3d  ", huh); 
	fcl_time_frame_pos += fcl_time_frame_length;
	fcl_time_next_frame = (uint32_t)fcl_time_frame_pos;
	//printf("%d  %5f\n", fcl_time_next_frame, fcl_time_frame_length);
}
