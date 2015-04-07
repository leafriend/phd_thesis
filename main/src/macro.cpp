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
	num_mobiles_in_range = 0;
	num_mobiles_interfered = 0;
	allocation_count = 0;
}

void Macro::sort_mobiles() {

	first_mobile = NULL;
	double first_lambda_r = DBL_MIN;

	//printf("num_mobiles_in_range: %d\n", num_mobiles_in_range);
	for (int mm = 0; mm < num_mobiles_in_range; mm++) {

		Mobile* mobile = (Mobile*) mobiles_in_range[mm]->get_mobile();
		const double macro_lambda_r = mobile->lambda * mobile->get_macro_throughput();

		//printf("lambda: %lf\n", mobile->lambda);
		//printf("mobile->get_get_macro_throughput(): %lf\n", mobile->get_macro_throughput());
		//printf("macro_lambda_r: %lf\n", macro_lambda_r);

		Pico* pico = NULL;
		if (mobile->get_pico() != NULL)
			pico = (Pico*) mobile->get_pico()->get_pico();

		Mobile* pico_first_mobile = NULL;
		if (pico != NULL)
			pico_first_mobile = pico->get_first_mobile();

		Mobile* pico_second_mobile = NULL;
		if (pico != NULL)
			pico_second_mobile = pico->get_second_mobile();

		if (mobile == pico_first_mobile) {

			double lambda_r = macro_lambda_r - mobile->lambda * mobile->get_pico_throughput();
			if (pico_second_mobile != NULL)
				lambda_r += pico_first_mobile->lambda * pico_first_mobile->get_pico_throughput();

			if (lambda_r > first_lambda_r) {
				first_mobile = mobile;
				first_lambda_r = lambda_r;
			}

		} else {

			if (macro_lambda_r > first_lambda_r) {
				first_mobile = mobile;
				first_lambda_r = macro_lambda_r;
			}

		}

	}

}

Mobile* Macro::get_first_mobile() {
	return first_mobile;
}
