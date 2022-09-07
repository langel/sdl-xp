
// LINEAR INTERPOLATION

// imprecise; can perform floating point wonkiness
float lerp(float a, float b, float t) {
	return a + (b - a) * t;
}

// precise; guarantees return = b when t = 1
float lerpp(float a, float b, float t) {
	return (1 - t) * a + t * b;
}

// imprecise bilinear
float bilerp(float a, float b, float c, float d, float tx, float ty) {
	return lerp(lerp(a, b, tx), lerp(c, d, tx), ty);
}

// precise bilinear
float bilerpp(float a, float b, float c, float d, float tx, float ty) {
	return lerpp(lerpp(a, b, tx), lerpp(c, d, tx), ty);
}


// CUBIC INTERPOLATION

// ripped and transcoded from https://www.paulinternet.nl/?page=bicubic
// imprecise cubic (1D)
float cuberp(float p[4], float x) {
	return p[1] + 0.5 * 
		x * (p[2] - p[0] + 
			x * (2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + 
				x * (3.0*(p[1] - p[2]) + p[3] - p[0])
			)
		);
}

// imprecise bicubic (2D)
float bicuberp(float p[4][4], float x, float y) {
	float arr[4];
	arr[0] = cuberp(p[0], y);
	arr[1] = cuberp(p[1], y);
	arr[2] = cuberp(p[2], y);
	arr[3] = cuberp(p[3], y);
	return cuberp(arr, x);
}

// imprecise tricubic (3D)
float tricuberp(float p[4][4][4], float x, float y, float z) {
	float arr[4];
	arr[0] = bicuberp(p[0], y, z);
	arr[1] = bicuberp(p[1], y, z);
	arr[2] = bicuberp(p[2], y, z);
	arr[3] = bicuberp(p[3], y, z);
	return cuberp(arr, x);
}
