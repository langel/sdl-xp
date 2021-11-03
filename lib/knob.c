#define KNOB_TURN 270


typedef struct {
	float min;
	float max;
	float pos;
	float rot;
	float val;
	float curve;
	char label[32];
	SDL_Rect rect;
} knob;


void knob_update(knob * k) {
	k->rot = k->pos * KNOB_TURN - (KNOB_TURN * 0.5f);
	k->val = (k->max - k->min) * curve_apply(k->pos, k->curve) + k->min;
}


void knob_init(knob * k) {
	// set knob position from value
	k->pos = curve_apply( 
		value_to_range_pos(k->min, k->max, k->val),
		curve_inverse(k->curve));
	knob_update(k);
}

void knob_update_relative(knob * k, float rel) {
	k->pos += rel;
	if (k->pos > 1) k->pos = 1;	
	if (k->pos < 0) k->pos = 0;	
	knob_update(k);
}
