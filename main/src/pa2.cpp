#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <limits>

#include "pico_mobile.h"
#include "func.h"
#include "pa2.h"

void pa2(Macro** macros, Pico** picos, Mobile** mobiles) {

	double best_sum_lambda_r = -std::numeric_limits<double>::infinity();
	MacroState best_macro_states[NUM_MACRO];

	FOREACH_MACROS
		macros[mac]->state = OFF;

	// 가능한 모든 Macro 상태(2 ^ NUM_MACRO = 1 << NUM_MACRO)에 대한 반복문
	int num_macro_state = 1 << NUM_MACRO;
	for (int s = 0; s < num_macro_state; s++) {

		// Macro 상태(ON/OFF) 지정
		FOREACH_MACROS
			macros[mac]->state
			= ((1 << mac) & s) >> mac
			? ON
			: OFF;

		double curr_sum_lambda_r = 0.0;

		//printf("SATAE   "); for (int mac = NUM_MACRO; mac --> 0;) printf("%d", macros[mac]->get_state()); printf("\n");

		FOREACH_MACROS_ {

			double macro_best_sum_lambda_r = -std::numeric_limits<double>::infinity();

			// TODO 매크로의 모바일 갯수가 사이즈를 넘어가는 경우 배열 등으로 변경
			int NUM_MOBILE_TS = macro->get_num_mobiles_to_service();
			if (NUM_MOBILE_TS > sizeof(long) * 8)
				fprintf(stderr, "Number of Mobile in range is over %d\n", sizeof(long));

			Macro_Mobile** mmobiles = macro->get_mobiles_to_service();

			if (macro->state == ON) {
				// 매크로가 켜져 있는 경우
				// : 각 모바일이 매크로와 피코 연결 상태의 r값 중 최대 값을 찾음

				MobileConnection* best_mobile_states
					= (MobileConnection*) malloc(sizeof(MobileConnection) * NUM_MOBILE_TS * NUM_RB);

				// 가능한 모든 Mobile Cell Association 상태
				long num_mobile_state = 1 << NUM_MOBILE_TS;
				for (int S = 1; S < num_mobile_state; S++) {

					double curr_sum_lambda_r = 0.0;
					int curr_allocated_mob[NUM_RB];
					FOREACH_RBS {

						double best_allocated_lambda_r = -std::numeric_limits<double>::infinity();
						FOREACH_MOBILES_TS {
							Mobile* mobile = (Mobile*) mmobiles[mob]->mobile;
							mobile->conns[ri] = NOTHING;

							// Mobile 연결 상태 지정
							if (1 == ((1 << mob) & S) >> mob) {
								// Macro에 연결한 경우

								// 현재 모바일이 현재 리소스블록에 연갤했을 때 쓰루풋
								double curr_lambda_r = mobile->get_macro_lambda_r(ri);
								if (best_allocated_lambda_r < curr_lambda_r) {
									best_allocated_lambda_r = curr_lambda_r;
									curr_allocated_mob[ri] = mob;
								}

							} else {

								if (mobile->get_pico() == NULL) {
									// 모바일이 피코에 연결되지 않은 경우
									// : 아무것도 하지 않는다.
								} else {
									
									// Pico에 연결한 경우
									const Pico* pico = mobile->get_pico()->pico;
									if (pico->get_non_sorted_mobile(ri, 0)->mobile == mobile) {
										if (pico->is_abs()) {
											// ABS인 경우
											curr_sum_lambda_r += mobile->get_abs_pico_lambda_r(ri);
											mobile->conns[ri] = ABS_PICO;
										} else {
											// non-ABS인 경우
											curr_sum_lambda_r += mobile->get_non_pico_lambda_r(ri);
											mobile->conns[ri] = NON_PICO_1;
										}
									} else {
										// 매크로가 버린 이용자가 피코에 첫번째 유저로 연결되지 않았으므로 버림
									}

								}

							}

						}
						curr_sum_lambda_r += best_allocated_lambda_r;

					}

					if (curr_sum_lambda_r > macro_best_sum_lambda_r) {
						macro_best_sum_lambda_r = curr_sum_lambda_r;
						FOREACH_RBS {
							FOREACH_MOBILES_TS {
								MobileConnection conn = mmobiles[mob]->mobile->conns[ri];
								switch (conn)
								{
								case NOTHING:
								case MACRO:
									if (curr_allocated_mob[ri] == mob) {
										best_mobile_states[mob * NUM_RB + ri] = MACRO;
									} else {
										best_mobile_states[mob * NUM_RB + ri] = NOTHING;
									}
									break;
								case ABS_PICO:
								case NON_PICO_1:
								case NON_PICO_2:
									best_mobile_states[mob * NUM_RB + ri] = conn;
									break;
								default:
									fprintf(stderr, "ERROR: Unexpected MobileConnection - %d\n", conn);
									break;
								}
							}
						}
					}

				}

				FOREACH_MOBILES_TS_ {
					FOREACH_RBS {
						((Mobile*) mobile)->conns[ri] = best_mobile_states[mob * NUM_RB + ri];
					}
				} END

				free(best_mobile_states);

			} else {
				// 매크로가 꺼져 있는 경우

				double curr_sum_lambda_r = 0.0;
				
				FOREACH_RBS {

					FOREACH_MOBILES_TS {
						Mobile* mobile = (Mobile*) mmobiles[mob]->mobile;
						// Pico에 연결한 경우

						if (mobile->get_pico() != NULL) {

							// Pico에 연결한 경우
							const Pico* pico = mobile->get_pico()->pico;
							if (pico->get_non_sorted_mobile(ri, 0)->mobile == mobile) {
								if (pico->is_abs()) {
									// ABS인 경우
									curr_sum_lambda_r += mobile->get_abs_pico_lambda_r(ri);
									mobile->conns[ri] = ABS_PICO;

								} else {
									// non-ABS인 경우
									curr_sum_lambda_r += mobile->get_abs_pico_lambda_r(ri);
									mobile->conns[ri] = NON_PICO_1;
								}
							}

						} else {
							mobile->conns[ri] = NOTHING;

						}

					}

				}

				macro_best_sum_lambda_r = curr_sum_lambda_r;

			}

			curr_sum_lambda_r += macro_best_sum_lambda_r;

		} END

		if (curr_sum_lambda_r > best_sum_lambda_r) {

			//printf("  BETTER%12.6lf > %12.6lf - ", curr_sum_lambda_r, best_sum_lambda_r);
			//FOREACH_MACROS printf("%d", macros[mac]->get_state()); printf("\n");

			best_sum_lambda_r = curr_sum_lambda_r;

			FOREACH_MACROS
				best_macro_states[mac] = macros[mac]->state;

		}

	}

	FOREACH_MACROS
		macros[mac]->state = best_macro_states[mac];

}
