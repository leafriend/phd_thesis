#include <stdio.h>

#include <stdlib.h>

#include <math.h>
#include <limits>

#include "func.h"
#include "pa1.h"
#include "pico_mobile.h"

void pa1(Macro** macros, Pico** picos, Mobile** mobiles) {

	Mobile* mobile_allocations[NUM_MACRO][NUM_RB];

	FOREACH_MACROS_TS_OPEN {

		FOREACH_RBS {

			mobile_allocations[mac][ri] = NULL;
			double first_lambda_r = -std::numeric_limits<double>::infinity();

			//printf("num_mobiles_to_service: %d\n", num_mobiles_to_service);
			FOREACH_MOBILES_TS_OPEN {

				const double macro_lambda_r = mobile->get_macro_lambda_r(ri);

				//printf("lambda: %lf\n", mobile->lambda);
				//printf("mobile->get_get_macro_throughput(): %lf\n", mobile->get_macro_throughput());
				//printf("macro_lambda_r: %lf\n", macro_lambda_r);

				if (mobile->get_pico() == NULL) {

					if (macro_lambda_r > first_lambda_r) {
						mobile_allocations[mac][ri] = (Mobile*) mobile;
						first_lambda_r = macro_lambda_r;
					}

				} else {

					Pico* pico = (Pico*) mobile->get_pico()->pico;
					const int num_pico_mobiles_to_service = pico->get_num_mobiles_to_service();
					
					// �� Pico�� ������� ���� ���� Pico�̹Ƿ� non_sorted_mobile ���� �׻� 1 �̻�
					const Mobile* pico_first_mobile = pico->get_non_sorted_mobile(ri, 0)->mobile;

					if (mobile == pico_first_mobile) {
						// Macro���� �Ҵ��Ϸ��� Mobile�� Pico�� ù ��°�� ���

						double lambda_r = macro_lambda_r - mobile->get_non_pico_lambda_r(ri);
						if (num_pico_mobiles_to_service > 1) {
							const Mobile* pico_second_mobile = pico->get_non_sorted_mobile(ri, 1)->mobile;

							// Pico�� �� ��° Mobile�� �ִ� ���
							// : Pico���� ���� ù ��° Mobile�� lambda r ��� �� ��° Mobile�� ���� �߰�
							// : TODO ������ �� ��° Mobile�� ���� Macro�� �ƴ϶� �ٸ� Macro���?
							lambda_r += pico_second_mobile->get_non_pico_lambda_r(ri);
						}

						if (lambda_r > first_lambda_r) {
							mobile_allocations[mac][ri] = (Mobile*) mobile;
							first_lambda_r = lambda_r;
						}

					} else {
						// Macro���� �Ҵ��Ϸ��� Mobile�� Pico�� ù ��°�� �ƴ� ���

						if (macro_lambda_r > first_lambda_r) {
							mobile_allocations[mac][ri] = (Mobile*) mobile;
							first_lambda_r = macro_lambda_r;
						}

					}

				}

			}

		}

	} CLOSE

	double best_sum_lambda_r = -std::numeric_limits<double>::infinity();
	MacroState best_macro_states[NUM_MACRO];

	FOREACH_MOBILES
		FOREACH_RBS
			mobiles[mob]->conns[ri] = NOTHING;

	FOREACH_MACROS
		macros[mac]->state = OFF;

	// ������ ��� Macro ����(2 ^ NUM_MACRO = 1 << NUM_MACRO)�� ���� �ݺ���
	int num_macro_state = 1 << NUM_MACRO;
	for (int s = 0; s < num_macro_state; s++) {

		// Macro ����(ON/OFF) ����
		FOREACH_MACROS
			macros[mac]->state
				= ((1 << mac) & s) >> mac
				? ON
				: OFF;

		double curr_sum_lambda_r = 0.0;

		//printf("SATAE   "); for (int mac = NUM_MACRO; mac --> 0;) printf("%d", macros[mac]->get_state()); printf("\n");

		// �̿��� (����) ���� �ʱ�ȭ
		MobileConnection curr_mobile_states[NUM_RB][NUM_MOBILE];
		FOREACH_RBS
			FOREACH_MOBILES
				curr_mobile_states[ri][mob] = NOTHING;

		FOREACH_MACROS {
			if (macros[mac]->state == ON) {
				FOREACH_RBS {
					Mobile* mobile = mobile_allocations[mac][ri];
					if (mobile != NULL) {
						curr_sum_lambda_r += mobile->get_macro_lambda_r(ri);
						curr_mobile_states[ri][mobile->idx] = MACRO;
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

						if (first_macro->state == OFF
							|| mobile_allocations[first_macro->idx][ri] != abs_first
						) {
							curr_sum_lambda_r += abs_first->get_abs_pico_lambda_r(ri);
							curr_mobile_states[ri][abs_first->idx] = ABS_PICO;
							//printf("ABS - %2d = 2\n", abs_first->idx);

						} else if (abs_second != NULL) {
							curr_sum_lambda_r += abs_second->get_abs_pico_lambda_r(ri);
							curr_mobile_states[ri][abs_second->idx] = ABS_PICO;
							//printf("ABS - %2d = 2\n", abs_second->idx);

						}

					}

				}

			} else {

				FOREACH_RBS {

					const Mobile* first = pico->get_non_sorted_mobile(ri, 0)->mobile;

					if (first != NULL) {

						Macro* first_macro = (Macro*) first->get_macro()->macro;

						if (first_macro->state == ON
							|| mobile_allocations[first_macro->idx][ri] != first
						) {

							const Mobile* second = pico->get_non_sorted_mobile(ri, 1)->mobile;
							if (second != NULL && curr_mobile_states[ri][second->idx] != 1) {
								curr_sum_lambda_r += second->get_abs_pico_lambda_r(ri);
								curr_mobile_states[ri][second->idx] = NON_PICO_2;
								//printf("non - %2d = 4\n", second->idx);
							} else {
								//printf("non\n");
							}

						} else {

							curr_sum_lambda_r += first->get_abs_pico_lambda_r(ri);
							curr_mobile_states[ri][first->idx] = NON_PICO_1;

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
				best_macro_states[mac] = macros[mac]->state;

			FOREACH_RBS
				FOREACH_MOBILES
					mobiles[mob]->conns[ri] = curr_mobile_states[ri][mob];


		}

	}

	FOREACH_MACROS
		macros[mac]->state = best_macro_states[mac];

	//gettimeofday(&stop, NULL);
	//subtract_timeval(&ellapse, &stop, &start);
	//printf("                        :%6d.%06d s\n", ellapse.tv_sec, ellapse.tv_usec);

	//printf("BEST    %12.6lf - ", best_sum_lambda_r);
	//for (int mac = NUM_MACRO; mac --> 0;) printf(macros[mac]->get_state() ? "%d" : "-", macros[mac]->get_state());
	//printf("    MOBILE             "); FOREACH_RBS for (int mac = NUM_MACRO; mac --> 0;) printf(macros[mac]->get_state() ? "%d" : "-", macros[mac]->get_first_mobile(ri)->get_state(ri) ); printf("\n");
	//printf("\n"); printf("\n");

}
