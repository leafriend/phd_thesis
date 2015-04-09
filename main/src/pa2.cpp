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

	// °¡´ÉÇÑ ¸ğµç Macro »óÅÂ(2 ^ NUM_MACRO = 1 << NUM_MACRO)¿¡ ´ëÇÑ ¹İº¹¹®
	int num_macro_state = 1 << NUM_MACRO;
	for (int s = 0; s < num_macro_state; s++) {

		// Macro »óÅÂ(ON/OFF) ÁöÁ¤
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

	// TODO ¸ÅÅ©·ÎÀÇ ¸ğ¹ÙÀÏ °¹¼ö°¡ »çÀÌÁî¸¦ ³Ñ¾î°¡´Â °æ¿ì ¹è¿­ µîÀ¸·Î º¯°æ
	int NUM_MOBILE_TS = macro->get_num_mobiles_to_service();
	if (NUM_MOBILE_TS > sizeof(long) * 8)
		fprintf(stderr, "Number of Mobile in range is over %d\n", sizeof(long));
	// °¡´ÉÇÑ ¸ğµç Mobile Cell Association »óÅÂ
	long num_mobile_state = 1 << NUM_MOBILE_TS;

	Macro_Mobile** mmobiles = macro->get_mobiles_to_service();

	if (macro->get_state()) {
		// ¸ÅÅ©·Î°¡ ÄÑÁ® ÀÖ´Â °æ¿ì
		// : °¢ ¸ğ¹ÙÀÏÀÌ ¸ÅÅ©·Î¿Í ÇÇÄÚ ¿¬°á »óÅÂÀÇ r°ª Áß ÃÖ´ë °ªÀ» Ã£À½

		bool* best_mobile_conn_macros = (bool*) malloc(sizeof(bool) * NUM_MOBILE_TS);

		for (int S = 0; S < num_mobile_state; S++) {

			double curr_sum_lambda_r = 0.0;

			// Mobile ¿¬°á »óÅÂ ÁöÁ¤
			FOREACH_MOBILES_TS {
				Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
				mobile->conn_macro = 1 == ((1 << mob) & S) >> mob;
			}

			FOREACH_RBS {

				FOREACH_MOBILES_TS {
					Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();

					if (mobile->conn_macro) {
						// Macro¿¡ ¿¬°áÇÑ °æ¿ì
						curr_sum_lambda_r += mobile->lambda * mobile->get_macro_throughput(ri);

					} else {
						// Pico¿¡ ¿¬°áÇÑ °æ¿ì

						if (mobile->get_pico()->get_pico()->is_abs()) {
							// ABSÀÎ °æ¿ì
							curr_sum_lambda_r += mobile->lambda * mobile->get_abs_pico_throughput(ri);

						} else {
							// non-ABSÀÎ °æ¿ì
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

		// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Î¸ï¿½ ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
		FOREACH_MOBILES_TS {
			Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
			mobile->conn_macro = best_mobile_conn_macros[mob];
		}
		
		free(best_mobile_conn_macros);

	} else {
		// ¸ÅÅ©·Î°¡ ²¨Á® ÀÖ´Â °æ¿ì

		double curr_sum_lambda_r = 0.0;
		
		FOREACH_RBS {

			FOREACH_MOBILES_TS {
				Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
				// Pico¿¡ ¿¬°áÇÑ °æ¿ì

				if (mobile->get_pico()->get_pico()->is_abs()) {
					// ABSÀÎ °æ¿ì
					curr_sum_lambda_r += mobile->lambda * mobile->get_abs_pico_throughput(ri);

				} else {
					// non-ABSÀÎ °æ¿ì
					curr_sum_lambda_r += mobile->lambda * mobile->get_pico_throughput(ri);
				}

			}

		}

		// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Å©ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Î¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ -- ï¿½ï¿½Å©ï¿½Î°ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ç·ï¿½
		FOREACH_MOBILES_TS {
			Mobile* mobile = (Mobile*) mmobiles[mob]->get_mobile();
			mobile->conn_macro = false;
		}

		*macro_best_sum_lambda_r = curr_sum_lambda_r;

	}

}
