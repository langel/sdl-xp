#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "char_roms/_lib.c"

#define KNOB_COUNT 4
#define KNOB_TURN 270
#define OVERSCALE_MUL 4

int texture_w = 420;
int texture_h = 200;
int window_w = 960;
int window_h = 540;
float scale_x;
float scale_y;

SDL_Color palette[8] = {
	{ 0xf0, 0xf0, 0xdc, 0xff }, // white
	{ 0xfa, 0xc8, 0x00, 0xff }, // yellow
	{ 0x10, 0xc8, 0x40, 0xff }, // green
	{ 0x00, 0xa0, 0xc8, 0xff }, // blue
	{ 0xd2, 0x40, 0x40, 0xff }, // red
	{ 0xa0, 0x69, 0x4b, 0xff }, // brown
	{ 0x73, 0x64, 0x64, 0xff }, // grey
	{ 0x10, 0x18, 0x20, 0xff }, // black
};

typedef struct {
	float max;
	float min;
	float pos;
	float rot;
	float val;
	SDL_Rect rect;
} knob;

knob knobs[4] = {
	// coarse tune
	{ 22000.f, 22.f, 0.f, 0.f, 420.f, { 30, 20, 72, 72 } },
	// fine tune
	{ 1.f, -1.f, 0.f, 0.f, 0.f, { 30, 120, 72, 72 } },
	// cutoff
	{ 22000.f, 22.f, 0.f, 0.f, 22000.f, { 200, 20, 72, 72 } },
	// res
	{ 10.f, 0.f, 0.f, 0.f, 0.f, { 200, 120, 72, 72 } },
};

void knob_update(knob * knob_ent) {
	if (knob_ent->val > knob_ent->max) knob_ent->val = knob_ent->max;
	if (knob_ent->val < knob_ent->min) knob_ent->val = knob_ent->min;
	knob_ent->pos = (knob_ent->val - knob_ent->min) / (knob_ent->max - knob_ent->min);
	knob_ent->rot = knob_ent->pos * KNOB_TURN - (KNOB_TURN * 0.5f);
//	printf("%f %f %f %f\n", rel, knob_ent->val, knob_ent->pos, knob_ent->rot);
}

void knob_update_relative(knob * knob_ent, float rel) {
	// val is between min and max
	// rot is between -135 and 135
	knob_ent->val += rel * ((knob_ent->max - knob_ent->min) / KNOB_TURN);
	knob_update(knob_ent);
}

int collision_detection(SDL_Rect a, SDL_Rect b) {
	if (a.x + a.w < b.x) return 0;
	if (a.x > b.x + b.w) return 0;
	if (a.y + a.h < b.y) return 0;
	if (a.y > b.y + b.h) return 0;
	return 1;
}

SDL_Texture * create_texture_from_image(SDL_Renderer * renderer, char * path) {
	SDL_Surface * surface = IMG_Load(path);
	if (!surface) {
		printf("Failed to load image at %s: %s\n", path, SDL_GetError());
//		return 1;
	}
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void set_color(SDL_Renderer * renderer, int color_id) {
	SDL_SetRenderDrawColor(renderer, palette[color_id].r, palette[color_id].g, palette[color_id].b, palette[color_id].a);
}

void update_scale() {
	scale_x = (float) texture_w / (float) window_w;
	scale_y = (float) texture_h / (float) window_h;
	printf("scale_x : %f\n", scale_x);
}


int main(int argc, char* args[]) {

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG);
	SDL_Event event;
	SDL_Window * window = SDL_CreateWindow("knob town", 100, 200,
		window_w, window_h, SDL_WINDOW_RESIZABLE);
	SDL_Renderer * renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_PRESENTVSYNC);
	update_scale();

	// font struct
	char_rom font = char_rom_create_texture(renderer, "char_roms/eagle_pc_cga.bin");
	SDL_Texture * font_texture;
	SDL_Rect font_rect = { 0, 0, 64, 8 };
	char font_string[10];

	// knob texture
	const char * knob_path = "assets/knob-small.png";
	SDL_Surface * knob_surface = IMG_Load(knob_path);
	if (!knob_surface) {
		printf("Failed to load image at %s: %s\n", knob_path, SDL_GetError());
		return 1;
	}
	SDL_Texture * knob_texture = SDL_CreateTextureFromSurface(renderer, knob_surface);
	SDL_SetTextureBlendMode(knob_texture, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(knob_surface);
	float knob_rot = 0.f;
	float knob_speed = 0.777f;
	float knob_max_turn = 270.f;
	for (int i = 0; i < KNOB_COUNT; i++) knob_update(&knobs[i]);

	// mouse cursor
	int mouse_x, mouse_y;
	int mouse_rel_x, mouse_rel_y;
	int mouse_grab = 0;
	int mouse_hover = 0;
	int mouse_target = 0; // knob id of grabbed knob
	uint32_t mouse_buttons;
	SDL_ShowCursor(SDL_DISABLE);
	//SDL_CaptureMouse(SDL_TRUE);
	SDL_Rect mouse_cursor_rect = { 0, 0, 16, 16 };
	SDL_Rect mouse_hotspot = { 0, 0, 4, 4 };
	SDL_Texture * mouse_hand_closed = create_texture_from_image(renderer, "assets/cursor_hand_closed.png");
	SDL_Texture * mouse_hand_open = create_texture_from_image(renderer, "assets/cursor_hand_open.png");
	SDL_Texture * mouse_pointer = create_texture_from_image(renderer, "assets/cursor_pointer.png");


	// canvas target
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	SDL_Texture * canvas_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, texture_w, texture_h);

	// overscale target
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
	SDL_Texture * overscale_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, texture_w * OVERSCALE_MUL, texture_h * OVERSCALE_MUL);


	int running = 1;
	while (running) {

		// update all assets aka graphics process
		// graphics process
		SDL_SetRenderTarget(renderer, canvas_texture);
		set_color(renderer, 3);
		SDL_RenderClear(renderer);
		// knob draws
		for (int i = 0; i < KNOB_COUNT; i++) {
			// actual knob
			SDL_RenderCopyEx(renderer, knob_texture, NULL, &knobs[i].rect, knobs[i].rot, NULL, SDL_FLIP_NONE);
			// knob value
			sprintf(font_string, "% 8.1f", knobs[i].val);
			font_texture = char_rom_get_texture_from_string(renderer, font, font_string);
			font_rect.x = knobs[i].rect.x + 72;
			font_rect.y = knobs[i].rect.y + 32;
//			SDL_SetRenderTarget(renderer, canvas_texture);
			SDL_RenderCopy(renderer, font_texture, NULL, &font_rect);
			SDL_DestroyTexture(font_texture);
		}
		knob_rot += knob_speed;

		// mouse draws
		if (mouse_x > 0 && mouse_x < window_w - 1 && mouse_y > 0 && mouse_y < window_h - 1) {
			mouse_cursor_rect.x = (int) ((float) mouse_x * scale_x) - 7;
			mouse_hotspot.x = mouse_cursor_rect.x;
			mouse_cursor_rect.y = (int) ((float) mouse_y * scale_y) - 7;
			mouse_hotspot.y = mouse_cursor_rect.y;

			// if hover and mouse down : mouse = grab
			// if mouse = grab : adjust knob on mouse move
			// if mouse = grab and mouse up : mouse = normal

			mouse_hover = 0;
			for (int i = 0; i < KNOB_COUNT; i++) {
				if (collision_detection(knobs[i].rect, mouse_hotspot)) {
					mouse_hover = 1;
//					knobs[i].rot += 0.5f;
					if (!mouse_grab) mouse_target = i;
				}
			}
			if (mouse_hover && (mouse_buttons & 1)) mouse_grab = 1;
			if (!(mouse_buttons & 1)) mouse_grab = 0;
			if (mouse_grab) {
				// change value based on mouse movement
				knob_update_relative(&knobs[mouse_target], (mouse_rel_x - mouse_rel_y) * 0.5f);
				SDL_RenderCopy(renderer, mouse_hand_closed, NULL, &mouse_cursor_rect);
			}
			else if (mouse_hover) {
				SDL_RenderCopy(renderer, mouse_hand_open, NULL, &mouse_cursor_rect);
			}
			else {
				SDL_RenderCopy(renderer, mouse_pointer, NULL, &mouse_cursor_rect);
			}
		}

		// graphics overscale
		SDL_SetRenderTarget(renderer, overscale_texture);
		SDL_RenderCopy(renderer, canvas_texture, NULL, NULL);
		// graphic display
		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, overscale_texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		// MOUSE SHIT
		mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
		SDL_GetRelativeMouseState(&mouse_rel_x, &mouse_rel_y);
		// EVENT SHIT
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYDOWN:
					printf( "keydown: %8s\n", SDL_GetKeyName( event.key.keysym.sym ) );
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = 0;
							break;
					}
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						window_w = event.window.data1;
						window_h = event.window.data2;
						printf("window size changed: %d x %d\n", window_w, window_h);
						update_scale();
					}
					break;
			}
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
	return 0;
}
