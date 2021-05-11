#include <arm_neon.h>

#ifndef BASEFIELD_H
#define BASEFIELD_H

//Formal interface

#define pow2to63 9223372036854775808U


void bf_print_expr(poly64x2_t p);

void bf_print_expr_nl(poly64x2_t p);

void bf_print_hex(poly64x2_t p);

void bf_print_hex_nl(poly64x2_t p);

static inline poly64x2_t bf_create_elem(uint64_t l, uint64_t h) {
	poly64x2_t a = {l, h};
	return a;
}

poly64x2_t bf_rand_elem();

static inline poly64x2_t bf_add(poly64x2_t a, poly64x2_t b) {
	return (poly64x2_t) veorq_u64((uint64x2_t) a, (uint64x2_t) b);
}

static inline poly64x2x2_t bf_pmull(poly64x2_t a, poly64x2_t b) {
	poly64x2_t t0, t1;
	poly64x2_t z = {0,0};
	poly64x2x2_t r;

	r.val[0] = (poly64x2_t) vreinterpretq_u64_p128(vmull_p64(a[0], b[0]));
	r.val[1] = (poly64x2_t) vreinterpretq_u64_p128(vmull_high_p64(a, b));
	t0 = vextq_p64(b, b, 1);
	t1 = (poly64x2_t) vreinterpretq_u64_p128(vmull_p64(a[0], t0[0]));
	t0 = (poly64x2_t) vreinterpretq_u64_p128(vmull_high_p64(a, t0));
	t0 = (poly64x2_t) veorq_u64((uint64x2_t) t0, (uint64x2_t) t1);
	t1 = vextq_p64(z, t0, 1);
	r.val[0] = (poly64x2_t) veorq_u64((uint64x2_t) r.val[0], (uint64x2_t) t1);
	t1 = vextq_p64(t0, z, 1);
	r.val[1] = (poly64x2_t) veorq_u64((uint64x2_t) r.val[1], (uint64x2_t) t1);

	return r;

}

static inline poly64x2x2_t bf_psquare(poly64x2_t a) {
  poly64x2x2_t r;
	r.val[0] = (poly64x2_t) vreinterpretq_u64_p128(vmull_p64(a[0], a[0]));
	r.val[1] = (poly64x2_t) vreinterpretq_u64_p128(vmull_p64(a[1], a[1]));
	return r;
}

static inline poly64x2_t bf_red(poly64x2x2_t c) {
	poly64x2_t t0, t1;
	//t0={bit126to0, bit190to128}
	//t1={(bit127 XOR bit191)*2^63, bit191*2^63)}
	t0[0] = (poly64_t) veor_u64((uint64x1_t) c.val[0][1], (uint64x1_t) c.val[1][0]);
	t1[0] = (poly64_t) vand_u64((uint64x1_t) t0[0], (uint64x1_t) pow2to63);
	t1[1] = (poly64_t) vand_u64((uint64x1_t) c.val[1][0], (uint64x1_t) pow2to63);
	t0[0] = (poly64_t) vand_u64((uint64x1_t) t0[0], (uint64x1_t) pow2to63-1);
	t0[1] = (poly64_t) vand_u64((uint64x1_t) c.val[1][0], (uint64x1_t) pow2to63-1);

	//Recognize almost bf_red_psquare
	c.val[1][0] = (poly64_t) veor_u64((uint64x1_t) t0[1], (uint64x1_t) c.val[1][1]);
	c.val[0][1] = (poly64_t) veor_u64((uint64x1_t) t0[0], (uint64x1_t) c.val[1][1]);
	c.val[1] = (poly64x2_t) vshlq_n_u64((uint64x2_t) c.val[1], 1);

	c.val[0][0] = (poly64_t) veor_u64((uint64x1_t) c.val[0][0], (uint64x1_t) t1[0]);
	t1 = (poly64x2_t) vshrq_n_u64((uint64x2_t) t1, 63);
	c.val[0] = (poly64x2_t) veorq_u64((uint64x2_t) c.val[0], (uint64x2_t) t1);

	return (poly64x2_t) veorq_u64((uint64x2_t) c.val[1], (uint64x2_t) c.val[0]);
}

static inline poly64x2_t bf_red_psquare(poly64x2x2_t c) {
  poly64x2_t t;
  t[0] = 0;
  t = vextq_p64(c.val[1], t, 1);
  c.val[1] = (poly64x2_t) veorq_u64((uint64x2_t) c.val[1], (uint64x2_t) t);
  t = vextq_p64(t, c.val[1], 1);
  c.val[0] = (poly64x2_t) veorq_u64((uint64x2_t) c.val[0], (uint64x2_t) t);
  c.val[1] = (poly64x2_t) vshlq_n_u64((uint64x2_t) c.val[1], 1);
  return (poly64x2_t) veorq_u64((uint64x2_t) c.val[0], (uint64x2_t) c.val[1]);
}

static inline poly64x2_t bf_multisquare_loop(poly64x2_t a, uint64_t n) {
	for(int i = 0; i < n; i++) {
		a = bf_red_psquare(bf_psquare(a));
	}
	return a;
}

poly64x2_t bf_inv(poly64x2_t a);

//Implementation alternatives:

poly64x2x2_t bf_pmull32(poly64x2_t a, poly64x2_t b);

poly64x2x2_t bf_pmull64(poly64x2_t a, poly64x2_t b);

poly64x2_t bf_red_generic(poly64x2x2_t c);

poly64x2_t bf_red_formula(poly64x2x2_t c);

poly64x2_t bf_red_neon(poly64x2x2_t c);

poly64x2_t bf_red_psquare_formula(poly64x2x2_t c);

poly64x2_t bf_red_psquare_neon(poly64x2x2_t c);

poly64x2_t bf_red_psquare_neonv2(poly64x2x2_t c);

poly64x2_t bf_fermat_inv(poly64x2_t a);

poly64x2_t bf_addchain_inv(poly64x2_t a);

poly64x2_t bf_addchain_lookup_inv(poly64x2_t a);

#endif
