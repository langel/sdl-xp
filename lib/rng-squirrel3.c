
// discovered here : https://www.youtube.com/watch?v=LWFzPP8ZbdU

uint32_t squirrel3(uint32_t position, uint32_t seed) {
	const uint32_t BIT_NOISE1 = 0xB5297A4D;
	const uint32_t BIT_NOISE2 = 0x68E31DA4;
	const uint32_t BIT_NOISE3 = 0x1B56C4E9;

	uint32_t mangled = position;
	mangled *= BIT_NOISE1;
	mangled += seed;
	mangled ^= (mangled >> 8);
	mangled += BIT_NOISE2;
	mangled ^= (mangled << 8);
	mangled *= BIT_NOISE3;
	mangled ^= (mangled >> 8);
	return mangled;
}

uint32_t squirrel3_get_2d(int x, int y, uint32_t seed) {
	const int prime = 198491317;
	return squirrel3((uint32_t) (x + y * prime), seed);
}

uint32_t squirrel3_get_3d(int x, int y, int z, uint32_t seed) {
	const int prime1 = 198491317;
	const int prime2 = 6542989;
	return squirrel3((uint32_t) (x + y * prime1 + z * prime2), seed);
}

// returns float between 0 and 1
float squirrel3_zero_float(uint32_t position, uint32_t seed) {
	return (float) squirrel3(position, seed) / (float) 0xffffffff;
}

// returns float between -1 and 1
float squirrel3_neg_float(uint32_t position, uint32_t seed) {
	return ((float) squirrel3(position, seed) / (float) 0x7fffffff) - 1.f;
}
