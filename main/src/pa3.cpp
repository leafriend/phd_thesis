#include <math.h>
#include <limits>
#include <float.h>

#include "func.h"
#include "pa2.h"

void pa3(Macro** macros, Pico** picos, Mobile** mobiles) {

	// 각 Mobile 별 Macro가 켜졌을때 Cell Association 결정
	// 여기서는 다른 곳과 달리 Pico의 ABS여부(is_abs())를 확인하지 않고
	// 무조건 non-ABS 값만 취한다
	//...

	double best_sum_lambda_r = -std::numeric_limits<double>::infinity();
	bool best_macro_states[NUM_MACRO];

	FOREACH_MACROS
		macros[mac]->set_state(false);

	// 가능한 모든 Macro 상태(2 ^ NUM_MACRO = 1 << NUM_MACRO)에 대한 반복문
	int num_macro_state = 1 << NUM_MACRO;
	for (int s = 0; s < num_macro_state; s++) {

		// Macro 상태(ON/OFF) 지정
		FOREACH_MACROS
			macros[mac]->set_state(1 == ((1 << mac) & s) >> mac);

		FOREACH_MOBILES {

			// Mobile의 Macro가 켜졌다면
			// 위에서 정한 Cell Association에 따라 lambda_r 가산
			// 각 서브 채널별 할당 대상 결정

			// Mobile의 Macro가 켜졌다면
			// Mobile의 Pico의 ABS 여부에 따라 lambda_r 가산

		}

		// 이전까지 최고 lambda_r과 현 상태의 lambda_r 비교

	}

	// 서브 채널 별 throughput 계산

	// Macro/Pico lambda_r

}
