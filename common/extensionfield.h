#include <arm_neon.h>
#include "basefield.h"

#ifndef EXTENSIONFIELD_H
#define EXTENSIONFIELD_H

typedef poly64x2x2_t ef_elem;

static inline ef_elem ef_create_elem(poly64x2_t p0, poly64x2_t p1) {
	ef_elem a;
	a.val[0] = p0;
	a.val[1] = p1;
	return a;
}

void ef_print_expr(ef_elem a);

void ef_print_expr_nl(ef_elem a);

void ef_print_hex(ef_elem a);

void ef_print_hex_nl(ef_elem a);

ef_elem ef_rand_elem();

uint64_t ef_equal(ef_elem a, ef_elem b);

static inline ef_elem ef_add(ef_elem a, ef_elem b) {
	return ef_create_elem(bf_add(a.val[0], b.val[0]), bf_add(a.val[1], b.val[1]));
}

ef_elem ef_mull(ef_elem a, ef_elem b);

static inline ef_elem ef_mull_A(ef_elem a) {
	ef_elem r;
	r.val[0] = a.val[1];
	r.val[1] = bf_add(a.val[0], a.val[1]);
	return r;
}

ef_elem ef_square(ef_elem a);

ef_elem ef_inv(ef_elem a);

void ef_sim_inv(ef_elem inputs[], ef_elem outputs[], uint64_t len);

#endif
