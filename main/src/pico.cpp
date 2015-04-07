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
	num_mobiles_in_range = 0;
	num_mobiles_interfered = 0;
}

bool Pico::is_neighbor(Macro* macro) {
	return sqrt(pow((macro->x - x), 2) + pow((macro->y - y), 2)) < MP_INT_DIST;
}

void Pico::sort_mobiles() {

	first_mobile = NULL;
	second_mobile = NULL;

	double first_lambda_r = DBL_MIN;
	double second_lambda_r = DBL_MIN;

	abs_first_mobile = NULL;
	abs_second_mobile = NULL;

	double abs_first_lambda_r = DBL_MIN;
	double abs_second_lambda_r = DBL_MIN;

	//printf("num_mobiles_in_range: %d\n", num_mobiles_in_range);
	for (int pm = 0; pm < num_mobiles_in_range; pm++) {

		Mobile* mobile = (Mobile*) mobiles_in_range[pm]->get_mobile();

		const double lambda_r = mobile->lambda * mobile->get_pico_throughput();

		if (first_mobile == NULL) {

			first_mobile = mobile;
			first_lambda_r = lambda_r;

		} else {

			if (lambda_r > first_lambda_r) {

				second_mobile = first_mobile;
				second_lambda_r = first_lambda_r;

				first_mobile = mobile;
				first_lambda_r = lambda_r;

			} else if (lambda_r > second_lambda_r) {

				second_mobile = mobile;
				second_lambda_r = lambda_r;

			} else {

				// DO NOTHING

			}

		}

		// /////////////////////////////////////////////////////////////////////
		// ABS

		const double abs_lambda_r = mobile->lambda * mobile->get_abs_pico_throughput();

		if (abs_first_mobile == NULL) {

			abs_first_mobile = mobile;
			abs_first_lambda_r = lambda_r;

		} else {

			if (abs_lambda_r > abs_first_lambda_r) {

				abs_second_mobile = abs_first_mobile;
				abs_second_lambda_r = abs_first_lambda_r;

				abs_first_mobile = mobile;
				abs_first_lambda_r = abs_lambda_r;

			} else if (abs_lambda_r > abs_second_lambda_r) {

				abs_second_mobile = mobile;
				abs_second_lambda_r = abs_lambda_r;

			} else {

				// DO NOTHING

			}

		}

	}

}

Mobile* Pico::get_first_mobile() {
	return first_mobile;
}

Mobile* Pico::get_second_mobile() {
	return second_mobile;
}

Mobile* Pico::get_abs_first_mobile() {
	return abs_first_mobile;
}

Mobile* Pico::get_abs_second_mobile() {
	return abs_second_mobile;
}
