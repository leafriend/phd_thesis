#include <stdio.h>

#include <stdlib.h>

#include <math.h>
#include <limits>

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

	pa3_lambda_r = 1.0;
}

void Macro::sort_mobiles() {

	FOREACH_RBS {

		first_mobile[ri] = NULL;
		double first_lambda_r = -std::numeric_limits<double>::infinity();

		//printf("num_mobiles_to_service: %d\n", num_mobiles_to_service);
		for (int mm = 0; mm < num_mobiles_to_service; mm++) {

			const Mobile* mobile = mobiles_to_service[mm]->mobile;
			const double macro_lambda_r = mobile->get_macro_lambda_r(ri);

			//printf("lambda: %lf\n", mobile->lambda);
			//printf("mobile->get_get_macro_throughput(): %lf\n", mobile->get_macro_throughput());
			//printf("macro_lambda_r: %lf\n", macro_lambda_r);

			if (mobile->get_pico() == NULL) {

				if (macro_lambda_r > first_lambda_r) {
					first_mobile[ri] = (Mobile*) mobile;
					first_lambda_r = macro_lambda_r;
				}

			} else {

				Pico* pico = (Pico*) mobile->get_pico()->pico;
				const int num_pico_mobiles_to_service = pico->get_num_mobiles_to_service();
				
				// 이 Pico는 모바일이 서비스 받을 Pico이므로 non_sorted_mobile 값은 항상 1 이상
				const Mobile* pico_first_mobile = pico->get_non_sorted_mobile(ri, 0)->mobile;

				if (mobile == pico_first_mobile) {
					// Macro에서 할당하려는 Mobile이 Pico의 첫 번째인 경우

					double lambda_r = macro_lambda_r - mobile->get_non_pico_lambda_r(ri);
					if (num_pico_mobiles_to_service > 1) {
						const Mobile* pico_second_mobile = pico->get_non_sorted_mobile(ri, 1)->mobile;

						// Pico의 두 번째 Mobile이 있는 경우
						// : Pico에서 빠진 첫 번째 Mobile의 lambda r 대신 두 번째 Mobile의 값을 추가
						// : TODO 하지만 두 번째 Mobile이 현재 Macro가 아니라 다른 Macro라면?
						lambda_r += pico_second_mobile->get_non_pico_lambda_r(ri);
					}

					if (lambda_r > first_lambda_r) {
						first_mobile[ri] = (Mobile*) mobile;
						first_lambda_r = lambda_r;
					}

				} else {
					// Macro에서 할당하려는 Mobile이 Pico의 첫 번째가 아닌 경우

					if (macro_lambda_r > first_lambda_r) {
						first_mobile[ri] = (Mobile*) mobile;
						first_lambda_r = macro_lambda_r;
					}

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

bool Macro::get_state() const {
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
