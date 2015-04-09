#include <stdio.h>

#include <stdlib.h>
#include <float.h>

#include "func.h"
#include "macro_mobile.h"
#include "pico_mobile.h"
#include "mobile.h"
#include "pico.h"
#include "macro.h"

Macro::Macro(int idx, double x, double y, double tx_power)
: idx(idx), x(x)
, y(y)
, tx_power(tx_power)
{
	num_mobiles_to_service = 0;
	num_mobiles_interfered = 0;
	allocation_count = 0;
}

void Macro::sort_mobiles() {

	FOREACH_RBS {

		first_mobile[ri] = NULL;
		double first_lambda_r = DBL_MIN;

		//printf("num_mobiles_to_service: %d\n", num_mobiles_to_service);
		for (int mm = 0; mm < num_mobiles_to_service; mm++) {

			Mobile* mobile = (Mobile*) mobiles_to_service[mm]->get_mobile();
			const double macro_lambda_r = mobile->lambda * mobile->get_macro_throughput(ri);

			//printf("lambda: %lf\n", mobile->lambda);
			//printf("mobile->get_get_macro_throughput(): %lf\n", mobile->get_macro_throughput());
			//printf("macro_lambda_r: %lf\n", macro_lambda_r);

			Pico* pico = (Pico*) mobile->get_pico()->get_pico();

			Mobile* pico_first_mobile = pico->get_first_mobile(ri);

			Mobile* pico_second_mobile = pico->get_second_mobile(ri);

			if (mobile == pico_first_mobile) {

				double lambda_r = macro_lambda_r - mobile->lambda * mobile->get_pico_throughput(ri);
				if (pico_second_mobile != NULL)
					lambda_r += pico_first_mobile->lambda * pico_first_mobile->get_pico_throughput(ri);

				if (lambda_r > first_lambda_r) {
					first_mobile[ri] = mobile;
					first_lambda_r = lambda_r;
				}

			} else {

				if (macro_lambda_r > first_lambda_r) {
					first_mobile[ri] = mobile;
					first_lambda_r = macro_lambda_r;
				}

			}

		}

	}

}

int Macro::get_num_mobiles_to_service() const {
	return num_mobiles_to_service;
}

Macro_Mobile** Macro::get_mobiles_to_service() {
	return mobiles_to_service;
}

void Macro::set_state(bool state) {
	this->state = state;
}

bool Macro::get_state() {
	return state;
}

void Macro::count_allocation() {
	if (state)
		allocation_count++;
}

Mobile* Macro::get_first_mobile(int ri) const {
	return first_mobile[ri];
}

int Macro::get_allocation_count() {
	return allocation_count;
}
