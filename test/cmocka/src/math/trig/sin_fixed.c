// SPDX-License-Identifier: BSD-3-Clause
//
// Copyright(c) 2018 Intel Corporation. All rights reserved.
//
// Author: Slawomir Blauciak <slawomir.blauciak@linux.intel.com>

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <math.h>
#include <cmocka.h>

#include <sof/audio/format.h>
#include <sof/math/trig.h>

#define CMP_TOLERANCE 0.000005

/* Reference table generated by sin(), gcc-4.3.2 */
static const float sin_ref_table[] = {
	0.0000000000, 0.0174524064, 0.0348994967, 0.0523359562,
	0.0697564737, 0.0871557427, 0.1045284633, 0.1218693434,
	0.1391731010, 0.1564344650, 0.1736481777, 0.1908089954,
	0.2079116908, 0.2249510543, 0.2419218956, 0.2588190451,
	0.2756373558, 0.2923717047, 0.3090169944, 0.3255681545,
	0.3420201433, 0.3583679495, 0.3746065934, 0.3907311285,
	0.4067366431, 0.4226182617, 0.4383711468, 0.4539904997,
	0.4694715628, 0.4848096202, 0.5000000000, 0.5150380749,
	0.5299192642, 0.5446390350, 0.5591929035, 0.5735764364,
	0.5877852523, 0.6018150232, 0.6156614753, 0.6293203910,
	0.6427876097, 0.6560590290, 0.6691306064, 0.6819983601,
	0.6946583705, 0.7071067812, 0.7193398003, 0.7313537016,
	0.7431448255, 0.7547095802, 0.7660444431, 0.7771459615,
	0.7880107536, 0.7986355100, 0.8090169944, 0.8191520443,
	0.8290375726, 0.8386705679, 0.8480480962, 0.8571673007,
	0.8660254038, 0.8746197071, 0.8829475929, 0.8910065242,
	0.8987940463, 0.9063077870, 0.9135454576, 0.9205048535,
	0.9271838546, 0.9335804265, 0.9396926208, 0.9455185756,
	0.9510565163, 0.9563047560, 0.9612616959, 0.9659258263,
	0.9702957263, 0.9743700648, 0.9781476007, 0.9816271834,
	0.9848077530, 0.9876883406, 0.9902680687, 0.9925461516,
	0.9945218954, 0.9961946981, 0.9975640503, 0.9986295348,
	0.9993908270, 0.9998476952, 1.0000000000, 0.9998476952,
	0.9993908270, 0.9986295348, 0.9975640503, 0.9961946981,
	0.9945218954, 0.9925461516, 0.9902680687, 0.9876883406,
	0.9848077530, 0.9816271834, 0.9781476007, 0.9743700648,
	0.9702957263, 0.9659258263, 0.9612616959, 0.9563047560,
	0.9510565163, 0.9455185756, 0.9396926208, 0.9335804265,
	0.9271838546, 0.9205048535, 0.9135454576, 0.9063077870,
	0.8987940463, 0.8910065242, 0.8829475929, 0.8746197071,
	0.8660254038, 0.8571673007, 0.8480480962, 0.8386705679,
	0.8290375726, 0.8191520443, 0.8090169944, 0.7986355100,
	0.7880107536, 0.7771459615, 0.7660444431, 0.7547095802,
	0.7431448255, 0.7313537016, 0.7193398003, 0.7071067812,
	0.6946583705, 0.6819983601, 0.6691306064, 0.6560590290,
	0.6427876097, 0.6293203910, 0.6156614753, 0.6018150232,
	0.5877852523, 0.5735764364, 0.5591929035, 0.5446390350,
	0.5299192642, 0.5150380749, 0.5000000000, 0.4848096202,
	0.4694715628, 0.4539904997, 0.4383711468, 0.4226182617,
	0.4067366431, 0.3907311285, 0.3746065934, 0.3583679495,
	0.3420201433, 0.3255681545, 0.3090169944, 0.2923717047,
	0.2756373558, 0.2588190451, 0.2419218956, 0.2249510543,
	0.2079116908, 0.1908089954, 0.1736481777, 0.1564344650,
	0.1391731010, 0.1218693434, 0.1045284633, 0.0871557427,
	0.0697564737, 0.0523359562, 0.0348994967, 0.0174524064,
	0.0000000000, -0.0174524064, -0.0348994967, -0.0523359562,
	-0.0697564737, -0.0871557427, -0.1045284633, -0.1218693434,
	-0.1391731010, -0.1564344650, -0.1736481777, -0.1908089954,
	-0.2079116908, -0.2249510543, -0.2419218956, -0.2588190451,
	-0.2756373558, -0.2923717047, -0.3090169944, -0.3255681545,
	-0.3420201433, -0.3583679495, -0.3746065934, -0.3907311285,
	-0.4067366431, -0.4226182617, -0.4383711468, -0.4539904997,
	-0.4694715628, -0.4848096202, -0.5000000000, -0.5150380749,
	-0.5299192642, -0.5446390350, -0.5591929035, -0.5735764364,
	-0.5877852523, -0.6018150232, -0.6156614753, -0.6293203910,
	-0.6427876097, -0.6560590290, -0.6691306064, -0.6819983601,
	-0.6946583705, -0.7071067812, -0.7193398003, -0.7313537016,
	-0.7431448255, -0.7547095802, -0.7660444431, -0.7771459615,
	-0.7880107536, -0.7986355100, -0.8090169944, -0.8191520443,
	-0.8290375726, -0.8386705679, -0.8480480962, -0.8571673007,
	-0.8660254038, -0.8746197071, -0.8829475929, -0.8910065242,
	-0.8987940463, -0.9063077870, -0.9135454576, -0.9205048535,
	-0.9271838546, -0.9335804265, -0.9396926208, -0.9455185756,
	-0.9510565163, -0.9563047560, -0.9612616959, -0.9659258263,
	-0.9702957263, -0.9743700648, -0.9781476007, -0.9816271834,
	-0.9848077530, -0.9876883406, -0.9902680687, -0.9925461516,
	-0.9945218954, -0.9961946981, -0.9975640503, -0.9986295348,
	-0.9993908270, -0.9998476952, -1.0000000000, -0.9998476952,
	-0.9993908270, -0.9986295348, -0.9975640503, -0.9961946981,
	-0.9945218954, -0.9925461516, -0.9902680687, -0.9876883406,
	-0.9848077530, -0.9816271834, -0.9781476007, -0.9743700648,
	-0.9702957263, -0.9659258263, -0.9612616959, -0.9563047560,
	-0.9510565163, -0.9455185756, -0.9396926208, -0.9335804265,
	-0.9271838546, -0.9205048535, -0.9135454576, -0.9063077870,
	-0.8987940463, -0.8910065242, -0.8829475929, -0.8746197071,
	-0.8660254038, -0.8571673007, -0.8480480962, -0.8386705679,
	-0.8290375726, -0.8191520443, -0.8090169944, -0.7986355100,
	-0.7880107536, -0.7771459615, -0.7660444431, -0.7547095802,
	-0.7431448255, -0.7313537016, -0.7193398003, -0.7071067812,
	-0.6946583705, -0.6819983601, -0.6691306064, -0.6560590290,
	-0.6427876097, -0.6293203910, -0.6156614753, -0.6018150232,
	-0.5877852523, -0.5735764364, -0.5591929035, -0.5446390350,
	-0.5299192642, -0.5150380749, -0.5000000000, -0.4848096202,
	-0.4694715628, -0.4539904997, -0.4383711468, -0.4226182617,
	-0.4067366431, -0.3907311285, -0.3746065934, -0.3583679495,
	-0.3420201433, -0.3255681545, -0.3090169944, -0.2923717047,
	-0.2756373558, -0.2588190451, -0.2419218956, -0.2249510543,
	-0.2079116908, -0.1908089954, -0.1736481777, -0.1564344650,
	-0.1391731010, -0.1218693434, -0.1045284633, -0.0871557427,
	-0.0697564737, -0.0523359562, -0.0348994967, -0.0174524064
};

static void test_math_trig_sin_fixed(void **state)
{
	(void)state;

	int theta;

	for (theta = 0; theta < 360; ++theta) {
		double rad = M_PI / (180.0 / theta);
		int32_t rad_q28 = Q_CONVERT_FLOAT(rad, 28);

		float r = Q_CONVERT_QTOF(sin_fixed(rad_q28), 30);
		float diff = fabsf(sin_ref_table[theta] - r);

		if (diff > CMP_TOLERANCE) {
			printf("%s: diff for %d deg = %.10f\n", __func__,
			       theta, diff);
		}

		assert_true(diff <= CMP_TOLERANCE);
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_math_trig_sin_fixed)
	};

	cmocka_set_message_output(CM_OUTPUT_TAP);

	return cmocka_run_group_tests(tests, NULL, NULL);
}
