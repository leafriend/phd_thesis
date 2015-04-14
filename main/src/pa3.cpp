#include <stdio.h>
#include <math.h>
#include <limits>
#include <float.h>

#include "func.h"
#include "pa2.h"

void pa3(Macro** macros, Pico** picos, Mobile** mobiles) {

	// �� Mobile �� Macro�� �������� Cell Association ����
	// ���⼭�� �ٸ� ���� �޸� Pico�� ABS����(is_abs())�� Ȯ������ �ʰ�
	// ������ non-ABS ���� ���Ѵ�
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
				// Mobile�� Macro�� �����ٸ�
				// ������ ���� Cell Association�� ���� lambda_r ����
				// �� ���� ä�κ� �Ҵ� ��� ����

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
				// Mobile�� Macro�� �����ٸ�
				// Mobile�� Pico�� ABS ���ο� ���� lambda_r ����
				
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

			// �������� �ְ� lambda_r�� �� ������ lambda_r ��

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

	// ���� ä�� �� throughput ���

	// Macro/Pico lambda_r

}
