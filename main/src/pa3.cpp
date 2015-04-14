#include <stdio.h>
#include <math.h>
#include <limits>
#include <float.h>

#include "func.h"
#include "pa2.h"

void pa3(Macro** macros, Pico** picos, Mobile** mobiles) {

	// 각 Mobile 별 Macro가 켜졌을때 Cell Association 결정
	// 여기서는 다른 곳과 달리 Pico의 ABS여부(is_abs())를 확인하지 않고
	// 무조건 non-ABS 값만 취한다
	bool conn_macros[NUM_MOBILE];
	MobileConnection states[NUM_MOBILE][NUM_RB];
	FOREACH_MOBILES {

		Mobile* mobile = mobiles[mob];

		double sum_macro_lambda_r = 0.0;
		FOREACH_RBS {
			sum_macro_lambda_r += mobile->get_macro_lambda_r(ri);
		}
		double ratio_macro_lambda_r
			= sum_macro_lambda_r
			/ mobile->get_macro()->bs->pa3_lambda_r;

		double sum_pico_lambda_r = 0.0;
		FOREACH_RBS {
			sum_pico_lambda_r += mobile->get_non_pico_lambda_r(ri);
		}
		double ratio_pico_lambda_r
			= sum_pico_lambda_r
			/ mobile->get_pico()->bs->pa3_lambda_r;

		conn_macros[mob] = ratio_macro_lambda_r > ratio_pico_lambda_r;

	}

	int mobile_states[NUM_MOBILE][NUM_RB];

	FIND_BEST_MACRO_STATE_OPEN(macros, best_macro_states) {

		MobileConnection curr_states[NUM_MOBILE][NUM_RB];

		FOREACH_MACROS_TS_OPEN {

			if (macro->state == ON) {
				// Mobile의 Macro가 켜졌다면
				// 위에서 정한 Cell Association에 따라 lambda_r 가산
				// 각 서브 채널별 할당 대상 결정

				int macro_rb_mobile[NUM_RB];
				double macro_rb_lambda_r[NUM_RB];
				FOREACH_RBS {
					macro_rb_mobile[ri] = -1;
					macro_rb_lambda_r[ri] = -std::numeric_limits<double>::infinity();
				}

				FOREACH_MOBILES_TS_OPEN {
					
					if (conn_macros[mobile->idx]) {

						FOREACH_RBS {
							double lambda_r = mobile->get_macro_lambda_r(ri);
							if (macro_rb_lambda_r[ri] < lambda_r) {
								macro_rb_lambda_r[ri] = lambda_r;
								macro_rb_mobile[ri] = mobile->idx;
							}
							curr_states[mobile->idx][ri] = NOTHING;
						}

					}

				} CLOSE

				for (int ri = 0; ri < NUM_RB; ri++) {
					curr_sum_lambda_r += macro_rb_lambda_r[ri];
					if (macro_rb_mobile[ri] >= 0)
						curr_states[macro_rb_mobile[ri]][ri] = MACRO;
				}
				printf("");

			} else {
				// Mobile의 Macro가 꺼졌다면
				// Mobile의 Pico의 ABS 여부에 따라 lambda_r 가산
				
				FOREACH_MOBILES_TS_OPEN {
					
					Pico* pico = (Pico*) mobile->get_pico()->bs;
					bool is_abs = pico->is_abs();
					FOREACH_RBS {
						if (is_abs) {
							if (pico->get_abs_sorted_mobile(ri, 0)->mobile == mobile) {
								curr_sum_lambda_r += mobile->get_abs_pico_lambda_r(ri);
								curr_states[mobile->idx][ri] = ABS_PICO;
							} else {
								curr_states[mobile->idx][ri] = NOTHING;
							}
						} else {
							if (pico->get_non_sorted_mobile(ri, 0)->mobile == mobile) {
								curr_sum_lambda_r += mobile->get_non_pico_lambda_r(ri);
								curr_states[mobile->idx][ri] = NON_PICO_1;
							} else {
								curr_states[mobile->idx][ri] = NOTHING;
							}
						}
					}

				} CLOSE

			}

			// 이전까지 최고 lambda_r과 현 상태의 lambda_r 비교

			//curr_sum_lambda_r = ...;

		} CLOSE

		if (curr_sum_lambda_r > best_sum_lambda_r) {

			//printf("  BETTER%12.6lf > %12.6lf - ", curr_sum_lambda_r, best_sum_lambda_r);
			//FOREACH_MACROS printf("%d", macros[mac]->get_state()); printf("\n");

			best_sum_lambda_r = curr_sum_lambda_r;

			FOREACH_MACROS
				best_macro_states[mac] = macros[mac]->state;

			FOREACH_MOBILES
				FOREACH_RBS
					states[mob][ri] = curr_states[mob][ri];

		}

	} FIND_BEST_MACRO_STATE_CLOSE(best_macro_states)
		
	FOREACH_MOBILES
		FOREACH_RBS
			mobiles[mob]->conns[ri] = states[mob][ri];

	// 서브 채널 별 throughput 계산

	// Macro/Pico lambda_r

}
