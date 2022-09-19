
int devpipe_watch_time;
char * devpipe_watch_file = "main.c";

int devpipe_get_watch_time() {
	struct stat attr;
	stat(devpipe_watch_file, &attr);
	return (int) attr.st_mtime;
}

void devpipe_init() {
	devpipe_watch_time = devpipe_get_watch_time();
}

int devpipe_check_update() {
	// returns true if watch file has been updated
	return (devpipe_watch_time == devpipe_get_watch_time()) ? 0 : 1;
}

void devpipe_kill_cycle() {
	FILE * fp = fopen("devpipe_death", "w");
	fclose(fp);
}
