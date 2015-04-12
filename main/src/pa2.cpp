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

	// 가능한 모든 Macro 상태(2 ^ NUM_MACRO = 1 << NUM_MACRO)에 대한 반복문
	int num_macro_state = 1 << NUM_MACRO;
	for (int s = 0; s < num_macro_state; s++) {

		// Macro 상태(ON/OFF) 지정
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

	// TODO 매크로의 모바일 갯수가 사이즈를 넘어가는 경우 배열 등으로 변경
	int NUM_MOBILE_TS = macro->get_num_mobiles_to_service();
	if (NUM_MOBILE_TS > sizeof(long) * 8)
		fprintf(stderr, "Number of Mobile in range is over %d\n", sizeof(long));

	Macro_Mobile** mmobiles = macro->get_mobiles_to_service();

	if (macro->get_state()) {
		// 매크로가 켜져 있는 경우
		// : 각 모바일이 매크로와 피코 연결 상태의 r값 중 최대 값을 찾음

		bool* best_mobile_conn_macros = (bool*) malloc(sizeof(bool) * NUM_MOBILE_TS);

		// 가능한 모든 Mobile Cell Association 상태
		long num_mobile_state = 1 << NUM_MOBILE_TS;
		for (int S = 1; S < num_mobile_state; S++) {

			// Mobile 연결 상태 지정
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
						// Macro에 연결한 경우

						// 현재 모바일이 현재 리소스블록에 연갤했을 때 쓰루풋
						double curr_lambda_r = mobile->lambda * mobile->get_macro_throughput(ri);
						if (best_allocated_mobile < curr_lambda_r) {
							best_allocated_mobile = curr_lambda_r;
						}

					} else {

						if (mobile->get_pico() == NULL) {
							// 모바일이 피코에 연결되지 않은 경우
							// : 아무것도 하지 않는다.
						} else {
							
							// Pico에 연결한 경우
							const Pico* pico = mobile->get_pico()->get_pico();
							if (pico->get_non_sorted_mobile(ri, 0)->get_mobile() == mobile) {
								if (pico->is_abs()) {
									// ABS인 경우
									curr_sum_lambda_r += mobile->lambda * mobile->get_abs_pico_throughput(ri);
								} else {
									// non-ABS인 경우
									curr_sum_lambda_r += mobile->lambda * mobile->get_non_pico_throughput(ri);
								}
							} else {
								// 매크로가 버린 이용자가 피코에 첫번째 유저로 연결되지 않았으므로 버림
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

		// 모바일의 매크로 연결 여부를 최대 값으로 복구
		FOREACH_MOBILES_TS {
			Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
			mobile->conn_macro = best_mobile_conn_macros[mob];
		}
		
		free(best_mobile_conn_macros);

	} else {
		// 매크로가 꺼져 있는 경우

		double curr_sum_lambda_r = 0.0;
		
		FOREACH_RBS {

			FOREACH_MOBILES_TS {
				Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
				// Pico에 연결한 경우

				if (mobile->get_pico() != NULL) {

					if (mobile->get_pico()->get_pico()->is_abs()) {
						// ABS인 경우
						curr_sum_lambda_r += mobile->lambda * mobile->get_abs_pico_throughput(ri);

					} else {
						// non-ABS인 경우
						curr_sum_lambda_r += mobile->lambda * mobile->get_abs_pico_throughput(ri);
					}

				}

			}

		}

		// 모바일의 매크로 연결 여부를 거짓으로 지정 -- 매크로가 꺼져 있으므로
		FOREACH_MOBILES_TS {
			Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
			mobile->conn_macro = false;
		}

		*macro_best_sum_lambda_r = curr_sum_lambda_r;

	}

}
