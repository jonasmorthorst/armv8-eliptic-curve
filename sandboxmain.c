#include <arm_neon.h>
#include <stdio.h>

#include "common/basefield.h"
#include "common/extensionfield.h"
#include "common/ec.h"
#include "common/ec_scalarmull.h"
#include "common/setup.h"
#include "common/utils.h"

int main() {
	init_components();

	// printf("\n -------------------- \n");

// 	k: 17543080488742735143, 14748808945444813383, 8374720340618217181, 1767656777874844946
// x: p0: 1fc34c09780b0839||0dbe04483b1838ea p1: 4be7046f6821989b||dbfe7553e9fd2369
//  l: p0: 284c3c7af24c119f||02352b17f9da5d0a p1: 1cb1c6318570bd4a||7e49abaf1ffd8b9f

	// uint64x2x2_t k = (uint64x2x2_t) {{{431572255129370311, 5384670855758030008}, {4353603234575721204, 773725909533223690}}};
	uint64x2x2_t k = (uint64x2x2_t) {{{2, 0}, {0, 0}}};

	ec_point_lproj expected = ec_neg(ec_scalarmull_single(ec_lproj_to_laffine((ec_point_lproj)GEN), k));
	//Act
	ec_point_lproj actual = ec_scalarmull_single_endo_w5_randaccess(ec_lproj_to_laffine((ec_point_lproj)GEN), k);

	//Assert
	uint64_t equal = ec_equal_point_lproj(expected, actual);
	uint64_t on_curve = ec_is_on_curve(actual);

	printf("Equal: %lu\n", equal);
	printf("On curve: %lu\n", on_curve);
	//
	// ec_naf result = ec_to_naf(k);
	// printf("%s\n", "Our naf");
	// ec_print_naf(result);
	//
	// elt k1_naf = {k[0], k[1]};
	// signed char naf[65];
	// int len = 0;
	// int order_len = 253;
	// int w = 4;
	//
	// scmul_wreg(naf, &len, k1_naf, order_len, w);
	// printf("\n\n\n%s\n", "Diego's naf");
	// printf("Len %d\n", len);
	//
	// ec_print_naf_arr(naf);

	// uint64x2x2_t k = (uint64x2x2_t) {{{1, 1}, {1, 1}}};
	//
	// // uint64x2x2_t k = (uint64x2x2_t) {{{3, 0}, {0, 0}}};
	// ec_split_scalar decomp = ec_scalar_decomp(k);
	//
	//
	//
	// ec_naf result = ec_to_naf(bf_create_elem(decomp.k1[0], decomp.k1[1]));
	// // printf("%s\n", "Our naf");
	// // ec_print_naf(result);
	//
	// result = ec_to_naf(bf_create_elem(decomp.k2[0], decomp.k2[1]));
	// // printf("\n");
	// // ec_print_naf(result);
	//
	// elt k1_naf = {decomp.k1[0], decomp.k1[1]};
	// signed char naf[100];
	// int len = 0;
	// int order_len = 253;
	// int w = 5;
	//
	// scmul_wreg(naf, &len, k1_naf, order_len, w);
	// // printf("\n\n\n%s\n", "Diego's naf");
	// // printf("Len %d\n", len);
	//
	// // ec_print_naf_arr(naf);
	//
	// elt k2_naf = {decomp.k2[0], decomp.k2[1]};
	// scmul_wreg(naf, &len, k2_naf, order_len, w);
	// // printf("\n");
	// //
	// // ec_print_naf_arr(naf);
	//
	// //Arrange
	// //uint64x2x2_t k = (uint64x2x2_t) {{{2243156791409331652485, 0}, {0, 0}}};
	// // ef_elem PX = ef_create_elem(bf_create_elem(0X7674C426F68A7C0D, 0X26C3E68569307393), bf_create_elem(0X9BFA0D5F1CB2BB3F, 0X53889FE5B08254D3));
	// // ef_elem PL = ef_create_elem(bf_create_elem(0X4F88EF9F49D18A5E, 0X5C7C38B577B3EAF4), bf_create_elem(0XCDD4DCBE486CC880, 0X18FEF6543ECA3ABC));
	// // ef_elem PZ = ef_create_elem(bf_create_elem(0X20000000000004, 0), bf_create_elem(0X8000000000000000, 0X80000));
	// // ec_point_lproj P = ec_create_point_lproj(PX, PL, PZ); //P = 78632917462800214 * GEN
	//
	// ec_point_lproj P = (ec_point_lproj) GEN;
	//
	// ec_point_lproj expected = ec_scalarmull_single(ec_lproj_to_laffine(P), k);
	//
	// //Act
	// ec_point_laffine actual = ec_scalarmull_single_endo_w5_randaccess(ec_lproj_to_laffine(P), k);
	//
	// printf("Expected: \n");
	// ec_print_expr(expected);
	//
	// printf("\n\nActual: \n");
	// ec_print_expr_laffine(actual);
	//
	// printf("\nEqual: %lu", ec_equal_point_laffine(ec_lproj_to_laffine(expected), actual));
	// printf("\nOn curve: %lu \n\n", ec_is_on_curve(ec_laffine_to_lproj(actual)));
	// printf("\nOn curve: %lu \n\n", ec_is_on_curve(expected));


	// ef_elem PX = ef_create_elem(bf_create_elem(0XD2C27333EFC0AE61, 0X4306673487679D76), bf_create_elem(0X909BEC5477E860BB, 0X480D39C8A1B98266));
	// ef_elem PL = ef_create_elem(bf_create_elem(0XF84FB0B45D95FC31, 0X24C3FF4B68C78BE3), bf_create_elem(0X963FE2DA0544E1A4, 0X17B6B0A1380A490));
	// ef_elem PZ = ef_create_elem(bf_create_elem(0X100, 0), bf_create_elem(0X8000000000000000, 0X4000000000000001));
	// ec_point_lproj P = ec_create_point_lproj(PX, PL, PZ); //99921481365893197563 * GEN
	//
	// ec_point_lproj table[4];
	// precompute(ec_lproj_to_laffine(P), table);


	// ec_naf result = ec_to_naf(bf_create_elem(153881, 0));
	// ec_naf result = ec_to_naf(bf_create_elem(35236236236, 2478623785632));
	// ec_print_naf(result);

	//printf("%p\n", &result);

	//153881

	// k = 3
	// m = 8
	//
	// i = 5
	// while (37 > 8)
	//
	// k0 = 1
	// k1 = -5
	// k2 = -3
	// k3 = 5
	// k4 = -3
	//
	// n = 5

	//sub_res_0 = 153880


	// i  |  ki  |  k
	// 0  |  -6  |  4540453076705
	// 1  |  -7  |

	// 1001 1010


	// 24411958 | 14699749183737301352

	// 1011101000111111100110110

// 01000011 11101001 10000000 00000000 00000000 00000000 00000000 00000001
//    1011101000111111100110 | 0001100110000000000000000000000000000000000000000000000000000000
//    1011101000111111100110	 1101100110000000000000000000000000000000000000000000000000000000
	// after division

	// 3051494 | 15672526703249326381

	dispose_components();
	return 0;
}
