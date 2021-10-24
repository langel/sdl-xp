

// only works with one byte per row
SDL_Texture * char_rom_create_texture(SDL_Renderer * renderer, char * path, char char_height) {
	// load char rom data and realize texture size
	printf("loading %s into font texture\n", path);
	// XXX error handling would be nice? :P
	FILE * file = fopen(path, "rb");
	fseek(file, 0, SEEK_END);
	int file_length = ftell(file);
	printf("%d bytes - space for %d characters\n", file_length, file_length / 8);
	fseek(file, 0, SEEK_SET);
	char * buffer = malloc(file_length);
	fread(buffer, file_length, 1, file);
	// create texture of correct dimensions
	int char_count = file_length / char_height;
	SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, char_count * 8, char_height);
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	// populate texture with char rom data
	for (int i = 0; i < file_length; i++) {
		unsigned char byte = buffer[i];
		unsigned char y = i % char_height;
		for (unsigned char x = 0; x < 8; x++) {
			if ((1 << (7 - x)) & byte) {
				SDL_RenderDrawPoint(renderer, i - y + x, y);
			}
		}
	}
	free(buffer);
	SDL_SetRenderTarget(renderer, NULL);
	return texture;
}
