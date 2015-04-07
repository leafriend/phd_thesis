#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "pico_mobile.h"
#include "func.h"
#include "pa2.h"

#define FOREACH_MOBILES_TS for (int mob = 0; mob < NUM_MOBILE_TS; mob++)

void pa2(Macro** macros, Pico** picos, Mobile** mobiles) {

	double best_sum_lambda_r = DBL_MIN;
	bool best_macro_states[NUM_MACRO];

	FOREACH_MACROS
		macros[mac]->set_state(false);

	// 가능한 모든 Macro 상태(2 ^ NUM_MACRO = 1 << NUM_MACRO)에 대한 반복문
	int num_macro_state = 1 << NUM_MACRO;
	for (int s = 0; s < num_macro_state; s++) {

		// Macro 상태(ON/OFF) 지정
		FOREACH_MACROS
			macros[mac]->set_state(1 == ((1 << mac) & s) >> mac);

		double curr_sum_lambda_r = 0.0;

		//printf("SATAE   "); for (int mac = NUM_MACRO; mac --> 0;) printf("%d", macros[mac]->get_state()); printf("\n");

		FOREACH_MACROS {

			if (!macros[mac]->get_state())
				continue;

			double macro_best_sum_lambda_r = DBL_MIN;
			pa2_find_best_mobile_state(macros[mac], &macro_best_sum_lambda_r);
			curr_sum_lambda_r += macro_best_sum_lambda_r;

		}

		if (curr_sum_lambda_r > best_sum_lambda_r) {

			//printf("  BETTER%12.6lf > %12.6lf - ", curr_sum_lambda_r, best_sum_lambda_r);
			//for (int mac = NUM_MACRO; mac --> 0;) printf("%d", macros[mac]->get_state());
			//printf("\n");

			best_sum_lambda_r = curr_sum_lambda_r;

			FOREACH_MACROS
				best_macro_states[mac] = macros[mac]->get_state();

		}

	}

	FOREACH_MACROS
		macros[mac]->set_state(best_macro_states[mac]);

}

void pa2_find_best_mobile_state(Macro* macro, double* best_sum_lambda_r) {

	// TODO 매크로의 모바일 갯수가 사이즈를 넘어가는 경우 배열 등으로 변경
	int NUM_MOBILE_TS = macro->get_num_mobiles_to_service();
	if (NUM_MOBILE_TS > sizeof(long) * 8)
		fprintf(stderr, "Number of Mobile in range is over %d\n", sizeof(long));
	// 가능한 모든 Mobile Cell Association 상태
	long num_mobile_state = 1 << NUM_MOBILE_TS;

	Macro_Mobile** mmobiles = macro->get_mobiles_to_service();

	bool* best_mobile_conn_macros = (bool*) malloc(sizeof(bool) * NUM_MOBILE_TS);

	for (int S = 0; S < num_mobile_state; S++) {

		double curr_sum_lambda_r = 0.0;

		// Mobile 연결 상태 지정
		FOREACH_MOBILES_TS {
			Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
			mobile->conn_macro = 1 == ((1 << mob) & S) >> mob;
		}

		FOREACH_RBS {

			FOREACH_MOBILES_TS {
				Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();

				if (mobile->conn_macro) {
					// Macro에 연결한 경우
					curr_sum_lambda_r += mobile->lambda * mobile->get_macro_throughput(ri);

				} else {
					// Pico에 연결한 경우

					if (mobile->get_pico()->get_pico()->is_abs()) {
						// ABS인 경우
						curr_sum_lambda_r += mobile->lambda * mobile->get_abs_pico_throughput(ri);

					} else {
						// non-ABS인 경우
						curr_sum_lambda_r += mobile->lambda * mobile->get_pico_throughput(ri);
					}

				}

			}

		}

		if (curr_sum_lambda_r > *best_sum_lambda_r) {

			*best_sum_lambda_r = curr_sum_lambda_r;

			FOREACH_MOBILES_TS
				best_mobile_conn_macros[mob] = mmobiles[mob]->get_mobile()->conn_macro;

		}

	}

	FOREACH_RBS {
		FOREACH_MOBILES_TS {
			Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
			mobile->conn_macro = best_mobile_conn_macros[mob];
		}
	}

	free(best_mobile_conn_macros);

}
