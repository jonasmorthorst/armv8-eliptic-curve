#include <stdio.h>

#include "basefield.h"
#include "utils.h"

/** 
 * basefield.c
 * 
 * Implementation of arithmetic for the basefield F_{2^127}.
 * The field elements have a little endian representation, 
 * meaning the least significant word has the lowest index.
 */ 
 
#define pow2to63 9223372036854775808U

void bf_print_expr(poly64x2_t p) {
	poly64_t c;
	int wasFirst = 1;
	for (int i=1; i>=0; i--) {
		// 2^63 = the value of the leftmost bit in a word
		c = pow2to63;
		for (int j = 63; j>=0; j--) {
			poly64_t polybitcopy = c & p[i];
			if (polybitcopy == c) {
				if(!wasFirst) {
					printf("+");
				}
				wasFirst = 0;
				
				if (i == 0 && j == 0) {
					printf("1");
				} else {
					printf("z^%d", (i)*64 +j);
				}
			}
			c /= 2;
		}
	}
	
	if (wasFirst) {
		printf("0");
	}
	printf("\n");
}

void bf_print_hex(poly64x2_t p) {
	printf("%016lx||%016lx\n", p[1], p[0]);
}

poly64x2_t bf_rand_elem() { 
	// 2^63-1 = 01111111...
	long c = pow2to63-1;
	
	poly64_t p0 = rand_uint64();
	poly64_t p1 = rand_uint64() & c;
	
	poly64x2_t p = {p0, p1};
	
	return p;
}

bf_polyx2 bf_pmull32(poly64x2_t a, poly64x2_t b) {
	poly64x2_t t;
	bf_polyx2 r;
	r.p0 = (poly64x2_t) vreinterpretq_u64_p128(vmull_p64(a[0], b[0]));
	r.p1 = (poly64x2_t) vreinterpretq_u64_p128(vmull_p64(a[1], b[1]));
	t[1] = (poly64_t) veor_u64((uint64x1_t) b[0], (uint64x1_t) b[1]);
	t[0] = (poly64_t) veor_u64((uint64x1_t) a[0], (uint64x1_t) a[1]);
	t = (poly64x2_t) vreinterpretq_u64_p128(vmull_p64(t[1], t[0]));
	t = (poly64x2_t) veorq_u64((uint64x2_t) t, (uint64x2_t) r.p0);
	t = (poly64x2_t) veorq_u64((uint64x2_t) t, (uint64x2_t) r.p1);
	r.p0[1] = (poly64_t) veor_u64((uint64x1_t) r.p0[1], (uint64x1_t) t[0]);
	r.p1[0] = (poly64_t) veor_u64((uint64x1_t) r.p1[0], (uint64x1_t) t[1]);
	
	return r;
}

bf_polyx2 bf_pmull64(poly64x2_t a, poly64x2_t b) {
	poly64x2_t t0, t1;
	poly64x2_t z = {0,0};
	bf_polyx2 r;
	
	r.p0 = (poly64x2_t) vreinterpretq_u64_p128(vmull_p64(a[0], b[0]));
	r.p1 = (poly64x2_t) vreinterpretq_u64_p128(vmull_high_p64(a, b));
	t0 = vextq_p64(b, b, 1);
	t1 = (poly64x2_t) vreinterpretq_u64_p128(vmull_p64(a[0], t0[0]));
	t0 = (poly64x2_t) vreinterpretq_u64_p128(vmull_high_p64(a, t0));
	t0 = (poly64x2_t) veorq_u64((uint64x2_t) t0, (uint64x2_t) t1);
	t1 = vextq_p64(z, t0, 1);
	r.p0 = (poly64x2_t) veorq_u64((uint64x2_t) r.p0, (uint64x2_t) t1);
	t1 = vextq_p64(t0, z, 1);
	r.p1 = (poly64x2_t) veorq_u64((uint64x2_t) r.p1, (uint64x2_t) t1);
	
	return r;
	
}

//First tried to find an arm bit-interleaving instruction, but later realised this works:
bf_polyx2 bf_psquare_neon(poly64x2_t a) {
	bf_polyx2 r;
	r.p0 = (poly64x2_t) vreinterpretq_u64_p128(vmull_p64(a[0], a[0]));
	r.p1 = (poly64x2_t) vreinterpretq_u64_p128(vmull_p64(a[1], a[1]));
	return r;
}

poly64x2_t bf_multisquare_loop(poly64x2_t a, uint64_t n) {
	for(int i = 0; i < n; i++) {
		a = bf_red_psquare(bf_psquare(a));
	}
	return a;
}

/* Alg 2.40 - Modular reduction (one bit at a time) */

int has_reduction_precomputed = 0;

poly64x2_t reduction_polynomials[64];

void bf_red_generic_precomp() {
	if(has_reduction_precomputed) {
		return;
	}
	has_reduction_precomputed = 1;
	reduction_polynomials[0][0] = pow2to63+1;
	reduction_polynomials[0][1] = 0;
	for(int i = 1; i < 64; i++) {
		reduction_polynomials[i][0] = reduction_polynomials[i-1][0] << 1;
		reduction_polynomials[i][1] = reduction_polynomials[i-1][1] << 1;
		if(i==1) {
			reduction_polynomials[i][1]++;
		}
	}
}

poly64x2_t bf_red_generic(bf_polyx2 c) {
	/* Step 1 */
	bf_red_generic_precomp();
	
	/* Step 2 */
	uint64_t digit_val = pow2to63 / 8;
	int index = 1;
	for(int digit = 252; digit >= 128; digit--) {
		/* Step 2.1 */
		if((c.p1[index] & digit_val) == digit_val) {
			int j = (digit - 127)/64; //Can be 0 or 1
			int k = (digit - 127) - 64*j;
			if(j == 1) {
				c.p0[1] = (poly64_t) veor_u64((uint64x1_t) reduction_polynomials[k][0], (uint64x1_t) c.p0[1]);
				c.p1[0] = (poly64_t) veor_u64((uint64x1_t) reduction_polynomials[k][1], (uint64x1_t) c.p1[0]);
			} else {
				c.p0 = bf_add(reduction_polynomials[k], c.p0);
			}
		}
		
		if(digit % 64 == 0) {
			digit_val = pow2to63;
			index--;
		} else {
			digit_val /= 2;
		}
	}
	if((c.p0[1] & pow2to63) == pow2to63) {
		c.p0[0] = reduction_polynomials[0][0] ^ c.p0[0];
		c.p0[1] &= pow2to63 - 1;
	}
	
	return c.p0;
}

//For future ref: vshlq_n_u64 doesn't carry from uint64x2_t[0] to [1]
poly64x2_t bf_red_formula(bf_polyx2 c) {
	poly64x2_t result = {0, 0};
	uint64_t bit127set = (c.p0[1] & pow2to63) == pow2to63;
	uint64_t bit191set = (c.p1[0] & pow2to63) == pow2to63;
	
	poly64x2_t term0to126 = {c.p0[0], c.p0[1] & (pow2to63 - 1)};
	result = bf_add(term0to126, result);
	
	poly64x2_t term252to127_rshift127 = {(c.p1[0] << 1) + bit127set, (c.p1[1] << 1) + bit191set};
	result = bf_add(term252to127_rshift127, result);
	
	poly64x2_t term190to127_rshift64 = {pow2to63*bit127set, c.p1[0] & (pow2to63 - 1)};
	result = bf_add(term190to127_rshift64, result);
	
	poly64x2_t term252to191_rshift128 = {pow2to63*bit191set, c.p1[1]};
	result = bf_add(term252to191_rshift128, result);
	
	poly64x2_t term252to191_rshift191 = {(c.p1[1] << 1) + bit191set, 0};
	result = bf_add(term252to191_rshift191, result);
	
	return result;
}
poly64x2_t bf_red_neon(bf_polyx2 c) {
	poly64x2_t t0, t1; 
	//t0={bit126to0, bit190to128}
	//t1={(bit127 XOR bit191)*2^63, bit191*2^63)}
	t0[0] = (poly64_t) veor_u64((uint64x1_t) c.p0[1], (uint64x1_t) c.p1[0]);
	t1[0] = (poly64_t) vand_u64((uint64x1_t) t0[0], (uint64x1_t) pow2to63);
	t1[1] = (poly64_t) vand_u64((uint64x1_t) c.p1[0], (uint64x1_t) pow2to63);
	t0[0] = (poly64_t) vand_u64((uint64x1_t) t0[0], (uint64x1_t) pow2to63-1);
	t0[1] = (poly64_t) vand_u64((uint64x1_t) c.p1[0], (uint64x1_t) pow2to63-1);
	
	//Recognize almost bf_red_psquare
	c.p1[0] = (poly64_t) veor_u64((uint64x1_t) t0[1], (uint64x1_t) c.p1[1]);
	c.p0[1] = (poly64_t) veor_u64((uint64x1_t) t0[0], (uint64x1_t) c.p1[1]);
	c.p1 = (poly64x2_t) vshlq_n_u64((uint64x2_t) c.p1, 1);
	
	c.p0[0] = (poly64_t) veor_u64((uint64x1_t) c.p0[0], (uint64x1_t) t1[0]);
	t1 = (poly64x2_t) vshrq_n_u64((uint64x2_t) t1, 63);
	c.p0 = (poly64x2_t) veorq_u64((uint64x2_t) c.p0, (uint64x2_t) t1);
	
	return (poly64x2_t) veorq_u64((uint64x2_t) c.p1, (uint64x2_t) c.p0);
}

//Carry shift is not needed here!! 191 bit is always 0.
poly64x2_t bf_red_psquare_formula(bf_polyx2 c) {
	poly64x2_t result = {0, 0};
	
	result = bf_add(c.p0, result);
	
	poly64x2_t term255to128_rshift127 = {c.p1[0] << 1, c.p1[1] << 1};
	result = bf_add(term255to128_rshift127, result);
	
	poly64x2_t term191to128_rshift64 = {0, c.p1[0]};
	result = bf_add(term191to128_rshift64, result);
	
	poly64x2_t term255to192_rshift128 = {0, c.p1[1]};
	result = bf_add(term255to192_rshift128, result);
	
	poly64x2_t term255to192_rshift191 = {c.p1[1] << 1, 0};
	result = bf_add(term255to192_rshift191, result);
	
	return result;
}

//veor3q not defined?
poly64x2_t bf_red_psquare_neon(bf_polyx2 c) {
	c.p1[0] = (poly64_t) veor_u64((uint64x1_t) c.p1[0], (uint64x1_t) c.p1[1]);
	c.p0[1] = (poly64_t) veor_u64((uint64x1_t) c.p0[1], (uint64x1_t) c.p1[0]);
	c.p1 = (poly64x2_t) vshlq_n_u64((uint64x2_t) c.p1, 1);
	return (poly64x2_t) veorq_u64((uint64x2_t) c.p0, (uint64x2_t) c.p1);
}

//Simple and slow, compute a^(-1) as a^((2^127)-2).
//Exploits the fact that 2^127 - 1 = 2^126 + 2^125 + ... + 2^1 + 1,
//hence a^-1 = a^(2^126)*...*a^(2^3)*a^(2^2)*a^2:
poly64x2_t bf_fermat_inv(poly64x2_t a) {
	poly64x2_t inv = {1, 0};
	poly64x2_t power = {a[0], a[1]};
	for(int i = 0; i < 126; i++) {
		power = bf_red_psquare(bf_psquare(power));
		inv = bf_red(bf_pmull(inv, power));
	}
	return inv;
}

//Addition chain for 126
//Addition chain means next term is sum of two previous terms.
// 1 -> 2 -> 3 -> 6 -> 12 -> 24 -> 30 -> 48 -> 96 -> 126
//Means need 9 multiplications & 126 squarings using Itoh & Tsujii alg
//In the end return (a^(2^126 -1))^2 = a^(2^127 -2) = a^-1 per Fermat
poly64x2_t bf_addition_chain_inv(poly64x2_t a) {
	poly64x2_t x_10 = bf_red_psquare(bf_psquare(a)); // 2
	poly64x2_t x_11 = bf_red(bf_pmull(a, x_10)); //1 + 2 = 3
	poly64x2_t x_110 = bf_red_psquare(bf_psquare(x_11)); //3*2 = 6
	poly64x2_t x_111 = bf_red(bf_pmull(a, x_110)); //1 + 6 = 7
	poly64x2_t x_111000 = bf_red_psquare(bf_psquare(bf_red_psquare(bf_psquare(bf_red_psquare(bf_psquare(x_111))))));
	//7*2^3 = 56
	poly64x2_t x_111111 = bf_red(bf_pmull(x_111, x_111000)); //56 + 7 = 2^6 - 1
	poly64x2_t x_x12 = bf_red(bf_pmull(bf_multisquare_loop(x_111111, 6), x_111111)); 
	//(2^6 -1)*2^6 + 2^6 - 1 = 2^12 - 1
	poly64x2_t x_x24 = bf_red(bf_pmull(bf_multisquare_loop(x_x12, 12), x_x12));
	//(2^12-1)*2^12 + 2^12 - 1 = 2^24 - 1
	poly64x2_t x_i34 = bf_multisquare_loop(x_x24, 6); //(2^24-1)*2^6 = 2^30 -2^6
	poly64x2_t x_x30 = bf_red(bf_pmull(x_111111, x_i34)); //(2^30-2^6) + 2^6 - 1 = 2^30 - 1
	poly64x2_t x_x48 = bf_red(bf_pmull(bf_multisquare_loop(x_i34, 18), x_x24));
	//(2^30 - 2^6)*2^18 + 2^24 - 1 = 2^48 - 1
	poly64x2_t x_x96 = bf_red(bf_pmull(bf_multisquare_loop(x_x48, 48), x_x48));
	//(2^48-1)*2^48 +2^48 - 1 = 2^96 -1
	poly64x2_t x_end = bf_red(bf_pmull(bf_multisquare_loop(x_x96, 30), x_x30));
	//(2^96-1)*2^30 + 2^30 - 1 = 2^126 - 1
	return bf_red_psquare(bf_psquare(x_end));
}

poly64x2_t bf_add(poly64x2_t a, poly64x2_t b) {
	return (poly64x2_t) veorq_u64((uint64x2_t) a, (uint64x2_t) b);
}

bf_polyx2 bf_pmull(poly64x2_t a, poly64x2_t b) {
	return bf_pmull64(a,b);
}

bf_polyx2 bf_psquare(poly64x2_t a) {
	return bf_psquare_neon(a);
}

poly64x2_t bf_red(bf_polyx2 c) {
	return bf_red_neon(c);
}

poly64x2_t bf_red_psquare(bf_polyx2 c) {
	return bf_red_psquare_neon(c);
}

poly64x2_t bf_inv(poly64x2_t a) {
	return bf_addition_chain_inv(a);
}
