#include <math.h>
#include <limits>
#include <float.h>

#include "pico_mobile.h"
#include "macro_mobile.h"
#include "func.h"
#include "pa2.h"

void pa3(Macro** macros, Pico** picos, Mobile** mobiles) {

	// 각 Mobile 별 Macro가 켜졌을때 Cell Association 결정
	// 여기서는 다른 곳과 달리 Pico의 ABS여부(is_abs())를 확인하지 않고
	// 무조건 non-ABS 값만 취한다
	bool conn_macros[NUM_MOBILE];
	FOREACH_MOBILES {

		Mobile* mobile = mobiles[mob];

		double sum_macro_lambda_r = 0.0;
		FOREACH_RBS {
			sum_macro_lambda_r += mobile->get_macro_lambda_r(ri);
		}
		double ratio_macro_lambda_r
			= sum_macro_lambda_r
			/ mobile->get_macro()->macro->pa3_lambda_r;

		double sum_pico_lambda_r = 0.0;
		FOREACH_RBS {
			sum_pico_lambda_r += mobile->get_non_pico_lambda_r(ri);
		}
		double ratio_pico_lambda_r
			= sum_pico_lambda_r
			/ mobile->get_pico()->pico->pa3_lambda_r;

		conn_macros[mob] = ratio_macro_lambda_r > ratio_pico_lambda_r;

	}

	double best_sum_lambda_r = -std::numeric_limits<double>::infinity();
	bool best_macro_states[NUM_MACRO];

	FOREACH_MACROS
		macros[mac]->state = OFF;

	// 가능한 모든 Macro 상태(2 ^ NUM_MACRO = 1 << NUM_MACRO)에 대한 반복문
	int num_macro_state = 1 << NUM_MACRO;
	for (int s = 0; s < num_macro_state; s++) {

		// Macro 상태(ON/OFF) 지정
		FOREACH_MACROS
			macros[mac]->state
				= ((1 << mac) & s) >> mac
				? ON
				: OFF;

		FOREACH_MACROS_TS {

			int* mobile_states = (int*) malloc(sizeof(int) * NUM_MOBILE_TS * NUM_RB);

			FOREACH_RBS {

				FOREACH_MOBILES_TS {

					const Mobile* mobile = mmobiles[mob]->mobile;

					if (mobile->get_macro()->macro->state == ON) {
						// Mobile의 Macro가 켜졌다면
						// 위에서 정한 Cell Association에 따라 lambda_r 가산
						// 각 서브 채널별 할당 대상 결정

						if (conn_macros[mob]) {

						} else {

						}

					} else {
						// Mobile의 Macro가 꺼졌다면
						// Mobile의 Pico의 ABS 여부에 따라 lambda_r 가산

					}

				}
			}

			// 이전까지 최고 lambda_r과 현 상태의 lambda_r 비교

			free(mobile_states);

		}

	} END

	// 서브 채널 별 throughput 계산

	// Macro/Pico lambda_r

}
