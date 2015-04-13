#include <stdio.h>

#include <stdlib.h>

#include <math.h>
#include <limits>

#include "func.h"
#include "pa1.h"
#include "pico_mobile.h"

void pa1(Macro** macros, Pico** picos, Mobile** mobiles) {
	
	FOREACH_MACROS
		macros[mac]->sort_mobiles();

	double best_sum_lambda_r = -std::numeric_limits<double>::infinity();
	bool best_macro_states[NUM_MACRO];

	FOREACH_MOBILES
		FOREACH_RBS
			mobiles[mob]->set_state(ri, 0);

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

		// 이용자 (연결) 상태 초기화
		int curr_mobile_states[NUM_RB][NUM_MOBILE];
		FOREACH_RBS
			FOREACH_MOBILES
				curr_mobile_states[ri][mob] = 0;

		FOREACH_MACROS {
			if (macros[mac]->get_state()) {
				FOREACH_RBS {
					Mobile* mobile = macros[mac]->get_first_mobile(ri);
					if (mobile != NULL) {
						curr_sum_lambda_r += mobile->get_macro_lambda_r(ri);
						curr_mobile_states[ri][mobile->idx] = 1;
					}
				}
			}
		}

		FOREACH_PICOS {

			const Pico* pico = picos[pic];

			if (pico->is_abs()) {

				FOREACH_RBS {

					const Mobile* abs_first = pico->get_abs_sorted_mobile(ri, 0)->mobile;
					const Mobile* abs_second = pico->get_abs_sorted_mobile(ri, 1)->mobile;

					if (abs_first != NULL) {

						Macro* first_macro = (Macro*) abs_first->get_macro()->macro;

						if (first_macro->get_state() == OFF
							|| first_macro->get_first_mobile(ri) != abs_first
						) {
							curr_sum_lambda_r += abs_first->get_abs_pico_lambda_r(ri);
							curr_mobile_states[ri][abs_first->idx] = 2;
							//printf("ABS - %2d = 2\n", abs_first->idx);

						} else if (abs_second != NULL) {
							curr_sum_lambda_r += abs_second->get_abs_pico_lambda_r(ri);
							curr_mobile_states[ri][abs_second->idx] = 2;
							//printf("ABS - %2d = 2\n", abs_second->idx);

						}

					}

				}

			} else {

				FOREACH_RBS {

					const Mobile* first = pico->get_non_sorted_mobile(ri, 0)->mobile;
					
					if (first != NULL) {

						Macro* first_macro = (Macro*) first->get_macro()->macro;

						if (first_macro->get_state() == ON
							&& first_macro->get_first_mobile(ri) == first
						) {

							const Mobile* second = pico->get_non_sorted_mobile(ri, 1)->mobile;
							if (second != NULL && curr_mobile_states[ri][second->idx] != 1) {
								curr_sum_lambda_r += second->get_abs_pico_lambda_r(ri);
								curr_mobile_states[ri][second->idx] = 4;
								//printf("non - %2d = 4\n", second->idx);
							} else {
								//printf("non\n");
							}

						} else {

							curr_sum_lambda_r += first->get_abs_pico_lambda_r(ri);
							curr_mobile_states[ri][first->idx] = 3;

						}

					}

				}

			}

			//FOREACH_RBS FOREACH_MOBILES printf("%d", curr_mobile_states[ri][mob]); printf("\n");

		}

		if (curr_sum_lambda_r > best_sum_lambda_r) {

			//printf("  BETTER%12.6lf > %12.6lf - ", curr_sum_lambda_r, best_sum_lambda_r);
			//for (int mac = NUM_MACRO; mac --> 0;) printf("%d", macros[mac]->get_state());
			//printf("\n");

			best_sum_lambda_r = curr_sum_lambda_r;

			FOREACH_MACROS
				best_macro_states[mac] = macros[mac]->get_state();

			FOREACH_RBS
				FOREACH_MOBILES
					mobiles[mob]->set_state(ri, curr_mobile_states[ri][mob]);


		}

	}

	FOREACH_MACROS
		macros[mac]->set_state(best_macro_states[mac]);

	//gettimeofday(&stop, NULL);
	//subtract_timeval(&ellapse, &stop, &start);
	//printf("                        :%6d.%06d s\n", ellapse.tv_sec, ellapse.tv_usec);

	//printf("BEST    %12.6lf - ", best_sum_lambda_r);
	//for (int mac = NUM_MACRO; mac --> 0;) printf(macros[mac]->get_state() ? "%d" : "-", macros[mac]->get_state());
	//printf("    MOBILE             "); FOREACH_RBS for (int mac = NUM_MACRO; mac --> 0;) printf(macros[mac]->get_state() ? "%d" : "-", macros[mac]->get_first_mobile(ri)->get_state(ri) ); printf("\n");
	//printf("\n"); printf("\n");

}
