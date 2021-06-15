#include <stdio.h>
#include <stdint.h>
#include "lib/rng-lfsr.h"


int main(int argc, char *argv[]) {

	long x;
	uint8_t s8 = 1;
	uint16_t s16 = 1;
	uint32_t s32 = 1;
	int c8, c16, c32;
	int test_length = 9999999;

	printf("lsfr test length: %d\n", test_length);

	for (x = 0; x < test_length; x++) {
		rng_lfsr_8bit_next(&s8,1);
		if (s8 == 1) c8++;
		rng_lfsr_16bit_next(&s16,1);
		if (s16 == 1) c16++;
		rng_lfsr_32bit_next(&s32,1);
		if (s32 == 1) c32++;
	}

	printf("8bit repeats: %d or %f\n", c8, (float)c8 / (float)test_length);
	printf("16bit repeats: %d or %f\n", c16, (float)c16 / (float)test_length);
	printf("32bit repeats: %d or %f\n", c32, (float)c32 / (float)test_length);

	return 0;

}
