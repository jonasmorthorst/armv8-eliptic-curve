#include <stdio.h>

#include "basefield.h"
#include "extensionfield.h"
#include "utils.h"

void ef_print_expr(ef_elem a) {
	uint64_t top_neq_0 = a.val[1][0] != 0 | a.val[1][1] != 0;
	uint64_t bot_neq_0 = a.val[0][0] != 0 | a.val[0][1] != 0;
	if(top_neq_0) {
		printf("(");
		bf_print_expr(a.val[1]);
		printf(")u");
	}
	if(top_neq_0 && bot_neq_0) {
		printf(" + ");
	}
	if(bot_neq_0) {
		printf("(");
		bf_print_expr(a.val[0]);
		printf(")");
	}
}

ef_elem ef_square(ef_elem a) {
	poly64x2_t a0_squared = bf_red_psquare(bf_psquare(a.val[0])); //a0^2
	poly64x2_t a1_squared = bf_red_psquare(bf_psquare(a.val[1])); //a1^2
	return ef_create_elem(bf_add(a0_squared, a1_squared), a1_squared); //(a0^2 + a1^2) + (a1^2)u
}

void ef_print_expr_nl(ef_elem a) {
	ef_print_expr(a);
	printf("\n");
}

void ef_print_hex(ef_elem a) {
	printf("p0: ");
	bf_print_hex(a.val[0]);
	printf(" p1: ");
	bf_print_hex(a.val[1]);
}

void ef_print_hex_nl(ef_elem a) {
	ef_print_hex(a);
	printf("\n");
}

ef_elem ef_rand_elem() {
	return ef_create_elem(bf_rand_elem(), bf_rand_elem());
}

uint64_t ef_equal(ef_elem a, ef_elem b) {
	return (a.val[0][0] == b.val[0][0]) &&
		   (a.val[0][1] == b.val[0][1]) &&
		   (a.val[1][0] == b.val[1][0]) &&
		   (a.val[1][1] == b.val[1][1]);
}

ef_elem ef_mull(ef_elem a, ef_elem b) {
	poly64x2_t a0b0 = bf_red(bf_pmull(a.val[0], b.val[0])); //a0*b0
	poly64x2_t a1b1 = bf_red(bf_pmull(a.val[1], b.val[1]));	//a1*b1
	poly64x2_t a0pa1 = bf_add(a.val[0], a.val[1]); //a0+a1
	poly64x2_t b0pb1 = bf_add(b.val[0], b.val[1]); //b0+b1
	poly64x2_t bigprod = bf_red(bf_pmull(a0pa1, b0pb1)); //(a0+a1)*(b0+b1)
	return ef_create_elem(bf_add(a0b0, a1b1), bf_add(a0b0, bigprod)); //(a0*b0+a1*b1)+(a0*b0 + (a0+a1)*(b0+b1))u
}

//Multiplies a with the curve parameter A=u
//(a1*u+a0)*u = a1*u^2 + a0*u = a1*(u+1) + a0*u = (a0 + a1)*u + a1

ef_elem ef_inv(ef_elem a) {
	poly64x2_t a0a1 = bf_red(bf_pmull(a.val[0], a.val[1])); //a0*a1
	poly64x2_t a0pa1 = bf_add(a.val[0], a.val[1]); //a0+a1
	poly64x2_t a0pa1_squared = bf_red_psquare(bf_psquare(a0pa1)); //a0^2 + a1^2
	poly64x2_t t = bf_add(a0a1, a0pa1_squared); //t = a0*a1 + a0^2 + a1^2
	poly64x2_t t_inv = bf_inv(t); //t^-1
	return ef_create_elem(bf_red(bf_pmull(a0pa1, t_inv)), bf_red(bf_pmull(a.val[1], t_inv))); //((a0+a1)*t^-1) + (a_1*t^-1)u
}

void ef_sim_inv(ef_elem inputs[], ef_elem outputs[], uint64_t len) {
	ef_elem c[len];
	c[0] = inputs[0];
	for(int i = 1; i < len; i++) {
		c[i] = ef_mull(c[i-1], inputs[i]);
	}
	ef_elem u = ef_inv(c[len-1]); //(a0 * a1 * ... * ak)^-1
	for(int i = len-1; i >= 1; i--) {
		outputs[i] = ef_mull(u, c[i-1]); //(a0 * a1 * ... ai-1 * ai)^-1 * (a0 * a1 * ... * ai-1) = ai^-1
		u = ef_mull(u, inputs[i]);
	}
	outputs[0] = u;
}
