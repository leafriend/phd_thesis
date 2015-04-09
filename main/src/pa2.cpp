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

	// ������ ��� Macro ����(2 ^ NUM_MACRO = 1 << NUM_MACRO)�� ���� �ݺ���
	int num_macro_state = 1 << NUM_MACRO;
	for (int s = 0; s < num_macro_state; s++) {

		// Macro ����(ON/OFF) ����
		FOREACH_MACROS
			macros[mac]->set_state(1 == ((1 << mac) & s) >> mac);

		double curr_sum_lambda_r = 0.0;

		//printf("SATAE   "); for (int mac = NUM_MACRO; mac --> 0;) printf("%d", macros[mac]->get_state()); printf("\n");

		FOREACH_MACROS {

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
	
	printf("SATAE   "); for (int mac = NUM_MACRO; mac --> 0;) printf("%d", macros[mac]->get_state()); printf("\n");

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

		bool* best_mobile_conn_macros = (bool*) malloc(sizeof(bool) * NUM_MOBILE_TS);

		// ������ ��� Mobile Cell Association ����
		long num_mobile_state = 1 << NUM_MOBILE_TS;
		for (int S = 0; S < num_mobile_state; S++) {

			double curr_sum_lambda_r = 0.0;

			// Mobile ���� ���� ����
			FOREACH_MOBILES_TS {
				Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
				mobile->conn_macro = 1 == ((1 << mob) & S) >> mob;
			}

			FOREACH_RBS {

				FOREACH_MOBILES_TS {
					Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();

					if (mobile->conn_macro) {
						// Macro�� ������ ���
						curr_sum_lambda_r += mobile->lambda * mobile->get_macro_throughput(ri);

					} else {
						// Pico�� ������ ���

						if (mobile->get_pico()->get_pico()->is_abs()) {
							// ABS�� ���
							curr_sum_lambda_r += mobile->lambda * mobile->get_abs_pico_throughput(ri);

						} else {
							// non-ABS�� ���
							curr_sum_lambda_r += mobile->lambda * mobile->get_pico_throughput(ri);
						}

					}

				}

			}

			if (curr_sum_lambda_r > *macro_best_sum_lambda_r) {

				*macro_best_sum_lambda_r = curr_sum_lambda_r;

				FOREACH_MOBILES_TS
					best_mobile_conn_macros[mob] = mmobiles[mob]->get_mobile()->conn_macro;

			}

		}

		// ������� ��ũ�� ���� ���θ� �ִ� ������ ����
		FOREACH_MOBILES_TS {
			Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
			mobile->conn_macro = best_mobile_conn_macros[mob];
		}
		
		free(best_mobile_conn_macros);

	} else {
		// ��ũ�ΰ� ���� �ִ� ���

		double curr_sum_lambda_r = 0.0;
		
		FOREACH_RBS {

			FOREACH_MOBILES_TS {
				Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
				// Pico�� ������ ���

				if (mobile->get_pico()->get_pico()->is_abs()) {
					// ABS�� ���
					curr_sum_lambda_r += mobile->lambda * mobile->get_abs_pico_throughput(ri);

				} else {
					// non-ABS�� ���
					curr_sum_lambda_r += mobile->lambda * mobile->get_pico_throughput(ri);
				}

			}

		}

		// ������� ��ũ�� ���� ���θ� �������� ���� -- ��ũ�ΰ� ���� �����Ƿ�
		FOREACH_MOBILES_TS {
			Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
			mobile->conn_macro = false;
		}

		*macro_best_sum_lambda_r = curr_sum_lambda_r;

	}

}
