#include <stdio.h>

#include "basefield.h"
#include "extensionfield.h"
#include "utils.h"

ef_elem ef_create_elem(poly64x2_t p0, poly64x2_t p1) {
	ef_elem a;
	a.p0 = p0;
	a.p1 = p1;
	return a;
}

void ef_print_expr(ef_elem a) {
	uint64_t top_neq_0 = a.p1[0] != 0 | a.p1[1] != 0;
	uint64_t bot_neq_0 = a.p0[0] != 0 | a.p0[1] != 0;
	if(top_neq_0) {
		printf("(");
		bf_print_expr(a.p1);
		printf(")u");
	}
	if(top_neq_0 && bot_neq_0) {
		printf(" + ");
	}
	if(bot_neq_0) {
		printf("(");
		bf_print_expr(a.p0);
		printf(")");
	}
}

void ef_print_expr_nl(ef_elem a) {
	ef_print_expr(a);
	printf("\n");
}

void ef_print_hex(ef_elem a) {
	printf("p0: ");
	bf_print_hex(a.p0);
	printf(" p1: ");
	bf_print_hex(a.p1);
}

void ef_print_hex_nl(ef_elem a) {
	ef_print_hex(a);
	printf("\n");
}

ef_elem ef_rand_elem() {
	return ef_create_elem(bf_rand_elem(), bf_rand_elem());
}

ef_elem ef_add(ef_elem a, ef_elem b) {
	return ef_create_elem(bf_add(a.p0, b.p0), bf_add(a.p1, b.p1));
}

ef_elem ef_mull(ef_elem a, ef_elem b) {
	poly64x2_t a0b0 = bf_red(bf_pmull(a.p0, b.p0)); //a0*b0
	poly64x2_t a1b1 = bf_red(bf_pmull(a.p1, b.p1));	//a1*b1
	poly64x2_t a0pa1 = bf_add(a.p0, a.p1); //a0+a1
	poly64x2_t b0pb1 = bf_add(b.p0, b.p1); //b0+b1
	poly64x2_t bigprod = bf_red(bf_pmull(a0pa1, b0pb1)); //(a0+a1)*(b0+b1)
	return ef_create_elem(bf_add(a0b0, a1b1), bf_add(a0b0, bigprod)); //(a0*b0+a1*b1)+(a0*b0 + (a0+a1)*(b0+b1))u
}

ef_elem ef_square(ef_elem a) {
	poly64x2_t a0_squared = bf_red_psquare(bf_psquare(a.p0)); //a0^2
	poly64x2_t a1_squared = bf_red_psquare(bf_psquare(a.p1)); //a1^2
	return ef_create_elem(bf_add(a0_squared, a1_squared), a1_squared); //(a0^2 + a1^2) + (a1^2)u
}

ef_elem ef_inv(ef_elem a) {
	poly64x2_t a0a1 = bf_red(bf_pmull(a.p0, a.p1)); //a0*a1
	poly64x2_t a0pa1 = bf_add(a.p0, a.p1); //a0+a1
	poly64x2_t a0pa1_squared = bf_red_psquare(bf_psquare(a0pa1)); //a0^2 + a1^2
	poly64x2_t t = bf_add(a0a1, a0pa1_squared); //t = a0*a1 + a0^2 + a1^2
	poly64x2_t t_inv = bf_inv(t); //t^-1
	return ef_create_elem(bf_red(bf_pmull(a0pa1, t_inv)), bf_red(bf_pmull(a.p1, t_inv))); //((a0+a1)*t^-1) + (a_1*t^-1)u
}