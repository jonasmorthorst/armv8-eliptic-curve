#include <stdio.h>

#include "ecpoint_tests.h"
#include "../common/ecpoint.h"
#include "../common/utils.h"

void ec_create_point_lproj_test_example(test_ctr *ctr) {
	//Arrange
	poly64x2_t a0 = {16785442, 721476};
	poly64x2_t a1 = {78099554548664, 6547959942615};

  ef_elem x = ef_create_elem(a0, a1);

  poly64x2_t b0 = {972573, 353523623};
	poly64x2_t b1 = {23523988509283, 2435335};

  ef_elem y = ef_create_elem(b0, b1);

	poly64x2_t c0 = {54689374, 4584};
	poly64x2_t c1 = {34548734, 5648574685};

  ef_elem z = ef_create_elem(c0, c1);

	//Act
	ec_point_lproj p = ec_create_point_lproj(x, y, z);

  ec_print_point_lproj_expr(p);

	//Assert
	// uint64_t correct = equal_poly64x2(a.p0, a0) & equal_poly64x2(a.p1, a1);
	// assert_true(correct, ctr, "extensionfield: ef_create_elem_test_example FAILED");
}

void ecpoint_tests(test_ctr *ctr) {
	ec_create_point_lproj_test_example(ctr);
}
