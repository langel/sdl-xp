#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "lib/core.c"

#define NOTE_COUNT 25
#define FREQ_CENTER 440
#define SAMPLE_RATE 32000

int window_w = 800;
int window_h = 400;

int keys_pressed[256];
int notes_on[NOTE_COUNT];
int note_to_scancode[NOTE_COUNT] = {
	29, // C-0
	22, // C#0
	27, // D-0
	7,  // D#0
	6,  // E-0
	25, // F-0
	10, // F#0
	5,  // G-0
	11, // G#0
	17, // A-0
	13, // A#0
	16, // B-0
	20, // C-1
	31, // C#1
	26, // D-1
	32, // D#1
	8,  // E-1
	21, // F-1
	34, // F#1
	23, // G-1
	35, // G#1
	28, // A-1
	36, // A#1
	24, // B-1
	12, // C-2
};
char accidentals[7] = { 1, 1, 0, 1, 1, 1, 0 };
int white_keys_to_note[15] = { 0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24 };
int black_keys_to_note[14] = { 1, 3, -1, 6, 8, 10, -1, 13, 15, -1, 18, 20, 22, -1 };
float note_freq[NOTE_COUNT];
float note_duty_len[NOTE_COUNT];
float note_duty_pos[NOTE_COUNT];
int note_trigger_time[NOTE_COUNT];
unsigned long time_counter = 0;

typedef struct {
	int attack;
	int decay;
	float sustain;
	int release;
} adsr_envelope;
adsr_envelope amp_adsr;
adsr_envelope filter_adsr;

void note_freq_init() {
	int note_offset = -9 - 12;
	for (int i = 0; i < NOTE_COUNT; i++) {
		note_freq[i] = FREQ_CENTER * powf(1.059463, note_offset + i);
		note_duty_len[i] = SAMPLE_RATE / note_freq[i];
		note_duty_pos[i] = 0.f;
		printf("note id: %3d    note freq: %12.8f    note duty: %12.8f \n", i, note_freq[i], note_duty_len[i]);
	}
}

float lfo_pos;

void audio_callback(void* userdata, uint8_t* byte_stream, int byte_stream_length) {
	float * float_stream = (float*) byte_stream;
	int float_stream_length = byte_stream_length >> 2;
	for (int i = 0; i < float_stream_length; i += 2) {
		float output = 0.f;
		for (int j = 0; j < NOTE_COUNT; j++) {
			float amp = 0.f;
			int envelope_timer = time_counter - note_trigger_time[j];
			if (notes_on[j]) {
				// is attack?
				if (envelope_timer < amp_adsr.attack) {
					amp = (float) envelope_timer / (float) amp_adsr.attack;
				}
				// is decay?
				else if (envelope_timer < amp_adsr.decay) {
					amp = ((1.f - amp_adsr.sustain) * (1.f - value_to_range_pos((float) amp_adsr.attack, (float) (amp_adsr.attack + amp_adsr.decay), (float) envelope_timer))) + amp_adsr.sustain;
				}
				// is sustain?
				else if (envelope_timer >= amp_adsr.decay) {
					amp = amp_adsr.sustain;
				}
			}
			// note is off or released?
			else {
				if (envelope_timer < amp_adsr.release) {
					amp = (1.f - ((float) envelope_timer / (float) amp_adsr.release)) * amp_adsr.sustain;
				}
			}
			note_duty_pos[j] += 1.f;
			if (note_duty_pos[j] > note_duty_len[j]) {
				note_duty_pos[j] -= note_duty_len[j];
			}
			amp *= amp;
			output += osc_square(note_duty_pos[j] / note_duty_len[j]) * amp * 0.25f;
		}
		float_stream[i] = output;
		float_stream[i+1] = output;
		lfo_pos += 0.00003f;
		time_counter++;
	}
}

void log_spec(SDL_AudioSpec *as) {
	printf(
		" freq______%5d\n"
		" format____%5d\n"
		" channels__%5d\n"
		" silence___%5d\n"
		" samples___%5d\n"
		" size______%5d\n\n",
		(int) as->freq,
		(int) as->format,
		(int) as->channels,
		(int) as->silence,
		(int) as->samples,
		(int) as->size
	);
}

SDL_AudioSpec audio_spec;
SDL_AudioSpec audio_actual;
SDL_AudioDeviceID audio_device;
void audio_init() {
	audio_spec.freq = SAMPLE_RATE;
	audio_spec.format = AUDIO_F32SYS;
	audio_spec.channels = 2;
	audio_spec.samples = 1024;
	audio_spec.callback = audio_callback;
	log_spec(&audio_spec);
	audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, &audio_actual, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (audio_device == 0) {
		SDL_Log("Failed to open audio: %s\n", SDL_GetError());
	}
	else {
		printf("device initialized\n");
		log_spec(&audio_actual);
		SDL_PauseAudioDevice(audio_device, 0);
	}
}


SDL_Texture * keyboard_texture;

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

#define KNOB_COUNT 4
knob knobs[KNOB_COUNT] = {
	// ADSR1 attack
	{ 0.005f, 25.f, 0.f, 0.f, 0.5f, 2.5f,
		"ATTACK", { 10, 10, 72, 72 } },
	// ADSR1 decay
	{ 0.005f, 25.f, 0.f, 0.f, 0.1f, 2.5f,
		"DECAY", { 92, 10, 72, 72 } },
	// ADSR1 sustain
	{ 0.f, 1.f, 0.f, 0.f, 0.75f, 0.5f,
		"SUSTAIN", { 174, 10, 72, 72 } },
	// ADSR1 release
	{ 0.005f, 25.f, 0.f, 0.f, 0.250f, 2.5f,
		"RELEASE", { 256, 10, 72, 72 } },
};

void notes_update() {
	for (int i = 0; i < NOTE_COUNT; i++) {
	}
}


int main(int argc, char* args[]) {

	SDL_Init(SDL_INIT_EVERYTHING);
	note_freq_init(); 
	audio_init();

	SDL_Event event;
	SDL_Rect window_rect = { 200, 200, window_w, window_h };
	SDL_Window * window = SDL_CreateWindow("the synth?", window_rect.x, window_rect.y, window_rect.w, window_rect.h, SDL_WINDOW_RESIZABLE);
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

	keyboard_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 334, 104);
	SDL_Rect keyboard_rect = { 0, window_h / 2, window_w, window_h / 2 };
	SDL_Rect keys_white_rect = { 0, 0, 20, 100 };
	SDL_Rect keys_black_rect = { 0, 0, 10, 60 };


	// knob texture
	SDL_Texture * knob_texture = texture_from_image(renderer, "assets/knob.png");
	for (int i = 0; i < KNOB_COUNT; i++) knob_init(&knobs[i]);

	// mouse cursor
	mouse_info mouse = mouse_init();
	int mouse_grab = 0;
	int mouse_hover = 0;
	int mouse_target = 0; // knob id of grabbed knob
	char mouse_info_str[20];
	SDL_ShowCursor(SDL_DISABLE);
	//SDL_CaptureMouse(SDL_TRUE);
	SDL_Rect mouse_cursor_rect = { 0, 0, 16, 16 };
	SDL_Rect mouse_hotspot = { 0, 0, 4, 4 };
	SDL_Texture * mouse_hand_closed = texture_from_image(renderer, "assets/cursor_hand_closed.png");
	SDL_Texture * mouse_hand_open = texture_from_image(renderer, "assets/cursor_hand_open.png");
	SDL_Texture * mouse_pointer = texture_from_image(renderer, "assets/cursor_pointer.png");
	

	int running = 1;
	while (running) {

		SDL_SetRenderTarget(renderer, NULL);
		renderer_set_color(renderer, &palette[7]);
		SDL_RenderClear(renderer);
		
		// process knobs
		mouse_hover = 0;
		// mouse draws
		mouse_cursor_rect.x = mouse.x;
		mouse_hotspot.x = mouse_cursor_rect.x;
		mouse_cursor_rect.y = mouse.y;
		mouse_hotspot.y = mouse_cursor_rect.y;
		for (int i = 0; i < KNOB_COUNT; i++) {
			SDL_RenderCopyEx(renderer, knob_texture, NULL, &knobs[i].rect, knobs[i].rot, NULL, SDL_FLIP_NONE);
			if (collision_detection(knobs[i].rect, mouse_hotspot)) {
				mouse_hover = 1;
				if (!mouse_grab) mouse_target = i;
			}
		}
		if (mouse_hover && mouse.button_left) mouse_grab = 1;
		if (!mouse.button_left) mouse_grab = 0;
		if (mouse_grab) {
			// change value based on mouse movement
			knob_update_relative(&knobs[mouse_target], (mouse.rel_x - mouse.rel_y) * 0.001f);
			SDL_RenderCopy(renderer, mouse_hand_closed, NULL, &mouse_cursor_rect);
			printf("attack: %6.3f  decay: %6.3f  sustain: %6.3f  release: %6.3f\n", knobs[0].val, knobs[1].val, knobs[2].val, knobs[3].val);
			printf("attack: %6d  decay: %6d  sustain: %6.3f  release: %6d\n", amp_adsr.attack, amp_adsr.decay, amp_adsr.sustain, amp_adsr.release);
		}
		else if (mouse_hover) {
			SDL_RenderCopy(renderer, mouse_hand_open, NULL, &mouse_cursor_rect);
		}
		else {
			SDL_RenderCopy(renderer, mouse_pointer, NULL, &mouse_cursor_rect);
		}
		amp_adsr.attack = (int) (knobs[0].val * (float) SAMPLE_RATE);
		amp_adsr.decay = (int) (knobs[1].val * (float) SAMPLE_RATE);
		amp_adsr.sustain = knobs[2].val;
		amp_adsr.release = (int) (knobs[3].val * (float) SAMPLE_RATE);

		SDL_SetRenderTarget(renderer, keyboard_texture);
		renderer_set_color(renderer, &palette[6]);
		SDL_RenderClear(renderer);

		// which notes are on?
		for (int i = 0; i < NOTE_COUNT; i++) {
			if (keys_pressed[note_to_scancode[i]]) {
				if (notes_on[i] == 0) note_trigger_time[i] = time_counter;
				notes_on[i] = 1;
			}
			else {
				if (notes_on[i] == 1) note_trigger_time[i] = time_counter;
				notes_on[i] = 0;
			}
		}
		// white keys
		for (int i = 0; i < 15; i++) {
			if (notes_on[white_keys_to_note[i]]) {
				renderer_set_color(renderer, &palette[2]); 
			}
			else {
				renderer_set_color(renderer, &palette[0]);
			}
			keys_white_rect.x = i * 22 + 2;
			SDL_RenderFillRect(renderer, &keys_white_rect);
		}
		// black keys
		for (int i = 0; i < 14; i++) {
			if (accidentals[i % 7]) {
				if (notes_on[black_keys_to_note[i]]) {
					renderer_set_color(renderer, &palette[2]); 
				}
				else {
					renderer_set_color(renderer, &palette[7]);
				}
				keys_black_rect.x = i * 22 + 18;
				SDL_RenderFillRect(renderer, &keys_black_rect);
			}
		}


		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, keyboard_texture, NULL, &keyboard_rect);
		SDL_RenderPresent(renderer);

		mouse_process(&mouse, &window_rect);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYDOWN:
					if (keys_pressed[event.key.keysym.scancode] == 0) {
						keys_pressed[event.key.keysym.scancode] = 1;
		//				printf( "keydown: %8s %3d\n", SDL_GetKeyName(event.key.keysym.sym), event.key.keysym.scancode);
					}
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = 0;
							break;
					}
					break;
				case SDL_KEYUP:
					keys_pressed[event.key.keysym.scancode] = 0;
		//			printf( "  keyup: %8s %3d\n", SDL_GetKeyName(event.key.keysym.sym), event.key.keysym.scancode);
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_MOVED) {
						window_rect.x = event.window.data1;
						window_rect.y = event.window.data2;
						printf("window position changed: %d x %d\n", window_rect.x, window_rect.y);
					}
					if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						window_w = event.window.data1;
						window_h = event.window.data2;
						window_rect.w = window_w;
						window_rect.h = window_h;
						printf("window size changed: %d x %d\n", window_w, window_h);
					}
					break;
			}
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_CloseAudio();
	SDL_Quit();
	return 0;
}
