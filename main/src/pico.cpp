#include <stdio.h>

#include <stdlib.h>
#include <float.h>

#include "func.h"

#include "pico_mobile.h"
#include "macro.h"
#include "mobile.h"
#include "pico.h"

Pico::Pico(int idx, double x, double y, double tx_power)
: idx(idx), x(x), y(y), tx_power(tx_power)
{
	num_mobiles_to_service = 0;
	num_mobiles_interfered = 0;
	num_macros_interfering = 0;
}

void Pico::check_interfering(Macro* macro) {
	if (sqrt(pow((macro->x - x), 2) + pow((macro->y - y), 2)) < MP_INT_DIST) {
		macros_interfering[num_macros_interfering++] = macro;
	}
}

bool Pico::is_abs() {
	for (int i = 0; i < num_macros_interfering; i++)
		if (macros_interfering[i]->get_state())
			return false;
	return true;
}

void Pico::sort_mobiles() {

	FOREACH_RBS {

		first_mobile[ri] = NULL;
		second_mobile[ri] = NULL;

		double first_lambda_r = DBL_MIN;
		double second_lambda_r = DBL_MIN;

		abs_first_mobile[ri] = NULL;
		abs_second_mobile[ri] = NULL;

		double abs_first_lambda_r = DBL_MIN;
		double abs_second_lambda_r = DBL_MIN;

		//printf("num_mobiles_to_service: %d\n", num_mobiles_to_service);
		for (int pm = 0; pm < num_mobiles_to_service; pm++) {

			Mobile* mobile = (Mobile*) mobiles_to_service[pm]->get_mobile();

			const double lambda_r = mobile->lambda * mobile->get_pico_throughput(ri);

			if (first_mobile[ri] == NULL) {

				first_mobile[ri] = mobile;
				first_lambda_r = lambda_r;

			} else {

				if (lambda_r > first_lambda_r) {

					second_mobile[ri] = first_mobile[ri];
					second_lambda_r = first_lambda_r;

					first_mobile[ri] = mobile;
					first_lambda_r = lambda_r;

				} else if (lambda_r > second_lambda_r) {

					second_mobile[ri] = mobile;
					second_lambda_r = lambda_r;

				} else {

					// DO NOTHING

				}

			}

			// /////////////////////////////////////////////////////////////////////
			// ABS

			const double abs_lambda_r = mobile->lambda * mobile->get_abs_pico_throughput(ri);

			if (abs_first_mobile[ri] == NULL) {

				abs_first_mobile[ri] = mobile;
				abs_first_lambda_r = lambda_r;

			} else {

				if (abs_lambda_r > abs_first_lambda_r) {

					abs_second_mobile[ri] = abs_first_mobile[ri];
					abs_second_lambda_r = abs_first_lambda_r;

					abs_first_mobile[ri] = mobile;
					abs_first_lambda_r = abs_lambda_r;

				} else if (abs_lambda_r > abs_second_lambda_r) {

					abs_second_mobile[ri] = mobile;
					abs_second_lambda_r = abs_lambda_r;

				} else {

					// DO NOTHING

				}

			}

		}

	}

}

Mobile* Pico::get_first_mobile(int ri) {
	return first_mobile[ri];
}

Mobile* Pico::get_second_mobile(int ri) {
	return second_mobile[ri];
}

Mobile* Pico::get_abs_first_mobile(int ri) {
	return abs_first_mobile[ri];
}

Mobile* Pico::get_abs_second_mobile(int ri) {
	return abs_second_mobile[ri];
}
