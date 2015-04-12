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

			double macro_best_sum_lambda_r = DBL_MIN;
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
	
	FOREACH_MACROS {
		Macro* macro = macros[mac];
		int NUM_MOBILE_TS = macro->get_num_mobiles_to_service();
		Macro_Mobile** mobiles_to_service = macro->get_mobiles_to_service();
		macro->set_state(best_macro_states[mac]);
	}

	FOREACH_MACROS {
		Macro* macro = macros[mac];
		int NUM_MOBILE_TS = macro->get_num_mobiles_to_service();
		Macro_Mobile** mobiles_to_service = macro->get_mobiles_to_service();
		//printf("\n");
		//printf("[%d] %d : ", mac, best_macro_states[mac]);

		FOREACH_RBS {

			FOREACH_MOBILES_TS {
				Mobile* mobile = (Mobile*) mobiles_to_service[mob]->get_mobile();
				if (best_macro_states[mac]) {
					if (macro->get_first_mobile(ri) == mobile) {
						mobile->set_state(ri, 1);
					} else {
						mobile->set_state(ri, 0);
					}
					//printf("[%d] 1 ", mobile->idx);
				} else {
					if (mobile->get_pico() == NULL) {
						mobile->set_state(ri, 0);
					} else {
						const Pico* pico = mobile->get_pico()->get_pico();
						if (pico->get_non_sorted_mobile(ri, 0)->get_mobile() == mobile) {
							if (pico->is_abs()) {
								mobile->set_state(ri, 2);
							} else {
								mobile->set_state(ri, 3);
							}
						} else {
							mobile->set_state(ri, 0);
						}
					}
				}
			}

		}

	}
	//printf("\n");
	
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

		bool* best_mobile_conn_macros = (bool*) malloc(sizeof(bool) * NUM_MOBILE_TS);

		// ������ ��� Mobile Cell Association ����
		long num_mobile_state = 1 << NUM_MOBILE_TS;
		for (int S = 1; S < num_mobile_state; S++) {

			// Mobile ���� ���� ����
			FOREACH_MOBILES_TS {
				Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
				mobile->conn_macro = 1 == ((1 << mob) & S) >> mob;
			}

			double curr_sum_lambda_r = 0.0;
			FOREACH_RBS {

				double best_allocated_mobile = DBL_MIN;
				FOREACH_MOBILES_TS {
					Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();

					if (mobile->conn_macro) {
						// Macro�� ������ ���

						// ���� ������� ���� ���ҽ���Ͽ� �������� �� ����ǲ
						double curr_lambda_r = mobile->get_macro_lambda_r(ri);
						if (best_allocated_mobile < curr_lambda_r) {
							best_allocated_mobile = curr_lambda_r;
						}

					} else {

						if (mobile->get_pico() == NULL) {
							// ������� ���ڿ� ������� ���� ���
							// : �ƹ��͵� ���� �ʴ´�.
						} else {
							
							// Pico�� ������ ���
							const Pico* pico = mobile->get_pico()->get_pico();
							if (pico->get_non_sorted_mobile(ri, 0)->get_mobile() == mobile) {
								if (pico->is_abs()) {
									// ABS�� ���
									curr_sum_lambda_r += mobile->get_abs_pico_lambda_r(ri);
								} else {
									// non-ABS�� ���
									curr_sum_lambda_r += mobile->get_non_pico_lambda_r(ri);
								}
							} else {
								// ��ũ�ΰ� ���� �̿��ڰ� ���ڿ� ù��° ������ ������� �ʾ����Ƿ� ����
							}

						}

					}

				}
				curr_sum_lambda_r += best_allocated_mobile;

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

				if (mobile->get_pico() != NULL) {

					if (mobile->get_pico()->get_pico()->is_abs()) {
						// ABS�� ���
						curr_sum_lambda_r += mobile->get_abs_pico_lambda_r(ri);

					} else {
						// non-ABS�� ���
						curr_sum_lambda_r += mobile->get_abs_pico_lambda_r(ri);
					}

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
