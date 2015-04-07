#include <stdio.h>

#include <stdlib.h>
#include <float.h>

#include "func.h"
#include "pa1.h"

void pa1(Macro** macros, Pico** picos, Mobile** mobiles) {

	FOREACH_MOBILES
		FOREACH_RBS
			mobiles[mob]->set_state(ri, 0);

	double best_sum_lambda_r = DBL_MIN;

	FOREACH_MACROS
		macros[mac]->set_state(false);

	// 가능한 모든 Macro 상태(2 ^ NUM_MACRO = 1 << NUM_MACRO)에 대한 반복문
	int num_macro_state = 1 << NUM_MACRO;
	for (int s = 0; s < num_macro_state; s++) {

		// Macro 상태(ON/OFF) 지정
		bool curr_macro_states[NUM_MACRO];
		FOREACH_MACROS
			curr_macro_states[mac] = 1 == ((1 << mac) & s) >> mac;

		double curr_sum_lambda_r = 0.0;

		//printf("SATAE   "); for (int mac = NUM_MACRO; mac --> 0;) printf("%d", curr_macro_states[mac]); printf("\n");

		// 이용자 (연결) 상태 초기화
		int curr_mobile_states[NUM_RB][NUM_MOBILE];
		FOREACH_RBS
			FOREACH_MOBILES
				curr_mobile_states[ri][mob] = 0;

		FOREACH_MACROS {
			if (curr_macro_states[mac]) {
				FOREACH_RBS {
					Mobile* mobile = macros[mac]->get_first_mobile(ri);

					curr_sum_lambda_r += mobile->lambda * mobile->get_macro_throughput(ri);
					curr_mobile_states[ri][mobile->idx] = 1;
				}
			}
		}

		FOREACH_PICOS {

			Pico* pico = picos[pic];

			bool is_abs = true;

			FOREACH_MACROS {
				if (pico->is_neighbor(macros[mac]) && curr_macro_states[mac] == ON) {
					is_abs = false;
					break;
				}
			}

			if (is_abs) {

				FOREACH_RBS {

					Mobile* abs_first = pico->get_abs_first_mobile(ri);
					Mobile* abs_second = pico->get_abs_second_mobile(ri);

					if (abs_first != NULL) {

						Macro* macro = abs_first->get_macro() == NULL ? NULL : (Macro*) abs_first->get_macro()->macro;

						if (macro != NULL
						&& ((curr_macro_states[macro->idx] == OFF
						||  macro->get_first_mobile(ri) != abs_first
						))) {
							curr_sum_lambda_r += abs_first->lambda * abs_first->get_abs_pico_throughput(ri);
							curr_mobile_states[ri][abs_first->idx] = 2;
							//printf("ABS - %2d = 2\n", abs_first->idx);

						} else if (abs_second != NULL) {
							curr_sum_lambda_r += abs_second->lambda * abs_second->get_abs_pico_throughput(ri);
							curr_mobile_states[ri][abs_second->idx] = 2;
							//printf("ABS - %2d = 2\n", abs_second->idx);

						}

					}

				}

			} else {

				FOREACH_RBS {

					Mobile* first = pico->get_first_mobile(ri);
					Macro* first_macro = first->get_macro() == NULL ? NULL : (Macro*) first->get_macro()->macro;

					if (first_macro != NULL
					&&  curr_macro_states[first_macro->idx] == ON
					&&  first_macro->get_first_mobile(ri) == first
					) {

						Mobile* second = pico->get_second_mobile(ri);
						if (second != NULL && curr_mobile_states[ri][second->idx] != 1) {
							curr_sum_lambda_r += second->lambda * second->get_pico_throughput(ri);
							curr_mobile_states[ri][second->idx] = 4;
							//printf("non - %2d = 4\n", second->idx);
						} else {
							//printf("non\n");
						}

					} else {

						curr_sum_lambda_r += first->lambda * first->get_pico_throughput(ri);
						curr_mobile_states[ri][first->idx] = 3;

					}

				}

			}

			//FOREACH_RBS FOREACH_MOBILES printf("%d", curr_mobile_states[ri][mob]); printf("\n");

		}

		if (curr_sum_lambda_r > best_sum_lambda_r) {

			//printf("  BETTER%12.6lf > %12.6lf - ", curr_sum_lambda_r, best_sum_lambda_r);
			//for (int mac = NUM_MACRO; mac --> 0;) printf("%d", curr_macro_states[mac]);
			//printf("\n");

			best_sum_lambda_r = curr_sum_lambda_r;

			FOREACH_MACROS
				macros[mac]->set_state(curr_macro_states[mac]);

			FOREACH_RBS
				FOREACH_MOBILES
					mobiles[mob]->set_state(ri, curr_mobile_states[ri][mob]);


		}

	}

	//gettimeofday(&stop, NULL);
	//subtract_timeval(&ellapse, &stop, &start);
	//printf("                        :%6d.%06d s\n", ellapse.tv_sec, ellapse.tv_usec);

	//printf("BEST    %12.6lf - ", best_sum_lambda_r);
	//for (int mac = NUM_MACRO; mac --> 0;) printf(macros[mac]->get_state() ? "%d" : "-", macros[mac]->get_state());
	//printf("    MOBILE             "); FOREACH_RBS for (int mac = NUM_MACRO; mac --> 0;) printf(macros[mac]->get_state() ? "%d" : "-", macros[mac]->get_first_mobile(ri)->get_state(ri) ); printf("\n");
	//printf("\n"); printf("\n");

}
