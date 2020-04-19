/* 
 * Free FFT and convolution (C)
 * Copyright (c) 2017 Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/free-small-fft-in-multiple-languages
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "fft_N.h"

// Private function prototypes

static size_t reverse_bits(size_t x, int n);
static void * memdup(const void *src, size_t n);

// setup functions

static CFTTYPE * cos_table = 0;
static CFTTYPE * sin_table = 0;
static unsigned len = 0;
static size_t size = 0;

void Fft_setup(size_t n) {
	if ((cos_table) && (sin_table) && (len == n))
		return;
	len = (unsigned) n;
	size = (n / 2) * sizeof(CFTTYPE);
	if (cos_table) free(cos_table);
	cos_table = malloc(size);
	if (sin_table) free(sin_table);
	sin_table = malloc(size);
	if (cos_table == NULL || sin_table == NULL)
		goto oops;
	float nF = (float) n;
	float iF = 0.0f;
	for (size_t i = 0; i < n / 2; i++) {
		cos_table[i] = cosf(2.0f * M_PI * iF / nF);
		sin_table[i] = sinf(2.0f * M_PI * iF / nF);
		iF++;
	}
	return;
oops:
	if (cos_table) {
		free(cos_table);
		cos_table = 0;
	}
	if (sin_table) {
		free(sin_table);
		sin_table = 0;
	}
}

bool Fft_transform(CFTTYPE real[], CFTTYPE imag[], size_t n) {
	if (n == 0)
		return true;
	if (n != len) {
		printf("Need to re-init FFT\n");
		return false;
	}
	if ((n & (n - 1)) == 0) {				// Is power of 2
		return Fft_transformRadix2(real, imag, n);
	} else {  								// otherwise complain
		printf("Only power of 2 fft len supported\n");
		return false;
	}
}

bool Fft_inverseTransform(CFTTYPE real[], CFTTYPE imag[], size_t n) {
	return Fft_transform(imag, real, n);
}

bool Fft_transformRadix2(CFTTYPE real[], CFTTYPE imag[], size_t n) {
								// Length variables
	bool status = false;
	int levels = 0;				// Compute levels = floor(log2(n))
	for (size_t temp = n; temp > 1U; temp >>= 1)
		levels++;
	if ((size_t)1U << levels != n)
		return false;  			// n is not a power of 2
								// Trignometric tables
	if (SIZE_MAX / sizeof(CFTTYPE) < n / 2)
		return false;
								// Bit-reversed addressing permutation
	for (size_t i = 0; i < n; i++) {
		size_t j = reverse_bits(i, levels);
		if (j > i) {
			CFTTYPE temp = real[i];
			real[i] = real[j];
			real[j] = temp;
			temp = imag[i];
			imag[i] = imag[j];
			imag[j] = temp;
		}
	}
								// Cooley-Tukey decimation-in-time radix-2 FFT
	for (size_t size = 2; size <= n; size *= 2) {
		size_t halfsize = size / 2;
		size_t tablestep = n / size;
		for (size_t i = 0; i < n; i += size) {
			for (size_t j = i, k = 0; j < i + halfsize; j++, k += tablestep) {
				size_t l = j + halfsize;
				CFTTYPE tpre =  real[l] * cos_table[k] + imag[l] * sin_table[k];
				CFTTYPE tpim = -real[l] * sin_table[k] + imag[l] * cos_table[k];
				real[l] = real[j] - tpre;
				imag[l] = imag[j] - tpim;
				real[j] += tpre;
				imag[j] += tpim;
			}
		}
		if (size == n)  // Prevent overflow in 'size *= 2'
			break;
	}
	status = true;
cleanup:
	return status;
}

static size_t reverse_bits(size_t x, int n) {
	size_t result = 0;
	for (int i = 0; i < n; i++, x >>= 1)
		result = (result << 1) | (x & 1U);
	return result;
}

static void * memdup(const void *src, size_t n) {
	void *dest = malloc(n);
	if (n > 0 && dest != NULL)
		memcpy(dest, src, n);
	return dest;
}
