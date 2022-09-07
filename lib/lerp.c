
// imprecise; can perform floating point wonkiness
float lerp(float a, float b, float t) {
	return a + (b - a) * t;
}

// precise; guarantees return = b when t = 1
float lerpp(float a, float b, float t) {
	return (1 - t) * a + t * b;
}

// imprecise bilinear
float lerp2d(float a, float b, float c, float d, float tx, float ty) {
	return lerp(lerp(a, b, tx), lerp(c, d, tx), ty);
}

// precise bilinear
float lerp2dp(float a, float b, float c, float d, float tx, float ty) {
	return lerpp(lerpp(a, b, tx), lerpp(c, d, tx), ty);
}


