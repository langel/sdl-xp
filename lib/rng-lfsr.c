#include <stdint.h>

int rng_lfsr_8bit_next(uint8_t *val, int max) {
	*val ^= (*val & 0x07) << 5;
	*val ^= *val >> 3;
	*val ^= (*val & 0x03) << 6;
	return (int)( ( (float) *val / (float) 0xff) * (float) max );
}

int rng8(int max) {
	static uint8_t val = 1;
	return rng_lfsr_8bit_next(&val, max);
}

int rng_lfsr_16bit_next(uint16_t *val, int max) {
	*val ^= (*val & 0x07ff) << 5;
	*val ^= *val >> 7;
	*val ^= (*val & 0x0003) << 14;
	return (int)( ( (float) *val / (float) 0xffff) * (float) max );
}

int rng16(int max) {
	static uint16_t val = 1;
	return rng_lfsr_16bit_next(&val, max);
}

int rng_lfsr_32bit_next(uint32_t *val, int max) {
	*val ^= (*val & 0x0007ffff) << 13;
	*val ^= *val >> 17;
	*val ^= (*val & 0x07ffffff) << 5;
	return (int)( ( (float) *val / (float) 0xffffffff) * (float) max );
}

int rng32(int max) {
	static uint32_t val = 1;
	return rng_lfsr_32bit_next(&val, max);
}

/*
	32 bit rng lfsr does not seem to be working
	too many repeats in rng_test

	solution found here:
	https://stackoverflow.com/questions/65661856/how-to-do-an-8-bit-16-bit-and-32-bit-linear-feedback-shift-register-prng-in-ja

	should look more into Xorshift:
	https://en.wikipedia.org/wiki/Xorshift
*/


int rng32alt() {
	// stolen from: https://www.excamera.com/sphinx/article-xorshift.html
	/* if this proves to be superior we should add a custom seed option */
	static uint32_t seed = 7;
	seed ^= seed << 13;
	seed ^= seed >> 17;
	seed ^= seed << 5;
	return seed;
}

/*
	Noise Functions aka Hashes
	ideas from gdc video: https://www.youtube.com/watch?v=LWFzPP8ZbdU
	instead of sequencing pseudo random numbers apply hashes to linear ints
	python version of his hash (shouldn't be hard to make into c): https://github.com/sublee/squirrel3-python/blob/master/squirrel3.py
	he also recomended using std::hash
*/
