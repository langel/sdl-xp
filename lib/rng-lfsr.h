
int rng_lfsr_8bit_next(unsigned char *val, int max) {
	*val ^= (*val & 0x07) << 5;
	*val ^= *val >> 3;
	*val ^= (*val & 0x03) << 6;
	return (int)( ( (float) *val / (float) 0xff) * (float) max );
}

int rng8(int max) {
	static unsigned char val = 1;
	return rng_lfsr_8bit_next(&val, max);
}

int rng_lfsr_16bit_next(unsigned short *val, int max) {
	*val ^= (*val & 0x07ff) << 5;
	*val ^= *val >> 7;
	*val ^= (*val & 0x0003) << 14;
	return (int)( ( (float) *val / (float) 0xffff) * (float) max );
}

int rng16(int max) {
	static unsigned short val = 1;
	return rng_lfsr_16bit_next(&val, max);
}

int rng_lfsr_32bit_next(unsigned int *val, int max) {
	*val ^= (*val & 0x0007ffff) << 13;
	*val ^= *val >> 17;
	*val ^= (*val & 0x07ffffff) << 5;
	return (int)( ( (float) *val / (float) 0xffffffff) * (float) max );
}

int rng32(int max) {
	static unsigned int val = 1;
	return rng_lfsr_32bit_next(&val, max);
}

