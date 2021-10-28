#include <string.h>

typedef struct char_rom {
	int char_count;
	unsigned char char_w;
	unsigned char char_h;
	SDL_Texture * texture;
	int texture_w;
	int texture_h;
} char_rom;

// only works with one byte per row

struct char_rom char_rom_create_texture_variable_height(SDL_Renderer * renderer, char * path, char char_height) {
	char_rom font;
	font.char_w = 8;
	font.char_h = char_height;
	// load char rom data and realize texture size
	printf("loading %s into font texture\n", path);
	// XXX error handling would be nice? :P
	FILE * file = fopen(path, "rb");
	fseek(file, 0, SEEK_END);
	int file_length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char * buffer = malloc(file_length);
	fread(buffer, file_length, 1, file);
	// create texture of correct dimensions
	font.char_count = file_length / char_height;
	printf("%d bytes - space for %d characters\n", file_length, font.char_count);
	SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, font.char_count * 8, font.char_h);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
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
	// cleanup & return
	free(buffer);
	SDL_SetRenderTarget(renderer, NULL);
	font.texture = texture;
	return font;
}

struct char_rom char_rom_create_texture(SDL_Renderer * renderer, char * path) {
	return char_rom_create_texture_variable_height(renderer, path, 8);
}

SDL_Rect char_rom_get_rect(char_rom font, int char_index) {
	SDL_Rect temp = { char_index * font.char_w, 0, font.char_w, font.char_h };
	return temp;
}

SDL_Texture * char_rom_get_texture_from_string(SDL_Renderer * renderer, char_rom font, char * string) {
	// stash render target
	SDL_Texture * stashed_target = SDL_GetRenderTarget(renderer);
	int string_length = strlen(string);
	SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, string_length * font.char_w, font.char_h);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_Rect src;
	SDL_Rect dest = { 0, 0, font.char_w, font.char_h };
	for (int i = 0; i < string_length; i++) {
		src = char_rom_get_rect(font, string[i]);
		SDL_RenderCopy(renderer, font.texture, &src, &dest);
		dest.x += font.char_w;
	}
	SDL_SetRenderTarget(renderer, stashed_target);
	return texture;
}
