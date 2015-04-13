#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <limits>

#include "pico_mobile.h"
#include "func.h"
#include "pa2.h"

void pa2(Macro** macros, Pico** picos, Mobile** mobiles) {

	double best_sum_lambda_r = -std::numeric_limits<double>::infinity();
	bool best_macro_states[NUM_MACRO];

	FOREACH_MACROS
		macros[mac]->set_state(false);

#ifdef PRINT_STATE
	printf("\n");
	FOREACH_MACROS printf("-"); printf(" " "            " "    " "            " "   ");
	FOREACH_MOBILES printf("%d", mobiles[mob]->conn_macro);
#endif

	// ������ ��� Macro ����(2 ^ NUM_MACRO = 1 << NUM_MACRO)�� ���� �ݺ���
	int num_macro_state = 1 << NUM_MACRO;
	for (int s = 0; s < num_macro_state; s++) {

		// Macro ����(ON/OFF) ����
		FOREACH_MACROS
			macros[mac]->set_state(1 == ((1 << mac) & s) >> mac);

		double curr_sum_lambda_r = 0.0;

		//printf("SATAE   "); for (int mac = NUM_MACRO; mac --> 0;) printf("%d", macros[mac]->get_state()); printf("\n");

		FOREACH_MACROS {

			double macro_best_sum_lambda_r = -std::numeric_limits<double>::infinity();
			pa2_find_best_mobile_state(macros[mac], &macro_best_sum_lambda_r);
			curr_sum_lambda_r += macro_best_sum_lambda_r;

		}

		if (curr_sum_lambda_r > best_sum_lambda_r) {

			//printf("  BETTER%12.6lf > %12.6lf - ", curr_sum_lambda_r, best_sum_lambda_r);
			//FOREACH_MACROS printf("%d", macros[mac]->get_state()); printf("\n");

			best_sum_lambda_r = curr_sum_lambda_r;

			FOREACH_MACROS
				best_macro_states[mac] = macros[mac]->get_state();

		}

#ifdef PRINT_STATE
			FOREACH_MOBILES printf("\b");
			FOREACH_MACROS printf("\b"); printf("\b" "\b\b\b\b\b\b\b\b\b\b\b\b" "\b\b\b\b" "\b\b\b\b\b\b\b\b\b\b\b\b" "\b\b\b");
			//printf("\n");
			FOREACH_MACROS printf("%d", macros[mac]->get_state()); printf(" %12.6f vs %12.6f - ", best_sum_lambda_r, curr_sum_lambda_r);
			FOREACH_MOBILES printf("%d", mobiles[mob]->conn_macro);
#endif

	}

	FOREACH_MACROS
		macros[mac]->set_state(best_macro_states[mac]);

#ifdef PRINT_STATE
	FOREACH_MOBILES printf("\b");
	FOREACH_MACROS printf("\b"); printf("\b" "\b\b\b\b\b\b\b\b\b\b\b\b" "\b\b\b\b" "\b\b\b\b\b\b\b\b\b\b\b\b" "\b\b\b");
	FOREACH_MACROS printf("%d", macros[mac]->get_state()); printf(" %12.6f" "    " "            " "   ", best_sum_lambda_r);
	FOREACH_MOBILES printf("%d", mobiles[mob]->get_state(0));
	printf("\n");
#endif

}

void pa2_find_best_mobile_state(Macro* macro, double* macro_best_sum_lambda_r) {

	// TODO ��ũ���� ����� ������ ����� �Ѿ�� ��� �迭 ������ ����
	int NUM_MOBILE_TS = macro->get_num_mobiles_to_service();
	if (NUM_MOBILE_TS > sizeof(long) * 8)
		fprintf(stderr, "Number of Mobile in range is over %d\n", sizeof(long));

	Macro_Mobile** mmobiles = macro->get_mobiles_to_service();

	if (macro->get_state()) {
		// ��ũ�ΰ� ���� �ִ� ���
		// : �� ������� ��ũ�ο� ���� ���� ������ r�� �� �ִ� ���� ã��

		int* best_mobile_states = (int*) malloc(sizeof(int) * NUM_MOBILE_TS * NUM_RB);

		// ������ ��� Mobile Cell Association ����
		long num_mobile_state = 1 << NUM_MOBILE_TS;
		for (int S = 1; S < num_mobile_state; S++) {

			double curr_sum_lambda_r = 0.0;
			int curr_allocated_mob[NUM_RB];
			FOREACH_RBS {

				double best_allocated_lambda_r = -std::numeric_limits<double>::infinity();
				FOREACH_MOBILES_TS {
					Mobile* mobile = (Mobile*) mmobiles[mob]->mobile;
					mobile->set_state(ri, 0);

					// Mobile ���� ���� ����
					if (1 == ((1 << mob) & S) >> mob) {
						// Macro�� ������ ���

						// ���� ������� ���� ���ҽ���Ͽ� �������� �� ����ǲ
						double curr_lambda_r = mobile->get_macro_lambda_r(ri);
						if (best_allocated_lambda_r < curr_lambda_r) {
							best_allocated_lambda_r = curr_lambda_r;
							curr_allocated_mob[ri] = mob;
						}

					} else {

						if (mobile->get_pico() == NULL) {
							// ������� ���ڿ� ������� ���� ���
							// : �ƹ��͵� ���� �ʴ´�.
						} else {
							
							// Pico�� ������ ���
							const Pico* pico = mobile->get_pico()->pico;
							if (pico->get_non_sorted_mobile(ri, 0)->mobile == mobile) {
								if (pico->is_abs()) {
									// ABS�� ���
									curr_sum_lambda_r += mobile->get_abs_pico_lambda_r(ri);
									mobile->set_state(ri, 2);
								} else {
									// non-ABS�� ���
									curr_sum_lambda_r += mobile->get_non_pico_lambda_r(ri);
									mobile->set_state(ri, 3);
								}
							} else {
								// ��ũ�ΰ� ���� �̿��ڰ� ���ڿ� ù��° ������ ������� �ʾ����Ƿ� ����
							}

						}

					}

				}
				curr_sum_lambda_r += best_allocated_lambda_r;

			}

			if (curr_sum_lambda_r > *macro_best_sum_lambda_r) {
				*macro_best_sum_lambda_r = curr_sum_lambda_r;
				FOREACH_RBS {
					FOREACH_MOBILES_TS {
						if (mmobiles[mob]->mobile->idx == 48)
							printf("");
						switch (mmobiles[mob]->mobile->get_state(ri))
						{
						case 0:
						case 1:
							if (curr_allocated_mob[ri] == mob) {
								best_mobile_states[mob * NUM_RB + ri] = 1;
							} else {
								best_mobile_states[mob * NUM_RB + ri] = 0;
							}
							break;
						case 2:
							best_mobile_states[mob * NUM_RB + ri] = 2;
							break;
						case 3:
							best_mobile_states[mob * NUM_RB + ri] = 3;
							break;
						default:
							break;
						}
					}
				}
			}

		}

		FOREACH_MOBILES_TS {
			FOREACH_RBS {
				int state = best_mobile_states[mob * NUM_RB + ri];
				((Mobile*) mmobiles[mob]->mobile)->set_state(ri, state);
			}
		}

		free(best_mobile_states);

	} else {
		// ��ũ�ΰ� ���� �ִ� ���

		double curr_sum_lambda_r = 0.0;
		
		FOREACH_RBS {

			FOREACH_MOBILES_TS {
				Mobile* mobile = (Mobile*) mmobiles[mob]->mobile;
				// Pico�� ������ ���

				if (mobile->get_pico() != NULL) {

					// Pico�� ������ ���
					const Pico* pico = mobile->get_pico()->pico;
					if (pico->get_non_sorted_mobile(ri, 0)->mobile == mobile) {
						if (pico->is_abs()) {
							// ABS�� ���
							curr_sum_lambda_r += mobile->get_abs_pico_lambda_r(ri);
							mobile->set_state(ri, 2);

						} else {
							// non-ABS�� ���
							curr_sum_lambda_r += mobile->get_abs_pico_lambda_r(ri);
							mobile->set_state(ri, 3);
						}
					}

				} else {
					mobile->set_state(ri, 0);

				}

			}

		}

		*macro_best_sum_lambda_r = curr_sum_lambda_r;

	}

}
