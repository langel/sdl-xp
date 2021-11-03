
const double osc_tao = M_PI * 2;

float osc_noise(float pos) {
	return (((float)rand()/(float)(RAND_MAX)) * 2) - 1;
}

float osc_pulse(float pos, float width) {
	return ((pos > width) * 2) - 1;
}

float osc_saw(float pos) {
	return (pos * 2) - 1;
}

float osc_sin(float pos) {
	return sin(pos * osc_tao);
}

float osc_square(float pos) {
	return ((pos > 0.5) * 2) - 1;
}

float osc_triangle(float pos) {
	return (((pos <= 0.5) ? pos * 2 : (1 - pos) * 2) - 0.5);
}

