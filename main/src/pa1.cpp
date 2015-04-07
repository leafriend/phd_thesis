#include <stdlib.h>
#include <float.h>

#include "pa1.h"

void pa1(int* best_mobile_states, Macro** macros, Pico** picos, Mobile** mobiles) {

	//int best_mobile_states[NUM_MOBILE];
	for (int mob = 0; mob < NUM_MOBILE; mob++)
		best_mobile_states[mob] = 0;

	double best_sum_lambda_r = DBL_MIN;

	bool best_macro_states[NUM_MACRO];
	for (int mac = 0; mac < NUM_MACRO; mac++)
		best_macro_states[mac] = false;

	int num_macro_state = 1 << NUM_MACRO;

	//struct timeval start, stop, ellapse;
	//gettimeofday(&start, NULL);

	for (int s = 0; s < num_macro_state; s++) {

		double curr_sum_lambda_r = 0.0;

		bool curr_macro_states[NUM_MACRO];
		for (int mac = 0; mac < NUM_MACRO; mac++)
			curr_macro_states[mac] = 1 == ((1 << mac) & s) >> mac;

		int curr_mobile_states[NUM_MOBILE];
		for (int mob = 0; mob < NUM_MOBILE; mob++)
			curr_mobile_states[mob] = 0;

		//printf("SATAE   "); for (int mac = NUM_MACRO; mac --> 0;) printf("%d", curr_macro_states[mac]); printf("\n");

		for (int mac = 0; mac < NUM_MACRO; mac++) {
			if (curr_macro_states[mac]) {
				Mobile* mobile = macros[mac]->get_first_mobile();

				curr_sum_lambda_r += mobile->lambda * mobile->get_macro_throughput();
				curr_mobile_states[mobile->idx] = 1;
			}
		}

		for (int pic = 0; pic < NUM_PICO; pic++) {

			Pico* pico = picos[pic];

			bool is_abs = true;

			for (int mac = 0; mac < NUM_MACRO; mac++) {
				if (curr_macro_states[mac] == ON && pico->is_neighbor(macros[mac])) {
					is_abs = false;
					break;
				}
			}

			if (is_abs) {

				Mobile* abs_first = pico->get_abs_first_mobile();
				Mobile* abs_second = pico->get_abs_second_mobile();

				if (abs_first != NULL) {

					Macro* macro = abs_first->get_macro() == NULL ? NULL : (Macro*) abs_first->get_macro()->macro;

					if (macro != NULL
					&& ((curr_macro_states[macro->idx] == OFF
					||  macro->get_first_mobile() != abs_first
					))) {
						curr_sum_lambda_r += abs_first->lambda * abs_first->get_abs_pico_throughput();
						curr_mobile_states[abs_first->idx] = 2;
						//printf("ABS - %2d = 2\n", abs_first->idx);

					} else if (abs_second != NULL) {
						curr_sum_lambda_r += abs_second->lambda * abs_second->get_abs_pico_throughput();
						curr_mobile_states[abs_second->idx] = 2;
						//printf("ABS - %2d = 2\n", abs_second->idx);

					}

				}

			} else {

				Mobile* first = pico->get_first_mobile();
				Macro* first_macro = first->get_macro() == NULL ? NULL : (Macro*) first->get_macro()->macro;

				if (first_macro != NULL
				&&  curr_macro_states[first_macro->idx] == ON
				&&  first_macro->get_first_mobile() == first
				) {

					Mobile* second = pico->get_second_mobile();
					if (second != NULL && curr_mobile_states[second->idx] != 1) {
						curr_sum_lambda_r += second->lambda * second->get_pico_throughput();
						curr_mobile_states[second->idx] = 4;
						//printf("non - %2d = 4\n", second->idx);
					} else {
						//printf("non\n");
					}

				} else {

					curr_sum_lambda_r += first->lambda * first->get_pico_throughput();
					//if (curr_mobile_states[first->idx] != 3) {
					//	printf("non - %2d @ %d[%d] = 3   ", first->idx, first_macro->idx, curr_macro_states[first_macro->idx]);
					//	for (int mac = NUM_MACRO; mac --> 0;) printf("%d", curr_macro_states[mac]);
					//	printf(" (%p == %p)\n", first_macro->first_mobile, first);
					//}
					curr_mobile_states[first->idx] = 3;

				}

			}

			//for (int mob = 0; mob < NUM_MOBILE; mob++) printf("%d", curr_mobile_states[mob]); printf("\n");

		}

		if (curr_sum_lambda_r > best_sum_lambda_r) {

			//printf("BETTER  %12.6lf > %12.6lf - ", curr_sum_lambda_r, best_sum_lambda_r);
			//for (int mac = NUM_MACRO; mac --> 0;) printf("%d", curr_macro_states[mac]);

			best_sum_lambda_r = curr_sum_lambda_r;

			for (int mac = 0; mac < NUM_MACRO; mac++)
				best_macro_states[mac] = curr_macro_states[mac];

			for (int mob = 0; mob < NUM_MOBILE; mob++)
				best_mobile_states[mob] = curr_mobile_states[mob];

			//printf(" .\n");

		}

	}

	//gettimeofday(&stop, NULL);
	//subtract_timeval(&ellapse, &stop, &start);
	//printf("                        :%6d.%06d s\n", ellapse.tv_sec, ellapse.tv_usec);

	//printf("BEST    %12.6lf - ", best_sum_lambda_r); for (int mac = NUM_MACRO; mac --> 0;) printf(best_macro_states[mac] ? "%d" : "-", best_macro_states[mac]); printf("\n");
	//printf("    MOBILE             "); for (int mac = NUM_MACRO; mac --> 0;) printf(best_macro_states[mac] ? "%d" : "-", best_mobile_states[ macros[mac]->first_mobile->idx ]); printf("\n");
	//printf("\n");

}
