
int devpipe_watch_time;
char * devpipe_watch_file = "main.c";

int devpipe_get_watch_time() {
	struct stat buff;
	stat(devpipe_watch_file, &buff);
	return (int) buff.st_mtime;
}

void devpipe_init(SDL_Window * window) {
	devpipe_watch_time = devpipe_get_watch_time();
	window_focus(window);
}

int devpipe_check_update() {
	// returns true if watch file has been updated
	return (devpipe_watch_time == devpipe_get_watch_time()) ? 0 : 1;
}

void devpipe_kill_cycle() {
	FILE * fp = fopen("devpipe_death", "w");
	fclose(fp);
}
