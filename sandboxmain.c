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

	// ef_elem PX = ef_create_elem(bf_create_elem(0XD2C27333EFC0AE61, 0X4306673487679D76), bf_create_elem(0X909BEC5477E860BB, 0X480D39C8A1B98266));
	// ef_elem PL = ef_create_elem(bf_create_elem(0XF84FB0B45D95FC31, 0X24C3FF4B68C78BE3), bf_create_elem(0X963FE2DA0544E1A4, 0X17B6B0A1380A490));
	// ef_elem PZ = ef_create_elem(bf_create_elem(0X100, 0), bf_create_elem(0X8000000000000000, 0X4000000000000001));
	// ec_point_lproj P = ec_create_point_lproj(PX, PL, PZ); //99921481365893197563 * GEN
	//
	// ec_point_lproj table[4];
	// precompute(ec_lproj_to_laffine(P), table);


	ec_naf result = ec_to_naf(bf_create_elem(153881, 0));
	//ec_naf result = ec_to_naf(bf_create_elem(153881, 1562365363));
	ec_print_naf(result);

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
